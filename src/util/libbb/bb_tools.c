
/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_tools.c,v 1.1 2005-02-18 23:43:49 erk Exp $

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
Maintainer : enoulard@free.fr
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

#include <bb_core.h>
#define BB_TOOLS_C
#include <bb_tools.h>

void
bbtools_init(bbtools_request_t* req) {
  req->verbose = 0;
  req->silent  = 0;
  req->argc    = 0;
  req->argv    = NULL;
  req->cmd     = E_BBTOOLS_UNKNOWN;
  req->stream  = NULL;
  req->bbname  = NULL;
  req->theBB   = NULL;
}  /* end of bbtools_init */

int32_t
bbtools(bbtools_request_t* req) {

  /* 
   * special case for bb_checkid which
   * does not need to open a blackboard
   */
  if (E_BBTOOLS_CHECKID==req->cmd) {    
    return bbtools_checkid(req->theBB,req->argc,req->argv);
  } else { /* other bbtools command needs to open BlackBoard */
    return -1;
  }
     
  switch (req->cmd) {
  case E_BBTOOLS_UNKNOWN:
    bbtools_usage(stderr,req->cmd,req->argc,req->argv);
    return -1;
    break;  
  case E_BBTOOLS_READ:    
    bbtools_read(req->theBB,req->argc,req->argv);
    break;
  case E_BBTOOLS_WRITE:
    break;
  case E_BBTOOLS_DUMP:
    break;
  case E_BBTOOLS_FIND:
    break;
  case E_BBTOOLS_CHECKID:
    /* nothing to do since we should not reach this statement */
    break;
  case E_BBTOOLS_DESTROY:
    break;
  case E_BBTOOLS_CREATE:
    break;
  case E_BBTOOLS_PUBLISH:
    break;
  case E_BBTOOLS_SYNCHRO_SEND:
    break;
  case E_BBTOOLS_SYNCHRO_RECV:
    break;
  default:
    bbtools_usage(stderr,E_BBTOOLS_UNKNOWN,req->argc,req->argv);
    return -1;
    break;
  }
} /* end of bbtools */

E_BBTOOLS_CMD_T 
bbtools_cmd(const char* bbtools_string) {

  E_BBTOOLS_CMD_T retval;
  int32_t          i;

  retval = E_BBTOOLS_UNKNOWN;

  for (i=E_BBTOOLS_GENERIC;i<E_BBTOOLS_LASTCMD;++i) {
    if (strncmp(bbtools_string,bbtools_cmdname_tab[i],strlen(bbtools_cmdname_tab[i]))) {
      retval = i;
      break;
    }
  }
  return retval;
} /* end of bbtools_cmd */

E_BBTOOLS_CMD_T
bbtools_checkargs(int argc, char** argv) {
  E_BBTOOLS_CMD_T retval = E_BBTOOLS_UNKNOWN;

  /* 
   * Check if we called us with non generic
   * bb_cmd name
   */
  retval = bbtools_cmd(argv[0]);

  /* if we were call with the generic name
   * compute bbtools command and shift arguments
   */
  if (E_BBTOOLS_GENERIC < retval) {
    retval = bbtools_cmd(argv[1]);
  } 
  
  return retval;
} /* end of bb_tools_check_args */

void 
bbtools_usage(FILE *stream, E_BBTOOLS_CMD_T bbtools_cmd, int argc, char** argv) {

  int32_t i;

  fprintf(stream, 
	  "Usage: %s::%s [bbtools_opts] <bbtools_cmd> [cmd_opts]\n",
	  bbtools_cmdname_tab[E_BBTOOLS_GENERIC],
	  bbtools_cmdname_tab[bbtools_cmd]);

  switch (bbtools_cmd) {
  case E_BBTOOLS_UNKNOWN:
    fprintf(stream,
	    "supported <bbtools_cmd> are: \n");
    for (i=E_BBTOOLS_GENERIC+1;i<E_BBTOOLS_LASTCMD-1;++i) {
      fprintf(stream,
	      "%s, ",
	      bbtools_cmdname_tab[i]
	      );
    }
    fprintf(stream,
	    "%s",
	    bbtools_cmdname_tab[i]  
	    );
    break;  
  case E_BBTOOLS_READ:    
  case E_BBTOOLS_WRITE:
  case E_BBTOOLS_DUMP:
  case E_BBTOOLS_FIND:
  case E_BBTOOLS_CHECKID:
  case E_BBTOOLS_DESTROY:
  case E_BBTOOLS_CREATE:
  case E_BBTOOLS_PUBLISH:
  case E_BBTOOLS_SYNCHRO_SEND:
  case E_BBTOOLS_SYNCHRO_RECV:
    break;
  default:
    fprintf(stream, 
	    "default: should never be reached?\n");
  }

} /* end of bb_tools_usage */

S_BB_T*
bbtools_check_bbname(const char* bbname) {
  S_BB_T* retval;
  retval = NULL;
  if (bb_attach(&retval,bbname) != E_OK) {
    retval = NULL;
  }  
  return retval;
} /* end of bbtools_check_bbname */

int32_t 
bbtools_unimplemented_cmd(const char* cmdname) {

  fprintf(stderr,"%s::<%s> not implemented!\n",
	  bbtools_cmdname_tab[E_BBTOOLS_GENERIC],
	  cmdname);
  return -1;
} /* end of bbtools_unimplemented_cmd */

int32_t 
bbtools_read(S_BB_T* bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_READ]);
} /* end of bbtools_read */

int32_t 
bbtools_write(S_BB_T* bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_WRITE]);
}  /* end of bbtools_write */

int32_t 
bbtools_dump(S_BB_T* bb, int argc, char** argv) {
   return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_DUMP]);
}  /* end of bbtools_dump */

int32_t
bbtools_find(S_BB_T* bb, int argc, char** argv) {
   return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_FIND]);
}  /* end of bbtools_find */

int32_t
bbtools_checkid(S_BB_T* bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_CHECKID]);
} /* end of bbtools_checkid */

int32_t
bbtools_destroy(S_BB_T** bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_DESTROY]);
}  /* end of bbtools_destroy */

int32_t
bbtools_create(S_BB_T** bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_CREATE]);
}  /* end of bbtools_create */

int32_t
bbtools_publish(S_BB_T* bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_PUBLISH]);
} /* end of bbtools_publish */

int32_t
bbtools_synchro_send(S_BB_T* bb, int argc, char** argv) {
  return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND]);
} /* end of bbtools_synchro_send */

int32_t
bbtools_synchro_recv(S_BB_T* bb, int argc, char** argv) {
 return  bbtools_unimplemented_cmd(bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_RECV]);
} /* end of bbtools_synchro_recv */
