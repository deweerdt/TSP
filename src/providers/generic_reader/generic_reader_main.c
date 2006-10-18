/*

$Id: generic_reader_main.c,v 1.7 2006-10-18 09:58:48 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD and Arnaud MORVAN 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.TSP_provider_run(TSPRunMode)

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Allow the output of a datapool of symbols from generic file

-----------------------------------------------------------------------
*/

#include <stdlib.h>
#include <strings.h>
#include <signal.h>
#include <assert.h>
#include <sched.h>
#include <unistd.h>

#include <glu_genreader.h>
#include <generic_reader.h>
#include <tsp_provider_init.h>

/*
 * Ugly declare for smooth TSP startup
 * FIXME.
 */
int TSP_provider_rqh_manager_get_nb_running();


FmtHandler_T*	  fmt_handler;
GenericReader_T* generic_reader;

/**
 * @defgroup TSP_GenericReader Generic File Reader
 * The TSP 'GENERIC' file reader, this provider is reading
 * a file (parameters) providing the contained symbols as a TSP provider.
 * @ingroup TSP_Providers
 */

int main(int argc, char *argv[])
{
  char	 **my_argv;
  int 	 i,my_argc;
  int     opt_ok=1;
  int32_t retcode=0;

  sigset_t allsigs;
  int whatsig;

  sigemptyset(&allsigs);
  sigaddset(&allsigs, SIGINT);
  sigprocmask(SIG_BLOCK, &allsigs, NULL);

  printf ("#========================================================================#\n");
  printf ("# Launching <generic reader server> for generation of Symbols from a generic file #\n");
  printf ("#========================================================================#\n");
  
  if (argc < 2) 
  {
    opt_ok  = 0;
    retcode = -1;
    fprintf(stderr,"%s: Insufficient number of options\n",argv[0]);
    fprintf(stderr,"Usage: %s -x=<source_file> [-f <format]>\n", argv[0]);
    fprintf(stderr,"   -x   determine the source file\n");
    fprintf(stderr,"   -f   specifying the format of source file\n");
    return -1;
  }

  my_argc= argc+2;
  my_argv= (char**)calloc(my_argc, sizeof(char*));
  my_argv[0] = argv[0];
  my_argv[1] = "--tsp-stream-init-start";

  for (i=1; i<argc; i++)
  {
      my_argv[i+1]=argv[i];
  }
  my_argv[my_argc-1] = "--tsp-stream-init-stop";

  /* create a default GLU */
  GLU_handle_t* GLU_genreader = NULL;

  GLU_genreader= GENREADER_GLU_create();
  
  assert(GLU_genreader);


  if (TSP_STATUS_OK==TSP_provider_init(GLU_genreader,&my_argc, &my_argv)) 
  {
        
    /* Start provider */
    if (TSP_STATUS_OK!=TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING)) {
      free(my_argv);
      my_argv=NULL;
      return -1;
    }
    /* 
     * Wait for provider thread start-up
     * FIXME ce mode "d'attente" est pourlingue il faut une
     * API TSP pour gérer ces synchros de démarrage de thread.
     */
    sleep(1);
    sched_yield();  
    while (TSP_provider_rqh_manager_get_nb_running()<1) {
      printf("Waiting TSP provider to start...\n");
      fflush(stdout);
      sleep(1);
      sched_yield();    
    } 
    
    /* Print URL */
    TSP_provider_urls(TSP_PUBLISH_URLS_PRINT);
    /* Wait provider and through catched signals */
    sigwait(&allsigs, &whatsig);

    TSP_provider_end();
    retcode = TSP_STATUS_OK;    
  } 
  else 
  {
    free(my_argv);
    my_argv=NULL;
    /* TSP_provider_init FAILED */
    return -1;
  }

  free(my_argv);
  my_argv=NULL;

  return 0;
} /* end of main */

   
