/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.c,v 1.23 2008-02-05 18:54:10 rhdv Exp $

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
Component  : Provider

-----------------------------------------------------------------------

Purpose   : Function calls to launch a TSP Provider program
            in the 'main'

-----------------------------------------------------------------------
 */
#include <string.h>

#include <tsp_sys_headers.h>

#include <tsp_provider.h>
#include <tsp_request_handler.h>
#include <tsp_provider_init.h>

#ifdef BUILD_XMLRPC
#include <tsp_xmlrpc_server.h>
#endif
#include <tsp_server.h>


int32_t 
TSP_provider_init(GLU_handle_t* theGLU, int* argc, char** argv[]) {

#if defined (_WIN32)
  WSADATA WSAData;
#endif

  int32_t retcode = TSP_STATUS_OK;

#if defined (_WIN32)
  if (WSAStartup(MAKEWORD(2,2), &WSAData)) {
	WSACleanup();
	retcode = TSP_STATUS_NOK;
    return retcode;
   }
#endif

  retcode = TSP_provider_private_init(theGLU,argc, argv);
  
  /* Initialize tsp request handlers structures */
  if (TSP_STATUS_OK==retcode) {
    if (TRUE != TSP_provider_rqh_manager_init()) {
      retcode = TSP_STATUS_ERROR_UNKNOWN;
    } else {
      /* install atexit handler to close properly */
      atexit(TSP_provider_end);
    }  
  }

  return retcode;
} /* End of TSP_provider_init */

int32_t 
TSP_provider_run(int spawn_mode) {
  int32_t retcode = TSP_STATUS_OK;

  if (TSP_provider_is_initialized()) {            
      /* build and install default request handler (RPC) */
      TSP_provider_rqh_manager_install(0,TSP_rpc_request);
#ifdef BUILD_XMLRPC
      TSP_provider_rqh_manager_install(1,TSP_xmlrpc_request);
#endif



      if (spawn_mode & TSP_ASYNC_REQUEST_DYNAMIC) {
	/*
	 * un-comment this if you want to test
	 * more then one RPC request handler
	 * TSP_provider_rqh_manager_install(1,TSP_rpc_request);
	 */

	/* TODO : call TSP_provider_rqh_manager_install with new ranks
	   & other protocols (XML, CORBA, ...) */
      }

      if (TRUE != TSP_provider_rqh_manager_refresh()) {
	retcode = TSP_STATUS_ERROR_UNKNOWN;
      }
      
      if (TSP_STATUS_OK==retcode) {
	retcode = TSP_provider_private_run();
      }

      /* 
       * If we are launched in a blocking mode 
       * Wait for every request handler thread to terminate
       * !!! Thread MUST NOT DETACHED themselves though !!!
       */
      if ((TSP_STATUS_OK==retcode) && 
	  (spawn_mode & TSP_ASYNC_REQUEST_BLOCKING)
	  ) {
	TSP_provider_rqh_manager_waitend();
      }
    }
  else {
    STRACE_ERROR("Call TSP_provider_init first, and then call TSP_provider_run ! ");
    retcode = TSP_STATUS_ERROR_UNKNOWN;
  }
  
  return retcode;  
  
} /* TSP_provider_run */


void 
TSP_provider_end() {
  /* TODO : call flush on streams */

  /* remove any published URL */
  {
#if !defined (_WIN32)
    char systemCmd[512];
    sprintf(systemCmd, "rm -f /tmp/TSP.%d", getpid());
    system(systemCmd);
#endif
  }

  /* Call handlers end function */
  TSP_provider_rqh_manager_end();

#if defined (_WIN32)
    WSACleanup();
#endif
} /* end of TSP_provider_end */

void 
TSP_provider_print_usage() {
   printf(TSP_ARG_PROVIDER_USAGE"\n");
} /* TSP_provider_print_usage */

char**
TSP_provider_urls(int pub_mode) {
  int rank, nb;
  char **urls = NULL, *url;

  /* Check running handlers and copy their URLs */
  nb = TSP_provider_rqh_manager_get_nb_running();
  if(nb > 0)
    {     
      urls = (char**)calloc(nb,sizeof(char*));
      for(rank = 0; rank < nb; rank ++)
	{
	  url = TSP_provider_rqh_manager_get_url(rank);
	  if(url)
	    {
	      urls[rank] = strdup(url);

	      if(pub_mode & TSP_PUBLISH_URLS_PRINT)
		printf("TSP Provider on PID %d - URL #%d : <%s>\n", getpid(), rank, url);

	      if(pub_mode & TSP_PUBLISH_URLS_FILE)
		{
#if !defined (_WIN32)
		  char systemCmd[512];
		  sprintf(systemCmd, "echo \"%s\" > /tmp/TSP.%d", url, getpid());
		  system(systemCmd);
#endif
		}

	      if(pub_mode & TSP_PUBLISH_URLS_SNMP)
		/* TODO */
		STRACE_ERROR("SNMP publication of TSP URLs not yet implemented");
	    }
	  else
	    {
	      STRACE_ERROR("Request Handler #%d did not provided an URL", rank);
	      urls[rank] = strdup("");
	    }
	}
    }
  return urls;
} /* TSP_provider_urls */

