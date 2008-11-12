/*

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.c,v 1.39 2008-11-12 12:32:39 erk Exp $

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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNULesser General Public License for more details.

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
#include <stdio.h>
#ifndef WIN32
    #include <netdb.h>
    #include <unistd.h> 
    #include <rpc/pmap_clnt.h>
#else
    #include <rpc/pmap_cln.h>
#endif

#include <tsp_sys_headers.h>
#include <tsp_glu.h>
/* 
 * FIXME RP : beurk, RPC is compiled before CTRL 
 * could export this include, how should I call 
 * Request Manager and GLU then ? 
 * All our troubles comes from using tsp_rpc.h
 * in tsp_datastruct.h
 * As soon as we have clean IDL generator
 * we will get rid of those circular include problem
 * We need 2 passes compile here, 
 * the first for IDL and the second for C compilation.
 */
#include "../ctrl/tsp_provider.h"
#include "../ctrl/tsp_request_handler.h"
/* same FIXME IDL <--> C dependency */
#include "../common/tsp_common_macros.h"

#include <tsp_server.h>
#include <tsp_rpc.h>
#include <tsp_rpc_confprogid.h>

void
tsp_rpc_1(struct svc_req *rqstp, register SVCXPRT *transp) ;

#define TSP_URL_MAXLENGTH 256

typedef struct {
  int  server_number;
  char url[TSP_URL_MAXLENGTH];
  SVCXPRT *xprt;
} TSP_rpc_request_config_t;

TSP_provider_info_t* tsp_provider_information_1_svc(struct svc_req *rqstp)
{
  static TSP_provider_info_t server_info;	
  server_info.info = (char*) TSP_provider_get_name();    
  return &server_info;
}

TSP_answer_open_t* tsp_request_open_1_svc(TSP_request_open_t req_open, struct svc_req* rqstp)
{

  static TSP_answer_open_t ans_open;
	
  TSP_provider_request_open(&req_open, &ans_open);	
  return &ans_open;

}

int *tsp_request_close_1_svc(TSP_request_close_t req_close, struct svc_req * rqstp)
{

  static int retcode = 0;
	
  TSP_provider_request_close(&req_close);
	
  return &retcode;
}

TSP_answer_sample_t* tsp_request_information_1_svc(TSP_request_information_t req_info, struct svc_req * rqstp)
{

  static TSP_answer_sample_t ans_sample;
	
  TSP_provider_request_information(&req_info, &ans_sample);
  return &ans_sample;

}

TSP_answer_sample_t* tsp_request_filtered_information_1_svc(TSP_request_information_t req_info, int filter_kind, char* filter_string, struct svc_req * rqstp)
{

  static TSP_answer_sample_t ans_sample;
	
  TSP_provider_request_filtered_information(&req_info, filter_kind, filter_string, &ans_sample);
  return &ans_sample;

}

TSP_answer_feature_t* tsp_request_feature_1_svc(TSP_request_feature_t req_feature, struct svc_req * rqstp)
{

  static TSP_answer_feature_t ans_feature;
	
  return &ans_feature;

}

TSP_answer_sample_t* 
tsp_request_sample_1_svc(TSP_request_sample_t req_sample, struct svc_req * rqstp) {

  static TSP_answer_sample_t as = TSP_AS_STATIC_INITIALIZER ;

  TSP_provider_request_sample(&req_sample, &as);
  return &as;
}
 
TSP_answer_sample_init_t* 
tsp_request_sample_init_1_svc(TSP_request_sample_init_t req_sample, struct svc_req * rqstp) {
  
  static TSP_answer_sample_init_t asi = TSP_ASI_STATIC_INITIALIZER;

  TSP_provider_request_sample_init(&req_sample, &asi);

  return &asi;
}

TSP_answer_sample_destroy_t* tsp_request_sample_destroy_1_svc(TSP_request_sample_destroy_t req_sample, struct svc_req * rqstp)
{

  static TSP_answer_sample_destroy_t ans_sample = TSP_ASD_STATIC_INITIALIZER;
	
  TSP_provider_request_sample_destroy(&req_sample, &ans_sample);
	
  return &ans_sample;

}

int* tsp_exec_feature_1_svc(TSP_exec_feature_t exec_feature, struct svc_req * rqstp)
{

  static int retcode = 0;	

  return &retcode;
}

int * tsp_request_async_sample_write_1_svc(TSP_async_sample_t async_sample_write, struct svc_req * rqstp)
{
  static int ret = TRUE;
  

  /* endianity managment*/
/*   STRACE_DEBUG("Len=%d,  pgi = %d value = %f et ret = %d", async_sample_write.data.data_len, async_sample_write.provider_global_index,(double*)(async_sample_write.data.data_val),ret); */

/*   switch (async_sample_write.data.data_len) { */
/*   case 2:  */
/*       TSP_UINT16_FROM_BE(*(uint16_t*)async_sample_write.data.data_val); */
/*       break; */
/*   case 4:  */
/*       TSP_UINT32_FROM_BE(*(uint32_t*)async_sample_write.data.data_val); */
/*       break; */
/*   case 8:   */
/*       TSP_UINT64_FROM_BE(*(uint64_t*)async_sample_write.data.data_val); */
/*       break; */
/*   default: */
/*       break; */
/*   } */
	

  STRACE_DEBUG("TSP_SERVER Before async_write : pgi %d value %s return %d ",async_sample_write.provider_global_index,async_sample_write.data.data_val,ret);
  
  ret = TSP_provider_request_async_sample_write(&async_sample_write);	
  
  STRACE_DEBUG("TSP_SERVER After async_write : pgi %d value %s return %d ",async_sample_write.provider_global_index,async_sample_write.data.data_val,ret);

  return &ret;
  
}

TSP_async_sample_t * tsp_request_async_sample_read_1_svc(TSP_async_sample_t async_sample_read, struct svc_req * rqstp)
{
  static TSP_async_sample_t ret;
  
  /* endianity managment*/
/*   STRACE_DEBUG("Len=%d,  pgi = %d value = %f et ret = %d", async_sample_read.data.data_len, async_sample_read.provider_global_index,(double*)(async_sample_read.data.data_val),ret); */

/*   switch (async_sample_read.data.data_len) { */
/*   case 2:  */
/*       TSP_UINT16_FROM_BE(*(uint16_t*)async_sample_read.data.data_val); */
/*       break; */
/*   case 4:  */
/*       TSP_UINT32_FROM_BE(*(uint32_t*)async_sample_read.data.data_val); */
/*       break; */
/*   case 8:   */
/*       TSP_UINT64_FROM_BE(*(uint64_t*)async_sample_read.data.data_val); */
/*       break; */
/*   default: */
/*       break; */
/*   } */
	

  
  ret = async_sample_read;

  if (TSP_STATUS_OK!=TSP_provider_request_async_sample_read(&ret)) {
    ret.provider_global_index = -1;
  }	

  return &ret;
  
}


static int TSP_rpc_init(TSP_rpc_request_config_t *config)
{
  int rpcport = -1;

  /* look for a free port */
  while(rpcport && (config->server_number<TSP_MAX_SERVER_NUMBER))
    {
      rpcport = (int)getrpcport("localhost", TSP_get_progid(config->server_number), TSP_RPC_VERSION_INITIAL, IPPROTO_TCP);
      if(rpcport)
	    config->server_number++;
    }

  if(rpcport && (config->server_number >= TSP_MAX_SERVER_NUMBER))
      config->server_number = -1;

  return config->server_number;
}

#if !defined(__rtems__)
static void TSP_rpc_run(TSP_rpc_request_config_t *config)
{
  int32_t rpc_progid = TSP_get_progid(config->server_number);

  /* svc_create does not exist for linux, we must use the deprecated function */
  /* pmap_unset (rpc_progid, TSP_RPC_VERSION_INITIAL); */

  config->xprt = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (config->xprt == NULL) 
    {
      STRACE_ERROR("Cannot create TCP service");
      return;
    }

  if (!svc_register(config->xprt, rpc_progid, TSP_RPC_VERSION_INITIAL, tsp_rpc_1, IPPROTO_TCP))
    {
      STRACE_ERROR("RPC server unable to register ProgId=%X",  rpc_progid);
      return;
    }

  STRACE_DEBUG("RPC server is being be started with ProgId=%X", rpc_progid);
  
  STRACE_DEBUG("launching svc_run...");
  svc_run();
  STRACE_INFO("svc_run returned");

}
#else
static void TSP_rpc_run(TSP_rpc_request_config_t *config)
{
	int32_t rpc_progid = TSP_get_progid(config->server_number);
	int sock;
	struct sockaddr_in srv_addr;
	SVCXPRT *transp;
	bool_t status;

	/*
	 * Create socket
	 */
	sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock < 0) {
		perror ("socket creation problem");
		return 1;
	}

	/*
	 * Bind to local address
	 */
	srv_addr.sin_family = htons (AF_INET);
	srv_addr.sin_port = htons (TCP_PORT);
	srv_addr.sin_addr.s_addr = htonl (INADDR_ANY);
	if (bind (sock, (struct sockaddr *)&srv_addr, sizeof srv_addr) < 0) {
		STRACE_ERROR("RPC server unable to register ProgId=%X",  strerror(errno));
		return 2;
	}

	/*
	 * Create TCP server
	 */
	config->xprt = svctcp_create(sock, 0, 0);
	if (config->xprt == NULL) {
		STRACE_ERROR(("Cannot create TCP service"));
		return;
	}
	if (!svc_register(config->xprt, rpc_progid, TSP_RPC_VERSION_INITIAL, tsp_rpc_1, 0))
	{
		STRACE_ERROR("RPC server unable to register ProgId=%X",  rpc_progid);
		return;
	}
	STRACE_DEBUG(("RPC server is being be started with ProgId=%X", rpc_progid));

	STRACE_DEBUG("launching svc_run...");
	svc_run();
	STRACE_INFO("svc_run returned");
}
#endif /* !defined(__rtems__) */

static void TSP_rpc_stop(TSP_rpc_request_config_t *config)
{
  /* STRACE_DEBUG("calling svc_exit..."); */
  /* svc_destroy(config->xprt); */

  /* Clean-up Port map so that next provider could use this ProgId */
  if(config->server_number >= 0)
    {
      svc_unregister(TSP_get_progid(config->server_number),TSP_RPC_VERSION_INITIAL);
      pmap_unset (TSP_get_progid(config->server_number), TSP_RPC_VERSION_INITIAL);
    }
}

int TSP_rpc_request(TSP_provider_request_handler_t* cthis)
{
  cthis->config             = TSP_rpc_request_config;
  cthis->run                = TSP_rpc_request_run;
  cthis->stop               = TSP_rpc_request_stop;
  cthis->url                = TSP_rpc_request_url;
#if defined (_WIN32)
  /* structure pthread_t different under Windows */
  cthis->tid.p              = -1;
#else
  cthis->tid                = (pthread_t)-1;
#endif
  cthis->config_param       = calloc(1, sizeof(TSP_rpc_request_config_t));

  cthis->status             = TSP_RQH_STATUS_IDLE;
  return TRUE;
}

int TSP_rpc_request_config(TSP_provider_request_handler_t* cthis)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t *)(cthis->config_param);
  char  hostname[MAXHOSTNAMELEN+1];
  char* servername;
  int  hostnameOk;
#undef NUMERIC_HOST
#if NUMERIC_HOST
  struct hostent*  myhost;
  union {
    uint32_t    addr;
    uint8_t     parts[4];
  } myu;
#endif

  config->server_number = TSP_provider_get_server_base_number();
  /* do not buffer overflow */
  memset(&config->url[0],'\0',TSP_URL_MAXLENGTH);
  
  config->server_number = TSP_rpc_init(config);
  if(config->server_number < 0) {
      STRACE_ERROR("unable to register any RPC progid :\n\tcheck RPC daemons, or use tsp_rpc_cleanup to clean-up all TSP RPC port mapping");
      cthis->status = TSP_RQH_STATUS_IDLE;
      
      return FALSE;
  }
  else {
    hostnameOk = gethostname(hostname, MAXHOSTNAMELEN);
#if NUMERIC_HOST
    myhost = gethostbyname(hostname);
    if (myhost == NULL) {
      STRACE_ERROR("Cannot gethostbyname '(hostname --> @IP)' for host <%s> check your /etc/hosts file.",hostname);
      /* be tolerant we keep going with hostname but... consumer may not handle this properly */    
    } else {
     /* 
      * Now translate hostname to @IP in order
      * to avoid name -> IP resolution on consumer side 
      */
      memset(hostname,0,sizeof(hostname));
      myu.addr =*((uint32_t*)myhost->h_addr_list[0]);

      sprintf(hostname,"%d.%d.%d.%d",myu.parts[0], myu.parts[1], myu.parts[2], myu.parts[3]);

    }

#endif
    servername = (char*)TSP_provider_get_name();
    
    sprintf(config->url, /* TSP_URL_MAXLENGTH, pour snprintf quand Solaris 2.5 sera mort */
	    TSP_URL_FORMAT, TSP_RPC_PROTOCOL, hostname, servername, config->server_number);
    
    cthis->status = TSP_RQH_STATUS_CONFIGURED;
    
    return TRUE;
  }

} /* end of TSP_rpc_request_config */



void* TSP_rpc_request_run(void* cthis)
{
  TSP_provider_request_handler_t* rthis = (TSP_provider_request_handler_t*) cthis;
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t *)(rthis->config_param);

  /* pthread_detach(pthread_self()); */ /* FIXME shoudl we do this */ 

  if(config->server_number >= 0)
    {
      rthis->status = TSP_RQH_STATUS_RUNNING;
      TSP_rpc_run(config);
      rthis->status = TSP_RQH_STATUS_IDLE;
   }

  return (void*)NULL;
} /* end of TSP_rpc_request_run */

char *TSP_rpc_request_url(TSP_provider_request_handler_t* cthis)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t*)(cthis->config_param);
  if(cthis->status == TSP_RQH_STATUS_RUNNING)
    return config->url;
  else
    return NULL;
}

int TSP_rpc_request_stop(TSP_provider_request_handler_t* cthis)
{
  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t*)(cthis->config_param);
  
  TSP_rpc_stop(config);
  cthis->status = TSP_RQH_STATUS_STOPPED;

  return TRUE;
} /* end of TSP_rpc_request_stop */

                                   
TSP_answer_extended_information_t* tsp_request_extended_information_1_svc(TSP_request_extended_information_t req_extinfo, struct svc_req * rqstp)
{

  static TSP_answer_extended_information_t ans_extinfo;
	
  TSP_provider_request_extended_information(&req_extinfo, &ans_extinfo);
  return &ans_extinfo;

}

