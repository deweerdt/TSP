/*

$Id: server_main.c,v 1.12 2006-10-18 14:51:09 erk Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server, for stub test
	    Allow the output of a datapool of 1000 symbols, 
	    cyclically generated at 100Hz

-----------------------------------------------------------------------
*/

#include <signal.h>

#include "tsp_provider_init.h"

#if defined (WIN32)
#include "tsp_time.h"
typedef unsigned long sigset_t;
#endif

GLU_handle_t* GLU_stub_create();

#if defined (WIN32)
static int sigint_reveived = 0;
/* intercept signal function : call once */
void intrpt(int signum)
{
    /* managed the SIGINT signal */
    (void) signal(SIGINT, SIG_DFL);
    /* End of the waiting */
    sigint_reveived = 1;
    /* could be modified to use CreateEvent, SetEvent and WaitForSingleObject functions */
}
#endif

int main(int argc, char *argv[])
{
  GLU_handle_t* GLU_stub;

/* Managing thr SIGINT signal */
#if defined (WIN32)
  /* intercept SIGINT signal */
  if ( SIG_ERR == signal(SIGINT, intrpt))
  {
    STRACE_ERROR(("Error initialisation signal intercept function"));
    exit(1);
  }
#else
  sigset_t allsigs;
  int whatsig;

  /* Allows the SIGINT signal */
  /* so the sigwait function may stop on a SIGINT signal */
  sigemptyset(&allsigs);
  sigaddset(&allsigs, SIGINT);
  sigprocmask(SIG_BLOCK, &allsigs, NULL);
#endif

  printf ("#===================================================================#\n");
  printf ("# Launching <StubbedServer> for generation of 1000 Symbols at 100Hz #\n");
  printf ("#===================================================================#\n");

  GLU_stub = GLU_stub_create();

  /* Init server */
  if(TSP_STATUS_OK==TSP_provider_init(GLU_stub,&argc, &argv)) {
    if (TSP_STATUS_OK!=TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING)) {
        return -1;
    }
    TSP_provider_urls(TSP_PUBLISH_URLS_PRINT | TSP_PUBLISH_URLS_FILE);
#if defined (WIN32)
    /* Wait until the intercept signal function is call */
    while (!sigint_reveived)
    {
        tsp_usleep(1000);
    }
#else
    /* wait until a SIGINT signal */
    sigwait(&allsigs, &whatsig);
#endif
    TSP_provider_end();
  }

  printf("#=== End ===#\n");
  return 0;
}
  

   
