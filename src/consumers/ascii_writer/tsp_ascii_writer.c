/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer.c,v 1.3 2004-09-22 20:18:56 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : TSP ascii writer consumer

-----------------------------------------------------------------------
 */
#include "tsp_ascii_writer.h"
#include "tsp_consumer.h"
#include "tsp_simple_trace.h"
#include "tsp_const_def.h"
#include "tsp_time.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <assert.h>
#include <errno.h>

int yyparse (void);
extern FILE *yyin, *yyout;

static TSP_provider_t* myproviders = NULL;
#define OUTPUT_STREAM_BUFFER_SIZE 1024*10
#define MAX_VAR_NAME_SIZE 256
static char tc_output_buffer[OUTPUT_STREAM_BUFFER_SIZE];
static int stop_it = 0;

pthread_cond_t  tsp_ascii_writer_condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t tsp_ascii_writer_mutex   = PTHREAD_MUTEX_INITIALIZER;

int tsp_ascii_writer_parse_error    =  0;
int tsp_ascii_writer_lineno         =  0;
int tsp_ascii_writer_colno          =  0;
int tsp_ascii_writer_nb_var         =  0;
int tsp_ascii_writer_current_var    = -1;
int tsp_ascii_writer_header_style   =  0;
int tsp_ascii_writer_sample_running =  0;
TSP_consumer_symbol_requested_t*  g_tsp_symbols = NULL; 

int32_t 
tsp_ascii_writer_initialise(int* argc, char** argv[]) {
  
  int32_t retcode;

  if (!TSP_consumer_init(argc, argv)) {
    STRACE_ERROR(("TSP init failed"));    
    retcode = -1;
  } else {
    retcode = 0;
  }
  return retcode;
}  /* end of tsp_ascii_writer_initialise */

int32_t
tsp_ascii_writer_add_var(char* symbol_name) {

  int32_t retcode;
  
  /* 
   * if the tsp symbols is not allocated
   * we only do counting
   */
  if (NULL == g_tsp_symbols) {
    ++tsp_ascii_writer_nb_var;
    ++tsp_ascii_writer_current_var;
    retcode =0;
  } else {
    /* increment first since initialised to -1 */
    ++tsp_ascii_writer_current_var;
    g_tsp_symbols[tsp_ascii_writer_current_var].name = strdup(symbol_name);
    STRACE_INFO(("Added var <%s>",symbol_name));
    retcode =0;
  }
  return retcode;
} /* tsp_ascii_writer_add_var */

int32_t 
tsp_ascii_writer_add_var_period(int32_t period) {
  if (NULL != g_tsp_symbols) {
    g_tsp_symbols[tsp_ascii_writer_current_var].period = period;
    STRACE_INFO(("Period <%d>",period));
  }
  return 0;
} /* tsp_ascii_writer_add_var_period */

int32_t 
tsp_ascii_writer_add_comment(char* comment) {
  /* FIXME add dictionnary reconstruction facility */
  return 0;
} /* tsp_ascii_writer_add_var_period */


/*
 * This is an internal TSP ascii writer function which validate
 * a symbol against a tsp symbols list obtained from a TSP 
 * request info.
 * This is a brute force linear search
 * FIXME should be done another way on a hashed version of the tsp_symbols array.
 */
int32_t
tsp_ascii_writer_validate_symbol_info(char* symbol_name, 
				      const TSP_consumer_symbol_info_list_t* tsp_symbols) {
  int     i;
  int32_t retval;
  char*   searched_array_symbol;

  assert(tsp_symbols);
  retval = 0;
  i = strlen(symbol_name);
  searched_array_symbol = malloc(i+2);
  strncpy(searched_array_symbol,symbol_name,i);
  searched_array_symbol[i] = '[';
  searched_array_symbol[i+1] = '\0';

  for (i=0; i< tsp_symbols->len; ++i) {
    /* scalar symbol match */
    if (0==strcmp(tsp_symbols->val[i].name,symbol_name)) {
      STRACE_DEBUG(("Scalar symbol match <%s>",symbol_name));
      retval =1;
      break;
    }
    /* 
     * consider arrays 
     * symbols whose name is found in several symbol 
     * and found symbol == searched symbol + '['
     * "toto" is an array iff we found "toto["
     */
    if (NULL != strstr(tsp_symbols->val[i].name,searched_array_symbol)) {
      ++retval;
    }
  }
  return retval;
} /* end of tsp_ascii_writer_validatesymbol_info */

int32_t
tsp_ascii_writer_validate_symbol_requested(char* symbol_name,
					   const TSP_consumer_symbol_requested_list_t* tsp_symbols) {
  int i;
  int32_t retval;

  assert(tsp_symbols);
  retval = 0;
  for (i=0; i< tsp_symbols->len; ++i) {
    if (NULL != strstr(tsp_symbols->val[i].name,symbol_name)) {
      ++retval;
    }
  }
  return retval;
} /* end of tsp_ascii_writer_validatesymbol */

int32_t 
tsp_ascii_writer_load_config(const char* conffilename, 
			     TSP_consumer_symbol_requested_t**  tsp_symbols,
			     int32_t* nb_symbols) {
  
  int32_t retcode;
  char   syserr[TSP_MAX_SYSMSG_SIZE];
    
  retcode = 0;  
  
  yyin = fopen(conffilename,"r");
  if (((FILE*)(NULL)) == yyin) {    
    strncpy(syserr,strerror(errno),TSP_MAX_SYSMSG_SIZE);
    STRACE_ERROR(("Cannot open config file <%s> (%s)",conffilename,syserr));
    retcode = -1;
  }
  /* Lets parse the config file */
  if (0 == retcode) {    
    STRACE_INFO(("Parsing config file..."));
    /* First read Count */
    yyparse();
    retcode = tsp_ascii_writer_parse_error;
    STRACE_INFO(("<%d> variables requested...",tsp_ascii_writer_nb_var));
    /*
     * Allocate the global tsp symbol array then
     * rewind the file for second parsing.
     */
    if (0==retcode) {
      g_tsp_symbols = (TSP_consumer_symbol_requested_t*) calloc(tsp_ascii_writer_nb_var,sizeof(TSP_consumer_symbol_requested_t));
      
      /* restart parsing */
      rewind(yyin);
      tsp_ascii_writer_current_var = -1;
      
      yyrestart(yyin);
      /* parse again */
      yyparse(); 
      fclose(yyin);
      *nb_symbols  = tsp_ascii_writer_nb_var;
      *tsp_symbols = g_tsp_symbols;
    } /* proceed second parse if first parse was OK */
  }  
  return retcode;
} /* tsp_ascii_writer_load_config */

int32_t 
tsp_ascii_writer_validate_symbols(TSP_consumer_symbol_requested_t*  tsp_symbols,
				  int32_t nb_symbols,
				  const char* tsp_provider_hostname,
				  TSP_consumer_symbol_requested_list_t* tsp_symbol_list) {
  int32_t retcode;
  int32_t nb_providers;
  const TSP_consumer_information_t* tsp_info=NULL;
  int32_t symbol_dim;
  int32_t nb_scalar_symbol; 
  int32_t i;
  int32_t j;
  int32_t var_index;
  int32_t forced_period;
  
  retcode = 0;
  /* 
   * Connect to the provider.
   */
  if (NULL == myproviders) {
    TSP_consumer_connect_all(tsp_provider_hostname,&myproviders, &nb_providers);
    /* Verify if there is at least one provider */
    if (nb_providers<1) {
      STRACE_ERROR(("No provider found?!?"));
      retcode = -1;
    } else if (!TSP_consumer_request_open(myproviders[0], 0, 0 )) {
      STRACE_ERROR(("Cannot connect to provider <%s> (TSP_request_open failed.)",TSP_consumer_get_connected_name(myproviders[0])));
      retcode = -1;
    }
  }
  if (0==retcode) {
    /* send request info for getting symbols list */
    if(!TSP_consumer_request_information(myproviders[0])) {
      STRACE_ERROR(("TSP_request_information failed"));
      retcode = -1;
    }
  }

  if (0==retcode) {
    tsp_info = TSP_consumer_get_information(myproviders[0]);	
    /* We now validate symbols from config file against
     * the list provided by the TSP provider
     * (we discover array var too)
     */
    nb_scalar_symbol = 0;
    forced_period    = -1;
    for (i=0;i<nb_symbols;++i) {
      symbol_dim = tsp_ascii_writer_validate_symbol_info(tsp_symbols[i].name,&(tsp_info->symbols));
      /* symbol not found */
      if (0==symbol_dim) {
	fprintf(stderr,"Symbol <%s> not found on provider side.\n",tsp_symbols[i].name);
	/* hack for ignoring unfound symbols */
	tsp_symbols[i].phase  = -1;
      } else { /* symbol found */
	fprintf(stdout,"Asking for symbol <%s> with period <%d>",
		tsp_symbols[i].name,
		tsp_symbols[i].period);
	/* 
	 * FIXME force period to be the same 
	 * as the first valid symbol
	 */
	if (-1 == forced_period) {
	  forced_period = tsp_symbols[i].period;
	} else {
	  if (tsp_symbols[i].period != forced_period) {
	    tsp_symbols[i].period = forced_period;
	    fprintf(stdout,"[period forced to <%d>]",tsp_symbols[i].period);
	  }
	}
	if (symbol_dim>1) {
	  fprintf(stdout," [array of size <%d>]\n",symbol_dim);
	} else {
	  fprintf(stdout,"\n");
	}
	/* 
	 * It's not so nice 
	 * but we use the phase to store symbol dim
	 * FXIME waiting for tsp to handle arrays !!!
	 */
	tsp_symbols[i].phase  = symbol_dim;
	nb_scalar_symbol     += symbol_dim;
      } /* else symbol found */
    } /* loop over symbols coming from config file */
  } /* retcode is OK */
  
  /* Now build request sample */
  if (0==retcode) {
    tsp_symbol_list->val = (TSP_consumer_symbol_requested_t*)calloc(nb_scalar_symbol, sizeof(TSP_consumer_symbol_requested_t));
    tsp_symbol_list->len = nb_scalar_symbol;
    var_index = 0;
    for (i=0;i<nb_symbols; ++i) {
      /* 
       * Generate symbol name for array var specified without index.
       */
      if (tsp_symbols[i].phase > 1) {
	for (j=0;j<tsp_symbols[i].phase;++j) {	    
	  tsp_symbol_list->val[var_index].name = malloc(MAX_VAR_NAME_SIZE*sizeof(char));
	  snprintf(tsp_symbol_list->val[var_index].name,
		   MAX_VAR_NAME_SIZE,		     
		   "%s[%0d]",
		   tsp_symbols[i].name,
		   j);
	  STRACE_DEBUG(("Asking for TSP var = <%s>",tsp_symbol_list->val[var_index].name));
	  tsp_symbol_list->val[var_index].period = tsp_symbols[i].period;
	  tsp_symbol_list->val[var_index].phase  = 0;
	  ++var_index;
	} /* loop over array var index */
      } else {
	/* ignore symbols with negative phase */
	if (tsp_symbols[i].phase >0) {
	  tsp_symbol_list->val[var_index].name   = strdup(tsp_symbols[i].name);
	  STRACE_DEBUG(("Asking for TSP var = <%s>",tsp_symbol_list->val[var_index].name));
	  tsp_symbol_list->val[var_index].period = tsp_symbols[i].period;
	  tsp_symbol_list->val[var_index].phase  = 0;
	  ++var_index;
	}
      } /* end if tsp_symbols[i].phase > 1 */
    } /* loop over nb_symbols */
  } /* end of build request_sample */
  
  /* Now send request sample */
  if (0==retcode) {
    if (!TSP_consumer_request_sample(myproviders[0],tsp_symbol_list)) {
      STRACE_ERROR(("TSP request sample refused by the provider?huh?..."));
      retcode = -1;
    }
  }
  
  return retcode;
} /* tsp_ascii_writer_validate_symbols */

int32_t 
tsp_ascii_writer_start(FILE* sfile, int32_t nb_sample_max_infile) {
  
  int32_t retcode;
  int             new_sample;
  int             symbol_index;
  TSP_sample_t    sample;
  const TSP_consumer_symbol_requested_list_t*  symbols;
  int             complete_line;
  char            charbuf[MAX_VAR_NAME_SIZE];
  int             symbol_dim;
  int             nb_sample;

  retcode = 0;

  /* buf RAZ  */
  memset(tc_output_buffer,'\0',OUTPUT_STREAM_BUFFER_SIZE);
  /* tailored output stream buffer (if not stdout) */
  /* if (stdout != sfile) { */
/*     setvbuf(sfile,tc_output_buffer,_IOFBF,OUTPUT_STREAM_BUFFER_SIZE);   */
/*   } */

  /* Get previously configured symbols */
  symbols = TSP_consumer_get_requested_sample(myproviders[0]);
  /* Write header if necessary */
  switch  (tsp_ascii_writer_header_style) {
    case 0:
    /* no header */
    break;
    /* xxx style header */
    case 1: 
      for (symbol_index=0;symbol_index<symbols->len;++symbol_index) {
	/* 
	 * Si la variable est un tableau on compte calcul
	 * la taille de ce tableau
	 */
	strncpy(charbuf,symbols->val[symbol_index].name,MAX_VAR_NAME_SIZE);
	if (NULL != index(charbuf,'[')) {
	  *(index(charbuf,'[')) = '\0';
	  symbol_dim   = tsp_ascii_writer_validate_symbol_requested(charbuf,symbols);
	  symbol_index += symbol_dim - 1;
	} else {
	  symbol_dim = 1;
	}
	fprintf(sfile,"%s : %d\n", charbuf, symbol_dim);    
      }
      fprintf(sfile," ==========================================\n");
      fflush(sfile);
    break;
  default:
    /* no header */
    break;
  }
 
  /* Demarrage des sample au niveau provider */
  if(!TSP_consumer_request_sample_init(myproviders[0],0,0)) {
    STRACE_ERROR(("Sample init refused by the provider??..."));
    retcode = -1;
  }

  tsp_ascii_writer_sample_running = 1;
  STRACE_DEBUG(("Begin sample read...\n"));
  if (0 == retcode) {
    complete_line = 0;
    nb_sample     = 0;
    /* write loop */
    while (TSP_consumer_read_sample(myproviders[0],&sample, &new_sample) && !stop_it) {
      if (new_sample) {
	fprintf(sfile,"%16.9E ",sample.user_value);
	++complete_line;
	/* We write the endl if we receive a whole sample line */
	if (symbols->len==complete_line) {
	  fprintf(sfile,"\n");
	  complete_line = 0;
	  ++nb_sample;
	  if ((0 != nb_sample_max_infile) && 
	      (0 == (nb_sample % nb_sample_max_infile ))
	      ) {
	    rewind(sfile);
	  }
	}
      } else {
	tsp_usleep(1000);
      }
    } /* end of while ecriture */
  } /* end of if 0 */
  return retcode;
}

void* 
tsp_ascii_writer_thread(void* sfile) {

  static int retcode;
  retcode = tsp_ascii_writer_start((FILE* )sfile,0);  
  return &retcode;
} 


int32_t 
tsp_ascii_writer_stop() {
  
  int32_t retcode;

  STRACE_IO(("-->IN"));    
  retcode = 0;
  stop_it = 1;
  STRACE_IO(("-->OUT"));   
  return retcode;
}


int32_t 
tsp_ascii_writer_finalise() {
  
  int32_t retcode;

  STRACE_IO(("-->IN"));  
  if (NULL != myproviders) {
    if (tsp_ascii_writer_sample_running) {
      if (!TSP_consumer_request_sample_destroy(myproviders[0])) {
	STRACE_ERROR(("TSP_request_sample_destroy en erreur??..."));
      }  
    }
  }
  TSP_consumer_end();
  retcode = 0;    
  STRACE_IO(("-->OUT"));   
  return retcode;
} /* end of tsp_ascii_writer_finalise */
