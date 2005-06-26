/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.c,v 1.22 2005-06-26 16:01:47 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 
Copyright (c) 2005 Cesare BERTONA

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

#include "tsp_sys_headers.h"

/* FIXME a quoi sert ce define ? */
#define PORTMAP
#include <rpc/rpc.h>
#include <netdb.h>
#include <rpc/pmap_clnt.h>
#include <stdio.h>
#include <unistd.h>


/* FIXME RP : beurk, RPC is compiled before CTRL could export this include, how sould I call Request Manager and GLU then ? */
#include "../ctrl/tsp_provider.h"
#include "../ctrl/tsp_request.h"
#include "../ctrl/glue_sserver.h"

#include "tsp_server.h"
#include "tsp_rpc.h"
#include "tsp_rpc_confprogid.h"

void
tsp_rpc_1(struct svc_req *rqstp, register SVCXPRT *transp) ;


#ifndef TSP_RPC_CLEANUP

#define TSP_URL_MAXLENGTH 256

typedef struct {
  int  server_number;
  char url[TSP_URL_MAXLENGTH];
  SVCXPRT *xprt;
} TSP_rpc_request_config_t;

TSP_provider_info_t* tsp_provider_information_1_svc(struct svc_req *rqstp)
{
  static TSP_provider_info_t server_info;	
  STRACE_IO(("-->IN"));    
  server_info.info = GLU_get_server_name();    
  STRACE_IO(("-->OUT"));
  return &server_info;
}

TSP_answer_open_t* tsp_request_open_1_svc(TSP_request_open_t req_open, struct svc_req* rqstp)
{

  static TSP_answer_open_t ans_open;
	
  STRACE_IO(("-->IN"));	
  TSP_provider_request_open(&req_open, &ans_open);	
  STRACE_IO(("-->OUT"));	
  return &ans_open;

}

void *tsp_request_close_1_svc(TSP_request_close_t req_close, struct svc_req * rqstp)
{

  static char* dummy;
	
  STRACE_IO(("-->IN"));

  TSP_provider_request_close(&req_close);
	
  STRACE_IO(("-->OUT"));
 
  return ((void*) &dummy);
}

TSP_answer_sample_t* tsp_request_information_1_svc(TSP_request_information_t req_info, struct svc_req * rqstp)
{

  static TSP_answer_sample_t ans_sample;
	
  STRACE_IO(("-->IN"));	
  TSP_provider_request_information(&req_info, &ans_sample);
  STRACE_IO(("-->OUT"));       
  return &ans_sample;

}

TSP_answer_feature_t* tsp_request_feature_1_svc(TSP_request_feature_t req_feature, struct svc_req * rqstp)
{

  static TSP_answer_feature_t ans_feature;
	
  STRACE_IO(("-->IN"));
  STRACE_IO(("-->OUT"));
	
  return &ans_feature;

}

TSP_answer_sample_t* tsp_request_sample_1_svc(TSP_request_sample_t req_sample, struct svc_req * rqstp)
{

  static int first_call = TRUE;
  static TSP_answer_sample_t ans_sample;

  STRACE_IO(("-->IN"));
  /* For each call memory was allocate by TSP_provider_request_sample */
  if(!first_call)
    {
      TSP_provider_request_sample_free_call(&ans_sample);    
    }
  first_call = FALSE;

  TSP_provider_request_sample(&req_sample, &ans_sample);

  STRACE_IO(("-->OUT"));
	
  return &ans_sample;

}
 
TSP_answer_sample_init_t* tsp_request_sample_init_1_svc(TSP_request_sample_init_t req_sample, struct svc_req * rqstp)
{

  static TSP_answer_sample_init_t ans_sample;

  STRACE_IO(("-->IN"));
    
  /*TBD FIXME Desallouer l'appel precedent*/
  /*if( 0 != ans_sample)
    {
    TSP_session_free_create_symbols_table_call(&ans_sample);
    }*/
    
  TSP_provider_request_sample_init(&req_sample, &ans_sample);
  STRACE_IO(("-->OUT"));

  return &ans_sample;

}

TSP_answer_sample_destroy_t* tsp_request_sample_destroy_1_svc(TSP_request_sample_destroy_t req_sample, struct svc_req * rqstp)
{

  static TSP_answer_sample_destroy_t ans_sample;
	
  STRACE_IO(("-->IN"));
  TSP_provider_request_sample_destroy(&req_sample, &ans_sample);
  STRACE_IO(("-->OUT"));

	
  return &ans_sample;

}

void* tsp_exec_feature_1_svc(TSP_exec_feature_t exec_feature, struct svc_req * rqstp)
{
	
  STRACE_IO(("-->IN"));

  STRACE_IO(("-->OUT"));

  return (void*)NULL;
}


static int TSP_rpc_init(TSP_rpc_request_config_t *config)
{
  int rpcport = -1;

  STRACE_IO(("-->IN"));

  /* look for a free port */
  while(rpcport && config->server_number<TSP_MAX_SERVER_NUMBER)
    {
      rpcport = getrpcport("localhost", TSP_get_progid(config->server_number), TSP_RPC_VERSION_INITIAL, IPPROTO_TCP);
      if(rpcport)
	config->server_number++;
    }

  if(rpcport && config->server_number >= TSP_MAX_SERVER_NUMBER)
      config->server_number = -1;

  STRACE_IO(("-->OUT "));

  return config->server_number;
}

static void TSP_rpc_run(TSP_rpc_request_config_t *config)
{
  int32_t rpc_progid = TSP_get_progid(config->server_number);

  /* svc_create does not exist for linux, we must use the deprecated function */
  /* pmap_unset (rpc_progid, TSP_RPC_VERSION_INITIAL); */

  config->xprt = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (config->xprt == NULL) 
    {
      STRACE_ERROR(("Cannot create TCP service"));
      return;
    }

  if (!svc_register(config->xprt, rpc_progid, TSP_RPC_VERSION_INITIAL, tsp_rpc_1, IPPROTO_TCP))
    {
      STRACE_ERROR(("RPC server unable to register ProgId=%X",  rpc_progid));
      return;
    }

  STRACE_DEBUG(("RPC server is being be started with ProgId=%X", rpc_progid));
  
  STRACE_DEBUG(("launching svc_run..."));
  svc_run();
  STRACE_INFO(("svc_run returned"));

}

static void TSP_rpc_stop(TSP_rpc_request_config_t *config)
{
  /* STRACE_DEBUG(("calling svc_exit...")); */
  /* svc_destroy(config->xprt); */

  /* Clean-up Port map so that next provider could use this ProgId */
  if(config->server_number >= 0)
    {
      svc_unregister(TSP_get_progid(config->server_number),TSP_RPC_VERSION_INITIAL);
      pmap_unset (TSP_get_progid(config->server_number), TSP_RPC_VERSION_INITIAL);
    }
}

/*==================================================================*/

int TSP_rpc_request(TSP_provider_request_handler_t* this)
{
  this->config             = TSP_rpc_request_config;
  this->run                = TSP_rpc_request_run;
  this->stop               = TSP_rpc_request_stop;
  this->url                = TSP_rpc_request_url;
  this->tid                = (pthread_t)-1;

  this->config_param       = calloc(1, sizeof(TSP_rpc_request_config_t));

  this->status             = TSP_RQH_STATUS_IDLE;
  return TRUE;
}

int TSP_rpc_request_config(TSP_provider_request_handler_t* this)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t *)(this->config_param);
  char hostname[MAXHOSTNAMELEN], *servername;

  config->server_number = TSP_provider_get_server_base_number();
  /* do not buffer overflow */
  memset(&config->url[0],'\0',TSP_URL_MAXLENGTH);
  
  config->server_number = TSP_rpc_init(config);
  if(config->server_number < 0)
    {
      STRACE_ERROR(("unable to register any RPC progid :\n\tcheck RPC daemons, or use tsp_rpc_cleanup to clean-up all TSP RPC port mapping"));
      this->status = TSP_RQH_STATUS_IDLE;

      return FALSE;
    }
  else
    {
      gethostname(hostname, MAXHOSTNAMELEN);
      servername = GLU_get_server_name();
      
      sprintf(config->url, /* TSP_URL_MAXLENGTH, pour snprintf quand Solaris 2.5 sera mort */
	       TSP_URL_FORMAT, TSP_RPC_PROTOCOL, hostname, servername, config->server_number);

      this->status = TSP_RQH_STATUS_CONFIGURED;

      return TRUE;
    }

} /* end of TSP_rpc_request_config */



void* TSP_rpc_request_run(TSP_provider_request_handler_t* this)
{

  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t *)(this->config_param);

  STRACE_IO(("-->IN"));  
 
  /* pthread_detach(pthread_self()); */ /* FIXME shoudl we do this */ 

  if(config->server_number >= 0)
    {
      this->status = TSP_RQH_STATUS_RUNNING;
      TSP_rpc_run(config);
      this->status = TSP_RQH_STATUS_IDLE;
   }

  STRACE_IO(("-->OUT"));

  return (void*)NULL;
} /* end of TSP_rpc_request_run */

char *TSP_rpc_request_url(TSP_provider_request_handler_t* this)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t*)(this->config_param);
  if(this->status == TSP_RQH_STATUS_RUNNING)
    return config->url;
  else
    return NULL;
}

int TSP_rpc_request_stop(TSP_provider_request_handler_t* this)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t*)(this->config_param);
  
  TSP_rpc_stop(config);
  this->status = TSP_RQH_STATUS_STOPPED;

  return TRUE;
} /* end of TSP_rpc_request_stop */


#else
int main(void)
{
  int servernumber;
  for(servernumber=0; servernumber<TSP_MAX_SERVER_NUMBER; servernumber++)
    {
      svc_unregister (TSP_get_progid(servernumber), TSP_RPC_VERSION_INITIAL);
      pmap_unset (TSP_get_progid(servernumber), TSP_RPC_VERSION_INITIAL);
    }
  return 0;
}
#endif /* TSP_RPC_CLEANUP */
