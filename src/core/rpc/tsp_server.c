/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.c,v 1.7 2002-11-29 17:41:18 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"


#include <pthread.h>
#define PORTMAP
#include <rpc/rpc.h>

#include "tsp_server.h"

#include "tsp_rpc.h"
/*#include "glue_sserver.h"*/

/*#include "tsp_provider.h" */

TSP_provider_info_t* tsp_provider_information_1_svc(struct svc_req *rqstp)
{
  
  SFUNC_NAME(tsp_provider_information_1_svc);

	
  static TSP_provider_info_t server_info;
	
  STRACE_IO(("-->IN"));

    
  server_info.info = GLU_get_server_name();
    
  STRACE_IO(("-->OUT"));

	

  return &server_info;
}

TSP_answer_open_t* tsp_request_open_1_svc(TSP_request_open_t req_open, struct svc_req* rqstp)
{
  SFUNC_NAME(tsp_request_open_1_svc);

  static TSP_answer_open_t ans_open;
	


  STRACE_IO(("-->IN"));

	
  TSP_provider_request_open(&req_open, &ans_open);
	
  STRACE_IO(("-->OUT"));

	
  return &ans_open;

}

void *tsp_request_close_1_svc(TSP_request_close_t req_close, struct svc_req * rqstp)
{
  SFUNC_NAME(tsp_request_close_1_svc);

  static char* dummy;
	
  STRACE_IO(("-->IN"));

  TSP_provider_request_close(&req_close);
	
  STRACE_IO(("-->OUT"));
 
  return ((void*) &dummy);
}

TSP_answer_sample_t* tsp_request_information_1_svc(TSP_request_information_t req_info, struct svc_req * rqstp)
{
  SFUNC_NAME(tsp_request_information_1_svc);

  static TSP_answer_sample_t ans_sample;
	
  STRACE_IO(("-->IN"));

	
  TSP_provider_request_information(&req_info, &ans_sample);
	
  STRACE_IO(("-->OUT"));

	
  return &ans_sample;

}

TSP_answer_feature_t* tsp_request_feature_1_svc(TSP_request_feature_t req_feature, struct svc_req * rqstp)
{

  SFUNC_NAME(tsp_request_feature_1_svc);

	
  static TSP_answer_feature_t ans_feature;
	
  STRACE_IO(("-->IN"));

  STRACE_IO(("-->OUT"));

	
  return &ans_feature;

}

TSP_answer_sample_t* tsp_request_sample_1_svc(TSP_request_sample_t req_sample, struct svc_req * rqstp)
{

  SFUNC_NAME(tsp_request_sample_1_svc);

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

  SFUNC_NAME(tsp_request_sample_init_1_svc);

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

  SFUNC_NAME(tsp_request_sample_destroy_1_svc);

  static TSP_answer_sample_destroy_t ans_sample;
	
  STRACE_IO(("-->IN"));

  TSP_provider_request_sample_destroy(&req_sample, &ans_sample);

  STRACE_IO(("-->OUT"));

	
  return &ans_sample;

}

void* tsp_exec_feature_1_svc(TSP_exec_feature_t exec_feature, struct svc_req * rqstp)
{

  SFUNC_NAME(tsp_exec_feature_1_svc);

	
  STRACE_IO(("-->IN"));

  STRACE_IO(("-->OUT"));

}

void
tsp_rpc_1(struct svc_req *rqstp, register SVCXPRT *transp);






static TSP_rpc_init(int server_number)
{
  SFUNC_NAME(TSP_command_init);
		
  int ret = TRUE;
  register SVCXPRT *transp;

  /* Create prog id */
  gint32 rpc_progid = TSP_get_progid(server_number);

  STRACE_IO(("-->IN server number=%d",server_number ));

  /* svc_create does not exist for linux, we must use the deprecated function */

  pmap_unset (rpc_progid, TSP_RPC_VERSION_INITIAL);
	
  transp = svctcp_create(RPC_ANYSOCK, 0, 0);
  if (transp == NULL) 
    {
    STRACE_ERROR(("Cannot create TCP service"));
    ret = FALSE;
  }
  if (ret && !svc_register(transp,rpc_progid,TSP_RPC_VERSION_INITIAL , tsp_rpc_1, IPPROTO_TCP)) {
    STRACE_ERROR(("unable to register (rpc_progid=%d, TSP_RPC_VERSION_INITIAL, tcp).",  rpc_progid))
    fprintf (stderr, "%s", "unable to register (TSP_RPC, TSP_RPC_VERSION_INITIAL, tcp).");
    ret = FALSE;
  }
/*#endif*/
	
  if (ret)
    {
      STRACE_INFO(("RPC server is going to be started with ProdId=%X",rpc_progid ));\
      STRACE_INFO(("launching svc_run..."));
      svc_run();
      STRACE_INFO(("svc_run returned"));
    }
	
  STRACE_IO(("-->OUT "));

	
  return ret;
}

static void* TSP_thread_rpc_init(void* arg)
{
  /* FIXME : creer le thread détaché */
    
  SFUNC_NAME( TSP_streamer_sender_thread_sender);
  int server_number = (int)arg;

  pthread_detach(pthread_self());
    
  STRACE_IO(("-->IN"));
 
   TSP_rpc_init(server_number);
      
  STRACE_IO(("-->OUT"));
}


int TSP_command_init(int server_number, int blocking)
{
  SFUNC_NAME(TSP_command_init);
  int ret = FALSE;



  STRACE_IO(("-->IN"));

  if(!blocking)
    {
      pthread_t thread_id;
      int status;
      status = pthread_create(&thread_id, NULL, TSP_thread_rpc_init,  (void*)server_number);
      TSP_CHECK_THREAD(status, FALSE);


    }
  else
    {
      ret = TSP_rpc_init(server_number);
    }

  STRACE_IO(("-->OUT"));

  return ret;
}
