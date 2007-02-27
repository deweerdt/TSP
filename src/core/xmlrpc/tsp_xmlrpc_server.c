/*

$Header: /home/def/zae/tsp/tsp/src/core/xmlrpc/tsp_xmlrpc_server.c,v 1.15 2007-02-27 20:00:45 sgalles Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2005 Frederik Deweerdt

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

Purpose   : 

-----------------------------------------------------------------------
*/

#include <rpc/rpc.h>
#include <netdb.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <unistd.h> 

#include <tsp_sys_headers.h>
#include <tsp_glu.h>
/* FIXME RP : beurk, RPC is compiled before CTRL could export this include, how sould I call Request Manager and GLU then ? */
#include "../ctrl/tsp_provider.h"
#include "../ctrl/tsp_request_handler.h"
#include "tsp_rpc.h"


#include <string.h>
#include <xmlrpc.h>
#include <xmlrpc_server_abyss.h>

/*#include "tsp_request_handler.h"


#include "tsp_xmlrpc_util.h"
#include "tsp_xmlrpc_server.h"
#include "tsp_xmlrpc_config.h"

/* same FIXME IDL <--> C dependency */
#include "../common/tsp_common_macros.h"

#define TSP_URL_MAXLENGTH 256
#define TSP_FILECONF_MAXLENGTH 256

typedef struct {
  int  server_number;
  char url[TSP_URL_MAXLENGTH];
  xmlrpc_env *env;
  char config_file[TSP_FILECONF_MAXLENGTH];
} TSP_xmlrpc_request_config_t;

char *strdup(const char *s);
char *GLU_get_server_name()
{
	static char ret[]="localhost";
	return ret;
}

xmlrpc_value *tsp_provider_information_xmlrpc(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data) {
  TSP_provider_info_t server_info;

  server_info.info = strdup(GLU_get_server_name());
  STRACE_DEBUG(("server_info.info: %s\n",server_info.info));
  return xmlrpc_build_value(env, "{s:s}", "info", server_info.info);
}

xmlrpc_value * tsp_request_open_xmlrpc(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
  TSP_answer_open_t ans_open;
  TSP_request_open_t req_open;
  xmlrpc_value *result;
	
  xmlrpc_parse_value(env, param_array, "({s:i,s:i,s:s,*})", 
					 "version_id", &req_open.version_id,
					 "TSP_argv_t_len", &req_open.argv.TSP_argv_t_len,
					 "TSP_argv_t_val", &req_open.argv.TSP_argv_t_val);

  TSP_provider_request_open(&req_open, &ans_open);	

  result = xmlrpc_build_value(env, "{s:i,s:i,s:i,s:s}", 
							  "version_id", ans_open.version_id,
							  "channel_id", ans_open.channel_id,
							  "status", ans_open.status,
							  "status_str", ans_open.status_str);
  return result;
}


void *tsp_request_close_xmlrpc(xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{

  xmlrpc_value *xr_result;
  TSP_request_close_t req_close;	

  xmlrpc_parse_value(env, param_array, "({s:i,s:i,*})", 
					 "version_id", &req_close.version_id,
					 "channel_id", &req_close.channel_id);

  TSP_provider_request_close(&req_close);
	
  xr_result = xmlrpc_build_value(env, "(i)", 1);
  
  return xr_result;
}

xmlrpc_value * tsp_request_information_xmlrpc (xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{

  TSP_request_information_t req_info;	
  TSP_answer_sample_t ans_sample;
  xmlrpc_value *value;
  xmlrpc_value *xmlrpc_ans_sample;
  int i;

  xmlrpc_parse_value(env, param_array, "({s:i,s:i,*})", 
					 "version_id", &req_info.version_id,
					 "channel_id", &req_info.channel_id);

  TSP_provider_request_information(&req_info, &ans_sample);

  value = xmlrpc_build_value (env, "()");
  
  xmlrpc_ans_sample = xmlrpc_build_value(env, "{s:i,s:i,s:i,s:i,s:d,s:i,s:i,s:i,s:i,s:i}",
										 "version_id", ans_sample.version_id,
										 "channel_id", ans_sample.channel_id,
										 "provider_timeout", ans_sample.provider_timeout,
										 "provider_group_number", ans_sample.provider_group_number,
										 "base_frequency", ans_sample.base_frequency,
										 "max_period", ans_sample.max_period,
										 "max_client_number", ans_sample.max_client_number,
										 "current_client_number", ans_sample.current_client_number,
										 "status", ans_sample.status,
										 "symbols.TSP_sample_symbol_info_list_t_len", ans_sample.symbols.TSP_sample_symbol_info_list_t_len);
  xmlrpc_array_append_item(env, value, xmlrpc_ans_sample);
  
  for (i=0; i < ans_sample.symbols.TSP_sample_symbol_info_list_t_len; i++) {
	xmlrpc_value *symbol;

	symbol = xmlrpc_build_value(env, "{s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
								"name", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].name,
								"provider_global_index", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
								"provider_group_index", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
								"provider_group_rank", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank,
								/*"xdr_tsp_t", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t,*/
								"dimension", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
								"period", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].period,
                                "phase", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].phase,
                                "nelem", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].nelem,
                                "offset", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].offset,                                
                                "dimension", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
								"type", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].type);
                                
	xmlrpc_array_append_item(env, value, symbol);
  }

  return value;
}


xmlrpc_value * tsp_request_sample_xmlrpc (xmlrpc_env *env, xmlrpc_value *param_array, void *user_data)
{
  static TSP_answer_sample_t ans_sample = TSP_AS_STATIC_INITIALIZER;
  TSP_request_sample_t *req_sample;
  xmlrpc_value *xmlrpc_ans_sample;
  xmlrpc_value *value;
  int i;

  req_sample = xmlrpc_value_to_TSP_request_sample(env, param_array);

  TSP_provider_request_sample(req_sample, &ans_sample);

  /* Build TSP_answer_sample_t xmlrpc */
  
  value = xmlrpc_build_value (env, "()");
  
  xmlrpc_ans_sample = xmlrpc_build_value(env, "{s:i,s:i,s:i,s:i,s:d,s:i,s:i,s:i,s:i,s:i}",
										 "version_id", ans_sample.version_id,
										 "channel_id", ans_sample.channel_id,
										 "provider_timeout", ans_sample.provider_timeout,
										 "provider_group_number", ans_sample.provider_group_number,
										 "base_frequency", ans_sample.base_frequency,
										 "max_period", ans_sample.max_period,
										 "max_client_number", ans_sample.max_client_number,
										 "current_client_number", ans_sample.current_client_number,
										 "status", ans_sample.status,
										 "symbols.TSP_sample_symbol_info_list_t_len", ans_sample.symbols.TSP_sample_symbol_info_list_t_len);
  xmlrpc_array_append_item(env, value, xmlrpc_ans_sample);
  
  for (i=0; i < ans_sample.symbols.TSP_sample_symbol_info_list_t_len; i++) {
	xmlrpc_value *symbol;


	symbol = xmlrpc_build_value(env, "{s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
								"name", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].name,
								"provider_global_index", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
								"provider_group_index", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
								"provider_group_rank", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank,
								/* "xdr_tsp_t", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t, */
								"dimension", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
								"period", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].period,
                                "phase", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].phase,
                                "nelem", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].nelem,
                                "offset", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].offset,
                                "dimension", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
								"type", ans_sample.symbols.TSP_sample_symbol_info_list_t_val[i].type);
	xmlrpc_array_append_item(env, value, symbol);
  }

	
  return value;

}
 
xmlrpc_value * 
tsp_request_sample_init_xmlrpc (xmlrpc_env *env, 
								xmlrpc_value *param_array, 
								void *user_data)
{
  /* TSP_answer_sample_init_t* tsp_request_sample_init_xmlrpc(TSP_request_sample_init_t req_sample, struct svc_req * rqstp) */

  TSP_answer_sample_init_t ans_sample_init;
  TSP_request_sample_init_t *req_sample_init;
  xmlrpc_value *xr_ans_sample_init;

  req_sample_init = xmlrpc_value_to_TSP_request_sample_init(env, param_array);

  /*TBD FIXME Desallouer l'appel precedent*/
  /*if( 0 != ans_sample)
    {
    TSP_session_free_create_symbols_table_call(&ans_sample);
    }*/
    
  TSP_provider_request_sample_init(req_sample_init, &ans_sample_init);

  xr_ans_sample_init = TSP_answer_sample_init_to_xmlrpc_value(env, &ans_sample_init);

  return xr_ans_sample_init;

}




xmlrpc_value * 
tsp_request_sample_destroy_xmlrpc (xmlrpc_env *env, 
								   xmlrpc_value *param_array, 
								   void *user_data)
{
  TSP_request_sample_destroy_t *req_sample_destroy;
  TSP_answer_sample_destroy_t ans_sample_destroy;
  xmlrpc_value *xr_ans_sample_destroy;

  req_sample_destroy = xmlrpc_value_to_TSP_request_sample_destroy(env, param_array);

  TSP_provider_request_sample_destroy(req_sample_destroy, &ans_sample_destroy);

  xr_ans_sample_destroy = TSP_answer_sample_destroy_to_xmlrpc_value(env, &ans_sample_destroy);
	
  return xr_ans_sample_destroy;

}

/*==================================================================*/

int TSP_xmlrpc_request(TSP_provider_request_handler_t* this)
{
  this->config             = TSP_xmlrpc_request_config;
  this->run                = TSP_xmlrpc_request_run;
  this->stop               = TSP_xmlrpc_request_stop;
  this->url                = TSP_xmlrpc_request_url;
  this->tid                = (pthread_t)-1;

  this->config_param       = calloc(1, sizeof(TSP_xmlrpc_request_config_t));    

  this->status             = TSP_RQH_STATUS_IDLE;
  return TRUE;
}

static clean_exit() 
{
  _exit(0);
}

static void TSP_xmlrpc_run(TSP_xmlrpc_request_config_t *config)
{
  xmlrpc_server_abyss_parms serverparm;
  xmlrpc_registry * registry;
  xmlrpc_env env;

  STRACE_DEBUG(("Launched Abyss server"));

  atexit(clean_exit);

  xmlrpc_env_init(&env);
  die_if_fault_occurred(&env);

  registry = xmlrpc_registry_new(&env);
  die_if_fault_occurred(&env);

  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_provider_information", 
							 tsp_provider_information_xmlrpc, 
							 NULL);

  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_close", 
							 tsp_request_close_xmlrpc, 
							 NULL);
  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_open", 
							 tsp_request_open_xmlrpc, 
							 NULL);
  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_information", 
							 tsp_request_information_xmlrpc, 
							 NULL);
  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_sample", 
							 tsp_request_sample_xmlrpc, 
							 NULL);
  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_sample_init", 
							 tsp_request_sample_init_xmlrpc, 
							 NULL);
  xmlrpc_registry_add_method(&env,
							 registry,
							 NULL,
							 "tsp.tsp_request_sample_destroy", 
							 tsp_request_sample_destroy_xmlrpc, 
							 NULL);


  serverparm.config_file_name = NULL;
  serverparm.registryP = registry;
  serverparm.port_number = XMLRPC_CONFIG_PORT;
  //TODO: make it configurable
  serverparm.log_file_name = "/dev/stderr";

  STRACE_INFO(("Starting abyss server..."));
  xmlrpc_server_abyss(&env, &serverparm, XMLRPC_APSIZE(log_file_name));
  STRACE_ERROR(("Abyss server returned"));
  return;
}

int TSP_xmlrpc_request_config(TSP_provider_request_handler_t* this)
{



  return TRUE;
} /* end of TSP_xmlrpc_request_config */



void* TSP_xmlrpc_request_run(TSP_provider_request_handler_t* this)
{

  TSP_xmlrpc_request_config_t *config = (TSP_xmlrpc_request_config_t *)(this->config_param);

  if(config->server_number >= 0) {
	this->status = TSP_RQH_STATUS_RUNNING;
	TSP_xmlrpc_run(config);
	this->status = TSP_RQH_STATUS_IDLE;
  }

  return (void*)NULL;
} /* end of TSP_xmlrpc_request_run */

char *TSP_xmlrpc_request_url(TSP_provider_request_handler_t* this)
{
  TSP_xmlrpc_request_config_t *config = (TSP_xmlrpc_request_config_t*)(this->config_param);
  if(this->status == TSP_RQH_STATUS_RUNNING)
    return config->url;
  else
    return NULL;
}

int TSP_xmlrpc_request_stop(TSP_provider_request_handler_t* this)
{
  TSP_xmlrpc_request_config_t *config = (TSP_xmlrpc_request_config_t*)(this->config_param);
  
  this->status = TSP_RQH_STATUS_STOPPED;

  return TRUE;
} /* end of TSP_rpc_request_stop */

