/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.c,v 1.4 2002-09-19 08:36:52 galles Exp $

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
	

void TSP_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open)
{
  SFUNC_NAME(TSP_request_open);
	
  STRACE_IO(("-->IN"));
	
  if(TSP_add_session(&(ans_open->channel_id)))
    {
      if(req_open->version_id <= TSP_VERSION)
	{
	  ans_open->version_id = TSP_VERSION;
	}
      else
	{
	  ans_open->version_id = UNDEFINED_VERSION_ID;
	  ans_open->channel_id = UNDEFINED_CHANNEL_ID;
	}
    }
  else
    {
      STRACE_ERROR(("TSP_add_session failed"));
      ans_open->version_id = UNDEFINED_VERSION_ID;
      ans_open->channel_id = UNDEFINED_CHANNEL_ID;
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
  GLU_get_sample_symbol_info_list(&(ans_sample->symbols));
	
		
  STRACE_IO(("-->OUT"));
}

/** Provider initialisation function.
 * @param server_number The server number (must be unique
 * for a given server).
 */
int TSP_init(int server_number)
{
  SFUNC_NAME(TSP_init);

  int ret = TRUE;

  STRACE_IO(("-->IN server number=%d", server_number));
	

  TSP_session_init();
	
  if(ret) ret = GLU_glue_sserver_init();
  if(ret) ret = TSP_datapool_init();
    
  /* Laisser en dernier, on reste bloqué ici si tout va bien*/
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
    
  int ret;
	
  STRACE_IO(("-->IN"));
	
  /* FIXME : ce n'est pas le meilleur endroit pour faire ca,
     le faire plutot lors du sample_init avec les données
     conservée dans la session*/
  ret = TSP_datapool_add_symbols(&(req_info->symbols));
  if(ret)
    {
        
      ret = TSP_session_create_symbols_table_by_channel(req_info, ans_sample);
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
    
  STRACE_IO(("-->IN"));
    
  /*FIXME : simplifier cette structure, on en utilise pas la moitie */
  /*FIXME : oups, fuite de memoire*/
  *ans_sample = (TSP_answer_sample_init_t*)calloc(1, sizeof(TSP_answer_sample_init_t));
  TSP_CHECK_ALLOC(*ans_sample,);
    
  ret = TSP_session_create_data_sender_by_channel(req_info->channel_id);
  if(!ret)
    {
      STRACE_ERROR(("TSP_data_sender_create failed"));
    }
    
  (*ans_sample)->data_address =
    (char*)TSP_session_get_data_address_string_by_channel(req_info->channel_id);
  STRACE_INFO(("DATA_ADDRESS = '%s'", (*ans_sample)->data_address));
    
		
  STRACE_IO(("-->OUT"));
    

}
