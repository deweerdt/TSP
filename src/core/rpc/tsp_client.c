/*

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_client.c,v 1.20 2007-03-01 22:26:39 erk Exp $

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
Component  : Consumer

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include "tsp_client.h"

#include "tsp_rpc.h"
#include "tsp_rpc_confprogid.h"

int tsp_wrap_rpc_clnt_set_timeout(CLIENT *client, int timeout);

#define LOCAL_RPCCHECK_NORETURN  	if( server == (TSP_server_t)0)  \
                                        { STRACE_ERROR(("RPCCHECK failed")) ;} 

#define LOCAL_RPCCHECK_FALSE  	if( server == (TSP_server_t)0)  \
				{ STRACE_ERROR(("RPCCHECK failed")) ; return FALSE ;} 
				
#define LOCAL_RPCCHECK_0  	if( server == (TSP_server_t)0) \
				{ STRACE_ERROR(("RPCCHECK failed")) ; return 0 ;} 

TSP_provider_info_t * tsp_provider_information(TSP_server_t server)
{

  TSP_provider_info_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_provider_information_1(server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
}	

CLIENT* tsp_remote_open_progid(const char *target_name, int progid)
{

  CLIENT* cl = (CLIENT *)0;
  /* struct timeval timeout = { 1, 0 }; */

  /*cl = clnt_create_timed(target_name, progid, TSP_RPC_VERSION_INITIAL, "tcp",&timeout );*/

  cl = clnt_create(target_name, progid, TSP_RPC_VERSION_INITIAL, "tcp");

  if (cl == (CLIENT *)0) {
    STRACE_ERROR(("ERROR : GLOBAL clnt_create failed for host <%s> / progid <%d/0x%08X>", target_name, progid,progid));
  }
  else {
    STRACE_INFO(("CONNECTED to server %s", target_name));
    /* Set time out */
    tsp_wrap_rpc_clnt_set_timeout(cl, TSP_RPC_CONNECT_TIMEOUT);    
  }
 
  return cl;
	
}

int TSP_remote_open_server( const char *protocol,
			    const char *target_name,
			    const char *server_name,
			    const int server_id, 
			    TSP_server_t* server,
			    TSP_server_info_string_t server_info)
{

  int prodid_max_number, progid;
  int ret = FALSE;
  TSP_provider_info_t* server_info_t;
	
  *server = (TSP_server_t)0;
  server_info[0] = '\0';
	
  prodid_max_number = TSP_get_progid_total_number();

  if(strcmp(protocol, TSP_RPC_PROTOCOL) != 0)
    {
      STRACE_ERROR(("Protocol %s not handled, use %s", protocol, TSP_RPC_PROTOCOL));
    }
  else if((server_id < prodid_max_number) && (server_id >=0) )
    {
      progid = TSP_get_progid(server_id);
      if(progid > 0) 
	{
	  *server = tsp_remote_open_progid(target_name, progid);
	  if( (*server) != (TSP_server_t)0)
	    {
	      /*  On recupere la chaine d'info du serveur) */
	      server_info_t = tsp_provider_information(*server);
	      if( server_info_t != NULL)
		{	
		  if( STRING_SIZE_SERVER_INFO >= strlen(server_info_t->info) )
		    {
		      /* check whether server name is the one requested */
		      if(strncmp(server_name,
				 server_info_t->info,
				 strlen(server_name)) == 0)
			{
			  strcpy(server_info, server_info_t->info);
			  STRACE_INFO(("Server opened : '%s'", server_info));
			  ret = TRUE;
			}
		      else
			{
			  STRACE_INFO(("Not the requested server : '%s'", server_info_t->info));
			  TSP_remote_close_server(*server);
			  ret = FALSE;
			}
		    }
		  else
		    {
		      STRACE_ERROR(("Returned info string too long"));
		    }
		}
	      else
		{
		  STRACE_ERROR(("%s", clnt_sperror(*server, "sp_server_info_1\n") ));
		}
	      
	    }
	  else
	    {
	      STRACE_DEBUG(("tsp_remote_open_progid failed"));
	    }
			
	}
      else
	{
	  STRACE_ERROR(("No ProgId for Server Id=%d", server_id));
	}
		
    }
  else
    {
      STRACE_ERROR(("server_id %d too high", server_id));
    }
	
  return ret;
	
}

void TSP_remote_close_server(TSP_server_t server)
{
  if(server)
    {
      clnt_destroy((CLIENT*)server);
    }
}


int TSP_get_server_max_number()
{
  return TSP_get_progid_total_number();
}

TSP_answer_open_t * TSP_request_open(const TSP_request_open_t* req_open, TSP_server_t server)
{

  TSP_answer_open_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_open_1(*req_open, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
}	

int* TSP_request_close(const TSP_request_close_t* req_close, TSP_server_t server)
{
  int* retcode;

  LOCAL_RPCCHECK_0;
    
  retcode = tsp_request_close_1(*req_close, server);
  TSP_STRACE_RPC_ERROR(server, retcode);
  
  return retcode;
}	

TSP_answer_sample_t * TSP_request_information(const TSP_request_information_t* req_info, TSP_server_t server)
{

  TSP_answer_sample_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_information_1(*req_info, server);
  TSP_STRACE_RPC_ERROR(server, result);
  
  return result;
}	

TSP_answer_sample_t * TSP_request_filtered_information(const TSP_request_information_t* req_info, int filter_kind, char* filter_string, TSP_server_t server)
{

  TSP_answer_sample_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_filtered_information_1(*req_info, filter_kind, filter_string, server);
  TSP_STRACE_RPC_ERROR(server, result);
  
  return result;
}	

TSP_answer_sample_t * TSP_request_sample(
					 const TSP_request_sample_t* req_sample,
					 TSP_server_t server)
{

  TSP_answer_sample_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
}

TSP_answer_sample_init_t * TSP_request_sample_init(
					      const TSP_request_sample_init_t* req_sample,
					      TSP_server_t server)
{

  TSP_answer_sample_init_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_init_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
}

TSP_answer_sample_destroy_t*
TSP_request_sample_destroy(const TSP_request_sample_destroy_t* req_sample,
			   TSP_server_t server)
{
  
  TSP_answer_sample_destroy_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_destroy_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
}

int* TSP_request_async_sample_write(const TSP_async_sample_t* async_sample_write, TSP_server_t server)
{
  int* result;
  
/*   switch (async_sample_write->data.data_len) { */
/*   case 2:  */
/*     TSP_UINT16_TO_BE(*(uint16_t*)async_sample_write->data.data_val);	 */
/*     break; */
/*   case 4:  */
/*     TSP_UINT32_TO_BE(*(uint32_t*)async_sample_write->data.data_val);	 */
/*     break; */
/*   case 8:   */
/*     TSP_UINT64_TO_BE(*(uint64_t*)async_sample_write->data.data_val); */
/*     break; */
/*   default: */
/*     break; */
/*   } */
  
  LOCAL_RPCCHECK_FALSE;  

  result = tsp_request_async_sample_write_1(*async_sample_write, server);
	  
  TSP_STRACE_RPC_ERROR(server, result);
  	
 return result;
} /* end of async_sample_write */	

TSP_async_sample_t* TSP_request_async_sample_read(const TSP_async_sample_t* async_sample_read, 
						  TSP_server_t server)
{
  TSP_async_sample_t* result;
  

/*   switch (async_sample_read->data.data_len) { */
/*   case 2:  */
/*     TSP_UINT16_TO_BE(*(uint16_t*)async_sample_read->data.data_val);	 */
/*     break; */
/*   case 4:  */
/*     TSP_UINT32_TO_BE(*(uint32_t*)async_sample_read->data.data_val);	 */
/*     break; */
/*   case 8:   */
/*     TSP_UINT64_TO_BE(*(uint64_t*)async_sample_read->data.data_val); */
/*     break; */
/*   default: */
/*     break; */
/*   } */
  
  LOCAL_RPCCHECK_FALSE;
  
  result = tsp_request_async_sample_read_1(*async_sample_read, server);
	
  TSP_STRACE_RPC_ERROR(server, result);
	
  return result;
} /* end of async_sample_read */	

TSP_answer_extended_information_t* 
TSP_request_extended_information(const TSP_request_extended_information_t* req_extinfo, TSP_server_t server)
{

  TSP_answer_extended_information_t* result;
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_extended_information_1(*req_extinfo, server);
  TSP_STRACE_RPC_ERROR(server, result);
  
  return result;
}	
