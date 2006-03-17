/*

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer_main.c,v 1.9 2006-03-17 13:46:54 erk Exp $

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

/**
 * @defgroup TSP_AsciiWriter ASCII Writer
 * A TSP ascii writer consumer.
 * a TSP consumer which is able to output symbols values in different ASCII file format. 
 * It's output may be standard output or file with other options to chose file format and eventual 
 * size limit. It's main purposes is to be able to export TSP distributed symbols and value to 
 * some kind of CSV (Comma Separated Value) format in order to be easily post processed by 
 * spreadsheet softwares or simpler plotting software like 
 * <a href="http://www.gnuplot.org/">Gnuplot (http://www.gnuplot.org/)</a>.
 * You may specify different file format output which essentialy change the header of the file. 
 *
 * \par tsp_ascii_writer [-n] -x=\<sample_config_file\> [-o=\<output_filename\>] [-f=\<output file format>] [-l=\<nb sample\>] [-u TSP_provider URL ] 
 * \par 
 * <ul>
 *   <li> \b -n  (optional) will check and enforce no duplicate symbols</li>
 *   <li> \b -x  the file specifying the list of symbols to be sampled</li>
 *   <li> \b -f  (optional) specifying the format of output file. Recognized file format are
 *               <ul>
 *                 <li> \b simple_ascii  tabulated ascii no header</li>
 *                 <li> \b bach          tabulated ascii with BACH header</li>
 *                 <li> \b macsim        tabulated ascii with MACSIM header</li>
 *               </ul>
 *               Default is \b simple_ascii.
 *   </li>
 *   <li> \b -o  (optional) the name of the output file. If not specified standard out is used</li>
 *   <li> \b -l  (optional) the maximum number of sample to be stored in file. Unlimited if not specified.</li>
 *   <li> \b -u  (optional) the TSP provider URL. If not specified default is localhost.</li>
 * </ul>
 * @ingroup TSP_Consumers
 */


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
  char*   file_format     = NULL;
  FILE*   output_stream   = NULL;
  int32_t output_limit    = 0;
  char*   provider_url    = "rpc://localhost/";
  int32_t no_duplicate;

  /* set up default output file format */
  int header_style  		  = SimpleAsciiTabulated_FileFmt;

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
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"x:u:o:l:f:hn")))) {    
    switch (c_opt) {
    case 'x':
      input_filename = strdup(optarg);
      fprintf(stdout,"%s: sample config file is <%s>\n",argv[0],input_filename);
      break;
    case 'o':
      output_filename = strdup(optarg);
      fprintf(stdout,"%s: output sample file is <%s>\n",argv[0],output_filename);
      break;
    case 'f':
      file_format = strdup(optarg);
      fprintf(stdout,"%s: provided output file format is <%s>\n",argv[0],file_format);
      OutputFileFormat_t fmt;
      for (fmt=SimpleAsciiTabulated_FileFmt;fmt<LAST_FileFmt;++fmt) {
	if (strcasecmp(file_format,OutputFileFormat_name_tab[fmt])==0) {
	  header_style=fmt;
	  break;
	}
      }
      fprintf(stdout,"%s: selected output file format is <%s>\n",argv[0],OutputFileFormat_desc_tab[header_style]);
      break;
   /*-------------*/
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
    case 'n':
      no_duplicate = 1;
      fprintf(stdout,"%s: will enforce no duplicate symbol\n",argv[0]);
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
    printf("Usage: %s [-n] -x=<sample_config_file> [-o=<output_filename>] [-f=<output file format] [-l=<nb sample>] [-u=<TSP_URL>]\n", argv[0]);
    printf("   -n   will check and enforce no duplicate symbols\n");
    printf("   -x   the file specifying the list of symbols to be sampled\n");
    printf("   -f   (optional) specifying the format of output file\n");
    printf("        possible formats are:\n");
    printf("           simple_ascii : tabulated ascii no header \n");
    printf("           bach         : tabulated ascii with BACH header\n");
    printf("           macsim       : tabulated ascii with MACSIM header\n");
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

  if (no_duplicate) {
    retcode = tsp_ascii_writer_make_unique(&mysymbols,&nb_symbols);

    if (0!=retcode) {
      fprintf(stderr,"<%s>: configuration file contains duplicate symbols with different period, please correct and re-run.\n",
	      input_filename);
      fprintf(stderr,"Seems to be symbol <%s>\n",mysymbols[retcode].name);
      tsp_ascii_writer_stop();
    }
  }

  if (0==retcode) {
    fprintf(stdout,"%s: Validate symbols against provider info...\n",argv[0]);
    fflush(stdout);
    retcode = tsp_ascii_writer_validate_symbols(mysymbols,nb_symbols,provider_url,&symbol_list);
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
    retcode = tsp_ascii_writer_start(output_stream,output_limit,header_style);
  }

  fprintf(stdout,"%s: Ascii writer stopped...\n",argv[0]);
  fflush(stdout);  
  tsp_ascii_writer_finalise();

  return (retcode);
}
