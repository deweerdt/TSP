/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_client.c,v 1.8 2004-09-22 14:25:58 tractobob Exp $

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

Purpose   : 

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_client.h"

#include "tsp_rpc.h"
#include "tsp_rpc_confprogid.h"


#define LOCAL_RPCCHECK_FALSE  	if( server == (TSP_server_t)0)  \
				{ STRACE_ERROR(("RPCCHECK failed")) ; return FALSE ;} 
				
#define LOCAL_RPCCHECK_0  	if( server == (TSP_server_t)0) \
				{ STRACE_ERROR(("RPCCHECK failed")) ; return 0 ;} 

TSP_provider_info_t * tsp_provider_information(TSP_server_t server)
{

  TSP_provider_info_t* result;
	
  STRACE_IO(("-->IN"));

	
  LOCAL_RPCCHECK_0;
	
  result = tsp_provider_information_1(server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  STRACE_IO(("-->OUT"));

	
  return result;
}	

CLIENT* tsp_remote_open_progid(const char *target_name, int progid)
{

  CLIENT* cl = (CLIENT *)0;
  struct timeval timeout = { 1, 0 };

  STRACE_IO(("-->IN"));

  /*cl = clnt_create_timed(target_name, progid, TSP_RPC_VERSION_INITIAL, "tcp",&timeout );*/

  

  cl = clnt_create(target_name, progid, TSP_RPC_VERSION_INITIAL, "tcp");

  if(cl == (CLIENT *)0)
    {
      STRACE_DEBUG(("ERROR : GLOBAL clnt_create failed for host %s", target_name));
    }
  else
    {
      STRACE_INFO(("CONNECTED to server %s", target_name));
      /* Set time out */
      tsp_wrap_rpc_clnt_set_timeout(cl, TSP_RPC_CONNECT_TIMEOUT);

    }
 
   

  STRACE_IO(("-->OUT"));

  return cl;
	
}

int TSP_remote_open_server( const char *target_name,
			    int server_id, 
			    TSP_server_t* server,
			    TSP_server_info_string_t server_info)
{

  int prodid_max_number, progid;
  int ret = FALSE;
  TSP_provider_info_t* server_info_t;
	
  *server = (TSP_server_t)0;
  server_info[0] = '\0';
	
  STRACE_IO(("-->IN"));

	
  prodid_max_number = TSP_get_progid_total_number();
	
  if((server_id < prodid_max_number) && (server_id >=0) )
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
		      strcpy(server_info, server_info_t->info);
		      STRACE_INFO(("Server opened : '%s'", server_info));
		      ret = TRUE;		
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
	
  STRACE_IO(("-->OUT"));

	
  return ret;
	
}

/**
* Close a server.
* @param server the server that must be close.
*/
void TSP_remote_close_server(TSP_server_t server)
{
  if(server)
    {
      clnt_destroy((CLIENT*)server);
    }
}

/**
 * Max server number.
 * Get how many server can exists on a given host.
 * @return Max server number
 */
int TSP_get_server_max_number()
{
  return TSP_get_progid_total_number();
}

TSP_answer_open_t * TSP_request_open(const TSP_request_open_t* req_open, TSP_server_t server)
{

  TSP_answer_open_t* result;
	
  STRACE_IO(("-->IN"));

	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_open_1(*req_open, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  STRACE_IO(("-->OUT"));

	
  return result;
}	

/**
* Close the session for a remote_opened provider.
* @param req_close the informations tout close the session
* @return The action result (TRUE or FALSE)
*/
int TSP_request_close(const TSP_request_close_t* req_close, TSP_server_t server)
{

  int result;
	
  STRACE_IO(("-->IN"));
	
  LOCAL_RPCCHECK_FALSE;
    
  result = (int)tsp_request_close_1(*req_close, server);
  TSP_STRACE_RPC_ERROR(server, result);
  
  STRACE_IO(("-->OUT"));
	
  return result;
}	

/**
* Close the session for a remote_opened provider.
* @param req_close the informations tout close the session
* @return The action result (TRUE or FALSE)
*/
TSP_answer_sample_t * TSP_request_information(const TSP_request_information_t* req_info, TSP_server_t server)
{

  TSP_answer_sample_t* result;
	
  STRACE_IO(("-->IN"));

	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_information_1(*req_info, server);
  TSP_STRACE_RPC_ERROR(server, result);
  
  STRACE_IO(("-->OUT"));
	
  return result;
}	

TSP_answer_sample_t * TSP_request_sample(
					 const TSP_request_sample_t* req_sample,
					 TSP_server_t server)
{

  TSP_answer_sample_t* result;
	
  STRACE_IO(("-->IN"));

	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  STRACE_IO(("-->OUT"));

	
  return result;
}

TSP_answer_sample_init_t * TSP_request_sample_init(
					      const TSP_request_sample_init_t* req_sample,
					      TSP_server_t server)
{

  TSP_answer_sample_init_t* result;
	
  STRACE_IO(("-->IN"));

	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_init_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  STRACE_IO(("-->OUT"));

	
  return result;
}

TSP_answer_sample_destroy_t*
TSP_request_sample_destroy(const TSP_request_sample_destroy_t* req_sample,
			   TSP_server_t server)
{
  
  TSP_answer_sample_destroy_t* result;
	
  STRACE_IO(("-->IN"));
	
  LOCAL_RPCCHECK_0;
	
  result = tsp_request_sample_destroy_1(*req_sample, server);
  TSP_STRACE_RPC_ERROR(server, result);
	
  STRACE_IO(("-->OUT"));
	
  return result;
}

	
