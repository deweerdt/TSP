/*

$Id: server_main.c,v 1.18 2008-08-02 11:01:47 deweerdt Exp $

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

#if defined (_WIN32)
#include  <stdio.h>
#include "tsp_time.h"
#include  "getopt.h"

#define assert(exp)     ((void)0)

typedef unsigned long sigset_t;

#else
    #include <unistd.h>
    #include <assert.h>
#endif

GLU_handle_t* GLU_stub_create(double baseFrequency);

#if defined (_WIN32)
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

/**
 * @defgroup TSP_StubbedServer Stubbed Provider
 * A Simple TSP provider producing 100 symbols at specified base frequency.
 *
 * \par tsp_stub_server [-h] [-f frequency (in HZ)]
 * \par 
 * <ul>
 *   <li> \b -h  (optional) print command line help.</li>
 *   <li> \b -f  (optional) produce symbols at specified frequency in Hz. Default is 100 Hz.</li>
 * </ul>
 * @ingroup TSP_Providers
 */

void 
stub_usage(const char* me) {
  printf("%s is a (very) simple TSP provider which produce 1000 Symbols at specified frequency.\n",me);
  printf("With no args %s will produce 1000 Symvols at 100 Hz\n",me); 
 
  printf("Usage: %s [-h] [-f frequency]\n",me);
  printf("   -h           (optional) print command line help.\n");
  printf("   -f frequency (optional) produce symbols at specified frequency in Hz. Default is 100 Hz.\n");
}

int 
main(int argc, char *argv[])
{
  GLU_handle_t* GLU_stub;
  int           opt_ok=1;
  int           c_opt;
  double        baseFrequency = 100.0; /* default frequency is 100Hz */

/* Managing the SIGINT signal */
#if defined (_WIN32)
  /* intercept SIGINT signal */
  if ( SIG_ERR == signal(SIGINT, intrpt))
  {
    STRACE_ERROR("Error initialisation signal intercept function");
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

    /* Analyse command line parameters */
  c_opt = getopt(argc,argv,"f:h");

  if(opt_ok && EOF != c_opt) {
      opt_ok  = 1;
    do { 
      switch (c_opt) {
      case 'f':
	baseFrequency = atof(optarg);
	break;
      case 'h':
	opt_ok = 0;
	break;
      case '?':
	fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
	opt_ok = 0;
	break;
      default:
	opt_ok = 0;
	break;
      } /* end of switch */  
      c_opt = getopt(argc,argv,"u:p:n:s:ht");  
    }
    while (opt_ok && (EOF != c_opt));
  }

  if (!opt_ok) {
    stub_usage(argv[0]);
    exit(-1);
  } else {
    printf ("#=====================================================================#\n");
    printf ("# Launching <StubbedServer> for generation of 1000 Symbols at %f Hz    \n",baseFrequency);
    printf ("#=====================================================================#\n");
  }

  /* Create our structured GLU callbacks */
  GLU_stub = GLU_stub_create(baseFrequency);

  /* Initialize TSP Provider library and register OUR GLU object
   * so that the TSP core knows it and is able
   * to call appropriate callback GLU member functions.
   */
  if(TSP_STATUS_OK==TSP_provider_init(GLU_stub,&argc, &argv)) {
    /* configure TSP request handling SIMPLE and NON BLOCKING mode and
     * 
     * Start TSP request handling loop
     * In this case the function will not return
     * until the program is interrupted (Ctrl-C).
     *
     * Provider run will:
     *    1- Call GLU-->initialize()
     *    2- Start a thread running GLU-->run()
     *    3- Start TSP request handler   
     */
    if (TSP_STATUS_OK!=TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING)) {
        return -1;
    }
    /* print out TSP URL */
    TSP_provider_urls(TSP_PUBLISH_URLS_PRINT | TSP_PUBLISH_URLS_FILE);
#if defined (_WIN32)
    /* Wait until the intercept signal function is call */
    while (!sigint_reveived)
    {
        tsp_usleep(1000);
    }
#else
    /* wait until a SIGINT signal */
    sigwait(&allsigs, &whatsig);
#endif
    /* Terminate TSP Provider library */
    TSP_provider_end();
    
    /* * * NO TSP_xxx functions may be called after this call * * */
  }

  printf("#=== End ===#\n");
  return 0;
}
  

   
