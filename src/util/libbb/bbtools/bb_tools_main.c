/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bbtools/bb_tools_main.c,v 1.8 2007-12-08 14:00:22 erk Exp $

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
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */

#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

#include <bb_core.h>
#include <bb_tools.h>

void 
bbtools_logMsg(FILE* stream, char* fmt, ...);


static void 
my_sighandler(int signum) {
  fprintf(stderr,"%s::Captured signal<%d>\n",
	  bbtools_cmdname_tab[E_BBTOOLS_GENERIC],
	  signum);
  fflush(stderr);
  exit(-1);
}

/**
 * @defgroup BBToolsCommandLine BB Tools Command Lines
 * The Blackboard tools command line interface.
 * bb_tools is a command line tool with a "BusyBox"-like design
 * (<A HREF="http://www.busybox.net">http://www.busybox.net</A>).
 * bb_tools is the 'main' command which may be used with its
 * generic interface of may be called (using symlink) with a 
 * different name such that the first arg may be ommitted. 
 * Any of those tools may be launch without argument and will provide
 * a command line option summary (if needed).
 *
 * \par \c bb_tools \c [bb_tools_opts] \c \<bb_tools_cmd\> [cmd_opts]
 * The generic bb_tools command.
 * \arg \c bb_tools_opts the bb_tools generic options which may be
 *     <ul>
 *        <li>-s silent mode (may be used for silent scripting)
 *        <li>-v verbose mode
 *        <li>-n no newline read mode
 *      </ul>
 *
 * \par \c bb_help 
 * Display the online help.
 *
 * \par \c bb_read \c \<bbname\> \c \<symname\>
 * Read the value of a specified symbol in specified Blackboard.\n
 * \arg \c bbname the blackboard name
 * \arg \c symname the exact name of the Blackboard symbol to read
 * Value of the symbols is printed iff it is printable, non-printable
 * value will given as hexadecimal dump.
 *
 * \par \c bb_write \c \<bbname\> \c \<symname\> \c \<value\>
 * Write to a BlackBoard symbol variable.
 * \arg \c bbname the blackboard name
 * \arg \c symname the blackboard symbol name
 * \arg \c value the value to be written, the value may be given in decimal 
 *         or hexadecimal if the <em>0x</em> prefix is added.
 *
 * \par \c bb_dump \c \<bbname\>
 * Dump the specified BlackBoard.\n
 * \arg \c bbname the blackboard name
 * The command will dump all symbols and associated value
 * output may be huge if not filtered.
 *
 * \par \c bb_find \c \<bbname\> \c \<labelmatch\>
 * A command line tool used to find a label
 * in the specified BlackBoard.\n
 * \arg \c bbname the blackboard name
 * \arg \c labelmatch the part to find (using a simple strstr(3))
 * The command will list the name of all symbols found in blackboard
 * that match \c labelmatch.
 *
 * \par \c bb_check_id \c \<bbname\> [\c \<user_specific_value\>]
 * Print out the IPC key used by the specified BlackBoard. 
 * \arg \c bbname the blackboard name
 * \arg \c user_specific_value an integer, default is getuid(2)
 * This may be used against ipcs(P) values to check if
 * IPC object belongs to a BlackBoard.
 *
 * \par \c bb_destroy \c \<bbname\>
 * Destroy the specified BlackBoard. 
 * \arg \c bbname the blackboard name
 * Note that BlackBoard will effectively be destroyed when 
 * the last process detached itself from the BlackBoard.
 *
 * \par \c bb_create \c \<bbname\>  \c \<ndate\>  \c \<datazonesize\>
 * \arg \c bbname the blackboard name
 * \arg \c ndata the number of publishable data in this BlackBoard
 * \arg \c datazonesize the size of the BlackBoard Data area.
 * Create a BlackBoard with the specified name and sizes.
 *
 * \par \c bb_publish \c \<bbname\>  \c \<symname\> [\c \<symtype\>=BB_UINT] [\c \<arraysize\>=1]
 * \arg \c bbname the blackboard name
 * \arg \c symname the name of the symbol to be published 
 * \arg \c symtype the BlackBoard type of the symbol to be published, default is BB_UINT 
 * \arg \c arraysize the array size of the symbol, default is 1 (i.e. scalar).
 *
 * \par \c bb_synchro_send \c \<bbname\> 
 * \arg \c bbname the blackboard name
 * Send a synchro message through the BlackBoard synchro message queue.
 *
 * \par \c bb_synchro_recv \c \<bbname\>
 * \arg \c bbname the blackboard name
 * Receive a synchro message from the BlackBoard synchro message queue.
 *
 * \par \c bb_memset \c \<bbname\>  \c \<bytevalue\>
 * \arg \c bbname the blackboard name
 * \arg \c bytevalue the byte value to write to BlackBoard Data Area
 * Fill the BlackBoard data area with \c bytevalue.
 * This may be used to memset the BlackBoard data area to binary 0,
 * just \c bb_memset \c \<bbname\> 0.
 *
 * \par \c bb_check_version \c \<bbname\>
 * Verify whether if the specified BlackBoard has the same BlackBoard
 * version ID as the bb_tools used.
 * \arg \c bbname the blackboard name
 * If version do not match <em>YOU SHOULD NOT USE THOSE</em> bb_tools 
 * to manipulate this BlackBoard. 
 * You should check you BlackBoard library installation too and/or
 * the BlackBoard library version used by the application which 
 * created the BlackBoard you are looking at.
 * 
 * @ingroup BBTools
 * @ingroup TSP_Applications
 */

int 
main (int argc, char ** argv) {

  struct sigaction   my_action;
  struct sigaction   old_action;
  int32_t            retcode=0;
  bbtools_request_t  the_request;

  /* Main options handling */
  /*  char*         error_string;*/
  int           opt_ok;
  int           c_opt;

  opt_ok            = 1;   
  
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  /* initialize bbtools request */
  bbtools_init(&the_request);
  
  /* Analyse command line parameters */
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"+nsvh")))) {    
    switch (c_opt) {
    case 's':
      the_request.silent  = 1;
      opt_ok++;
      break;
    case 'v':
/*       if (optarg !=NULL) { */
/* 	the_request.verbose = strtol(optarg,&error_string,10); */
 	/* verbose format error default to level 1 */ 
/* 	if ('\0' != *error_string) { */
/* 	  the_request.verbose = 1; */
/* 	}  */
/*       } else { */
	the_request.verbose = 1;
	opt_ok++;
/*       } */
      bbtools_logMsg(stdout,"Verbose mode enabled.\n");
      fflush(stdout);
      break;
    case 'h':
      opt_ok             = 0;
      the_request.cmd    = E_BBTOOLS_HELP;
      break;
    case 'n':
      opt_ok++;
      the_request.newline[0] = ' ' ;
      break;
    case '?':
      fprintf(stderr,"%s::Invalid command line option(s), correct it and rerun\n",argv[0]);
      retcode = -1;
      opt_ok  = 0;
      the_request.cmd    = E_BBTOOLS_UNKNOWN;
      break;
    default:
      retcode = -1;
      opt_ok  = 0;
      the_request.cmd    = E_BBTOOLS_UNKNOWN;
      break;
    } /* end of switch */     
  }

  /* indicates number of global options to skip */
  the_request.nb_global_opt = opt_ok-1;
  the_request.argv          = argv;
  the_request.argc          = argc;

  /* check if global options are OK */
  if (!opt_ok) {
    the_request.stream = stderr;
    bbtools_usage(&the_request);
    exit(retcode); 
  }

  /* invoke bbtools */
  retcode = bbtools(&the_request);

  return (retcode);
} /* end of main */
