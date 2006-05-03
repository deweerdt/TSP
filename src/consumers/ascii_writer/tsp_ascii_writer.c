/*

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer.c,v 1.24 2006-05-03 21:17:48 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD,Robert PAGNOT and Arnaud MORVAN

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

static TSP_provider_t myprovider;
#define OUTPUT_STREAM_BUFFER_SIZE 1024*10
#define MAX_VAR_NAME_SIZE 256
static char tc_output_buffer[OUTPUT_STREAM_BUFFER_SIZE];
static int stop_it = 0;
const static char* fmt_tab[] = { 0,
			      "%16.9F ",
			      "%16.9F ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%d ",
			      "%c ",
			      "%c ",
			      "%c ",
			      0				
};
const static char* libelle_type_tab[] = { "unknown",
			      "double",
			      "float",
			      "entier",
			      "entier",
			      "entier",
			      "entier",
			      "entier",
			      "entier",
			      "entier",
			      "entier",
			      "character",
			      "character",
			      "type_raw",
			      0				
};

int tsp_ascii_writer_parse_error    =  0;
int tsp_ascii_writer_lineno         =  0;
int tsp_ascii_writer_colno          =  0;
int tsp_ascii_writer_nb_var         =  0;
int tsp_ascii_writer_current_var    = -1;
int tsp_ascii_writer_header_style   =  0;
int tsp_ascii_writer_sample_running =  0;
TSP_sample_symbol_info_t*  g_tsp_symbols = NULL; 
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

  retcode = TSP_consumer_init(argc, argv);
  if (TSP_STATUS_OK!=retcode) {
    STRACE_ERROR(("TSP init failed"));    
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
 

int32_t 
tsp_ascii_writer_load_config(const char* conffilename, 
			     TSP_sample_symbol_info_t**  tsp_symbols,
			     uint32_t* nb_symbols) {
  
  int32_t retcode;
  char   syserr[TSP_MAX_SYSMSG_SIZE];
    
  retcode = TSP_STATUS_OK;  
  
  yyin = fopen(conffilename,"r");
  if (((FILE*)(NULL)) == yyin) {    
    strncpy(syserr,strerror(errno),TSP_MAX_SYSMSG_SIZE);
    STRACE_ERROR(("Cannot open config file <%s> (%s)",conffilename,syserr));
    retcode = TSP_STATUS_ERROR_AW_CONFIG_FILE_INVALID;
  }
  /* Lets parse the config file */
  if (TSP_STATUS_OK == retcode) {    
    STRACE_INFO(("Parsing config file..."));
    /* First parse used for COUNTING requested symbols */
    yyparse();
    STRACE_INFO(("<%d> variables requested...",tsp_ascii_writer_nb_var));
    if (0!=tsp_ascii_writer_parse_error) {
      retcode = TSP_STATUS_ERROR_AW_CONFIG_FILE_PARSE_ERROR;
    }
    /*
     * Allocate the global tsp symbol array then
     * rewind the file for second parsing.
     */
    if ((TSP_STATUS_OK==retcode)) {
      g_tsp_symbols = (TSP_sample_symbol_info_t*) calloc(tsp_ascii_writer_nb_var,
							 sizeof(TSP_sample_symbol_info_t));
      
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
tsp_ascii_writer_make_unique(TSP_sample_symbol_info_t**  tsp_symbols,
			     uint32_t* nb_symbols) {
  int i;
  int j;
  int nbsym_in;
  int nbsym_out;
  TSP_sample_symbol_info_t* symbols;
  
  nbsym_in  = *nb_symbols;
  nbsym_out = nbsym_in;
  symbols = *tsp_symbols;
  /* quick return if less than 2 symbols */
  if (nbsym_in<2) {
    return TSP_STATUS_OK;
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
	  my_logMsg("Duplicate symbol <%s> with period <%d> (removed)\n",
		    symbols[i].name,symbols[i].period);
	  /* copy remaining symbols */
	  if ((i+1)<nbsym_out) {
	    memmove(&symbols[i],&symbols[i+1],(nbsym_out-i)*sizeof(TSP_sample_symbol_info_t));
	  } else {
	    /* special case for last symbol remove */
	    --nbsym_out;
	    *nb_symbols = nbsym_out;
	    return TSP_STATUS_OK;
	  }
	  /* remove one */
	  --nbsym_out;
	  /* start over the check */
	  j=0;
	} else {
	  *nb_symbols = j;
	  return TSP_STATUS_ERROR_AW_DUPLICATE_SYMBOLS;
	}
      } else {	
	++j;
      }
    } /* while (j<i) */
    ++i;
  } /* while (i<nbsym_out) */

  /* zero out duplicate at the end */
  if (nbsym_in>nbsym_out) {
    memset(&symbols[nbsym_out],0,(nbsym_in-nbsym_out)*sizeof(TSP_sample_symbol_info_t));
    *nb_symbols = nbsym_out;
  }
  return TSP_STATUS_OK;
} /* tsp_ascii_writer_make_unique */

int32_t 
tsp_ascii_writer_validate_symbols(TSP_sample_symbol_info_list_t* requestedSSIL,
				  const char* tsp_provider_url,
				  TSP_sample_symbol_info_list_t* validatedSSIL) {

  int32_t retcode;
  const TSP_answer_sample_t* current_tsp_info=NULL;
  int32_t nbSymbolMatch;
  int32_t nbRequestedSymbols; 
  int32_t i;
  int32_t valid_index;
  int32_t forced_period;
  TSP_sample_symbol_info_list_t current_requestedSSIL;
  TSP_sample_symbol_info_t* currentSymbol = NULL;

  assert(requestedSSIL);
  assert(validatedSSIL);
    
  retcode = TSP_STATUS_OK;
  /* 
   * Connect to the provider.
   */
  if (NULL == myprovider) {
    myprovider = TSP_consumer_connect_url(tsp_provider_url);
    /* Verify if the provider was reached */
    if (0==myprovider) {
      STRACE_ERROR(("Unreachable or no TSP provider running at TSP URL <%s> ?!?",tsp_provider_url));
      retcode = TSP_STATUS_ERROR_PROVIDER_UNREACHABLE;
      return retcode;
    } else  {
      retcode =TSP_consumer_request_open(myprovider, 0, 0 );
      if (TSP_STATUS_OK!=retcode) {
	STRACE_ERROR(("TSP_request_open to TSP URL <%s> FAILED",tsp_provider_url));
	return retcode;
      }
    }
  }
  
  /* First build a request_sample with provided requested symbols */
  TSP_SSIList_initialize(&current_requestedSSIL,TSP_SSIList_getSize(*requestedSSIL));
  TSP_SSIList_copy(&current_requestedSSIL,*requestedSSIL);
  nbRequestedSymbols = TSP_SSIList_getSize(current_requestedSSIL);
  my_logMsg("Initially asking for <%d> symbol(s)\n",nbRequestedSymbols);

  /* 
   * Force period BEFORE sending the first request_sample 
   * if not you'll get complicated answer_sample you really
   * don't want.
   */
  my_logMsg("Enforcing same period for every symbols <begin>...\n");
  forced_period = TSP_SSIList_getSSI(current_requestedSSIL,0)->period;

  for (i=1;i<TSP_SSIList_getSize(current_requestedSSIL);++i) {
    if (TSP_SSIList_getSSI(current_requestedSSIL,i)->period != forced_period) {
      my_logMsg("  ---> Symbol <%s> with period <%d> enforced to period <%d>\n",
		TSP_SSIList_getSSI(current_requestedSSIL,i)->name,
		TSP_SSIList_getSSI(current_requestedSSIL,i)->period,
		forced_period);
      TSP_SSIList_getSSI(current_requestedSSIL,i)->period = forced_period;
    }
  }
  my_logMsg("Enforcing same period <done>.\n");
  
  /* Send the initial request_sample for obtaining provider-side validation */  
  if (TSP_STATUS_OK!=TSP_consumer_request_sample(myprovider,&current_requestedSSIL)) {

    /* Now build request filtered info to handle invalid symbol(s) */
    nbRequestedSymbols = 0;
    for (i=0;i<TSP_SSIList_getSize(current_requestedSSIL);++i) { 
      
      currentSymbol = TSP_SSIList_getSSI(current_requestedSSIL,i);
      STRACE_INFO(("Examining symbol <%s> of pgi <%d>",
		   currentSymbol->name,
		   currentSymbol->provider_global_index));

      if (currentSymbol->provider_global_index == -1)  {
	my_logMsg("Checking for symbol like <%s> on provider side.\n",currentSymbol->name);

	/* Ask for filtered information using the name of the currently invalid symbol */
	retcode = TSP_consumer_request_filtered_information(myprovider,TSP_FILTER_SIMPLE,currentSymbol->name);
	if (TSP_STATUS_OK!=retcode) {
	  return retcode;
	}
	current_tsp_info = TSP_consumer_get_information(myprovider);
	/* 
	 * the number of symbols which matches the filter
	 * is the length of the SSIList embedded in answer_sample
	 */
	nbSymbolMatch = TSP_SSIList_getSize(current_tsp_info->symbols);
	if (0==nbSymbolMatch) {
	  my_logMsg("Symbol <%s> not found on provider side (will be ignored).\n",currentSymbol->name);
	  /* Hack for ignoring unfound symbols */	  
	  TSP_SSIList_getSSI(current_requestedSSIL,i)->phase  = -1;   
	  continue;
	} 
	if (1==nbSymbolMatch) { /* symbol found: single match be nice accept */
	  my_logMsg("Only <%d> partial match for symbol <%s>, accepted\n",
		    nbSymbolMatch,
		    TSP_SSIList_getSSI(current_requestedSSIL,i)->name);

	  free(TSP_SSIList_getSSI(current_requestedSSIL,i)->name);
	  TSP_SSIList_getSSI(current_requestedSSIL,i)->name = 
	    strdup(TSP_SSIList_getSSI(current_tsp_info->symbols,0)->name);

	  my_logMsg("Asking for symbol <%s> with period <%d>\n",
		  TSP_SSIList_getSSI(current_requestedSSIL,i)->name,
		  TSP_SSIList_getSSI(current_requestedSSIL,i)->period);
	}
	if (nbSymbolMatch>1) { /* symbol found: multiple match */
	  /* 
	   * We may have partial matches which indicates mistakes
	   */
	  my_logMsg("<%d> partial match(es) for symbol <%s>, check your config file\n",
		    nbSymbolMatch,
		    TSP_SSIList_getSSI(current_requestedSSIL,i)->name,
		    TSP_SSIList_getSSI(current_requestedSSIL,i)->period);
	  my_logMsg("Either comment-out the offending symbol or correct its name\n");
	  return TSP_STATUS_ERROR_UNKNOWN;
	}
      } 
      /* else symbol is ok and scalar */
      else {
	nbSymbolMatch = 1;
	my_logMsg("Asking for symbol <%s> with period <%d>\n",
		  TSP_SSIList_getSSI(current_requestedSSIL,i)->name,
		  TSP_SSIList_getSSI(current_requestedSSIL,i)->period);
      }
      nbRequestedSymbols  += nbSymbolMatch;
    } /* end for loop */
  } 
      
  /* Now build final request sample */
  if (TSP_STATUS_OK==retcode)  {
    TSP_SSIList_initialize(validatedSSIL,nbRequestedSymbols);
    valid_index = 0;
    for (i=0;i<TSP_SSIList_getSize(current_requestedSSIL); ++i) {
      /* 
       * Ignore symbols with negative phase since they are
       * unknown from provider and must be ignored
       */
      currentSymbol = TSP_SSIList_getSSI(current_requestedSSIL,i);
      if (currentSymbol->phase >= 0) {
	TSP_SSI_copy(TSP_SSIList_getSSI(*validatedSSIL,valid_index),
		     *currentSymbol);
	STRACE_DEBUG(("Asking for TSP var = <%s>",currentSymbol->name));
	++valid_index;
      } /* end if tsp_symbols[i].phase >= 0 */
    } /* loop over nb_symbols */
  } /* end of build final request_sample */

  /* destroy previously requested SSI */
  TSP_SSIList_finalize(&current_requestedSSIL);
  
  /* Now send request sample */
  if (TSP_STATUS_OK==retcode) {
    my_logMsg("Finally asking for <%d> symbol(s)\n",TSP_SSIList_getSize(*validatedSSIL));    
    retcode = TSP_consumer_request_sample(myprovider,validatedSSIL);
    if (TSP_STATUS_OK!=retcode) {
      TSP_consumer_print_invalid_symbols(stderr,&(validatedSSIL->TSP_sample_symbol_info_list_t_val[0]),tsp_provider_url);
      STRACE_ERROR(("(final) TSP request sample refused by the provider?huh?..."));
      return retcode;
    }
  }

  /* Now send request sample for retrieving extended information */
  if (TSP_STATUS_OK==retcode) {
    int32_t * pgis;
    int32_t pgis_len;
    
    pgis_len =TSP_SSIList_getSize(*validatedSSIL);
    pgis=malloc(sizeof(int32_t) * pgis_len);

    for (i=0;i<pgis_len;++i) {
      pgis[i]=TSP_SSIList_getSSI(*validatedSSIL,i)->provider_global_index;
    }

    retcode = TSP_consumer_request_extended_information(myprovider, pgis, pgis_len);
    if (TSP_STATUS_OK!=retcode) {
      STRACE_ERROR(("TSP request extended information refused by the provider?huh?..."));
      free(pgis);
    }
    free(pgis);
  }
  return retcode;  
} /* end of tsp_ascii_writer_validate_symbols */

int32_t 
tsp_ascii_writer_start(FILE* sfile, 
		       int32_t nb_sample_max_infile, 
		       OutputFileFormat_t file_format,
		       TSP_sample_symbol_info_list_t* validatedSSIL) {
  
  int32_t retcode;
  int             new_sample;
  int             symbol_index;
  TSP_sample_t    sample;
  const TSP_sample_symbol_info_list_t*  symbols;
  TSP_sample_symbol_info_t* currentSSI;

  const TSP_sample_symbol_extended_info_list_t* extended_symbols;
  const TSP_extended_info_t* ext_info_profil;
  const TSP_extended_info_t* ext_info_unit;
  const TSP_extended_info_t* ext_info_ordre;
  TSP_extended_info_list_t* currentEIList;
  char* array_label;

  int             complete_line;
  char            charbuf[MAX_VAR_NAME_SIZE];
  int             symbol_dim;
  int             nb_sample;
  int		  indice;
  int32_t         nb_data_receive=0;
  
  char** tab_colonne=NULL;

  assert(validatedSSIL);

  retcode = 0;

  /* buf RAZ  */
  memset(tc_output_buffer,'\0',OUTPUT_STREAM_BUFFER_SIZE);
  /* tailored output stream buffer (if not stdout) */
  /* if (stdout != sfile) { */
/*     setvbuf(sfile,tc_output_buffer,_IOFBF,OUTPUT_STREAM_BUFFER_SIZE);   */
/*   } */

  /* Get previously configured symbols */
  symbols = TSP_consumer_get_requested_sample(myprovider);

  extended_symbols=TSP_consumer_get_extended_information(myprovider);

  tsp_ascii_writer_header_style=file_format;

  /* Write header if necessary */
  switch  (tsp_ascii_writer_header_style) {
  case SimpleAsciiTabulated_FileFmt:
    /* no header */
    break;
  case BACH_FileFmt: 
    for (symbol_index=0;
	 symbol_index<TSP_SSIList_getSize(*symbols);
	 ++symbol_index) {

      currentSSI = TSP_SSIList_getSSI(*symbols,symbol_index);
      /* its better to print nelem than dimension since nelem MAY be smaller than dimension */
      fprintf(sfile,"%s : %d\n", currentSSI->name, currentSSI->nelem);    
    }
    fprintf(sfile," ==========================================\n");
    fflush(sfile);
    break;
    
  case MACSIM_FileFmt:
    
    /* 
     * array with the column title
     */
    tab_colonne=(char**)malloc(TSP_SSIList_getSize(*symbols) * sizeof(char*));
    
    for (symbol_index=0;
	 symbol_index < TSP_SSIList_getSize(*symbols);
	 ++symbol_index) {

	/* retrieve the extended information*/
        ext_info_unit   = NULL;
        ext_info_profil = NULL;
	currentEIList   = TSP_SSEIList_getEIList(*extended_symbols,symbol_index);

	ext_info_unit   = TSP_EIList_findEIByKey(currentEIList,"unit");	
	ext_info_profil = TSP_EIList_findEIByKey(currentEIList,"profile");
	ext_info_ordre  = TSP_EIList_findEIByKey(currentEIList,"order");
 
	/* 
	 * If the variable is an array, we calculate his size
	 */
	strncpy(charbuf,TSP_SSIList_getSSI(*symbols,symbol_index)->name,MAX_VAR_NAME_SIZE);

	/* test if dimension is 1 or more*/
	if((NULL==ext_info_profil)?0:strcmp(ext_info_profil->value,"1")) {
	  /* FIXME commenter la creation des labels de tableaux */
	    array_label=new_array_label(charbuf,ext_info_profil->value,ext_info_ordre->value,0);	    
	    *(tab_colonne + symbol_index)=(char*)malloc(strlen(charbuf)+strlen(array_label)+1);
	    sprintf(*(tab_colonne + symbol_index),"%s",array_label);
	    free(array_label);	   
	    symbol_dim   = TSP_SSIList_getSSI(*symbols,symbol_index)->nelem;	  
	} else {
	  symbol_dim = 1;	    
	  /*
	   * write variable name with 1 dimension in this array
	   */
	  strcat(charbuf,"\t");
	  *(tab_colonne + symbol_index)=(char*)malloc(strlen(charbuf)+1);
	  strcpy(*(tab_colonne + symbol_index),charbuf);
	  
	}
	
	fprintf(sfile,"%s : %s : %s : %s \n", 
		charbuf, 
		(NULL==ext_info_profil)?"1":ext_info_profil->value,
		libelle_type_tab[symbols->TSP_sample_symbol_info_list_t_val[symbol_index].type],
		(NULL==ext_info_unit)?" ":ext_info_unit->value);
	
      } /*end for*/
    fprintf(sfile," ==========================================\n");   
    /*
     * write the column title
     */
    for (symbol_index=0;symbol_index<symbols->TSP_sample_symbol_info_list_t_len;++symbol_index) {
      fprintf(sfile,"%s	", *(tab_colonne + symbol_index));
      free(*(tab_colonne + symbol_index));
    }  /* For each requested symbol, we store it */
    free(tab_colonne);    
    fprintf(sfile,"\n");    
    fflush(sfile);    
    break;
  default:
    /* no header */
    break;
  }
 

  /*calculate the total data receive: simple + table*/
  for(indice=0;indice<symbols->TSP_sample_symbol_info_list_t_len;++indice)
  {
    nb_data_receive+=symbols->TSP_sample_symbol_info_list_t_val[indice].nelem;
  }

  /* Demarrage des sample au niveau provider */
  if(TSP_STATUS_OK!=TSP_consumer_request_sample_init(myprovider,0,0)) {
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
    while (TSP_STATUS_OK==TSP_consumer_read_sample(myprovider,&sample, &new_sample) && !stop_it) 
    {
      if (new_sample) 
      {
			if (tsp_ascii_writer_header_style==2) 
			{
				tsp_ascii_writer_display_value(sfile,sample);

				++complete_line;
				/* We write the endl if we receive a whole sample line */
				if (nb_data_receive==complete_line) 
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
			        tsp_ascii_writer_display_value(sfile,sample);

				++complete_line;
				/* We write the endl if we receive a whole sample line */
				if (nb_data_receive==complete_line) 
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
} /* tsp_ascii_writer_start */

void* 
tsp_ascii_writer_thread(void* sfile) {

  static int retcode;
  /* FIXME should get Validated SSIL instead of NULL */
  retcode = tsp_ascii_writer_start((FILE* )sfile,0,0,NULL);  
  return &retcode;
} /* end of tsp_ascii_writer_thread */


int32_t 
tsp_ascii_writer_stop() {
  
  int32_t retcode;
  retcode = 0;
  stop_it = 1;
  return retcode;
} /* end of tsp_ascii_writer_stop */


int32_t 
tsp_ascii_writer_finalise() {
  
  int32_t retcode;

  if (NULL != myprovider) {
    if (tsp_ascii_writer_sample_running) {
      if (TSP_STATUS_OK!=TSP_consumer_request_sample_destroy(myprovider)) {
	STRACE_ERROR(("TSP_request_sample_destroy en erreur??..."));
      }  
    }
  }
  TSP_consumer_end();
  retcode = 0;    
  return retcode;
} /* end of tsp_ascii_writer_finalise */

int32_t 
tsp_ascii_writer_display_value(FILE* sfile,TSP_sample_t sample)
{

 switch(sample.type) {

  case TSP_TYPE_DOUBLE :
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.double_value);
    break;
    
  case TSP_TYPE_FLOAT :
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.float_value);
    break;
    
  case TSP_TYPE_INT8 :
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.int8_value);
    break;
    
  case TSP_TYPE_INT16:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.int16_value);
    break;
    
  case TSP_TYPE_INT32 :
      fprintf(sfile,fmt_tab[sample.type],sample.uvalue.int32_value);
      break;
      
  case TSP_TYPE_INT64 :
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.int64_value);
    break;
    
  case TSP_TYPE_UINT8:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.uint8_value);
    break;
    
  case TSP_TYPE_UINT16:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.uint16_value);
    break;
    
  case TSP_TYPE_UINT32:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.uint32_value);
    break;
    
  case TSP_TYPE_UINT64:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.uint64_value);
    break;
    
  case TSP_TYPE_CHAR:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.char_value);
    break;
    
  case TSP_TYPE_UCHAR:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.uchar_value);
    break;
    
  case TSP_TYPE_RAW:
    fprintf(sfile,fmt_tab[sample.type],sample.uvalue.raw_value);
    break;
    
  default:
    return -1;
  }

  return 0;
}

char* new_array_label(const char* libelle,const char* profil, const char* ordre, const int recursif)
{

  uint8_t indice_etoile;
  uint8_t rang;
  uint32_t dimension;
  uint32_t i;
  char *chaine_lib;
  char *nouveau_libelle=NULL;
  char *nouveau_profil=NULL;
  char* reponse=NULL;
  char indice[10];


  if(!recursif)
  {
    chaine_lib=(char*)calloc(sizeof(char),strlen(libelle)+2);
    sprintf(chaine_lib,"%s(",libelle);
  }
  else
  {
    chaine_lib=(char*)calloc(1,strlen(libelle)+1);
    sprintf(chaine_lib,"%s",libelle);
  }


  rang=strspn(profil,"*")+1;

  if(1==rang)
  {
    dimension=(int32_t)atoi(profil);

    for(i=0;i<dimension;++i)
    {
      sprintf(indice,"%d",i+1); 

      if(NULL==reponse)
      {
	reponse=(char*)calloc(sizeof(char),strlen(chaine_lib)+strlen(indice)+3);
        sprintf(reponse,"%s%s)\t",chaine_lib,indice);
      }
      else
      {
	reponse=(char*)realloc(reponse,(strlen(reponse)+strlen(chaine_lib)+strlen(indice)+3)*sizeof(char));
	sprintf(&(reponse[strlen(reponse)]),"%s%s)\t",chaine_lib,indice);
      }

    }

  }
  else
  {
    indice_etoile=(int32_t)strstr(profil,"*");
   
    nouveau_profil=(char*)calloc(sizeof(char),strlen(&(profil[indice_etoile+1]))+1);
    strcpy(nouveau_profil,&(profil[indice_etoile+1]));

    dimension=(int32_t)atoi(profil);

    strcpy(profil,&profil[indice_etoile+1]);

    for(i=0;i<dimension;++i)
    {
      sprintf(indice,"%d",dimension+1); 

      nouveau_libelle=(char*)calloc(sizeof(char),strlen(chaine_lib)+strlen(indice)+2);
      
      sprintf(nouveau_libelle,"%s%s,",chaine_lib,indice);

      reponse= new_array_label(nouveau_libelle,nouveau_profil,ordre,1);

      free(nouveau_libelle);

    }

    free(nouveau_profil);


  }

  free(chaine_lib);  

  return(reponse);


}
