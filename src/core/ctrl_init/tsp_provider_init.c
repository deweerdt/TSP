/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.c,v 1.8 2004-09-23 16:11:57 tractobob Exp $

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

Purpose   : Function calls to launch a TSP Provider program
            in the 'main'

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_provider.h"
#include "tsp_server.h"
#include "tsp_request.h"
#include "tsp_provider_init.h"

int TSP_provider_init(int* argc, char** argv[])
{

  int ret;
  STRACE_IO(("-->IN"));

  ret = TSP_provider_private_init(argc, argv);
  
  /* Initialize tsp request handlers structures */
  ret = TSP_provider_rqh_manager_init();
  STRACE_IO(("-->OUT"));
  return ret;

} /* End of TSP_provider_init */

int TSP_provider_run(int spawn_mode)
{
  int ret = FALSE;

  STRACE_IO(("-->IN"));
  
  
  if(TSP_provider_is_initialized())
    {            
      /* build and install default request handlers */
      TSP_provider_request_handler_t rqh;
      rqh.config = TSP_rpc_request_config;
      rqh.run    = TSP_rpc_request_run;
      rqh.stop   = TSP_rpc_request_stop;
      
      TSP_provider_rqh_manager_install(0,rqh);

      /*
       * un-comment this if you want to test
       * more then one RPC request handler
       * TSP_provider_rqh_manager_install(1,rqh);
       */
      
      ret = TSP_provider_rqh_manager_refresh();


      /* If we are launched in a blocking mode 
       * Wait for every request handler thread to terminate
       * !!! Thread MUST NOT DETACHED themslevs though !!!
       */
      if (spawn_mode) {
	TSP_provider_rqh_manager_waitend();
      }
    }
  else
    {
      STRACE_ERROR(("Call TSP_provider_init first, and then call TSP_provider_run ! "))
    }
  
  STRACE_IO(("-->OUT"));
  return ret;  
  
  } /* TSP_provider_run */


void TSP_provider_print_usage(void)
{
   printf(TSP_ARG_PROVIDER_USAGE"\n");
}

char *TSP_provider_url(int rank)
{
  /* Check running handlers and return their URLs */
  if(rank >= 0 && rank < TSP_provider_rqh_manager_get_nb_running())
    return  TSP_provider_rqh_manager_get(rank)->url;
  else
    return NULL;
}

