/*

$Id: generic_reader_main.c,v 1.1 2006-03-21 09:56:59 morvan Exp $

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
Lesser General Public License for more details.

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

#include "generic_reader.h"
#include "tsp_provider_init.h"

void RES_GLU_loop(void);

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
 
  int32_t retcode=0;
  char*   input_filename  	= NULL;
  char*   format_file 		= NULL;
  
  /* Main options handling */
  char*   errorString;
  int     opt_ok;
  char    c_opt;

  opt_ok  = 1;
 

  printf ("#========================================================================#\n");
  printf ("# Launching <generic reader server> for generation of Symbols from a generic file #\n");
  printf ("#========================================================================#\n");
  
  if (argc < 2) 
  {
    opt_ok  = 0;
    retcode = -1;
    fprintf(stderr,"%s: Insufficient number of options\n",argv[0]);
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
  
  
  /************/
 /* bb_tsp_provider_createGLU(&GLU_genreader,"GENREADER",GLU_SERVER_TYPE_PASSIVE,1.0);*/
 /******************/
  bb_tsp_provider_createGLU(&GLU_genreader,"GENREADER",GLU_SERVER_TYPE_PASSIVE,1.0);
  

  if (TSP_provider_init(GLU_resreader,&my_argc, &my_argv))
  {
 	  /* Analyse command line parameters */
  	  /*--------*/
	  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"x:f")))) 
	  {    TSP_GenericReader
  			/*-------*/
    		switch (c_opt) 
		{
    		case 'x':
      			input_filename = strdup(optarg);
      			fprintf(stdout,"%s: source file is <%s>\n",argv[0],input_filename);
      			break;
		case 'f':
      			format_file = strdup(optarg);
      			fprintf(stdout,"%s: format file is <%s>\n",argv[0],format_file);
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
	 
	if (!opt_ok) 
	{
    		printf("Usage: %s -x=<source_file> [-f <format]>\n", argv[0]);
    		printf("   -x   determine the source file\n");
   	   	printf("   -f   specifying the format of source file\n");
    		exit(retcode);
  	}

	fmt_handler=genreader_createFmHandler(format_file,input_filename);
		
	genreader_create(&generic_reader,fmt_handler);
	
	genreader_open(generic_reader);
	
	genreader_read_header_create_bb(generic_reader);
	
	genreader_read_header_create_symbole(generic_reader);
	
	tsp_bb_provider_setname("GENREADER");

	genreader_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING);
	
	/* read loop */
	/* lecture MAJ BB synchro */
	/* tester si client connecté */
        /* CHERCHER A TIRER LIEN ENTRE TSP_SESSION et GLU */
	int[] TSP_provider_get_session_id(GLU_instance);
	TSP_provider_get_is_session_connected(id_session);
	genreader_finalize(generic_reader);

  }

  return 0;
}

   
