/*!  \file 

$Header $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

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

Project   : TSP
Maintainer: tsp@astrium.eads.net
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>
#include <signal.h>


#include "bb_tsp_provider.h"
#include "tsp_provider_init.h"
#include "tsp_const_def.h"

#ifndef VXWORKS

int 
main (int argc, char ** argv) {

  sigset_t allsigs;
  int whatsig;

  sigemptyset(&allsigs);
  sigaddset(&allsigs, SIGINT);
  sigprocmask(SIG_BLOCK, &allsigs, NULL);

  if (argc<2) {
    fprintf(stderr, 
	    "TSP bb_provider (%s)\n",TSP_PROJECT_URL);
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage: %s <bbname>\n",argv[0]);
    exit(-1);
  }
  
  bb_tsp_provider_initialise(&argc,&argv,
			     TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING,
			     argv[1]);
  sigwait(&allsigs, &whatsig);
  bb_tsp_provider_finalize();
  
  return 0;
}

#else
#include <bb_core.h>
#include <bb_simple.h>
#include <bb_tools.h>


int launch_bb_vx_provider (char * bb_name) 
{
  int	argc = 2;
  char	**argv;

  argv = malloc(2*sizeof(char *));

  argv[0] = strdup("launch_bb_vx_provider");
  argv[1] = bb_name;

  bb_simple_synchro_config(1);  /* FIXME : try to use mode PROCESS  */
				/* 0 = BB_SIMPLE_SYNCHRO_PROCESS    */
				/* 1 = BB_SIMPLE_SYNCHRO_THREAD     */

  bb_tsp_provider_initialise(&argc,&argv,
			     TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING,
			     argv[1]);
  return 0;
}

int debug_bb(char * cmd, char * bb_name, char * sym_name, char * value)
{
  bbtools_request_t  the_request;


  the_request.argv[0] = bb_name;
  the_request.verbose = 1;
  the_request.silent = 0;

  /* initialize bbtools request */
  bbtools_init(&the_request);
  the_request.theBB=bbtools_checkbbname(the_request.argv[0]);
  
  if(strcmp("READ", cmd) == 0) {
    the_request.argc = 2;
    the_request.argv[1] = sym_name;
    bbtools_read(&the_request);
  } else if(strcmp("WRITE", cmd) == 0) {
    the_request.argc = 3;
    the_request.argv[1] = sym_name;
    the_request.argv[2] = value;
    bbtools_write(&the_request);
  } else if(strcmp("DUMP", cmd) == 0) {
    the_request.argc = 1;
    bbtools_dump(&the_request);
  }
  
  return 0;
}

#endif
