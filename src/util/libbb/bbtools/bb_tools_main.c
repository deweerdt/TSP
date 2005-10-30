
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

int 
main (int argc, char ** argv) {

  struct sigaction   my_action;
  struct sigaction   old_action;
  int32_t            retcode=0;
  bbtools_request_t  the_request;

  /* Main options handling */
  /*  char*         error_string;*/
  int           opt_ok;
  char          c_opt;

  opt_ok            = 1;   
  
  /* install SIGINT handler (POSIX way) */
  my_action.sa_handler = &my_sighandler;  
  sigfillset(&my_action.sa_mask);
  my_action.sa_flags = SA_RESTART;
  sigaction(SIGINT,&my_action,&old_action);    

  /* initialize bbtools request */
  bbtools_init(&the_request);
  
  /* Analyse command line parameters */
  while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"nsvh")))) {    
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
