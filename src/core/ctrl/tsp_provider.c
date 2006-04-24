/*

$Id: tsp_provider.c,v 1.49 2006-04-24 19:53:32 erk Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Provider

-----------------------------------------------------------------------

Purpose   : Main implementation for the producer module

-----------------------------------------------------------------------
 */

#include <tsp_sys_headers.h>
#include <tsp_provider.h>
#include <tsp_filter_symbol.h>
#include <tsp_session.h>
#include <tsp_glu.h>	
#include <tsp_time.h>
#include <tsp_common.h>
#include <tsp_datapool.h>

/** modified argc and argv, will be returned to user code after init */
static  char** X_argv = 0;

/** Default values for args to the GLU */
static  char** X_glu_argv = 0;
static  int X_glu_argc = 0;

static  GLU_handle_t* firstGLU = NULL;

/** server base number is an offset allowing to distinguish TSP providers **/
/** each Request handler shall look for a channel number from this offset,
    and limited to TSP_MAX_SERVER_NUMBER **/
static int X_server_base_number = 0;


static int X_tsp_provider_init_ok = FALSE;

/**
 * The mutex used to serialize 
 * the asynchronous request that may be issued by different 
 * TSP asynchronous command link (RPC, XML-RPC, CORBA...)
 * Note that normally we may use several mutexes
 * in order to serialize more efficiently the asynchronous request.
 * That is one for tsp_request_open and 1 for each client after that.
 */
static pthread_mutex_t X_tsp_request_mutex = PTHREAD_MUTEX_INITIALIZER;

/* polling time for session garbage collector */
#define TSP_GARBAGE_COLLECTOR_POLL_TIME_US ((int)(5e6))

/**
 * Check TSP protocol version
 * and TSP Channel Id are avlid or not.
 * @param[in] version_id the requested TSP Version
 * @param[in] channel_id the requested TSP channel id
 * @param[in,out] glu pointer to GLU handle pointer, non NULL on entry
 *                contain pointer to GLU handle attached to the requested
 *                session on return if the channel id is valid.
 * @return TSP_STATUS_OK on success, TSP_STATUS_ERROR_VERSION if version does not match
 *         TSP_STATUS_ERROR_INVALID_CHANNEL_ID if channel id is invalid.
 */
static int32_t 
TSP_provider_checkVersionAndChannelId(int32_t version_id, int32_t channel_id,
				      GLU_handle_t** glu) {
  int32_t retval = TSP_STATUS_OK;

  if (version_id != TSP_PROTOCOL_VERSION) {
    STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",version_id, TSP_PROTOCOL_VERSION ));    
    retval = TSP_STATUS_ERROR_VERSION;
  }

  if ((TSP_STATUS_OK==retval) && (NULL!=glu)) {
    *glu = TSP_session_get_GLU_by_channel(channel_id);
    if (NULL==*glu) {
      STRACE_ERROR(("TSP channel id ERROR. Trying to use Channel Id=%d ",channel_id));
      retval=TSP_STATUS_ERROR_INVALID_CHANNEL_ID;
    }
  }

  return retval;
} /* end of TSP_provider_check_versionAndId */

static int 
TSP_cmd_line_parser(int* argc, char** argv[]) {
  int i;
  int final_argc = 1;
  int found_stream_start = FALSE;
  int found_stream_stop = FALSE;
  int found_server_number_flag = FALSE;
  char* p;
  int ret = TRUE;

  /* FIXME : FUITE */
  X_argv = (char**)calloc(*argc, sizeof(char*));
  X_glu_argv = (char**)calloc(*argc, sizeof(char*));
  X_glu_argc = 0;
  TSP_CHECK_ALLOC(X_argv, FALSE);
  TSP_CHECK_ALLOC(X_glu_argv, FALSE);
  /* Get program name anyway */
  X_argv[0] = (*argv)[0];
  for( i = 1 ; i < *argc && ret ; i++)
    {
      /* Is the arg a TSP arg ? */
      p = strstr( (*argv)[i], TSP_ARG_PREFIX );
      if(p && (p == (*argv)[i] ))
	{
	  /* TSP Arg */
	  STRACE_INFO(("Tsp ARG : '%s'", (*argv)[i]));

	  /* First we must not be looking for a argument */
	  if(found_server_number_flag)
	    {
	      /* Error we are expecting an arguement not a TSP option*/
	      STRACE_WARNING(("Unexpected %s", (*argv)[i]));
	      ret = FALSE;
	    }
	  /* Look for start flag */
	  else if(!strcmp(TSP_ARG_STREAM_INIT_START, (*argv)[i]))
	    {
	      if(!found_stream_stop && !found_stream_start)
		{
		  found_stream_start = TRUE;
		  /* Ok the user wants a default stream control, put
		     the first dummy element */
		  if ( 0 == X_glu_argc )
		    {
		      X_glu_argv[0] = TSP_ARG_DUMMY_PROG_NAME;
		      X_glu_argc = 1;
		    }
		}
	      else
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_STREAM_INIT_START));
		  ret = FALSE;
		}
	    }
	  else if (!strcmp(TSP_ARG_STREAM_INIT_STOP, (*argv)[i]))
	    {
	      if(found_stream_start && !found_stream_stop)		
		{
		  found_stream_stop = TRUE;
		}
	      else
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_STREAM_INIT_STOP));
		  ret = FALSE;
		}
	    }
	  else if (!strcmp(TSP_ARG_SERVER_NUMBER, (*argv)[i]))
	    {
	      found_server_number_flag = TRUE;
	      if(found_stream_start && !found_stream_stop)		
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_SERVER_NUMBER));
		  ret = FALSE;
		}
	    }
	  else
	    {
	      /* Unkown option */
	      STRACE_WARNING(("Unknown TSP option : '%s'",(*argv)[i] ))
	      ret = FALSE;
	    }
	}
      else /* Not a TSP arg */
	{
	  /* Are we in the TSP command line ? */
	  if ( found_stream_start && !found_stream_stop )
	    {
	      X_glu_argv[X_glu_argc++] = (*argv)[i];
	    }
	  else if(found_server_number_flag)
	    {
	      /* Found arg for server number option */
	      found_server_number_flag = FALSE;
	      X_server_base_number = atoi((*argv)[i]);
	      STRACE_INFO(("Server base number = %d", X_server_base_number));
	    }
	  else
	    {	      
	      /* Nop, this arg is for the user */
	      X_argv[final_argc] = (*argv)[i];
	      final_argc++;
	    }
	}
    } /* for */
  
  /* Check is the stop was found */
  
  if( found_stream_start && !found_stream_stop )
    {
      STRACE_WARNING(("A " TSP_ARG_STREAM_INIT_STOP " flag was expected"));
      ret = FALSE;
    }

  if( found_server_number_flag )
    {
      STRACE_WARNING(("An argument was expected after " TSP_ARG_SERVER_NUMBER));
      ret = FALSE;
    }

  /* swap argc and argv values */
  *argc = final_argc;
  *argv = X_argv;
  
  /* Display usage */
  if(!ret)
    {
      STRACE_WARNING((TSP_ARG_PROVIDER_USAGE));
    }
  else
    {
      if (! X_glu_argc )
	{
	  STRACE_INFO(("No GLU stream init provided on command line"));
	}
    }

  return ret;
}

int 
TSP_provider_is_initialized(void) {
  return  X_tsp_provider_init_ok;
}

int TSP_provider_get_server_base_number(void)
{
  return  X_server_base_number;
}

const char* 
TSP_provider_get_name() {
  assert(firstGLU);
  return firstGLU->get_name(firstGLU);
}


void 
TSP_provider_request_open(const TSP_request_open_t* req_open,
			  TSP_answer_open_t* ans_open) {
  GLU_handle_t* glu_h;
  char* error_info;
  int i;
	
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);

  ans_open->version_id = TSP_UNDEFINED_VERSION_ID;
  ans_open->channel_id = TSP_UNDEFINED_CHANNEL_ID;
  ans_open->status     = TSP_STATUS_ERROR_UNKNOWN;
  ans_open->status_str = "";

  if (req_open->argv.TSP_argv_t_len) {
    for( i = 0; i< req_open->argv.TSP_argv_t_len ;i++) {
      STRACE_DEBUG(("arg %d is '%s'", i,  req_open->argv.TSP_argv_t_val[i]));
    }
  }
  else {
    STRACE_DEBUG(("No custom args from consumer"));
  }

  /*  
   * Get a GLU instance for this session. 
   * The GLU instance is obtained through the firstGLU instance
   * which was provided by the TSP_provider_init.
   * Most ACTIVE GLU do only have one instance,
   * Most PASSIVE GLU do have one instance by TSP session
   * but it is up to the GLU implementor to chose what
   * to give in get_instance.
   * If a stream init is provided, use it, else, use default 
   */   
  if( 0 != req_open->argv.TSP_argv_t_len ) {
       glu_h = firstGLU->get_instance(firstGLU, 
				      req_open->argv.TSP_argv_t_len, 
				      req_open->argv.TSP_argv_t_val, 
				      &error_info);
  }
  else {
    /* use fallback if provided */
    glu_h = firstGLU->get_instance(firstGLU, 
				   X_glu_argc, 
				   X_glu_argv, 
				   &error_info);
  }
   
  if (NULL != glu_h) {
    ans_open->status = TSP_add_session(&(ans_open->channel_id), glu_h);
    if (TSP_STATUS_OK==ans_open->status) {
      if (req_open->version_id == TSP_PROTOCOL_VERSION) {
	ans_open->version_id  = TSP_PROTOCOL_VERSION;
	ans_open->status      = TSP_STATUS_OK;
      }
      else {
	STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",req_open->version_id, TSP_PROTOCOL_VERSION ));
	ans_open->status = TSP_STATUS_ERROR_VERSION;
      }
    }
    else {
      STRACE_ERROR(("TSP_add_session failed"));
    }
  }
  else {
    STRACE_INFO(("Unable to get first GLU instance"));
    ans_open->status = TSP_STATUS_ERROR_SEE_STRING;
    ans_open->status_str = error_info;
  }  
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);  
} /* End of TSP_provider_request_open */


static void 
TSP_provider_request_close_priv(channel_id_t channel_id) {
  TSP_session_destroy_symbols_table_by_channel(channel_id);
  TSP_session_close_session_by_channel(channel_id);
}

void 
TSP_provider_request_close(const TSP_request_close_t* req_close) {
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);
  
  if (TSP_STATUS_OK==TSP_provider_checkVersionAndChannelId(req_close->version_id,
							   req_close->channel_id,
							   NULL)) {
    TSP_provider_request_close_priv(req_close->channel_id);
  }
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_close */

/* 
 * Request info is implemented as a forwarder to filtered info 
 * with filter_kind=TSP_FILTER_NONE
 * so NO MUTEX taken here.
 */
void  
TSP_provider_request_information(TSP_request_information_t* req_info, 
				 TSP_answer_sample_t* ans_sample) {
  TSP_provider_request_filtered_information(req_info,TSP_FILTER_NONE,NULL,ans_sample);  
  STRACE_DEBUG(("Nb symbol = %d",ans_sample->symbols.TSP_sample_symbol_info_list_t_len));
} /* End of TSP_provider_request_information */

void TSP_provider_update_answer_with_minimalinfo(int32_t version_id, int32_t channel_id,
						 TSP_answer_sample_t* ans_sample,
						 GLU_handle_t** glu) {

  ans_sample->status = TSP_provider_checkVersionAndChannelId(version_id,
							     channel_id,
							     glu);

  if (TSP_STATUS_OK==ans_sample->status) {
    ans_sample->version_id            = version_id;
    ans_sample->channel_id            = channel_id;    
    ans_sample->provider_group_number = 0;
    ans_sample->base_frequency        = (*glu)->get_base_frequency(*glu);
    ans_sample->max_client_number     = (*glu)->get_nb_max_consumer(*glu);
    ans_sample->current_client_number = TSP_session_get_nb_session();
    ans_sample->max_period            = TSP_MAX_PERIOD;

    ans_sample->symbols.TSP_sample_symbol_info_list_t_len = 0;
    ans_sample->symbols.TSP_sample_symbol_info_list_t_val = NULL;  
  }  
} /* end of TSP_provider_update_answer_with_minimalinfo */

void  
TSP_provider_request_filtered_information(TSP_request_information_t* req_info, 
					  int filter_kind, char* filter_string,
					  TSP_answer_sample_t* ans_sample) {

  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);  
  GLU_handle_t* myGLU = NULL;
  
  /* fill-in minimal info in answer_sample */
  TSP_provider_update_answer_with_minimalinfo(req_info->version_id,req_info->channel_id,
					      ans_sample,&myGLU);

  /* Proceed processing iff status is OK */
  if (TSP_STATUS_OK == ans_sample->status) {
    /* switch case for filter_kind */
    switch (filter_kind) {
    case TSP_FILTER_NONE:
      STRACE_INFO(("Requested filter NONE"));
      TSP_filter_symbol_none(req_info,filter_string,ans_sample);
      break;
    case TSP_FILTER_MINIMAL:
      STRACE_INFO(("Requested filter MINIMAL, filter string = <%s>",filter_string));
      TSP_filter_symbol_minimal(req_info,filter_string,ans_sample);
      break;
    default:
      STRACE_INFO(("Requested filter kind <%d>, filter string = <%s>",filter_kind,filter_string));
      /* 
       * Forward other filtered request directly to GLU 
       * such that even non anticipated filtering method could
       * be implemented by specialized consumer and provider pair
       * default GLU will provider reasonnable default implementation.
       */    
      myGLU->get_filtered_ssi_list(myGLU,filter_kind,filter_string,ans_sample);
      break;
    } /* end switch filter_kind */
  } /* if TSP_STATUS_OK */
    
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_filtered_information */


void 
TSP_provider_request_sample_free_call(TSP_answer_sample_t* ans_sample) {
  TSP_session_create_symbols_table_by_channel_free_call(ans_sample);
}

void  
TSP_provider_request_sample(TSP_request_sample_t* req_sample, 
			    TSP_answer_sample_t* ans_sample) {

  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);	
  GLU_handle_t* myGLU;

  TSP_provider_update_answer_with_minimalinfo(req_sample->version_id,
					      req_sample->channel_id,
					      ans_sample,
					      &myGLU);
  
  STRACE_INFO(("Consumer No %d asked for %d symbols",req_sample->channel_id,req_sample->symbols.TSP_sample_symbol_info_list_t_len  ));
  
  if (TSP_STATUS_OK == ans_sample->status) {
    if(TSP_session_get_symbols_global_index_by_channel(req_sample->channel_id, &(req_sample->symbols) )) {  
      /* The datapool will be created here (if it does not already exist) */
      ans_sample->status =
	TSP_session_create_symbols_table_by_channel(req_sample, ans_sample);
      if (TSP_STATUS_OK!= ans_sample->status) {
	STRACE_ERROR(("Function TSP_session_create_symbols_table_by_channel failed"));
      }    
      /* 
       * DO NOT start PASSIVE GLU here
       * Start It only upon request_sample_init request.
       */
    } else {
      STRACE_WARNING(("Function TSP_session_get_symbols_global_index_by_channel failed"));
      ans_sample->status = TSP_STATUS_ERROR_SYMBOLS;
      /* 
       * Now we shall update answer_sample->symbols in order to indicates
       * to consumer side which symbols are marked as 'unknown'
       */
      TSP_SSIList_copy(&(ans_sample->symbols), 
		       req_sample->symbols);
    }
  } else {
    STRACE_WARNING(("TSP ERROR"));
  }

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_sample */

void  
TSP_provider_request_sample_init(TSP_request_sample_init_t* req_sample_init, 
				 TSP_answer_sample_init_t* ans_sample_init) {  
  GLU_handle_t* myGLU;
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);
    
  ans_sample_init->status = TSP_provider_checkVersionAndChannelId(req_sample_init->version_id,
								  req_sample_init->channel_id,
								  &myGLU);
  if(TSP_STATUS_OK==ans_sample_init->status) {
    ans_sample_init->version_id = req_sample_init->version_id;
    ans_sample_init->channel_id = req_sample_init->channel_id;

    ans_sample_init->status =
      TSP_session_create_data_sender_by_channel(req_sample_init->channel_id);
    
    if (TSP_STATUS_OK!=ans_sample_init->status) {
      STRACE_ERROR(("TSP_data_sender_create failed"));
    } else {      
      /* send data address to client */
      ans_sample_init->data_address =
	(char*)TSP_session_get_data_address_string_by_channel(req_sample_init->channel_id);  
      STRACE_DEBUG(("ANSWER SAMPLE INIT data_address = '%s'", ans_sample_init->data_address));
    }
  }
  
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_sample_init */

static 
void TSP_provider_request_sample_destroy_priv(channel_id_t channel_id) {
  if(TSP_STATUS_OK!=TSP_session_destroy_data_sender_by_channel(channel_id)) {
    STRACE_ERROR(("TSP_session_destroy_data_sender_by_channel failed"));
  }
}

void  
TSP_provider_request_sample_destroy(TSP_request_sample_destroy_t* req_sample_destroy, 
				    TSP_answer_sample_destroy_t*  ans_sample_destroy) {

  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);

  ans_sample_destroy->status = TSP_provider_checkVersionAndChannelId(req_sample_destroy->version_id,
								     req_sample_destroy->channel_id,
								     NULL);
  if(TSP_STATUS_OK==ans_sample_destroy->status) {
    ans_sample_destroy->version_id = req_sample_destroy->version_id;
    ans_sample_destroy->channel_id = req_sample_destroy->channel_id;
    TSP_provider_request_sample_destroy_priv(req_sample_destroy->channel_id);
  }

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of  TSP_provider_request_sample_destroy */


void* 
TSP_provider_garbage_collector_thread(void* dummy) {
   channel_id_t channel_id;

   /* Save memory ! */
   pthread_detach(pthread_self());

   while(TRUE) {
     while(TSP_session_get_garbage_session(&channel_id)) {
       /* Do what some rude consumer should have done itself */
       TSP_provider_request_sample_destroy_priv(channel_id);
       TSP_provider_request_close_priv(channel_id);
       STRACE_INFO(("Session No %d 'garbage-collected'", channel_id));
     }
     tsp_usleep(TSP_GARBAGE_COLLECTOR_POLL_TIME_US);
   }
   
   /* never reached */
   return (void*)NULL;
} /* end of TSP_provider_garbage_collector_thread */

int 
TSP_provider_private_init(GLU_handle_t* theGLU, int* argc, char** argv[]) {
  int ret = TRUE;
  int status;
  pthread_t thread;
  assert(argc);
  assert(argv);
  assert(theGLU);

  firstGLU = theGLU;
  
  ret = TSP_cmd_line_parser(argc, argv);
  if (ret) {
    /* init sessions */
    TSP_session_init();
    
    /* Initialise GLU server */
    ret = theGLU->initialize(theGLU, X_glu_argc, X_glu_argv);
    
    /* Launch garbage collection for sessions */
    status = pthread_create(&thread, NULL, TSP_provider_garbage_collector_thread,  NULL);
    TSP_CHECK_THREAD(status, FALSE);      
  }

  if (!ret) {
    STRACE_INFO(("TSP private init error"));
  }

  X_tsp_provider_init_ok = ret;
  
  return ret;
} /* End of TSP_provider_private_init */

int 
TSP_provider_private_run() {
  int retcode = 0;
  /* 
   * Launch GLU and instantiate global 
   * datapool  iff it is an ACTIVE one.
   * PASSIVE GLU should not be launched by TSP_provider_run
   * since the should only when a consumer 'request_sample_init'
   * moreover they have a local datapool.
   */
  if (GLU_SERVER_TYPE_ACTIVE == firstGLU->type) {
    /* Instantiate GLOBAL datapool */
    TSP_datapool_instantiate(firstGLU);    
    /* Start GLU now since it is an ACTIVE one */
    retcode = firstGLU->start(firstGLU);
    if (retcode) {
      STRACE_ERROR(("Cannot start GLU (ACTIVE case)"));
    }	
  }
  return retcode;
} /* end of TSP_provider_private_run */


int32_t 
TSP_provider_request_async_sample_write(TSP_async_sample_t* async_sample_write) {
  int32_t ret = TSP_STATUS_OK;
  
  STRACE_DEBUG(("TSP_PROVIDER Before async_write: pgi %d value %s return %d ",async_sample_write->provider_global_index,async_sample_write->data.data_val,ret ));

  /* FIXME should check channel id and get GLU from session object */
  ret = firstGLU->async_write(firstGLU,
			      async_sample_write->provider_global_index,
  			      async_sample_write->data.data_val,async_sample_write->data.data_len);

  STRACE_DEBUG(("TSP_PROVIDER After async_write: pgi %d value %s return %d ",async_sample_write->provider_global_index,async_sample_write->data.data_val,ret ));
  
  return ret;

} /* end of TSP_provider_request_async_sample_write */


int32_t 
TSP_provider_request_async_sample_read(TSP_async_sample_t* async_sample_read) {
  int32_t ret = TSP_STATUS_OK;

  STRACE_DEBUG(("TSP_PROVIDER Before async_read: pgi %d value %s return %d ",async_sample_read->provider_global_index,async_sample_read->data.data_val,ret ));
  
  /* FIXME should check channel id and get GLU from session object */

  ret = firstGLU->async_read(firstGLU,
			     async_sample_read->provider_global_index,
			     (async_sample_read->data.data_val),
			     &(async_sample_read->data.data_len));

  STRACE_DEBUG(("TSP_PROVIDER After async_read: pgi %d value %f return %d ",async_sample_read->provider_global_index,*((double*)(async_sample_read->data.data_val)),ret ));
  
  return ret;

} /* End of TSP_async_sample_read */


void  
TSP_provider_request_extended_information(TSP_request_extended_information_t* req_extinfo, 
					  TSP_answer_extended_information_t* ans_extinfo) {
  GLU_handle_t* myGLU;
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);  

  ans_extinfo->status= TSP_provider_checkVersionAndChannelId(req_extinfo->version_id,
							     req_extinfo->channel_id,
							     &myGLU);

  if (TSP_STATUS_OK==ans_extinfo->status) {
    /* fill-in minimal info in ans_extinfo */  
    ans_extinfo->version_id = req_extinfo->version_id;
    ans_extinfo->channel_id = req_extinfo->channel_id;
    if (ans_extinfo->extsymbols.TSP_sample_symbol_extended_info_list_t_len!=0) {
      TSP_SSEIList_finalize(&(ans_extinfo->extsymbols));
    }
    
    /* 
     * Ask the GLU for extended info by PGI 
     */
    TSP_SSEIList_initialize(&(ans_extinfo->extsymbols),req_extinfo->pgi.pgi_len);
    ans_extinfo->status = myGLU->get_ssei_list_fromPGI(myGLU,
						       req_extinfo->pgi.pgi_val,
						       req_extinfo->pgi.pgi_len,
						       &(ans_extinfo->extsymbols));
  }
      
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* end of TSP_provider_request_extended_information */


