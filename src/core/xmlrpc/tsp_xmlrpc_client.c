/*

$Header: /home/def/zae/tsp/tsp/src/core/xmlrpc/Attic/tsp_xmlrpc_client.c,v 1.3 2006-02-07 21:10:41 deweerdt Exp $

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

#include "tsp_xmlrpc_client.h"
#include "tsp_xmlrpc_config.h"

#include <stdio.h>

#include <xmlrpc.h>
#include <xmlrpc_client.h>

#include "tsp_xmlrpc_util.h"

#define NAME "XML-RPC TSP Client"
#define VERSION "0.0a"


static TSP_provider_info_t * tsp_provider_information(TSP_server_t server)
{
  TSP_provider_info_t *prov_info;
  xmlrpc_value *result;
	
  STRACE_IO(("-->IN"));

  result = xmlrpc_client_call(&server->xmlrpc_envi, server->url, "tsp.tsp_provider_information", "()");
  die_if_fault_occurred(&server->xmlrpc_envi);
    
  prov_info = xmlrpc_value_to_TSP_provider_info(&server->xmlrpc_envi, result);
  xmlrpc_DECREF(result);

  STRACE_IO(("-->OUT"));

  return prov_info;
}	

int TSP_remote_open_server( const char *protocol,
			    const char *target_name,
			    const char *server_name,
			    const int port, 
			    TSP_server_t* server,
			    TSP_server_info_string_t server_info)
{

  int ret = TRUE;
  TSP_provider_info_t* server_info_t;
	
  *server = (TSP_server_t)malloc(sizeof(**server));
  server_info[0] = '\0';
	
  STRACE_IO(("-->IN"));

  /* XMLRPC client library startup */
  xmlrpc_client_init(XMLRPC_CLIENT_NO_FLAGS, NAME, VERSION);

  /* XMLRPC error-handling environment. */
  xmlrpc_env_init(&((*server)->xmlrpc_envi));

  (*server)->name = strdup(server_name);
  (*server)->url = (char *)malloc(TSP_MAX_URL_LENGTH);
  sprintf((*server)->url, "http://%s/RPC2", target_name);

  if(strcmp(protocol, TSP_XMLRPC_PROTOCOL) != 0) {
		  STRACE_ERROR(("Protocol %s not handled, use %s", protocol, TSP_XMLRPC_PROTOCOL));
		  return FALSE;
  } 

  /*  On recupere la chaine d'info du serveur) */
  server_info_t = tsp_provider_information(*server);
  if( server_info_t != NULL) {
		  if( STRING_SIZE_SERVER_INFO >= strlen(server_info_t->info) ) {
				  /* check whether server name is the one requested */
				  if(strncmp(server_name,
							 server_info_t->info,
							 strlen(server_name)) == 0) {
						  strcpy(server_info, server_info_t->info);
						  STRACE_INFO(("Server opened : '%s'", server_info));
						  ret = TRUE;
				  } else {																					 
						  STRACE_INFO(("Not the requested server : '%s'", server_info_t->info));
						  TSP_remote_close_server(*server);
						  ret = FALSE;
				  }
		  } else {
				  STRACE_ERROR(("Returned info string too long"));
		  }
  } else {
		  STRACE_ERROR(("Could not connect to specified server"));
  }
  
  STRACE_IO(("-->OUT"));

	
  return ret;
	
}

void TSP_remote_close_server(TSP_server_t server)
{
  STRACE_IO(("-->IN"));

  
  /* Clean up our error-handling environment. */
  xmlrpc_env_clean(&server->xmlrpc_envi);
    
  /* Shutdown our XML-RPC client library. */
  xmlrpc_client_cleanup();

  STRACE_IO(("-->OUT"));
}


int TSP_get_server_max_number()
{
  //FIXME: find a sane value here
  return 1;
}

TSP_answer_open_t * TSP_request_open(const TSP_request_open_t* req_open, TSP_server_t server)
{
  TSP_answer_open_t *ans_open;
  xmlrpc_value *result;
	
  STRACE_IO(("-->IN"));

  STRACE_IO((server->url));


  result = xmlrpc_client_call(&server->xmlrpc_envi, server->url, "tsp.tsp_request_open", 
 							  "({s:i,s:i,s:s})",  
							  "version_id", req_open->version_id, 
							  "TSP_argv_t_len", req_open->argv.TSP_argv_t_len, 
							  "TSP_argv_t_val", req_open->argv.TSP_argv_t_val); 
  die_if_fault_occurred(&server->xmlrpc_envi);
    

  ans_open = xmlrpc_value_to_TSP_answer_open(&server->xmlrpc_envi, result);

  xmlrpc_DECREF(result);

  STRACE_IO(("-->OUT"));
  return   ans_open;
}	

int TSP_request_close(const TSP_request_close_t* req_close, TSP_server_t server)
{

  int res;
  xmlrpc_value *result;
	
  STRACE_IO(("-->IN"));

  result = xmlrpc_client_call(&server->xmlrpc_envi, 
							  server->url, 
							  "tsp.tsp_request_close", 
							  "({s:i,s:i})",
							  "version_id", req_close->version_id,
							  "channel_id", req_close->channel_id);
  die_if_fault_occurred(&server->xmlrpc_envi);
    
  /* Get our state name and print it out. */
  xmlrpc_parse_value(&server->xmlrpc_envi, result, "(i)", &res);
  die_if_fault_occurred(&server->xmlrpc_envi);
    
  /* Dispose of our result value. */
  xmlrpc_DECREF(result);

  STRACE_IO(("-->OUT"));
	
  return res;
}	

TSP_answer_sample_t * TSP_request_information(const TSP_request_information_t* req_info, TSP_server_t server)
{

  TSP_answer_sample_t *ans_sample;
  xmlrpc_value *result;

  STRACE_IO(("-->IN"));

  result = xmlrpc_client_call(&server->xmlrpc_envi, 
							  server->url, 
							  "tsp.tsp_request_information", 
							  "({s:i,s:i})",
							  "version_id", req_info->version_id,
							  "channel_id", req_info->channel_id);
  die_if_fault_occurred(&server->xmlrpc_envi);

  ans_sample = xmlrpc_value_to_TSP_answer_sample(&server->xmlrpc_envi, result);

  xmlrpc_DECREF(result);

  STRACE_IO(("-->OUT"));
  return ans_sample;
  
}	

TSP_answer_sample_t * TSP_request_sample(
					 const TSP_request_sample_t* req_sample,
					 TSP_server_t server)
{

  TSP_answer_sample_t *ans_sample;
  xmlrpc_value *xmlrpc_params, *result;
	
  STRACE_IO(("-->IN"));

  xmlrpc_params = TSP_request_sample_to_xmlrpc_value(&server->xmlrpc_envi, req_sample);

  result = xmlrpc_client_call_params(&server->xmlrpc_envi, 
									 server->url, 
									 "tsp.tsp_request_sample", 
									 xmlrpc_params);

  die_if_fault_occurred(&server->xmlrpc_envi);
    
  ans_sample = xmlrpc_value_to_TSP_answer_sample(&server->xmlrpc_envi, result);
  xmlrpc_DECREF(result);

  STRACE_IO(("-->OUT"));
	
  return ans_sample;
}

TSP_answer_sample_init_t * TSP_request_sample_init(
					      const TSP_request_sample_init_t* req_sample_init,
					      TSP_server_t server)
{

  TSP_answer_sample_init_t* ans_sample_init;
  xmlrpc_value *xr_result;
	
  STRACE_IO(("-->IN"));

  xr_result = xmlrpc_client_call(&server->xmlrpc_envi, 
							  server->url, 
							  "tsp.tsp_request_sample_init", 
							  "({s:i,s:i})",
							  "version_id", req_sample_init->version_id,
							  "channel_id", req_sample_init->channel_id);

  die_if_fault_occurred(&server->xmlrpc_envi);
    
  ans_sample_init = xmlrpc_value_to_TSP_answer_sample_init(&server->xmlrpc_envi, xr_result);
  xmlrpc_DECREF(xr_result);

  STRACE_IO(("-->OUT"));
	
  return ans_sample_init;
}

TSP_answer_sample_destroy_t*
TSP_request_sample_destroy(const TSP_request_sample_destroy_t* req_sample_destroy,
						   TSP_server_t server)
{
  
  TSP_answer_sample_destroy_t* ans_sample_destroy;
  xmlrpc_value *xr_result;
	
  STRACE_IO(("-->IN"));
	
  xr_result = xmlrpc_client_call(&server->xmlrpc_envi, 
								 server->url, 
								 "tsp.tsp_request_sample_destroy", 
								 "({s:i,s:i,s:i})",
								 "version_id", req_sample_destroy->version_id,
								 "channel_id", req_sample_destroy->channel_id,
								 "version_id", req_sample_destroy->version_id);


  ans_sample_destroy = xmlrpc_value_to_TSP_answer_sample_destroy(&server->xmlrpc_envi,
																 xr_result);
 	
  xmlrpc_DECREF(xr_result);

  STRACE_IO(("-->OUT"));
	
  return ans_sample_destroy;
}

	
