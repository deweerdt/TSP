/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.c,v 1.5 2002-10-01 15:24:43 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

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

void TSP_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open)
{
  SFUNC_NAME(TSP_request_open);
  GLU_handle_t glu_h;
  char* error_info;
  char* real_stream_init;
	
  STRACE_IO(("-->IN"));


   ans_open->version_id = UNDEFINED_VERSION_ID;
   ans_open->channel_id = UNDEFINED_CHANNEL_ID;
   ans_open->status = TSP_STATUS_ERROR_UNKNOWN;
   ans_open->status_str = "";



   /*  get GLU instance. If a stream init is provided, use it, else, use default */   
   /* Note : for an active server, the function GLU_get_instance wall called ins TSP_provider_init
      so it is useless here */
   if( TRUE == req_open->use_stream_init)
     {
       real_stream_init = req_open->stream_init;
     }
   else
     {
       real_stream_init = X_default_stream_init;
     }

   glu_h = GLU_get_instance(real_stream_init, &error_info);
   
   /* FIXME : condition  de sortie */
	
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
	
}

void TSP_request_close(const TSP_request_close_t* req_close)

{
  SFUNC_NAME(TSP_request_close);
		
  STRACE_IO(("-->IN"));
		
  /* On ferme la session */
  if(TSP_close_session_by_channel(req_close->channel_id))
    {	
      STRACE_INFO(("Session for channel_id=%u is closed", req_close->channel_id));
    }
  else
    {
      STRACE_ERROR(("Unable to close session for channel_id=%u", req_close->channel_id));
    }
		
  STRACE_IO(("-->OUT"));
	
}

void  TSP_request_information(TSP_request_information_t* req_info, 
 			      TSP_answer_sample_t* ans_sample)
{

  SFUNC_NAME(tsp_request_information);
	
  STRACE_IO(("-->IN"));
	
  ans_sample->version_id = TSP_VERSION;
  ans_sample->channel_id = req_info->channel_id;
  /* FIXME : gerer le retour */  
  TSP_session_get_sample_symbol_info_list_by_channel(req_info->channel_id,
						     &(ans_sample->symbols));

  		
  STRACE_IO(("-->OUT"));
}

/** Provider initialisation function.
 * @param server_number The server number (must be unique
 * for a given server).
 * @param s The char string that may be used by the sample server
 */

int TSP_init(int server_number, char* stream_init)
{
  SFUNC_NAME(TSP_init);

  int ret = TRUE;
  STRACE_IO(("-->IN server number=%d", server_number));

  /* Faire le traitement du nom du serveur */
  TSP_session_init();

  if(stream_init)
    {
      X_default_stream_init = stream_init;
      STRACE_INFO(("default stream_init from program used : '%s'", X_default_stream_init));
    }
  else if(0) /* si ligne de commande, alors affecter */
    {
      assert(0);
      STRACE_INFO(("default stream_init from command line used : '%s'", X_default_stream_init));
    }
  else
    {
      STRACE_INFO(("No default stream init"));
      X_default_stream_init = 0;
    }
  


  /* We must start a global datapool when the sample server is active */
  if ( GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
    {
      /* We initialize the active server (0 is there coz a fall back stream does not make sense)*/
      
      if(ret) ret = GLU_init(0);

      /* We get the datapool instance. It MUST be GLOBAL because the server is active */      	 
      if(ret)
	{	  
	  if ( GLU_GLOBAL_HANDLE != GLU_get_instance(X_default_stream_init,0) )
	    {
	      ret = FALSE;
	      STRACE_ERROR(("function GLU_get_instance(stream_init) failed"));
	    }
	}
      if(ret) ret = TSP_global_datapool_init();	
    }
  else
    {
      if(ret) ret = GLU_init(X_default_stream_init);
    }

    
  /* TSP_command_init must be the latest called init func, coz we will be blocked here forever */
  if (ret) ret = TSP_command_init(server_number);
	

  if(!ret)
    {
      STRACE_ERROR(("TSP init error"));
    }
	
  STRACE_IO(("-->OUT"));
}

void  TSP_request_sample(TSP_request_sample_t* req_info, 
			 TSP_answer_sample_t** ans_sample)
{
  SFUNC_NAME(TSP_request_sample);
    
  int ret = TRUE;
  int use_global_datapool;
	
  STRACE_IO(("-->IN"));
	
  /* FIXME : ce n'est pas le meilleur endroit pour faire ca,
     le faire plutot lors du sample_init avec les données
     conservée dans la session*/
  /*ret = TSP_global_datapool_add_symbols(&(req_info->symbols));*/
  if(ret)
    {
     
      /* Must we use a session or global data pool ? */
      if (  GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
	  use_global_datapool = TRUE;
      else
	  use_global_datapool = FALSE;

      
      ret = TSP_session_create_symbols_table_by_channel(req_info, ans_sample, use_global_datapool);
      if(!ret) 
        {
	  STRACE_ERROR(("Function TSP_session_create_symbols_table_by_channel failed"));
        }
        
    }
  else
    {
      STRACE_ERROR(("Function TSP_datapool_add_symbols failed"));
    }
	
  STRACE_IO(("-->OUT"));
}

void  TSP_request_sample_init(TSP_request_sample_init_t* req_info, 
 			      TSP_answer_sample_init_t** ans_sample)
{

  SFUNC_NAME(TSP_request_sample_init);
  int ret = FALSE;

  int start_local_thread;

    
  STRACE_IO(("-->IN"));
    

  /*FIXME : oups, fuite de memoire*/
  *ans_sample = (TSP_answer_sample_init_t*)calloc(1, sizeof(TSP_answer_sample_init_t));
  TSP_CHECK_ALLOC(*ans_sample,);
  
  /* If the sample server is a lazy pasive server, we need a thread per session*/
  if (  GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
    start_local_thread = FALSE;
  else
    start_local_thread = TRUE;
  
  ret = TSP_session_create_data_sender_by_channel(req_info->channel_id, start_local_thread);
  if(!ret)
    {
      STRACE_ERROR(("TSP_data_sender_create failed"));
    }
    
  /* send data address to client */
  (*ans_sample)->data_address =
    (char*)TSP_session_get_data_address_string_by_channel(req_info->channel_id);
  
  STRACE_INFO(("DATA_ADDRESS = '%s'", (*ans_sample)->data_address));
  STRACE_IO(("-->OUT"));

}
