/*!  \file 

$Id: tsp_provider.c,v 1.19 2003-07-15 14:42:24 erk Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Main implementation for the producer module

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_provider.h"

#include "tsp_session.h"
#include "tsp_server.h"
#include "tsp_datapool.h"
#include "glue_sserver.h"	

static char* X_default_stream_init = 0;

/** modified argc and argv, will be returned to user code after init */
static  char** X_argv = 0;
static  int X_argc = 0;

/** Default values for args to the GLU */
static  char** X_glu_argv = 0;
static  int X_glu_argc = 0;

/** default server number is 0, that's what we want
when there is one single server on a given host*/
static int X_server_number = 0;


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

/** Tells is the GLU is active or pasive */
static int X_glu_is_active;

/* polling time for session garbage collector */
#define TSP_GARBAGE_COLLECTOR_POLL_TIME_US ((int)(5e6))

static int TSP_cmd_line_parser(int* argc, char** argv[])
{
  int i;
  int final_argc = 1;
  int found_stream_start = FALSE;
  int found_stream_stop = FALSE;
  int found_server_number_flag = FALSE;
  char* p;
  int ret = TRUE;

  SFUNC_NAME((TSP_cmd_line_parser));
  STRACE_IO(("-->IN"));

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
	      X_server_number = atoi((*argv)[i]);
	      STRACE_INFO(("Server number = %d", X_server_number));
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

  /* Memorize GLU type */
  if (  GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
    X_glu_is_active = TRUE;
  else
    X_glu_is_active = FALSE;

  STRACE_IO(("-->OUT"));
  
  return ret;


}

int TSP_provider_is_initialized(void)
{
  return  X_tsp_provider_init_ok;
}

int TSP_provider_get_server_number(void)
{
  return  X_server_number;
}



void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open)
{
  SFUNC_NAME(TSP_provider_request_open);

  GLU_handle_t glu_h;
  char* error_info;
  int i;
	
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);
  STRACE_IO(("-->IN"));


    /* Fortify calls */
  
    /*Fortify_EnterScope();*/

   ans_open->version_id = UNDEFINED_VERSION_ID;
   ans_open->channel_id = UNDEFINED_CHANNEL_ID;
   ans_open->status = TSP_STATUS_ERROR_UNKNOWN;
   ans_open->status_str = "";

   if(req_open->argv.TSP_argv_t_len)
     {
       for( i = 0; i< req_open->argv.TSP_argv_t_len ;i++)
	 {
	   STRACE_DEBUG(("arg %d is '%s'", i,  req_open->argv.TSP_argv_t_val[i]));
	 }
     }
   else
     {
          STRACE_DEBUG(("No custom args from consumer"));
     }


   /*  get GLU instance. If a stream init is provided, use it, else, use default */   
   if( 0 != req_open->argv.TSP_argv_t_len )
     {
       glu_h = GLU_get_instance(req_open->argv.TSP_argv_t_len, req_open->argv.TSP_argv_t_val, &error_info);
     }
   else
     {
       /* use fallback if provided */
       glu_h = GLU_get_instance(X_glu_argc, X_glu_argv, &error_info);
     }

   
   if(glu_h)
     {
       if(TSP_add_session(&(ans_open->channel_id), glu_h))
	 {
	   if(req_open->version_id <= TSP_VERSION)
	     {
	       ans_open->version_id = TSP_VERSION;
	       ans_open->status = TSP_STATUS_OK;
	     }
	   else
	     {
	       STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",req_open->version_id, TSP_VERSION ));
	       ans_open->status = TSP_STATUS_ERROR_VERSION;
	     }
	 }
       else
	 {
	   STRACE_ERROR(("TSP_add_session failed"));
	 }
     }
   else
     {
       STRACE_INFO(("Unable to get GLU instance"));
        ans_open->status = TSP_STATUS_ERROR_SEE_STRING;
	ans_open->status_str = error_info;
     }

  STRACE_IO(("-->OUT"));

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);	

} /* End of TSP_provider_request_open */


static void TSP_provider_request_close_priv(channel_id_t channel_id)
{
  SFUNC_NAME(TSP_provider_request_close_priv);		
  STRACE_IO(("-->IN"));

  TSP_session_destroy_symbols_table_by_channel(channel_id);
  TSP_session_close_session_by_channel(channel_id);

  STRACE_IO(("-->OUT"));
}

void TSP_provider_request_close(const TSP_request_close_t* req_close)

{
  SFUNC_NAME(TSP_provider_request_close);		
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);

  STRACE_IO(("-->IN"));

  TSP_provider_request_close_priv(req_close->channel_id);

  STRACE_IO(("-->OUT"));

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_close */

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
 			      TSP_answer_sample_t* ans_sample)
{
  SFUNC_NAME(tsp_request_information);	


  int ret;
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);
  STRACE_IO(("-->IN"));
       
  
  ans_sample->version_id = TSP_VERSION;
  ans_sample->channel_id = req_info->channel_id;
  ans_sample->status = TSP_STATUS_ERROR_UNKNOWN;
  ans_sample->provider_group_number = 0;
  ans_sample->base_frequency = GLU_get_base_frequency();
  ans_sample->max_client_number = TSP_MAX_CLIENT_NUMBER;
  ans_sample->current_client_number = TSP_session_get_nb_session();
  ans_sample->max_period = TSP_MAX_PERIOD;
  ans_sample->symbols.TSP_sample_symbol_info_list_t_len = 0;
  ans_sample->symbols.TSP_sample_symbol_info_list_t_val = 0;  
  
  if(req_info->version_id <= TSP_VERSION)
    {
      
      ret = TSP_session_get_sample_symbol_info_list_by_channel(req_info->channel_id,						     &(ans_sample->symbols));
      if(ret)
	{
	  ans_sample->status = TSP_STATUS_OK;
	}
      else
	{
	  STRACE_ERROR(("Function TSP_session_get_sample_symbol_info_list_by_channel failed"));
	}
    
  }
  else
  {
    STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",req_info->version_id, TSP_VERSION ));
    ans_sample->status = TSP_STATUS_ERROR_VERSION;
  }
  		
  STRACE_IO(("-->OUT"));

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_information */


void TSP_provider_request_sample_free_call(TSP_answer_sample_t* ans_sample)
{
  SFUNC_NAME(TSP_provider_request_sample_free_call);

  STRACE_IO(("-->IN"));

  TSP_session_create_symbols_table_by_channel_free_call(ans_sample);

  STRACE_IO(("-->OUT"));
}

void  TSP_provider_request_sample(TSP_request_sample_t* req_info, 
			 TSP_answer_sample_t* ans_sample)
{
  SFUNC_NAME(TSP_request_sample);


  int use_global_datapool;

  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);	
  STRACE_IO(("-->IN"));

  ans_sample->version_id = TSP_VERSION;
  ans_sample->channel_id = req_info->channel_id;
  ans_sample->status = TSP_STATUS_ERROR_UNKNOWN;
  ans_sample->provider_group_number = 0;
  ans_sample->base_frequency = GLU_get_base_frequency();
  ans_sample->max_client_number = TSP_MAX_CLIENT_NUMBER;
  ans_sample->current_client_number = TSP_session_get_nb_session();
  ans_sample->max_period = TSP_MAX_PERIOD;
  ans_sample->symbols.TSP_sample_symbol_info_list_t_len = 0;
  ans_sample->symbols.TSP_sample_symbol_info_list_t_val = 0;
  
  STRACE_INFO(("Consumer No %d asked for %d symbols",req_info->channel_id,req_info->symbols.TSP_sample_symbol_info_list_t_len  ));
  
  if(req_info->version_id <= TSP_VERSION)
    {
      if(TSP_session_get_symbols_global_index_by_channel(req_info->channel_id, &(req_info->symbols) ))
	{     
	  /* Must we use a session or global data pool ? */
	  use_global_datapool = ( X_glu_is_active ? TRUE : FALSE );
      
	  /* The datapool will be created here (if it does not already exist) */
	  if(TSP_session_create_symbols_table_by_channel(req_info, ans_sample, use_global_datapool)) 
	    {
	      ans_sample->status = TSP_STATUS_OK;
	    }
	  else  
	    {
	      STRACE_ERROR(("Function TSP_session_create_symbols_table_by_channel failed"));
	    }        
	}
      else
	{
	  STRACE_WARNING(("Function TSP_session_get_symbols_global_index_by_channel failed"));
	  ans_sample->status = TSP_STATUS_ERROR_SYMBOLS;
	}
    }
  else
    {
      STRACE_WARNING(("TSP version ERROR. Requested=%d Current=%d",req_info->version_id, TSP_VERSION ));
      ans_sample->status = TSP_STATUS_ERROR_VERSION;
    }


	
  STRACE_IO(("-->OUT"));

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_sample */

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_info, 
				       TSP_answer_sample_init_t* ans_sample)
{  
  SFUNC_NAME(TSP_request_sample_init);


  int start_local_thread;
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);    
  STRACE_IO(("-->IN"));
    
  ans_sample->version_id = UNDEFINED_VERSION_ID;
  ans_sample->channel_id = req_info->channel_id;
  ans_sample->status = TSP_STATUS_ERROR_UNKNOWN;
 
  if(req_info->version_id <= TSP_VERSION)
    {
      ans_sample->version_id = req_info->version_id;
      /* If the sample server is a lazy pasive server, we need a thread per session*/
      start_local_thread = ( X_glu_is_active ? FALSE : TRUE );
      
      if(TSP_session_create_data_sender_by_channel(req_info->channel_id, start_local_thread))
	{
	  ans_sample->status = TSP_STATUS_OK;
	}
      else     
	{
	  STRACE_ERROR(("TSP_data_sender_create failed"));
	}
    
      /* send data address to client */
      ans_sample->data_address =
	(char*)TSP_session_get_data_address_string_by_channel(req_info->channel_id);
  
      STRACE_DEBUG(("DATA_ADDRESS = '%s'", ans_sample->data_address));
    }
  else
    {
      STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",req_info->version_id, TSP_VERSION ));
      ans_sample->status = TSP_STATUS_ERROR_VERSION;
    }

  STRACE_IO(("-->OUT"));

  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of TSP_provider_request_sample_init */

static void TSP_provider_request_sample_destroy_priv(channel_id_t channel_id)
{
  SFUNC_NAME(TSP_provider_request_sample_destroy_priv);     

  int stop_local_thread = ( X_glu_is_active ? FALSE : TRUE );  
  if(!TSP_session_destroy_data_sender_by_channel(channel_id, stop_local_thread))
    {
      STRACE_ERROR(("TSP_session_destroy_data_sender_by_channel failed"));
    }
}

void  TSP_provider_request_sample_destroy(TSP_request_sample_destroy_t* req_info, 
					  TSP_answer_sample_destroy_t* ans_sample)
{
  SFUNC_NAME(TSP_provider_request_sample_destroy);
  TSP_LOCK_MUTEX(&X_tsp_request_mutex,);

  STRACE_IO(("-->IN"));
    
  ans_sample->version_id = req_info->version_id;
  ans_sample->channel_id = req_info->channel_id;
  ans_sample->status = TSP_STATUS_OK;
 
  if(req_info->version_id <= TSP_VERSION)
    {      
      TSP_provider_request_sample_destroy_priv(req_info->channel_id);
    }
  else
    {
      STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d for session %d",
		    req_info->version_id, TSP_VERSION,req_info->channel_id  ));
      ans_sample->status = TSP_STATUS_ERROR_VERSION;
    }

  STRACE_IO(("-->OUT"));
  TSP_UNLOCK_MUTEX(&X_tsp_request_mutex,);
} /* End of  TSP_provider_request_sample_destroy */


static void* TSP_provider_garbage_collector_thread(void* dummy)
{
   SFUNC_NAME(TSP_provider_garbage_collector_thread);
   channel_id_t channel_id;

   STRACE_IO(("-->IN"));

   /* Save memory ! */
   pthread_detach(pthread_self());

   while(TRUE)
     {
       while(TSP_session_get_garbage_session(&channel_id))
	 {
	   /* Do what some rude consumer should have done itself */
	   TSP_provider_request_sample_destroy_priv(channel_id);
	   TSP_provider_request_close_priv(channel_id);
	   STRACE_INFO(("Session No %d 'garbage-collected'", channel_id));
	 }
       tsp_usleep(TSP_GARBAGE_COLLECTOR_POLL_TIME_US);
     }

   STRACE_IO(("-->OUT"));
}


int TSP_provider_private_init(int* argc, char** argv[])
{
  SFUNC_NAME(TSP_provider_private_init);

  int ret = TRUE;
  int status;
  pthread_t thread;
  assert(argc);
  assert(argv);
  
  ret = TSP_cmd_line_parser(argc, argv);
  if(ret)
     {
      /* init sessions */
      TSP_session_init();

      /* Initialise GLU server */
      ret = GLU_init(X_glu_argc, X_glu_argv);

      /* Launch garbage collection for sessions */
      status = pthread_create(&thread, NULL, TSP_provider_garbage_collector_thread,  NULL);
      TSP_CHECK_THREAD(status, FALSE);
      
    }

  if(!ret)
    {
      STRACE_INFO(("TSP init error"));
    }

  X_tsp_provider_init_ok = ret;
  STRACE_IO(("-->OUT"));
  
  return ret;
} /* End of TSP_provider_private_init */
