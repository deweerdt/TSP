/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.c,v 1.14 2004-09-23 16:11:57 tractobob Exp $

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

Purpose   : 

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"

#define PORTMAP
#include <rpc/rpc.h>
#include <netdb.h>

#include "tsp_server.h"

#include "tsp_rpc.h"
/*#include "glue_sserver.h"*/

/*#include "tsp_provider.h" */

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

}



void
tsp_rpc_1(struct svc_req *rqstp, register SVCXPRT *transp) ;

static int TSP_rpc_init(int server_number)
{
  int ret = TRUE;
  register SVCXPRT *transp;


  /* Create prog id */
  int32_t rpc_progid = TSP_get_progid(server_number);

  STRACE_IO(("-->IN server number=%d",server_number ));

#ifdef VXWORKS
  if(rpcTaskInit() == ERROR)
    ret = FALSE;
#endif


  /* svc_create does not exist for linux, we must use the deprecated function */

  pmap_unset (rpc_progid, TSP_RPC_VERSION_INITIAL);
	
  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (transp == NULL) 
    {
    STRACE_ERROR(("Cannot create TCP service"));
    ret = FALSE;
  }
  if (ret && !svc_register(transp,rpc_progid, TSP_RPC_VERSION_INITIAL, tsp_rpc_1, IPPROTO_TCP)) {
    STRACE_ERROR(("RPC server unable to register ProgId=%X",  rpc_progid));
  }

  if(ret)
    STRACE_DEBUG(("RPC server is ready to be started with ProgId=%X", rpc_progid));

  STRACE_IO(("-->OUT "));

  return ret;
}

static void TSP_rpc_run(void)
{
  STRACE_DEBUG(("launching svc_run..."));
  svc_run();
  STRACE_INFO(("svc_run returned"));
}

char* TSP_rpc_request_config(void** config_param)
{
  char *ret = NULL, hostname[MAXHOSTNAMELEN], *servername;
  int i, servernumber;
  TSP_rpc_request_config_t *config;

  STRACE_IO(("-->IN"));

  /* Recurse RPC init calls to find a free PROG ID,
     limited to TSP_MAX_SERVER_NUMBER */
  servernumber = TSP_provider_get_server_base_number();
  for(i=0; i<TSP_MAX_SERVER_NUMBER; i++)
    {
      /* when TSP_rpc_init returns TRUE, svc_run was executed and exited OK */
      if(TSP_rpc_init(servernumber + i))
	break;
    }
  if(i == TSP_MAX_SERVER_NUMBER)
    {
      STRACE_ERROR(("unable to register any RPC progid, check daemons"));
      *config_param = NULL;
    }
  else
    {
      config = calloc(1, sizeof(TSP_rpc_request_config_t));

      config->server_number = servernumber + i;
      gethostname(hostname, MAXHOSTNAMELEN);
      servername = GLU_get_server_name();
      sprintf(config->url, TSP_URL_FORMAT, TSP_RPC_PROTOCOL, hostname, servername, config->server_number);

      *config_param = (void*)config;
      ret = config->url;
    }
  STRACE_IO(("-->OUT"));
  return ret;

} /* end of TSP_rpc_request_config */


void *TSP_rpc_request_run(void* config_param)
{

  TSP_rpc_request_config_t *config = (TSP_rpc_request_config_t*) config_param;

  STRACE_IO(("-->IN"));  
 
  pthread_detach(pthread_self()); /* FIXME shoudl we do this */

  if(config && config->server_number >= 0)
    TSP_rpc_run();

  STRACE_IO(("-->OUT"));
  /* FIXME implements cond var notification */
} /* end of TSP_rpc_request_run */

int TSP_rpc_request_stop(void)
{

  int retval = TRUE;
  /* FIXME NOP */
  return retval;
} /* end of TSP_rpc_request_stop */

