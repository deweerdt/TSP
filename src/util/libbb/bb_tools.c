
/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_tools.c,v 1.17 2005-10-30 15:31:43 erk Exp $

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
#include <libgen.h>
#include <stdarg.h>

#include <tsp_abs_types.h>
#include <tsp_const_def.h>
#include <bb_core.h>
#include <bb_utils.h>
#define BB_TOOLS_C
#include <bb_tools.h>
#include <bb_simple.h>

void 
bbtools_logMsg(FILE* stream, char* fmt, ...) {
  va_list args;
  char    message[2048];

  memset(message,0,2048);
  va_start(args, fmt);
  fprintf(stream,"%s::",bbtools_cmdname_tab[E_BBTOOLS_GENERIC]);
  vfprintf(stream,fmt,args);
  va_end(args);

} /* end of bbtools_logMsg */

void
bbtools_init(bbtools_request_t* req) {
  req->verbose       = 0;
  req->silent        = 0;
  req->argc          = 0;
  req->argv          = NULL;
  req->nb_global_opt = 0;
  req->cmd           = E_BBTOOLS_UNKNOWN;
  req->stream        = stdout;
  req->bbname        = NULL;
  req->theBB         = NULL;
  req->newline[1]    = '\0';
  req->newline[0]    = '\n';
}  /* end of bbtools_init */

E_BBTOOLS_CMD_T 
bbtools_cmd(const char* bbtools_string) {

  E_BBTOOLS_CMD_T retval;
  int32_t          i;
  char*           cmdbasename;
  char*           cmdstr_cpy;
  char*           cmdstr_abbrev;

  cmdstr_cpy = strdup(bbtools_string);
  cmdbasename = basename(cmdstr_cpy);

  retval = E_BBTOOLS_UNKNOWN;
  
  /* complete name match */
  for (i=E_BBTOOLS_GENERIC;i<E_BBTOOLS_LASTCMD;++i) {
    if (!strncmp(cmdbasename,bbtools_cmdname_tab[i],strlen(bbtools_cmdname_tab[i]))) {
      retval = i;
      break;
    }
  }
  
  /* abbreviate name match */
  /* FIXME should implement partial match */
  if (E_BBTOOLS_UNKNOWN == retval) {
    for (i=E_BBTOOLS_GENERIC;i<E_BBTOOLS_LASTCMD;++i) {
      cmdstr_abbrev = strstr(bbtools_cmdname_tab[i],"_")+1;
      if (!strncmp(cmdbasename,cmdstr_abbrev,strlen(cmdstr_abbrev))) {
	retval = i;
	break;
      }
    }
  }

  free(cmdstr_cpy);
  return retval;
} /* end of bbtools_cmd */

E_BBTOOLS_CMD_T
bbtools_checkargs(bbtools_request_t* req) {
  E_BBTOOLS_CMD_T retval = E_BBTOOLS_UNKNOWN;

  /* 
   * Check if we called us with non generic
   * bb_cmd name
   */
  retval = bbtools_cmd(req->argv[0]);

  /* 
   * If we were call with the generic name
   * compute bbtools command and shift args in the request.
   */
  if ((E_BBTOOLS_GENERIC == retval) && (req->argc - req->nb_global_opt)>1) {
    retval = bbtools_cmd(req->argv[1+req->nb_global_opt]);
    /* shift argv */
    req->argv = &(req->argv[2+req->nb_global_opt]);    
    /* shift argc since we are in the bb_tools generic call case */
    req->argc -= 2 + req->nb_global_opt;
  }  else {
    /* shift argv */
    req->argv = &(req->argv[1+req->nb_global_opt]);    
    /* shift argc since we are in the bb_tools generic call case */
    req->argc -= 1 + req->nb_global_opt;
  }

  return retval;
} /* end of bb_tools_checkargs */

S_BB_T*
bbtools_checkbbname(const char* bbname) {
  S_BB_T* retval;
  retval = NULL;
  if (bb_attach(&retval,bbname) != E_OK) {
    retval = NULL;
  }  
  return retval;
} /* end of bbtools_checkbbname */


int32_t
bbtools_parsearrayname(const char* provided_symname, S_BB_DATADESC_T* sym_data_desc, int32_t* array_index) {
  char*    array_name;
  char*    symname;
  int32_t  retcode = 0;
  assert(sym_data_desc);

  symname = strdup(provided_symname);

  array_name = strstr(symname,"[");
  if (array_name) {
    char* temp = "%d";
    char* temp2;
    temp2  = strdup(symname);
    array_name  = strtok(temp2,"[");
    strncpy(sym_data_desc->name,array_name,VARNAME_MAX_SIZE);
    array_name = strtok(NULL,"]");
    if (sscanf(array_name,temp,array_index)<1) {
      retcode = -1;
    }
    free(temp2);
  } else {
    *array_index = -1;
    strncpy(sym_data_desc->name,provided_symname,VARNAME_MAX_SIZE);  
  }
  free(symname);
  return retcode;
} /* end bbtools_parsearrayname */


int32_t
bbtools(bbtools_request_t* req) {

  int32_t retval = 0;

  /* Get real bb_tools command */
  req->cmd = bbtools_checkargs(req);

  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s\n",
		   bbtools_cmdname_tab[req->cmd]);
  }

  /*
   * If NOT in those cases we need to open BB 
   */
  if (!((E_BBTOOLS_UNKNOWN==req->cmd) || 
	(E_BBTOOLS_GENERIC==req->cmd) ||  
	(E_BBTOOLS_HELP   ==req->cmd) ||  
	(E_BBTOOLS_CHECK_ID==req->cmd) ||
	(E_BBTOOLS_CREATE ==req->cmd))
      /* should not try to open BB if the bbname arg is missing */
      && (req->argc > 0)
      ) {
    /* first request arg should be bbname */
    req->theBB=bbtools_checkbbname(req->argv[0]);    
    if (NULL == req->theBB) {
      if (!req->silent) {
	bbtools_logMsg(req->stream,
		       "Blackboard <%s> does not exist\n",
		       req->argv[0]);
      }
      return -1;
    } else { /* assign bbname */
      req->bbname = req->argv[0];
    }
  }

  /* FIXME should assign retval using 
   * retcode from bb_<cmd>
   */
  switch (req->cmd) {
  case E_BBTOOLS_UNKNOWN:
    req->stream = stderr;
    bbtools_usage(req);
    return -1;
    break;  
  case E_BBTOOLS_GENERIC:
    req->stream = stdout;
    bbtools_usage(req);
    break;
  case E_BBTOOLS_HELP:
    req->stream = stdout;
    bbtools_usage(req);
    return -1;
    break;  
  case E_BBTOOLS_READ:    
    bbtools_read(req);
    break;
  case E_BBTOOLS_WRITE:
    bbtools_write(req);
    break;
  case E_BBTOOLS_DUMP:
    bbtools_dump(req);
    break;
  case E_BBTOOLS_FIND:
    bbtools_find(req);
    break;
  case E_BBTOOLS_CHECK_ID:
    return bbtools_check_id(req);
    break;
  case E_BBTOOLS_DESTROY:
    bbtools_destroy(req);
    break;
  case E_BBTOOLS_CREATE:
    bbtools_create(req);
    break;
  case E_BBTOOLS_PUBLISH:
    bbtools_publish(req);
    break;
  case E_BBTOOLS_SYNCHRO_SEND:
    bbtools_synchro_send(req);
    break;
  case E_BBTOOLS_SYNCHRO_RECV:
    bbtools_synchro_recv(req);
    break;
  case E_BBTOOLS_MEMSET:
    bbtools_memset(req);
    break;
  case E_BBTOOLS_CHECK_VERSION:
    bbtools_check_version(req);
    break;
  default:
    req->stream = stderr;
    req->cmd    = E_BBTOOLS_UNKNOWN;
    bbtools_usage(req);
    return -1;
    break;
  }

  if (NULL != req->theBB) {
    bb_detach(&req->theBB);
  }
  return retval;
} /* end of bbtools */


#define BBTOOLS_MAX_BLANK 80

char* bbtools_fillspace(int size, const char* str) {
  static char myspace[BBTOOLS_MAX_BLANK];
  int i;
  for (i=0;i<size-strlen(str);++i) {
    myspace[i] = ' ';    
  }
  myspace[i] = '\0';
  return myspace;
} /* end of bbtools_fillspace */

void 
bbtools_usage(bbtools_request_t* req) {

  int32_t i;

  switch (req->cmd) {
  case E_BBTOOLS_UNKNOWN:
    fprintf(req->stream, 
	    "%s::unknown %s command\n",
	    bbtools_cmdname_tab[E_BBTOOLS_GENERIC],
	    bbtools_cmdname_tab[E_BBTOOLS_GENERIC]);	    
  case E_BBTOOLS_GENERIC:
  case E_BBTOOLS_HELP:
    fprintf(req->stream, 
	    "TSP bbtools v%s (%s)\n",TSP_SOURCE_VERSION,TSP_PROJECT_URL);
    fprintf(req->stream, 
	    "Usage: %s [bbtools_opts] <bbtools_cmd> [cmd_opts]\n",
	    bbtools_cmdname_tab[E_BBTOOLS_GENERIC]);
    fprintf(req->stream,"   bbtools_opts:\n");
    fprintf(req->stream,"    -s silent mode (may be used for silent scripting)\n");
    fprintf(req->stream,"    -v verbose mode\n");
    fprintf(req->stream,"    -n no newline read mode\n");
    fprintf(req->stream,"   supported <bbtools_cmd> are: \n");
    for (i=E_BBTOOLS_GENERIC+1;i<E_BBTOOLS_LASTCMD;++i) {
      fprintf(req->stream,
	      "    %s%s: %s\n",
	      bbtools_cmdname_tab[i],
	      bbtools_fillspace(20,bbtools_cmdname_tab[i]),
	      bbtools_cmdhelp_tab[i]
	      );
    }
    break;
  case E_BBTOOLS_READ:    
    fprintf(req->stream,
	    "Usage: %s <bbname> <symbol_name>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_READ]);    	    
    break;    
  case E_BBTOOLS_WRITE:
    fprintf(req->stream,
	    "Usage: %s <bbname> <symbol_name> <value>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_WRITE]);    	    
    break;    
  case E_BBTOOLS_DUMP:
    fprintf(req->stream,"Usage : %s <bbname>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_DUMP]);    	    
    break;
  case E_BBTOOLS_FIND:
    fprintf(req->stream,"Usage : %s <bbname> <symbolpattern>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_FIND]); 
    break;
  case E_BBTOOLS_CHECK_ID:
    	fprintf(req->stream,
		"Usage: %s <bbname> [<user_specific_value>]\n",
		bbtools_cmdname_tab[E_BBTOOLS_CHECK_ID]);
    break;
  case E_BBTOOLS_DESTROY:
    fprintf(req->stream,"Usage : %s <bbname>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_DESTROY]);    	  
    break;
  case E_BBTOOLS_CREATE:
    fprintf(req->stream,"Usage : %s <bbname> <ndata> <datazonesize>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_CREATE]);  
    break;
  case E_BBTOOLS_PUBLISH:
    fprintf(req->stream,"Usage : %s <bbname> <symbol_name> [<symbol_type>=BB_UINT] [<arraysize>=1]\n",
	    bbtools_cmdname_tab[E_BBTOOLS_PUBLISH]);  
    break;
  case E_BBTOOLS_SYNCHRO_SEND:
    fprintf(req->stream,"Usage : %s <bbname>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND]); 
    break;
  case E_BBTOOLS_SYNCHRO_RECV:
    fprintf(req->stream,"Usage : %s <bbname>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_RECV]); 
    break;
  case E_BBTOOLS_MEMSET:
    fprintf(req->stream,"Usage : %s <bbname> <bytevalue>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_MEMSET]);   
    break;
  case E_BBTOOLS_CHECK_VERSION:
    fprintf(req->stream,"Usage : %s <bbname>\n",
	    bbtools_cmdname_tab[E_BBTOOLS_CHECK_VERSION]);   
    break;
  default:
    fprintf(req->stream, 
	    "default: should never be reached?\n");
  }

  if ((req->cmd>=E_BBTOOLS_UNKNOWN) &&
      (req->cmd<E_BBTOOLS_LASTCMD)) {
    fprintf(req->stream, 
	    "%s::%s\n",
	    bbtools_cmdname_tab[req->cmd],
	    bbtools_cmdhelp_tab[req->cmd]);
  }

} /* end of bb_tools_usage */


int32_t 
bbtools_unimplemented_cmd(const char* cmdname) {

  bbtools_logMsg(stderr,"<%s> not implemented!\n",
		 cmdname);

  return -1;
} /* end of bbtools_unimplemented_cmd */

int32_t 
bbtools_read(bbtools_request_t* req) {
  int32_t retval  = 0;
  S_BB_DATADESC_T sym_data_desc;
  int32_t array_index;
  void *sym_value;
  
  if (req->argc<2) {
    bbtools_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   bbtools_cmdname_tab[E_BBTOOLS_READ],
		   2-req->argc);
    bbtools_usage(req);
    retval = -1;
    return retval;
  }
  if (bbtools_parsearrayname(req->argv[1],&sym_data_desc,&array_index)) {
    bbtools_logMsg(req->stream,"%s: cannot parse symname <%s>",
		   bbtools_cmdname_tab[E_BBTOOLS_READ],
		   req->argv[1]);
    retval = -1;
  } else {
    /* scalar read case */
    if (-1 == array_index) {
      if (req->verbose) {
	bbtools_logMsg(req->stream,
		       "%s: Trying to read symbol <%s> on blackboard <%s>...\n",
		       bbtools_cmdname_tab[E_BBTOOLS_READ],
		       sym_data_desc.name,
		       req->bbname);
      }
    } else { /* single array element case */
      if (req->verbose) {
	bbtools_logMsg(req->stream,
		       "%s: Trying to read index <%d> of array symbol <%s> on blackboard <%s>...\n",
		       bbtools_cmdname_tab[E_BBTOOLS_READ],
		       array_index,
		       sym_data_desc.name,
		       req->bbname);
      }		       
    }
    /* 
     * Use low-level subscribe in order to discover the 
     * type of the variable
     */
    sym_data_desc.type      = E_BB_DISCOVER;
    sym_data_desc.type_size = 0;
    sym_data_desc.dimension = 0;
    sym_value = bb_subscribe(req->theBB,&sym_data_desc);    

    if (NULL==sym_value) {
      bbtools_logMsg(req->stream,"%s: symbol <%s> not found in BB <%s>\n",
		     bbtools_cmdname_tab[E_BBTOOLS_READ],
		     sym_data_desc.name,
		     req->bbname);
    } else {
      if ((array_index!=-1) && (sym_data_desc.dimension <= array_index)) {
	bbtools_logMsg(req->stream,"%s: index <%d> exceeding symbol array dimension <%d>\n",
		       bbtools_cmdname_tab[E_BBTOOLS_READ],
		       array_index,
		       sym_data_desc.dimension);
      } else {
	if (req->verbose) {
	  bb_data_header_print(sym_data_desc,req->stream,array_index);
	  bb_value_print(req->theBB,sym_data_desc,req->stream,array_index);
	  bb_data_footer_print(sym_data_desc,req->stream,array_index);
	} else {
	  bb_value_print(req->theBB,sym_data_desc,req->stream,array_index);
	  fprintf(req->stream,"%s",req->newline);
	}
      }
    }	       
  }

  return retval;

} /* end of bbtools_read */

int32_t 
bbtools_write(bbtools_request_t* req) {
  int32_t retval = 0;
  S_BB_DATADESC_T sym_data_desc;
  int32_t array_index;
  void *sym_value;

  if (req->argc<3) {
    bbtools_logMsg(req->stream,"%s: <%d> argument(s) missing\n",
		   bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		   3-req->argc);
    bbtools_usage(req);
    retval = -1;
    return retval;
  }

  if (bbtools_parsearrayname(req->argv[1],&sym_data_desc,&array_index)) {
    bbtools_logMsg(req->stream,"%s: cannot parse symname <%s>",
		   bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		   req->argv[1]);
    retval = -1;
  } else {
    /* scalar write case */
    if (-1 == array_index) {
      if (req->verbose) {
	bbtools_logMsg(req->stream,
		       "%s: Trying to write symbol <%s> on blackboard <%s>...\n",
		       bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		       sym_data_desc.name,
		       req->bbname);
      }
      array_index = 0;
    } else { /* single array element case */      
      if (req->verbose) {
	bbtools_logMsg(req->stream,
		       "%s: Trying to write index <%d> of array symbol <%s> on blackboard <%s>...\n",
		       bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		       array_index,
		       sym_data_desc.name,
		       req->bbname);
      }		       
    }
    /* 
     * Use low-level subscribe in order to discover the 
     * type of the variable
     */
    sym_data_desc.type      = E_BB_DISCOVER;
    sym_data_desc.type_size = 0;
    sym_data_desc.dimension = 0;
    sym_value = bb_subscribe(req->theBB,&sym_data_desc);
    if ((sym_data_desc.dimension>1) && (-1==array_index)) {
      if (req->verbose) {
	  bbtools_logMsg(req->stream,"%s: Implicit first array element write\n",
			 bbtools_cmdname_tab[E_BBTOOLS_WRITE]);

      }
      array_index = 0;
      if (req->verbose) {
	bbtools_logMsg(req->stream,
		       "%s: Trying to write index <%d> of array symbol <%s> on blackboard <%s>...\n",
		       bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		       array_index,
		       sym_data_desc.name,
		       req->bbname);
      }
    }

    if (NULL==sym_value) {
      bbtools_logMsg(req->stream,"%s: symbol <%s> not found in BB <%s>\n",
		     bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		     sym_data_desc.name,
		     req->bbname);
    } else {
      if ((array_index!=-1) && (sym_data_desc.dimension <= array_index)) {
	bbtools_logMsg(req->stream,"%s: index <%d> exceeding symbol array dimension <%d>\n",
		       bbtools_cmdname_tab[E_BBTOOLS_WRITE],
		       array_index,
		       sym_data_desc.dimension);
      } else {
	if (req->verbose) {
	  bbtools_logMsg(req->stream,"Writing <%s> (index=%d)\n",
			 req->argv[2],array_index);
	}
	bb_value_write(req->theBB,sym_data_desc,req->argv[2],array_index);
      }
    }	       
  }
  return retval;
}  /* end of bbtools_write */

int32_t 
bbtools_dump(bbtools_request_t* req) {
  int32_t retcode = 0;
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_DUMP],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: dump BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_DUMP],
		   req->bbname);
  }
  retcode = bb_dump(req->theBB,req->stream);
  return retcode;
}  /* end of bbtools_dump */

int32_t
bbtools_find(bbtools_request_t* req) {
  int32_t    retcode = 0;
  char*      varmatch;
  int32_t    i; 
  int32_t    nmatch = 0;

  if (req->argc<2) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_FIND],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }

  varmatch = strdup(req->argv[1]);

  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: find symbol matching <%s> in  BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_FIND],
		   varmatch,
		   req->bbname);
  }


  for (i=0; i< req->theBB->n_data;++i) {
    if (NULL != strstr((bb_data_desc(req->theBB)[i]).name,varmatch)) {
      fprintf(req->stream,"%s",(bb_data_desc(req->theBB)[i]).name);
      fprintf(req->stream," %s",req->newline);
      ++nmatch;
     } 
   } /* end for */
    
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: found <%d> symbol(s) matching <%s> in  BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_FIND],
		   nmatch,
		   varmatch,
		   req->bbname);
  }
  free(varmatch);
  return retcode;
}  /* end of bbtools_find */

int32_t
bbtools_check_id(bbtools_request_t* req) {
  int32_t retcode=0;
  int32_t user_specific_value;
  char*   shm_name;
  char*   sem_name;
  char*   msg_name;
  
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: at least <%d> argument(s) missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_CHECK_ID],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }


  if (req->argc > 1) {
    user_specific_value = atoi(req->argv[1]);
  } else {
    user_specific_value = getuid();
  }
  if (req->verbose) {
    fprintf(stdout,"Computing BB IPC ID for BB <%s> with specific user value <%d>\n",
	    req->argv[0],user_specific_value);
  }

  shm_name = bb_utils_build_shm_name(req->argv[0]);
  sem_name = bb_utils_build_sem_name(req->argv[0]);
  msg_name = bb_utils_build_msg_name(req->argv[0]);
  bbtools_logMsg(req->stream,
		 "SHM Key [name=<%s>] is 0x%08x\n",
		 shm_name,
		 bb_utils_ntok_user(shm_name,user_specific_value));
  bbtools_logMsg(req->stream,
		 "SEM Key [name=<%s>] is 0x%08x\n",
		 sem_name,
		 bb_utils_ntok_user(sem_name,user_specific_value));
  bbtools_logMsg(req->stream,
		 "MSG Key [name=<%s>] is 0x%08x\n",
		 msg_name,
		 bb_utils_ntok_user(msg_name,user_specific_value));
  
  free(msg_name);
  free(sem_name);    
  free(shm_name);
  return  retcode;
} /* end of bbtools_check_id */

int32_t
bbtools_destroy(bbtools_request_t* req) {
  int32_t retcode = 0;
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_DESTROY],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: destroying BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_DESTROY],
		   req->bbname);
  }
  retcode = bb_destroy(&(req->theBB));
  return retcode;
}  /* end of bbtools_destroy */

int32_t
bbtools_create(bbtools_request_t* req) {
  int32_t retcode = 0;
  uint32_t ndata;
  uint32_t datasize;

  if (req->argc<2) {
    bbtools_logMsg(req->stream,"%s: at least <%d> argument(s) missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_CREATE],
		   2-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }

  req->bbname = req->argv[0];
  ndata =  atoi(req->argv[1]);
  if (req->argc>2) {
    datasize =  atoi(req->argv[2]);
  } else {
    datasize = 3*8*ndata;
  }

  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: creating BB <%s> with <%d> elements and <%d> bytes for datazone\n",
		   bbtools_cmdname_tab[E_BBTOOLS_CREATE],
		   req->bbname,
                   ndata,
                   datasize);
  }
 
  retcode = bb_create(&(req->theBB),req->bbname,ndata,datasize);
  if ((retcode != E_OK) && (req->verbose)) {
    bbtools_logMsg(req->stream,
		   "%s: creating failed to create BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_CREATE],
		   req->bbname);
  }
  return retcode;
}  /* end of bbtools_create */

int32_t
bbtools_publish(bbtools_request_t* req) {
  int32_t retcode = 0;
  char* symbol_type_str;
  S_BB_DATADESC_T  symbol_desc;
  int32_t dimension;

  memset(&symbol_desc,0,sizeof(S_BB_DATADESC_T));
  if (req->argc<2) {
    bbtools_logMsg(req->stream,"%s: at least <%d> argument(s) missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_PUBLISH],
		   2-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  


  if (req->argc>2) {
    symbol_type_str = req->argv[2];
  } else {
    symbol_type_str = "UINT32";
  }

  /* guess if we have an array type or not using parse array... */
  bbtools_parsearrayname(symbol_type_str,&symbol_desc,&dimension);
  if (dimension==-1) {
    /* default dimension is 1 (scalar) */
    symbol_desc.dimension = 1;
  } else {
    symbol_desc.dimension = dimension;
  }
  /* copy symbol name */
  strncpy(symbol_desc.name,req->argv[1],VARNAME_MAX_SIZE);    
  /* find the requested type */
  symbol_desc.type = bb_type_string2bb_type(symbol_type_str);

  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: publish symbol <%s> of type <%s> in BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_PUBLISH],
                   symbol_desc.name,
		   symbol_type_str,
		   req->bbname);
  }

  if (symbol_desc.type!=0) {
    /* guess the type size */
    if (symbol_desc.type != E_BB_USER) {
      symbol_desc.type_size = sizeof_bb_type(symbol_desc.type);
    } else {
      /* FIXME the user case is not supported */
      symbol_desc.type_size = 1;
    }
    /* do we have dimension (array type) */
    
    
    bb_publish(req->theBB,&symbol_desc);
  } else {
    bbtools_logMsg(req->stream,
		   "%s: publish FAILED unrecognized bb_type: <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_PUBLISH],
		   symbol_type_str);
  }

  return retcode;
} /* end of bbtools_publish */

int32_t
bbtools_synchro_send(bbtools_request_t* req) {
  int32_t retcode = 0;
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: sending synchro to BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND],
		   req->bbname);
  }
  
  /* FIXME simple synchro for now 
   * should use bb_snd_msg and enrich 
   * the API
   */
  retcode = bb_simple_synchro_go(req->theBB,BB_SIMPLE_MSGID_SYNCHRO_COPY);
  return retcode;
} /* end of bbtools_synchro_send */

int32_t
bbtools_synchro_recv(bbtools_request_t* req) {
  int32_t retcode = 0;
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: sending synchro to BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_SYNCHRO_SEND],
		   req->bbname);
  }
  
  /* FIXME simple synchro for now 
   * should use bb_snd_recv and enrich 
   * the API
   */
  retcode = bb_simple_synchro_wait(req->theBB,BB_SIMPLE_MSGID_SYNCHRO_COPY);
  return retcode;
} /* end of bbtools_synchro_recv */

int32_t
bbtools_memset(bbtools_request_t* req) {
  int32_t retcode = 0;
  int32_t hexval;
  char    value;

  if (req->argc<2) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_MEMSET],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }

  /* check if we enter hexa or decimal value for memset */
  if ((NULL != strstr(req->argv[1],"0x")) | 
      (NULL != strstr(req->argv[1],"0X"))
      ) {
    hexval = 1;
  } else {
    hexval = 0;
  }

  value  = (char) strtol(req->argv[1],(char **)NULL,hexval ? 16 : 10);
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: setting all data zone of BB <%s> to <0x%02x>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_MEMSET],
		   req->bbname,
		   value,value);
  }
  retcode = bb_data_memset(req->theBB,value);
  return retcode;
} /* end of bbtools_memset */

int32_t
bbtools_check_version(bbtools_request_t* req) {
  int32_t retcode = 0;
  if (req->argc<1) {
    bbtools_logMsg(req->stream,"%s: <%d> argument missing\n", 
		   bbtools_cmdname_tab[E_BBTOOLS_CHECK_VERSION],
		   1-req->argc);
    bbtools_usage(req);
    retcode = -1;
    return retcode;
  }
  if (req->verbose) {
    bbtools_logMsg(req->stream,
		   "%s: checking BB version for BB <%s>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_CHECK_VERSION],
		   req->bbname);
    
  }
  retcode = bb_check_version(req->theBB);
  if (retcode != 0) {
     bbtools_logMsg(req->stream,
		   "%s: ERROR BB version mismatch !!\nbb_tools with BB version <0x%08X> used to access BB version <0x%08X>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_CHECK_VERSION],
		    BB_VERSION_ID,req->theBB->bb_version_id);
  } else {
     bbtools_logMsg(req->stream,
		   "%s: OK.\nbb_tools and accessed BB [%s] version are the same <0x%08X>\n",
		   bbtools_cmdname_tab[E_BBTOOLS_CHECK_VERSION],
		    req->bbname,
		   req->theBB->bb_version_id);
  }
  return retcode;
} /* end of bbtools_check_version */
