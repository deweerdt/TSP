/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_tools.h,v 1.4 2005-02-23 01:34:48 erk Exp $

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
 * @defgroup BBTools_API
 * The BlackBoard tools.
 * @ingroup BBTools
 */

/**
 * @defgroup BBTools_Command
 * The BB Tools command line interface.
 * Those tools may be launch without argument and will provide
 * a command line option summary (if needed).
 *
 * \par \c bb_findlabel \c \<bbname\> \c \<labelmatch\>
 * A command line tool used to find a label
 * in the specified BlackBoard.\n
 * \arg \c bbname the blackboard name
 * \arg \c labelmatch the part to find
 * The command will list the name of
 * all symbols found in blackboard
 * that match \c labelmatch.
 *
 * \par \c bb_dump \c \<bbname\>
 * Dump the specified BlackBoard.\n
 * \arg \c bbname the blackboard name
 * The command will dump all symbols and associated value
 * output may be huge if not filtered.
 *
 * \par \c bb_read \c \<bbname\> \c \<symname\>
 * Read the value of a specified symbol in specified Blackboard.\n
 * \arg \c bbname the blackboard name
 * \arg \c symname the exact name of the Blackboard symbol to read
 * Value of the symbols is printed iff it is printable, non-printable
 * value will gives '?'.
 *
 * \par \c bb_write \c \<bbname\> \c \<symname\> \c \<value\>
 * Write to a BlackBoard symbol variable.
 * \arg \c bbname the blackboard name
 * \arg \c symname the blackboard symbol name
 * \arg \c value the value to be written
 *
 * @ingroup BBTools
 * @ingroup TSP_Applications
 */

/**
 * The list of BB Tools command.
 * @ingroup BBTools_API
 */
typedef enum {E_BBTOOLS_UNKNOWN=0,
              E_BBTOOLS_GENERIC,
	      E_BBTOOLS_HELP,
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
	      E_BBTOOLS_MEMSET,
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
				     "bb_checkid",
				     "bb_destroy",
				     "bb_create",
				     "bb_publish",
				     "bb_synchro_send",
				     "bb_synchro_recv",
				     "bb_memset",
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
				     "BB LAST COMMAND"
};
#else
extern const char* bbtools_cmdname_tab[];
extern const char* bbtools_cmdhelp_tab[];
#endif

BEGIN_C_DECLS

void 
bbtools_logMsg(FILE* stream, char* fmt, ...);

/**
 * Initialise BBTools request.
 * @param req OUT, the request to initialize.
 */
void
bbtools_init(bbtools_request_t* req);

/**
 * The BBTools command generic API.
 * @ingroup BBTools_API
 */
int32_t
bbtools(bbtools_request_t* req);

/**
 * Return the BB Tools command type
 * from the string passed as parameter
 * @param bbtools_string the name of the bbtools command
 * @ingroup BBTools_API
 */
E_BBTOOLS_CMD_T
bbtools_cmd(const char* bbtools_string);

/**
 *
 * @ingroup BBTools_API
 */
E_BBTOOLS_CMD_T
bbtools_check_args(int argc, char** argv);

/**
 * Print usage of the specified bbtools command.
 * @param stream IN, the stream to print on
 * @param bbtools_cmd IN, the bbtools command.
 * @ingroup BBTools_API
 */
void 
bbtools_usage(bbtools_request_t* req);

/**
 * Return the BB structure if the named
 * BB exists, NULL if not.
 * @param bbname IN, the name of a blackboard
 * @ingroup BBTools_API
 */
S_BB_T*
bbtools_check_bbname(const char* bbname);

/**
 *
 * @ingroup BBTools_API
 */
int32_t 
bbtools_unimplemented_cmd(const char* bbtools_cmdname);

/**
 *
 * @ingroup BBTools_API
 */
int32_t 
bbtools_read(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t 
bbtools_write(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t 
bbtools_dump(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t 
bbtools_find(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_checkid(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_destroy(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_create(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_publish(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_synchro_send(bbtools_request_t* req);

/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_synchro_recv(bbtools_request_t* req);


/**
 *
 * @ingroup BBTools_API
 */
int32_t
bbtools_memset(bbtools_request_t* req);

END_C_DECLS

#endif /* _BB_TOOLS_H_ */
