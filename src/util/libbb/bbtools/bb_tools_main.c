
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <signal.h>

#include <bb_core.h>
#include <bb_tools.h>

static void 
my_sighandler(int signum) {
  fprintf(stderr,"%s::Captured signal<%d>\n",
	  bbtools_cmdname_tab[E_BBTOOLS_GENERIC],
	  signum);
  fflush(stderr);
  exit(-1);
}

int 
main (int argc, char ** argv) {

  struct sigaction   my_action;
  struct sigaction   old_action;
  int32_t            retcode=0;
  bbtools_request_t  the_request;

  /* Main options handling */
  char*         error_string;
  int           opt_ok;
  char          c_opt;
  int           verbose;
  int           silent;

  opt_ok            = 1;   
  verbose           = 0;
  silent            = 0;
  
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  bbtools_init(&the_request);

  /* Analyse command line parameters */
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"sv:")))) {    
    switch (c_opt) {
    case 's':
      silent  = 1;
      break;
    case 'v':
      verbose = strtol(optarg,&error_string,10);
      /* verbose format error default to level 1 */
      if ('\0' != *error_string) {
	verbose = 1;
      } 
      fprintf(stdout,"%s:: Verbose mode enabled.\n",bbtools_cmdname_tab[E_BBTOOLS_GENERIC]);
      break;
    case '?':
      fprintf(stderr,"%s:: Invalid command line option(s), correct it and rerun\n",argv[0]);
      retcode = -1;
      opt_ok  = 0;
      break;
    default:
      retcode = -1;
      opt_ok  = 0;
      break;
    } /* end of switch */    
  }

  /* check if global options are OK */
  if (!opt_ok) {
    //bbtools_usage(stderr,bbtools_cmd,argc,argv);
    exit(retcode);
  }

  /* retrieve command invoked */
  //bbtools_cmd = bbtools_checkargs(argc,argv);

  /* unknown command */
  if (bbtools_cmd<=0) {
    retcode = -1;
    //bbtools_usage(stderr,bbtools_cmd,argc,argv);
    exit(retcode);
  }
  
  //retcode = bbtools(bbtools_cmd,argc,argv);
  
  return (retcode);
}
