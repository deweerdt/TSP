/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_tools.h,v 1.1 2005-02-18 23:43:49 erk Exp $

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
#ifndef _BB_TOOLS_H_
#define _BB_TOOLS_H_

#include <stdio.h>
#include <bb_core.h>

/**
 * @defgroup BBTools
 * The BlackBoard tools.
 * @ingroup BlackBoard
 */

/**
 * The list of BB Tools command.
 * @ingroup BBTools
 */
typedef enum {E_BBTOOLS_UNKNOWN=0,
              E_BBTOOLS_GENERIC,
              E_BBTOOLS_READ, 
	      E_BBTOOLS_WRITE,
	      E_BBTOOLS_DUMP, 	                  
	      E_BBTOOLS_FIND,
	      E_BBTOOLS_CHECKID,  
              E_BBTOOLS_DESTROY,
	      E_BBTOOLS_CREATE,
              E_BBTOOLS_PUBLISH, 
              E_BBTOOLS_SYNCHRO_SEND,
              E_BBTOOLS_SYNCHRO_RECV,
              E_BBTOOLS_LASTCMD} E_BBTOOLS_CMD_T;

typedef struct bbtools_request {
  int32_t          verbose;
  int32_t          silent;
  int32_t          argc;
  char**           argv;
  E_BBTOOLS_CMD_T  cmd;
  FILE*            stream;
  char*            bbname;
  S_BB_T*          theBB;
} bbtools_request_t;
	      
#ifdef BB_TOOLS_C
const char* bbtools_cmdname_tab[] = {"bb_unknown",
				    "bb_tools",
				    "bb_read",
				    "bb_write",
				    "bb_dump",
				    "bb_find",
				    "bb_checkid",
				    "bb_destroy",
				    "bb_create",
				    "bb_publish",
				    "bb_synchro_send",
				    "bb_synchro_recv"
};
#else
extern const char* bbtools_cmdname_tab[];
#endif

BEGIN_C_DECLS

/**
 * Initialise BBTools request.
 * @param req OUT, the request to initialize.
 */
void
bbtools_init(bbtools_request_t* req);

/**
 * The BBTools command generic API.
 * @ingroup BBTools
 */
int32_t
bbtools(bbtools_request_t* req);

/**
 * Return the BB Tools command type
 * from the string passed as parameter
 * @param bbtools_string the name of the bbtools command
 * @ingroup BBTools
 */
E_BBTOOLS_CMD_T
bbtools_cmd(const char* bbtools_string);

/**
 *
 * @ingroup BBTools
 */
E_BBTOOLS_CMD_T
bbtools_check_args(int argc, char** argv);

/**
 * Print usage of the specified bbtools command.
 * @param stream IN, the stream to print on
 * @param bbtools_cmd IN, the bbtools command.
 * @ingroup BBTools
 */
void 
bbtools_usage(FILE *stream, E_BBTOOLS_CMD_T bbtools_cmd, int argc, char** argv);

/**
 * Return the BB structure if the named
 * BB exists, NULL if not.
 * @param bbname IN, the name of a blackboard
 * @ingroup BBTools
 */
S_BB_T*
bbtools_check_bbname(const char* bbname);

/**
 *
 * @ingroup BBTools
 */
int32_t 
bbtools_unimplemented_cmd(const char* bbtools_cmdname);

/**
 *
 * @ingroup BBTools
 */
int32_t 
bbtools_read(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t 
bbtools_write(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t 
bbtools_dump(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t 
bbtools_find(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_checkid(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_destroy(S_BB_T** bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_create(S_BB_T** bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_publish(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_synchro_send(S_BB_T* bb, int argc, char** argv);

/**
 *
 * @ingroup BBTools
 */
int32_t
bbtools_synchro_recv(S_BB_T* bb, int argc, char** argv);


END_C_DECLS

#endif /* _BB_TOOLS_H_ */
