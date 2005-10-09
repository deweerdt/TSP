/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer_main.c,v 1.6 2005-10-09 23:01:23 erk Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : TSP ascii writer consumer (main)

-----------------------------------------------------------------------
 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "tsp_ascii_writer.h"
#include "tsp_consumer.h"


static void 
my_sighandler(int signum) {
  fprintf(stderr,"tsp_ascii_writer::Captured signal<%d>\n",signum);
  fflush(stderr);
  tsp_ascii_writer_stop();    
}
                                                                                                 
int
main (int argc, char* argv[]) {
  
  struct sigaction my_action;
  struct sigaction old_action;
  int32_t                               retcode=0;
  int32_t                               nb_symbols;
  TSP_consumer_symbol_requested_list_t  symbol_list;
  TSP_consumer_symbol_requested_t*      mysymbols;
  char*   input_filename  = NULL;
  char*   output_filename = NULL;
  FILE*   output_stream   = NULL;
  int32_t output_limit    = 0;
  char*   provider_url   = "rpc://localhost/";

  /* Main options handling */
  char*         errorString;
  int           opt_ok;
  char          c_opt;

  opt_ok            = 1;
    
  if (argc < 2) {
    opt_ok  = 0;
    retcode = -1;
    fprintf(stderr,"%s: Insufficient number of options\n",argv[0]);
  }

  /* Analyse command line parameters */
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"x:u:o:l:h")))) {    
    switch (c_opt) {
    case 'x':
      input_filename = strdup(optarg);
      fprintf(stdout,"%s: sample config file is <%s>\n",argv[0],input_filename);
      break;
    case 'o':
      output_filename = strdup(optarg);
      fprintf(stdout,"%s: output sample file is <%s>\n",argv[0],output_filename);
      break;
    case 'l':
      errorString = NULL;
      output_limit = strtol(optarg,&errorString,10);
      if ('\0' != *errorString) {
	fprintf(stderr,"%s: incorrect output limit format : <%s> (error begin at <%s>)\n",argv[0],optarg, errorString);
	opt_ok = 0;
      } else {
	fprintf(stderr,"%s: TSP sample output file limited to <%d> sample(s).\n",argv[0],output_limit);
      }
      break;
    case 'u':
      provider_url = strdup(optarg);
      fprintf(stdout,"%s: TSP provider URL is <%s>\n",argv[0],provider_url);
      break;
    case '?':
      fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
      opt_ok = 0;
      break;
    default:
      opt_ok = 0;
      break;
    } /* end of switch */    
  }

  if (!opt_ok) {
    printf("Usage: %s -x=<sample_config_file> [-o=<output_filename>] [-l=<nb sample>] [-u=<TSP_URL>]\n", argv[0]);
    printf("   -x   the file specifying the list of symbols to be sampled\n");
    printf("   -o   the name of the output file\n");
    printf("   -l   (optional) the maximum number of sample to be stored in file\n");
    printf("   -u   (optional) the  TSP provider URL <TSP_URL> \n");
    printf("%s",TSP_URL_FORMAT_USAGE);
    exit(retcode);
  }

  tsp_ascii_writer_initialise(&argc,&argv);
    
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  fprintf(stdout,"%s: Load config file...\n",argv[0]);
  retcode = tsp_ascii_writer_load_config(input_filename,&mysymbols,&nb_symbols);

  if (0!=retcode) {
    fprintf(stderr,"<%s>: Invalid configuration file (%d parse error(s)).",
	    input_filename, tsp_ascii_writer_parse_error);
    tsp_ascii_writer_stop();
  }

  if (0==retcode) {
    fprintf(stdout,"%s: Validate symbols against provider info...\n",argv[0]);
    fflush(stdout);
    retcode = tsp_ascii_writer_validate_symbols(mysymbols,nb_symbols,
						                                    provider_url,&symbol_list);
  }

  if (0==retcode) {
    fprintf(stdout,"%s: Ascii writer running...\n",argv[0]);
    fflush(stdout);  
    if (NULL == output_filename) {
      output_stream = stdout;
      output_limit  = 0;
    } else {
      output_stream = fopen(output_filename,"w");
      if ((FILE*)NULL == output_stream) {
	fprintf(stderr,"Cannot open output file <%s> for writing\n",output_filename);
	retcode = -1;
      }    
    }
  }
  
  if (0 == retcode) {
    retcode = tsp_ascii_writer_start(output_stream,output_limit);
  }

  fprintf(stdout,"%s: Ascii writer stopped...\n",argv[0]);
  fflush(stdout);  
  tsp_ascii_writer_finalise();

  return (retcode);
}
