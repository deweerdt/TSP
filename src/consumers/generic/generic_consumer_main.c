/*

$Id: generic_consumer_main.c,v 1.8 2006-04-13 22:28:25 erk Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Generic tsp consumer

-----------------------------------------------------------------------
*/
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_consumer.h"
#include "generic_consumer.h"

/**
 * @defgroup TSP_GenericConsumer Generic Consumer
 * The Generic consumer set of tools may be used
 * to test in a simple command line interface  
 * most of the TSP Asynchronous Requests.
 *
 * Just like @ref BBToolsCommandLine the generic consumer has a BusyBox-like
 * design ((<A HREF="http://www.busybox.net">http://www.busybox.net</A>).
 *
 * \par tsp_request_generic [generic_opts]  \<tsp_request\> [request_opts]
 * \par generic_opts
 * <ul> 
 *   <li> \b -u  (optional) TSP Provider URL. Default is localhost</li>
 *   <li> \b -s  (optional) silent mode (may be used for silent scripting) </li>
 *   <li> \b -v  (optional) verbose mode </li>
 *   <li> \b -n  (optional) no newline read mode </li>
 * </ul>
 * \par tsp_request
 * <ul>
 *   <li>   tsp_request_open </li>
 *   <li>   tsp_request_close </li>
 *   <li>   tsp_request_information </li>
 *   <li>   tsp_request_filtered_information </li>
 *   <li>   tsp_request_extended_information </li>
 *   <li>   tsp_request_feature </li>
 *   <li>   tsp_request_sample </li>
 *   <li>   tsp_request_sample_init </li>
 *   <li>   tsp_request_sample_destroy </li>
 *   <li>   tsp_request_async_sample_write </li>
 *   <li>   tsp_request_async_sample_read </li>      
 * </ul>
 * @ingroup TSP_Consumers
 */

static void 
my_sighandler(int signum) {
  fprintf(stderr,"%s::Captured signal<%d>\n",
	  "tsp_generic_consumer",
	  signum);
  fflush(stderr);
  exit(-1);
}

int 
main(int argc, char *argv[]){

  struct sigaction   my_action;
  struct sigaction   old_action;
  int32_t            retcode=0;
  /* Main options handling */
  /*  char*         error_string;*/
  int           opt_ok;
  char          c_opt;

  generic_consumer_request_t req;

  opt_ok            = 1;   
  
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  if(!TSP_consumer_init(&argc, &argv)) {
      STRACE_ERROR(("TSP init failed"));
      return -1;
  }

  generic_consumer_request_create(&req);

  /* Analyse command line parameters */
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"u:hnsv")))) {    
    switch (c_opt) {
    case 'u':
      opt_ok+=2;
      req.provider_url = strdup(optarg);
      break;
    case 'n':
      opt_ok++;
      req.newline[0] = ' ' ;
      break; 
    case 'v':
      opt_ok++;
      req.verbose = 1;
      break;
    case 's':
      opt_ok++;
      req.silent = 1;
      break;
    case 'h':
      opt_ok++;
      req.help = 1;
    case '?':
      fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
      opt_ok = 0;
      break;
    default:
      opt_ok = 0;
      break;
    } /* end of switch */
  } /* end of while */
     
  /* indicates number of global options to skip */
  req.nb_global_opt = opt_ok-1;
  req.argv          = argv;
  req.argc          = argc;

   /* check if global options are OK */
  if (!opt_ok) {
    req.stream = stderr;
    generic_consumer_usage(&req);
    exit(retcode); 
  }

  if ( (NULL!=req.provider_url) && (!req.silent)) {
    fprintf(stdout,"%s: TSP provider URL is <%s>\n",argv[0],req.provider_url);
  }
  /* invoke generic_consumer */
  retcode = generic_consumer(&req);

  return (retcode);
}  /* end of main */
