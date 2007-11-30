/*

$Id: generic_consumer.c,v 1.18 2007-11-30 15:42:02 erk Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Generic tsp consumer

-----------------------------------------------------------------------
*/
#include <stdlib.h>
#ifdef _WIN32
    #define assert(exp)     ((void)0)
#else
    #include <unistd.h>
    #include <assert.h>
#include <libgen.h>
#endif
#include <string.h>
#include <stdarg.h>

#include <tsp_sys_headers.h>
#include <tsp_prjcfg.h>
#include <generic_consumer.h>
#include <tsp_consumer.h>
#include <tsp_common.h>
#include <tsp_encoder.h>


void 
generic_consumer_logMsg(FILE* stream, char* fmt, ...) {
  va_list args;
  char    message[2048];

  memset(message,0,2048);
  va_start(args, fmt);
  fprintf(stream,"%s::",tsp_reqname_tab[E_TSP_REQUEST_GENERIC]);
  vfprintf(stream,fmt,args);
  va_end(args);

} /* end of generic_consumer_logMsg */


#define GENERIC_CONSUMER_MAX_BLANK 80

char* generic_consumer_fillspace(int size, const char* str) {
  static char myspace[GENERIC_CONSUMER_MAX_BLANK];
  int i;
  for (i=0;i<size-strlen(str);++i) {
    myspace[i] = ' ';    
  }
  myspace[i] = '\0';
  return myspace;
} /* end of generic_consumer_fillspace */

void
generic_consumer_request_create(generic_consumer_request_t* req) {
  req->verbose       = 0;
  req->silent        = 0;
  req->help          = 0;
  req->argc          = 0;
  req->argv          = NULL;
  req->nb_global_opt = 0;
  TSP_request_create(&(req->request),E_TSP_REQUEST_INVALID);
  req->stream        = stdout;
  req->provider_url  = "localhost";
  req->the_provider  = NULL;
  req->newline[1]    = '\0';
  req->newline[0]    = '\n';
}  /* end of generic_consumer_request_create */

TSP_request_type_t
generic_consumer_cmd(const char* generic_consumer_request_string) {

  TSP_request_type_t retval;
  int32_t            i;
  char*              cmdbasename;
  char*              cmdstr_cpy;
  char*              cmdstr_abbrev;

  cmdstr_cpy = strdup(generic_consumer_request_string);

  cmdbasename = basename(cmdstr_cpy);

  retval = E_TSP_REQUEST_INVALID;
  
  /* complete name match */
  for (i=E_TSP_REQUEST_GENERIC;i<E_TSP_REQUEST_LAST;++i) {
    if (!strncmp(cmdbasename,tsp_reqname_tab[i],strlen(tsp_reqname_tab[i]))) {
      retval = i;
      break;
    }
  }
  
  /* abbreviate name match */
  /* FIXME should implement partial match */
  if (E_TSP_REQUEST_INVALID == retval) {
    for (i=E_TSP_REQUEST_GENERIC;i<E_TSP_REQUEST_LAST;++i) {
      cmdstr_abbrev = strstr(tsp_reqname_tab[i],"_")+1;
      if (!strncmp(cmdbasename,cmdstr_abbrev,strlen(cmdstr_abbrev))) {
	retval = i;
	break;
      }
    }
  }

  free(cmdstr_cpy);
  return retval;
} /* end of generic_consumer_cmd */

TSP_request_type_t
generic_consumer_checkargs(generic_consumer_request_t* req) {
  TSP_request_type_t retval = E_TSP_REQUEST_INVALID;

  /* 
   * Check if we called us with non generic
   * tsp_request name
   */
  retval = generic_consumer_cmd(req->argv[0]);

  /* 
   * If we were call with the generic name
   * compute generic consumer command and shift args in the request.
   */
  if ((E_TSP_REQUEST_GENERIC == retval) && (req->argc - req->nb_global_opt)>1) {
    retval = generic_consumer_cmd(req->argv[1+req->nb_global_opt]);
    /* shift argv */
    req->argv = &(req->argv[1+req->nb_global_opt]);    
    /* shift argc since we are in the tsp consumer generic call case */
    req->argc -= 1 + req->nb_global_opt;
  }  else {
    /* shift argv */
    req->argv = &(req->argv[0+req->nb_global_opt]);    
    /* shift argc since we are in the bb_tools generic call case */
    req->argc -= 0 + req->nb_global_opt;
  }

  return retval;
} /* end of generic_consumer_checkargs */

int32_t
generic_consumer(generic_consumer_request_t* req) {

  int32_t retval = 0;

  /* Get real generic_consumer request */
  req->request.req_type = generic_consumer_checkargs(req);

  if (req->verbose) {
    generic_consumer_logMsg(req->stream,
			    "%s\n",
			    tsp_reqname_tab[req->request.req_type]);
  }

  /*
   * If NOT in those cases we need to TSP request open 
   */
  if (!((E_TSP_REQUEST_INVALID==req->request.req_type))) {
    req->the_provider = TSP_consumer_connect_url(req->provider_url);
    if (NULL == req->the_provider) {
      if (!req->silent) {
	generic_consumer_logMsg(req->stream,
				"Unable to find any provivder using URL <%s>\n",
				NULL == req->provider_url ? "NULL" : req->provider_url );
      }
      return -1;
    } else {
      if (TSP_STATUS_OK==TSP_consumer_request_open(req->the_provider, 0, 0)) {
	fprintf(req->stream,
		"Request Open successfully sent to : <%s>\n",
		TSP_consumer_get_connected_name(req->the_provider));
	fprintf(req->stream,
		"Obtained channel Id : <%d>\n",
		TSP_consumer_get_channel_id(req->the_provider));
      } else {
	generic_consumer_logMsg(req->stream,
				"Request Open FAILED.\n");
	return -1;
      }
    }
  }

  switch (req->request.req_type) {
  case E_TSP_REQUEST_INVALID:
    req->stream = stderr;
    generic_consumer_usage(req);
    return -1;
    break;  
  case E_TSP_REQUEST_GENERIC:
    req->stream = stdout;
    generic_consumer_usage(req);
    break;
  case E_TSP_REQUEST_OPEN:    
    retval=generic_consumer_open(req);
    break;
  case E_TSP_REQUEST_CLOSE:
    retval=generic_consumer_close(req);
    break;
  case E_TSP_REQUEST_INFORMATION:
    retval=generic_consumer_information(req);
    break;
  case E_TSP_REQUEST_FILTERED_INFORMATION:
    retval=generic_consumer_filtered_information(req);
    break;  
  case E_TSP_REQUEST_ASYNC_SAMPLE_READ:    
    retval=generic_consumer_async_read(req);
    break;
  case E_TSP_REQUEST_ASYNC_SAMPLE_WRITE:
    retval=generic_consumer_async_write(req);
    break;
  case E_TSP_REQUEST_EXTENDED_INFORMATION:
    retval=generic_consumer_extended_information(req);
    break; 
  default:
    req->stream               = stderr;
    req->request.req_type    = E_TSP_REQUEST_INVALID;
    generic_consumer_usage(req);
    return -1;
    break;
  }
  
  /* Be nice close the session */
  if (NULL != req->the_provider) {
    
    if (TSP_STATUS_OK==TSP_consumer_request_close(req->the_provider)) {
      fprintf(req->stream,
	      "Request Close successfully sent to <%s>\n",
	      TSP_consumer_get_connected_name(req->the_provider));
    } else {
      generic_consumer_logMsg(req->stream,
			      "Request Close FAILED.\n");
      return -1;
    }
  }
  
  return retval;
} /* end of generic_consumer */


void 
generic_consumer_usage(generic_consumer_request_t* req) {

  int32_t i;

  switch (req->request.req_type) {
  case E_TSP_REQUEST_INVALID:
    fprintf(req->stream, 
	    "%s::unknown %s request\n",
	    tsp_reqname_tab[E_TSP_REQUEST_GENERIC],
	    tsp_reqname_tab[E_TSP_REQUEST_GENERIC]);	    
    /* no break */
  case E_TSP_REQUEST_GENERIC:
    fprintf(req->stream, 
	    "TSP generic consumer v%s (%s)\n",TSP_SOURCE_VERSION,TSP_PROJECT_URL);
    fprintf(req->stream,
	    "TSP XDR encoding/decoding method is: <%s>\n",
	    TSP_data_channel_get_encoder_method());
    fprintf(req->stream, 
	    "Usage: %s [generic_opts] <tsp_request> [request_opts]\n",
	    tsp_reqname_tab[E_TSP_REQUEST_GENERIC]);
    fprintf(req->stream,"   generic_opts:\n");
    fprintf(req->stream,"    -u TSP provider URL (defaulted to localhost) \n");
    fprintf(req->stream,"    -s silent mode (may be used for silent scripting)\n");
    fprintf(req->stream,"    -v verbose mode\n");
    fprintf(req->stream,"    -n no newline read mode\n");
    fprintf(req->stream,"   supported <tsp_request> are: \n");
    for (i=E_TSP_REQUEST_GENERIC+1;i<E_TSP_REQUEST_LAST;++i) {
      fprintf(req->stream,
	      "    %s%s: %s\n",
	      tsp_reqname_tab[i],
	      generic_consumer_fillspace(40,tsp_reqname_tab[i]),
	      tsp_reqhelp_tab[i]
	      );
    }
    break;
  case E_TSP_REQUEST_INFORMATION:    
    fprintf(req->stream,
	    "Usage: %s\n",
	    tsp_reqname_tab[req->request.req_type]);    	    
    break;    
  case E_TSP_REQUEST_FILTERED_INFORMATION:
    fprintf(req->stream,
	    "Usage: %s <filter_kind> <filter_string>\n",
	    tsp_reqname_tab[req->request.req_type]);    	    
    break;    
  case E_TSP_REQUEST_ASYNC_SAMPLE_READ:
    fprintf(req->stream,"Usage : %s <symbol_pgi>\n",
	    tsp_reqname_tab[req->request.req_type]);    	    
    break;
  case E_TSP_REQUEST_ASYNC_SAMPLE_WRITE:
    fprintf(req->stream,"Usage : %s <symbol_pgi>\n",
	    tsp_reqname_tab[req->request.req_type]);    	    
    break; 
  case E_TSP_REQUEST_EXTENDED_INFORMATION:
    fprintf(req->stream,"Usage : %s <symbol_pgi>\n",
	    tsp_reqname_tab[req->request.req_type]);    	    
    break;
  default:
    fprintf(req->stream, 
	    "default: should never be reached?\n");
  }

  if ((req->request.req_type>=E_TSP_REQUEST_INVALID) &&
      (req->request.req_type<E_TSP_REQUEST_LAST)) {
    fprintf(req->stream, 
	    "%s::%s\n",
	    tsp_reqname_tab[req->request.req_type],
	    tsp_reqhelp_tab[req->request.req_type]);
  }

} /* end of generic_consumer_usage */

int32_t 
generic_consumer_unimplemented_cmd(generic_consumer_request_t* req) {

  const char* cmdname = tsp_reqname_tab[req->request.req_type];
  generic_consumer_logMsg(stderr,"<%s> not implemented!\n",
			  cmdname);

  return -1;
} /* end of bbtools_unimplemented_cmd */


int32_t 
generic_consumer_open(generic_consumer_request_t* req) {
  int32_t retval  = 0;
  fprintf(req->stream,"Nothing more to do for <%s>...\n",
	  tsp_reqname_tab[req->request.req_type]);
  return retval;  
}

int32_t 
generic_consumer_close(generic_consumer_request_t* req) {
  int32_t retval  = 0;
  fprintf(req->stream,"Nothing more to do for <%s>...\n",
	  tsp_reqname_tab[req->request.req_type]);
  return retval;  
}

void generic_consumer_printinfo(generic_consumer_request_t* req) {
  const TSP_answer_sample_t* pinfo;
  int32_t i;
  pinfo = TSP_consumer_get_information(req->the_provider);
  fprintf(req->stream,"Provider::base frequency      = %f\n",pinfo->base_frequency);
  fprintf(req->stream,"Provider::max period          = %d\n",pinfo->max_period);
  fprintf(req->stream,"Provider::max consumer        = %d\n",pinfo->max_client_number);
  fprintf(req->stream,"Provider::current consumer nb = %d\n",pinfo->current_client_number);
  fprintf(req->stream,"Provider <symbols list begin>\n");
  for (i=0;i<pinfo->symbols.TSP_sample_symbol_info_list_t_len;i++) {
    fprintf(req->stream,"    pgi = %08d, %s, type = %s, dim = %d\n",
	    pinfo->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
	    pinfo->symbols.TSP_sample_symbol_info_list_t_val[i].name,
	    tsp_type_name[pinfo->symbols.TSP_sample_symbol_info_list_t_val[i].type],
	    pinfo->symbols.TSP_sample_symbol_info_list_t_val[i].dimension);
  }
  fprintf(req->stream,"Provider <symbols list end>.\n");
} /* end of generic_consumer_printinfo */

void generic_consumer_printextendedinfo(generic_consumer_request_t* req) {
  const TSP_sample_symbol_extended_info_list_t* pexinfo;
  int32_t i,j;
  pexinfo = TSP_consumer_get_extended_information(req->the_provider);
  fprintf(req->stream,"Extented info list <begins>\n");
  for (j=0;j<pexinfo->TSP_sample_symbol_extended_info_list_t_len;++j) {
    fprintf(req->stream,"    Extended info for PGI <%d> <begins>\n",
	    pexinfo->TSP_sample_symbol_extended_info_list_t_val[j].provider_global_index);
    for (i=0;i<pexinfo->TSP_sample_symbol_extended_info_list_t_val[j].info.TSP_extended_info_list_t_len;i++) {

      fprintf(req->stream,"        key=%s, value=%s\n",
	      pexinfo->TSP_sample_symbol_extended_info_list_t_val[j].info.TSP_extended_info_list_t_val[i].key,
	      pexinfo->TSP_sample_symbol_extended_info_list_t_val[j].info.TSP_extended_info_list_t_val[i].value);
    }
    fprintf(req->stream,"    Extended info for PGI <%d> <ends>\n",pexinfo->TSP_sample_symbol_extended_info_list_t_val[j].provider_global_index);
  }
  fprintf(req->stream,"Provider <symbols list end>.\n");
} /* end of generic_consumer_printextendedinfo */

int32_t 
generic_consumer_information(generic_consumer_request_t* req) {
  int32_t retval  = -1;
  

  if (req->argc<1) {
    generic_consumer_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   tsp_reqname_tab[E_TSP_REQUEST_INFORMATION],
		   1-req->argc);
    generic_consumer_usage(req);
    retval = -1;
    return retval;
  }
  if (TSP_STATUS_OK!=TSP_consumer_request_information(req->the_provider)) {
    generic_consumer_logMsg(req->stream,"%s: TSP request failed\n",
			    tsp_reqname_tab[E_TSP_REQUEST_INFORMATION]);
  } else {
    generic_consumer_printinfo(req);
    retval = 0;
  }
  return retval;  
}

int32_t 
generic_consumer_filtered_information(generic_consumer_request_t* req) {
  int32_t retval  = -1;
  if (req->argc<3) {
    generic_consumer_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   tsp_reqname_tab[E_TSP_REQUEST_FILTERED_INFORMATION],
		   3-req->argc);
    generic_consumer_usage(req);
    retval = -1;
    return retval;
  }

  if (TSP_STATUS_OK!=TSP_consumer_request_filtered_information(req->the_provider,TSP_FILTER_SIMPLE,req->argv[2])) {
    generic_consumer_logMsg(req->stream,"%s: TSP request failed\n",
			    tsp_reqname_tab[E_TSP_REQUEST_FILTERED_INFORMATION]);
			    
  } else {
    generic_consumer_printinfo(req);
    retval = 0;
  }
  return retval;  
}

int32_t 
generic_consumer_async_read(generic_consumer_request_t* req) {
  int32_t retval  = 0;
  TSP_consumer_async_sample_t async_sample;
  double value;
  
  if (req->argc<2) {
    generic_consumer_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_READ],
		   2-req->argc);
    generic_consumer_usage(req);
    retval = -1;
    return retval;
  }

  async_sample.provider_global_index = atoi(req->argv[1]);
  async_sample.value_ptr  = &value;
  async_sample.value_size = sizeof(value);
  retval = TSP_consumer_request_async_sample_read(req->the_provider,&async_sample);
 
  if ((TSP_STATUS_OK != retval) && (!(req->silent))) {
    generic_consumer_logMsg(req->stream,
			    "%s: async read refused (or not handled) by provider\n",
			    tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_READ]);
  } else {

    fprintf(req->stream,"%f",*((double*)async_sample.value_ptr));
    fprintf(req->stream,"%s",req->newline);
    if (req->verbose) {
      generic_consumer_logMsg(req->stream,
			      "%s: Trying to async read symbol <pgi=%d>, value read is <%f> on provider <%s>...\n",
			      tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_READ],
			      async_sample.provider_global_index,
			      *((double*)async_sample.value_ptr),
			      req->provider_url);
    }	       
  }

  return retval;

} /* end of generic_consumer_async_read */

int32_t 
generic_consumer_async_write(generic_consumer_request_t* req) {
  int32_t retval = 0;
  TSP_consumer_async_sample_t async_sample;
  double value;
      
  if (req->argc<3) {
    generic_consumer_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
			    tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_WRITE],
			    3-req->argc);
    generic_consumer_usage(req);
    retval = -1;
    return retval;
  }

  async_sample.provider_global_index = atoi(req->argv[1]);

  value                   = atof(req->argv[2]);
  async_sample.value_ptr  = &value;
  async_sample.value_size = sizeof(value);
  
  if (req->verbose) {
    generic_consumer_logMsg(req->stream,
			    "%s: Trying to async write symbol <pgi=%d> with value <%s> on provider <%s>...\n",
			    tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_WRITE],
			    async_sample.provider_global_index,
			    req->argv[2],
			    req->provider_url);
  }		       

  retval = TSP_consumer_request_async_sample_write(req->the_provider,&async_sample);
  if ((TSP_STATUS_OK != retval) && (!(req->silent))) {
    generic_consumer_logMsg(req->stream,
			    "%s: async write refused (or not handled) by provider\n",
			    tsp_reqname_tab[E_TSP_REQUEST_ASYNC_SAMPLE_WRITE]);
  }
  return retval;
}  /* end of generic_consumer_async_write */

int32_t 
generic_consumer_extended_information(generic_consumer_request_t* req) {
  int32_t  retval   = -1;
  int32_t* pgis     = NULL; 
  int32_t  pgis_len = 1;
  
  if (req->argc<2) {
    generic_consumer_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   tsp_reqname_tab[E_TSP_REQUEST_EXTENDED_INFORMATION],
		   2-req->argc);
    generic_consumer_usage(req);
    retval = -1;
    return retval;
  }

  /* FIXME handle multiple PGIS */
  pgis_len = 1;
  pgis = (int32_t*)malloc(pgis_len*sizeof(int32_t));
  assert(pgis);
  pgis[0] = atoi(req->argv[1]);

  if (TSP_STATUS_OK!=TSP_consumer_request_extended_information(req->the_provider,pgis,pgis_len)) {
    generic_consumer_logMsg(req->stream,"%s: TSP request failed\n",
			    tsp_reqname_tab[E_TSP_REQUEST_EXTENDED_INFORMATION]);
  } else {    
    generic_consumer_printextendedinfo(req);
    retval = 0;
  }
  return retval;  
}
