/*

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer.c,v 1.14 2006-03-17 13:46:53 erk Exp $

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <errno.h>
#include <stdarg.h>

#define ASCII_WRITER_C
#include <tsp_ascii_writer.h>
#include <tsp_consumer.h>
#include <tsp_simple_trace.h>
#include <tsp_const_def.h>
#include <tsp_time.h>


int yyrestart(FILE*);
int yyparse (void);
extern FILE *yyin, *yyout;

static TSP_provider_t* myproviders = NULL;
#define OUTPUT_STREAM_BUFFER_SIZE 1024*10
#define MAX_VAR_NAME_SIZE 256
static char tc_output_buffer[OUTPUT_STREAM_BUFFER_SIZE];
static int stop_it = 0;

int tsp_ascii_writer_parse_error    =  0;
int tsp_ascii_writer_lineno         =  0;
int tsp_ascii_writer_colno          =  0;
int tsp_ascii_writer_nb_var         =  0;
int tsp_ascii_writer_current_var    = -1;
int tsp_ascii_writer_header_style   =  0;
int tsp_ascii_writer_sample_running =  0;
TSP_consumer_symbol_requested_t*  g_tsp_symbols = NULL; 
static tsp_ascii_writer_logMsg_ft my_logMsg = tsp_ascii_writer_logMsg_stdout; 

void tsp_ascii_writer_set_logMsgCB(tsp_ascii_writer_logMsg_ft logMsgCB) {
  my_logMsg = logMsgCB;
}

int32_t tsp_ascii_writer_logMsg_stdout(char* fmt, ...) {
  va_list args;
  char    message[2048];

  memset(message,0,2048);
  va_start(args, fmt);
  vsnprintf(message, 2048, fmt, args);
  va_end(args);

  fprintf(stdout,"AsciiWriterLib:: %s",message);
  return 0;
}

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



 
/* Used to accelerate array search, we store the last symbol found */
static char    previous_symbol_found[128] = "this_string_might_not_be_a_symbol_name_14742572";
static int32_t previous_symbol_found_dim = 0;

int32_t
tsp_ascii_writer_validate_symbol_info(char* symbol_name, 
				      const TSP_consumer_symbol_info_list_t* tsp_symbols) {
  int     i;
  int32_t retval;
  char*   searched_array_symbol;
  
  /* Compare this symbol with previous (accelerate array match) */
  if ((previous_symbol_found_dim != 0) &&
      (strncmp(symbol_name,previous_symbol_found,strlen(previous_symbol_found)) == 0 )) {
    STRACE_DEBUG(("Symbol [%s] matches with previous_symbol [%s]",symbol_name,previous_symbol_found));
    return previous_symbol_found_dim;
  }
  
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
  
  /* Store found symbol for later */
  if ( retval > 0) {
    /* Get the name of the symbol, without the brackets for an array */
    for ( i=0 ; ( (symbol_name[i] != '\0') && (symbol_name[i] != '[') ) ; i++ ) {
      previous_symbol_found[i]=symbol_name[i];
    }
    previous_symbol_found[i]='\0';
    STRACE_DEBUG(("symbol=[%s], previous_symbol_found =[%s]",symbol_name,previous_symbol_found));
    previous_symbol_found_dim=retval;
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
tsp_ascii_writer_make_unique(TSP_consumer_symbol_requested_t**  tsp_symbols,
			     int32_t* nb_symbols) {
  int i;
  int j;
  int nbsym_in;
  int nbsym_out;
  TSP_consumer_symbol_requested_t* symbols;
  
  nbsym_in  = *nb_symbols;
  nbsym_out = nbsym_in;
  symbols = *tsp_symbols;
  /* quick return if less than 2 symbols */
  if (nbsym_in<2) {
    return 0;
  }

  /* loop over the symbols */
  i = 1;
  while (i<nbsym_out) {
    /* loop in order to find */
    j = 0;
    while (j<i) {
      /* verify the to be accepted symbols is not already there */
      if (0==strcmp(symbols[i].name,symbols[j].name)) {
	/* remove duplicate iff period and phase are the same */
	if ((symbols[i].period == symbols[j].period) &&
	    (symbols[i].phase == symbols[j].phase)) {
	  /* copy remaining symbols */
	  if ((i+1)<nbsym_out) {
	    memmove(&symbols[i],&symbols[i+1],(nbsym_out-i)*sizeof(TSP_consumer_symbol_requested_t));
	  } else {
	    /* special case for last symbol remove */
	    --nbsym_out;
	    *nb_symbols = nbsym_out;
	    return 0;
	  }
	  /* remove one */
	  --nbsym_out;
	  /* start over the check */
	  j=0;
	} else {
	  return i;
	}
      } else {	
	++j;
      }
    } /* while (j<i) */
    ++i;
  } /* while (i<nbsym_out) */

  /* zero out duplicate at the end */
  if (nbsym_in>nbsym_out) {
    memset(&symbols[nbsym_out],0,(nbsym_in-nbsym_out)*sizeof(TSP_consumer_symbol_requested_t));
    *nb_symbols = nbsym_out;
  }
  return 0;
}

int32_t 
tsp_ascii_writer_new_validate_symbols(TSP_consumer_symbol_requested_t*  tsp_symbols,
				      int32_t nb_symbols,
				      const char* tsp_provider_url,
				      TSP_consumer_symbol_requested_list_t* tsp_symbol_list) {

  int32_t retcode;
  const TSP_consumer_information_t* current_tsp_info=NULL;
  int32_t symbol_dim;
  int32_t nb_scalar_symbol; 
  int32_t i;
  int32_t j;
  int32_t var_index;
  int32_t forced_period;
  TSP_consumer_symbol_requested_list_t current_requested_symbols_list;
  
  
  retcode = 0;
  /* 
   * Connect to the provider.
   */
  if (NULL == myproviders) {
    myproviders = calloc(1,sizeof(TSP_provider_t));
    myproviders[0] = TSP_consumer_connect_url(tsp_provider_url);
    /* Verify if there is at least one provider */
    if (0==myproviders[0]) {
      STRACE_ERROR(("No provider found?!?"));
      retcode = -1;
    } else if (!TSP_consumer_request_open(myproviders[0], 0, 0 )) {
      STRACE_ERROR(("Cannot connect to provider <%s> (TSP_request_open failed.)",TSP_consumer_get_connected_name(myproviders[0])));
      retcode = -1;
    }
  }

  /* first build a request_sample with provided symbols */
  current_requested_symbols_list.val = (TSP_consumer_symbol_requested_t*)calloc(nb_symbols, sizeof(TSP_consumer_symbol_requested_t));
  current_requested_symbols_list.len = nb_symbols;
  memcpy(current_requested_symbols_list.val,tsp_symbols,nb_symbols*sizeof(TSP_consumer_symbol_requested_t));
  STRACE_INFO(("Initial number of asked symbol = %d",nb_symbols));
  /* send the initial request_sample for obtaining provider-side validation */  
  if (!TSP_consumer_request_sample(myproviders[0],&current_requested_symbols_list)) {
    /* now build request filtered info to handle invalid symbols */
    nb_scalar_symbol = 0;
    forced_period    = -1;
    for (i=0;i<current_requested_symbols_list.len;++i) {
      STRACE_INFO(("Examining symbol <%s> of pgi <%d>",current_requested_symbols_list.val[i].name,current_requested_symbols_list.val[i].index));
      if (current_requested_symbols_list.val[i].index == -1) {
	my_logMsg("Checking for symbol like <%s> on provider side.\n",current_requested_symbols_list.val[i].name);
	/* Ask for filtered information using the name of the currently invalid symbol */
	TSP_consumer_request_filtered_information(myproviders[0],TSP_FILTER_SIMPLE,current_requested_symbols_list.val[i].name);
	current_tsp_info = TSP_consumer_get_information(myproviders[0]);
	symbol_dim = tsp_ascii_writer_validate_symbol_info(current_requested_symbols_list.val[i].name,&(current_tsp_info->symbols));
	if (0==symbol_dim) {
	  my_logMsg("Symbol <%s> not found on provider side.\n",tsp_symbols[i].name);
	  /* hack for ignoring unfound symbols */
	  tsp_symbols[i].phase  = -1;   
	  continue;
	} else { /* symbol found */
	  my_logMsg("Asking for array symbol <%s> with period <%d>\n",
		  tsp_symbols[i].name,
		  tsp_symbols[i].period);
	}
      } 
      /* else symbol is ok and scalar */
      else {
	symbol_dim = 1;
	my_logMsg("Asking for symbol <%s> with period <%d>\n",
		tsp_symbols[i].name,
		tsp_symbols[i].period);
      }
      /* 
       * FIXME force period to be the same 
       * as the first valid symbol
       */
      if (-1 == forced_period) {
	forced_period = tsp_symbols[i].period;
      } else {
	if (tsp_symbols[i].period != forced_period) {
	  tsp_symbols[i].period = forced_period;
	  my_logMsg(" ---> [period forced to <%d>]\n",tsp_symbols[i].period);
	}
      }
      if (symbol_dim>1) {
	my_logMsg(" ---> [array of size <%d>]\n",symbol_dim);
      }
      /* 
       * It's not so nice 
       * but we use the phase to store symbol dim
       * FXIME waiting for tsp to handle arrays !!!
       */
      tsp_symbols[i].phase  = symbol_dim;
      nb_scalar_symbol     += symbol_dim;
    } /* end for loop */
  } else { /* initial request_sample is ok */
    nb_scalar_symbol = current_requested_symbols_list.len;
    /* now force period to first one */
    forced_period = tsp_symbols[0].period;
    for (i=0;i<nb_scalar_symbol;++i) {
      my_logMsg("Asking for symbol <%s> with period <%d>\n",
	      tsp_symbols[i].name,
	      tsp_symbols[i].period);
      if (tsp_symbols[i].period != forced_period) {
	tsp_symbols[i].period = forced_period;
	my_logMsg(" ---> [period forced to <%d>]\n",tsp_symbols[i].period);
      }
    }
  }
      
  /* Now build final request sample */
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
	if (tsp_symbols[i].phase >= 0) {
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
      TSP_consumer_print_invalid_symbols(stderr,&tsp_symbol_list,tsp_provider_url);
      STRACE_ERROR(("TSP request sample refused by the provider?huh?..."));
      retcode = -1;
    }
  }
  
  return retcode;
  
}

int32_t 
tsp_ascii_writer_validate_symbols(TSP_consumer_symbol_requested_t*  tsp_symbols,
				  int32_t nb_symbols,
				  const char* tsp_provider_url,
				  TSP_consumer_symbol_requested_list_t* tsp_symbol_list) {

  return tsp_ascii_writer_new_validate_symbols(tsp_symbols,
					       nb_symbols,
					       tsp_provider_url,
					       tsp_symbol_list);

}


int32_t 
tsp_ascii_writer_start(FILE* sfile, int32_t nb_sample_max_infile, OutputFileFormat_t file_format) {
  
  int32_t retcode;
  int             new_sample;
  int             symbol_index;
  TSP_sample_t    sample;
  const TSP_consumer_symbol_requested_list_t*  symbols;
  int             complete_line;
  char            charbuf[MAX_VAR_NAME_SIZE];
  int             symbol_dim;
  int             nb_sample;
  int					indice;
  char				indice_char[5];
  
  char** tab_colonne=NULL;

  retcode = 0;

  /* buf RAZ  */
  memset(tc_output_buffer,'\0',OUTPUT_STREAM_BUFFER_SIZE);
  /* tailored output stream buffer (if not stdout) */
  /* if (stdout != sfile) { */
/*     setvbuf(sfile,tc_output_buffer,_IOFBF,OUTPUT_STREAM_BUFFER_SIZE);   */
/*   } */

  /* Get previously configured symbols */
  symbols = TSP_consumer_get_requested_sample(myproviders[0]);

  tsp_ascii_writer_header_style=file_format;

  /* Write header if necessary */
  switch  (tsp_ascii_writer_header_style) {
    case SimpleAsciiTabulated_FileFmt:
    /* no header */
    break;
    /* xxx style header */
    case 1: 
      for (symbol_index=0;symbol_index<symbols->len;++symbol_index) 
		{
			/* 
	 		 * Si la variable est un tableau on compte calcul
	 		 * la taille de ce tableau
	 		 */
			strncpy(charbuf,symbols->val[symbol_index].name,MAX_VAR_NAME_SIZE);
			if (NULL != index(charbuf,'[')) 
			{
	 			 *(index(charbuf,'[')) = '\0';
	  			 symbol_dim   = tsp_ascii_writer_validate_symbol_requested(charbuf,symbols);
	  			 symbol_index += symbol_dim - 1;
			}
			else
			{
	  			symbol_dim = 1;
			}
			fprintf(sfile,"%s : %d\n", charbuf, symbol_dim);    
      }
      fprintf(sfile," ==========================================\n");
      fflush(sfile);
    break;
	 /*--------------*/
	 case 2:
	 
	 	/* tableau contenant l'entete des colonnes
		 */
	 	tab_colonne=(char**)malloc(symbols->len * sizeof(char*));
		
	   for (symbol_index=0;symbol_index<symbols->len;++symbol_index) 
		{
			/* 
	 		 * Si la variable est un tableau on compte calcul
	 		 * la taille de ce tableau
	 		 */
			strncpy(charbuf,symbols->val[symbol_index].name,MAX_VAR_NAME_SIZE);
			if (NULL != index(charbuf,'[')) 
			{
	  			*(index(charbuf,'[')) = '\0';
	  			symbol_dim   = tsp_ascii_writer_validate_symbol_requested(charbuf,symbols);
	  						
				/*
				 * ecriture des entetes de colonnes a n dimension dans le tableau
				 */
				for(indice=0;indice<symbol_dim;++indice)
				{
				
					sprintf(indice_char,"%d",indice+1);
					*(tab_colonne + symbol_index + indice)=(char*)malloc(strlen(charbuf)+strlen(indice_char)+3);
					sprintf(*(tab_colonne + symbol_index + indice),"%s(%s)",charbuf,indice_char);
					
				}
				
				symbol_index += symbol_dim - 1;


			}
			else
			{
	 			 symbol_dim = 1;
				 
				 /*
				  * ecriture des entetes des variables à 1 dimension dans le tableau
				  */
				 *(tab_colonne + symbol_index)=(char*)malloc(strlen(charbuf)+1);
			    strcpy(*(tab_colonne + symbol_index),charbuf);

			}
			fprintf(sfile,"%s : %d : double : s\n", charbuf, symbol_dim);
			
      }
      fprintf(sfile," ==========================================\n");
		
		/*
		 * ecriture dans le fichier des entetes de colonnes
		 */
		for (symbol_index=0;symbol_index<symbols->len;++symbol_index)
		{
			fprintf(sfile,"%s	", *(tab_colonne + symbol_index));
			free(*(tab_colonne + symbol_index));
		}
		free(tab_colonne);
		
		fprintf(sfile,"\n");

      fflush(sfile);

	 break;
	 /*--------------*/
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
  if (0 == retcode) 
  {
    complete_line = 0;
    nb_sample     = 0;
    /* write loop */
    while (TSP_consumer_read_sample(myproviders[0],&sample, &new_sample) && !stop_it) 
    {
      if (new_sample) 
      {
			if (tsp_ascii_writer_header_style==2) 
			{
				fprintf(sfile,"%f	",sample.user_value);
				++complete_line;
				/* We write the endl if we receive a whole sample line */
				if (symbols->len==complete_line) 
				{
	  				fprintf(sfile,"\n");
	 				complete_line = 0;
	  				++nb_sample;
	  				if ((0 != nb_sample_max_infile) && 
	      			(0 == (nb_sample % nb_sample_max_infile ))) 
					{
	    				rewind(sfile);
	  				}
				}
			} 
			else 
			{
				fprintf(sfile,"%16.9E ",sample.user_value);
				++complete_line;
				/* We write the endl if we receive a whole sample line */
				if (symbols->len==complete_line) 
				{
	  				fprintf(sfile,"\n");
	 				complete_line = 0;
	  				++nb_sample;
	  				if ((0 != nb_sample_max_infile) && 
	      			(0 == (nb_sample % nb_sample_max_infile ))) 
					{
	    				rewind(sfile);
	  				}
				}
			}
      } 
      else 
      {
	tsp_usleep(1000);
      }
    } /* end of while ecriture */
  } /* end of if 0 */
  return retcode;
}

void* 
tsp_ascii_writer_thread(void* sfile) {

  static int retcode;
  retcode = tsp_ascii_writer_start((FILE* )sfile,0,0);  
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
