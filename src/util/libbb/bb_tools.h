/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_tools.h,v 1.10 2007-05-04 13:35:51 deweerdt Exp $

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
#include <tsp_abs_types.h>

/**
 * @defgroup BBTools The BB Tools
 * The BlackBoard tools.
 * @ingroup BlackBoard
 */

/**
 * @defgroup BBToolsLib The BB Tools Library
 * The BlackBoard tools.
 * @ingroup BBTools
 */

/** 
 * @addtogroup BBToolsLib
 * @{
 */

/**
 * The list of BB Tools command.
 */
typedef enum {E_BBTOOLS_UNKNOWN=0,
              E_BBTOOLS_GENERIC,
	      E_BBTOOLS_HELP,
              E_BBTOOLS_READ, 
	      E_BBTOOLS_WRITE,
	      E_BBTOOLS_DUMP, 	                  
	      E_BBTOOLS_FIND,
	      E_BBTOOLS_CHECK_ID,  
              E_BBTOOLS_DESTROY,
	      E_BBTOOLS_CREATE,
              E_BBTOOLS_PUBLISH, 
              E_BBTOOLS_SYNCHRO_SEND,
              E_BBTOOLS_SYNCHRO_RECV,
	      E_BBTOOLS_MEMSET,
	      E_BBTOOLS_CHECK_VERSION,
	      E_BBTOOLS_LOAD,
              E_BBTOOLS_LASTCMD} E_BBTOOLS_CMD_T;

typedef struct bbtools_request {
  int32_t          verbose;
  int32_t          silent;
  int32_t          nb_global_opt;
  int32_t          argc;
  char**           argv;
  E_BBTOOLS_CMD_T  cmd;
  FILE*            stream;
  char*            bbname;
  S_BB_T*          theBB;
  char             newline[2];
} bbtools_request_t;
	      
#ifdef BB_TOOLS_C
const char* bbtools_cmdname_tab[] = {"bb_unknown",
  				     "bb_tools",
				     "bb_help",
				     "bb_read",
				     "bb_write",
				     "bb_dump",
				     "bb_find",
				     "bb_check_id",
				     "bb_destroy",
				     "bb_create",
				     "bb_publish",
				     "bb_synchro_send",
				     "bb_synchro_recv",
				     "bb_memset",
				     "bb_check_version",
				     "bb_load",
				     "bb_last_cmd"
};

const char* bbtools_cmdhelp_tab[] = {"unknown bbtools command",
  				     "generic bbtools command interface",
				     "write bbtools help",
				     "read symbol value from blackboard",
				     "write symbol value from blackboard",
				     "dump blackboard content",
				     "find symbol in blackboard",
				     "check blackboard IPC ID values",
				     "destroy a blackboard",
				     "create a blackboard",
				     "publish symbol in blackboard",
				     "send synchro message through blackboard MSQ queue",
				     "recv [wait] synchro message from blackboard MSQ queue",
				     "memset blackboard data region",
				     "check blackboard version vs bb_tools runtime version",
				     "load a file into the BB",
				     "BB LAST COMMAND"
};
#else
extern const char* bbtools_cmdname_tab[];
extern const char* bbtools_cmdhelp_tab[];
#endif

/** @} */


BEGIN_C_DECLS

/**
 * @addtogroup BBToolsLib
 * @{
 */

/**
 * Initialise BBTools request.
 * @param[out] req the request to initialize.
 */
void 
bbtools_init(bbtools_request_t* req);

/**
 * The BBTools command generic API.
 * @param[in,out] req the bbtools request to be handled
 * @return E_OK on success E_NOK otherwise.
 */
int32_t 
bbtools(bbtools_request_t* req);

/**
 * Return the BB Tools command type
 * from the string passed as parameter
 * @param[in] bbtools_string the name of the bbtools command
 */
E_BBTOOLS_CMD_T
bbtools_cmd(const char* bbtools_string);

/**
 *
 */
E_BBTOOLS_CMD_T
bbtools_checkargs(bbtools_request_t* req);

/**
 * Print usage of the specified bbtools request.
 * @param[in] req the bbtools request.
 */
void 
bbtools_usage(bbtools_request_t* req);

/**
 * Return the BB structure if the named
 * BB exists, NULL if not.
 * @param[in] bbname the name of a blackboard
 */
S_BB_T*
bbtools_checkbbname(const char* bbname);

/**
 *
 */
int32_t 
bbtools_unimplemented_cmd(const char* bbtools_cmdname);

/**
 *
 */
int32_t 
bbtools_read(bbtools_request_t* req);

/**
 *
 */
int32_t 
bbtools_write(bbtools_request_t* req);

/**
 *
 */
int32_t 
bbtools_dump(bbtools_request_t* req);

/**
 *
 */
int32_t 
bbtools_find(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_check_id(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_destroy(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_create(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_publish(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_synchro_send(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_synchro_recv(bbtools_request_t* req);


/**
 *
 */
int32_t
bbtools_memset(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_check_version(bbtools_request_t* req);

/**
 *
 */
int32_t
bbtools_load(bbtools_request_t* req);

/**  @} */


END_C_DECLS

#endif /* _BB_TOOLS_H_ */
