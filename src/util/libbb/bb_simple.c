/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_simple.c,v 1.6 2005-03-17 22:33:42 dufy Exp $

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

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>

#include <bb_utils.h>
#include <bb_core.h>
#include <bb_simple.h>

static int bb_simple_synchro_type = BB_SIMPLE_SYNCHRO_PROCESS;

pthread_cond_t  bb_simple_go_condvar       = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_go_mutex         = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  bb_simple_stockage_condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_stcokage_mutex   = PTHREAD_MUTEX_INITIALIZER;

void* bb_simple_publish(S_BB_T* bb_simple,
			const char* var_name, 
			const char* module_name,
			const int module_instance,
			E_BB_TYPE_T type,
			int type_size,
			int dimension) {
  
  void* retval;
  S_BB_DATADESC_T s_data_desc;

  memset(s_data_desc.name,0,VARNAME_MAX_SIZE);
  
  if (strcmp(module_name,"")==0) {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s",
	     var_name);
  } else if (-1!=module_instance) {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%d_%s",
	     module_name,module_instance,var_name);
  } else {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%s",
	     module_name,var_name);
  }

  s_data_desc.type        = type;
  s_data_desc.type_size   = type_size;
  s_data_desc.dimension   = dimension;
  retval = bb_publish(bb_simple,&s_data_desc);
  if (retval == NULL) {
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_publish", 
		"Cannot publish data <%s> instance <%d> module <%s>",
		var_name,module_instance,module_name);
  }  
  
  return retval;
} /* end of bb_simple_publish */


void* bb_simple_subscribe(S_BB_T* bb_simple,
			  const char* var_name,
			  const char* module_name,
			  const int module_instance,
			  E_BB_TYPE_T*  type,
			  int* type_size,
			  int* dimension) {
  
  void* retval;
  S_BB_DATADESC_T s_data_desc;

  memset(s_data_desc.name,0,VARNAME_MAX_SIZE);
  if (0!=module_instance) {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%d_%s",
	     module_name,module_instance,var_name);
  } else {
    snprintf(s_data_desc.name,VARNAME_MAX_SIZE,
	     "%s_%s",
	     module_name,var_name);
  }
  
  retval = bb_subscribe(bb_simple,&s_data_desc);  
  *type      = s_data_desc.type;
  *type_size = s_data_desc.type_size;
  *dimension = s_data_desc.dimension;
  return retval;
} /* end of bb_simple_subscribe */



int32_t 
bb_simple_synchro_config(int synchro_type) {
  
  int32_t retcode=E_OK;
  
  switch (synchro_type) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
  case BB_SIMPLE_SYNCHRO_THREAD:
    bb_simple_synchro_type = synchro_type;
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_synchro_config",
		"Invalid synchro type <%d>, BB_SIMPLE_SYNCHRO_PROCESS <%d> used",
		synchro_type,BB_SIMPLE_SYNCHRO_PROCESS);
    bb_simple_synchro_type = BB_SIMPLE_SYNCHRO_PROCESS;
  }
  
  return retcode;
} /* end of bb_simple_synchro_config */

int32_t 
bb_simple_synchro_go(S_BB_T* bb_simple,int type_msg) {
  
  int32_t retcode;
  S_BB_MSG_T s_bb_msg;
  
  switch (bb_simple_synchro_type) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
    s_bb_msg.mtype = type_msg;
    retcode = bb_snd_msg(bb_simple,&s_bb_msg);
    break;
  case BB_SIMPLE_SYNCHRO_THREAD:
    retcode=bb_simple_thread_synchro_go(type_msg);
    break;
  default:
    retcode=bb_simple_thread_synchro_go(type_msg); 
  }
  
  return retcode;
} /* end of bb_simple_synchro_go */

int32_t 
bb_simple_synchro_wait(S_BB_T* bb_simple,int type_msg) {
  
  int32_t retcode;
  S_BB_MSG_T s_bb_msg;
  
  switch (bb_simple_synchro_type) {
  case BB_SIMPLE_SYNCHRO_PROCESS:
    s_bb_msg.mtype = type_msg;
    retcode = bb_rcv_msg(bb_simple,&s_bb_msg);      
    break;
  case BB_SIMPLE_SYNCHRO_THREAD:
    retcode=bb_simple_thread_synchro_wait(type_msg);
    break;
  default:
    retcode=bb_simple_thread_synchro_wait(type_msg);    
  }
  
  return retcode;
} /* end of bb_simple_synchro_wait */

int32_t 
bb_simple_synchro_verify(S_BB_T* bb_simple) {
  
  int32_t retcode;
  struct msqid_ds s_msgq_stat;

  msgctl(bb_msg_id(bb_simple),
	 IPC_STAT,
	 &s_msgq_stat);
  
  if (s_msgq_stat.msg_qnum > 0) {
    retcode = E_NOK;
    
  } else {
    retcode = E_OK;
  }  
  
  return retcode;
} /* end of bb_simple_synchro_verify */

int32_t 
bb_simple_thread_synchro_go(int msg_type) {
  
  int32_t retcode;
  
  switch (msg_type) {
  case BB_SIMPLE_MSGID_SYNCHRO_COPY:
    pthread_mutex_lock(&bb_simple_go_mutex);
    pthread_cond_broadcast(&bb_simple_go_condvar);
    pthread_mutex_unlock(&bb_simple_go_mutex);
    break;
  case BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK:
    pthread_mutex_lock(&bb_simple_stockage_mutex);
    pthread_cond_broadcast(&bb_simple_stockage_condvar);
    pthread_mutex_unlock(&bb_simple_stockage_mutex);
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_thread_synchro_go", 
	      "Invalid thread synchro ID <%d>",msg_type);
  }
  /* pthread_cond_broadcast always succeed */
  retcode = E_OK;
  
  return retcode;
} /* end of bb_simple_synchro_go */

int32_t 
bb_simple_thread_synchro_wait(int msg_type) {
  
  int32_t retcode;
  
  switch (msg_type) {
  case BB_SIMPLE_MSGID_SYNCHRO_COPY:
    pthread_mutex_lock(&bb_simple_go_mutex);
    pthread_cond_wait(&bb_simple_go_condvar,&bb_simple_go_mutex);
    pthread_mutex_unlock(&bb_simple_go_mutex);
    break;
  case BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK:
    pthread_mutex_lock(&bb_simple_stockage_mutex);
    pthread_cond_wait(&bb_simple_stockage_condvar,&bb_simple_stockage_mutex);
    pthread_mutex_unlock(&bb_simple_stockage_mutex);
    break;
  default:
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_thread_synchro_wait", 
	      "Invalid thread synchro ID <%d>",msg_type);
  }
  /* pthread_cond_wait always succeed (or never return)*/
  retcode = E_OK;
  
  return retcode;
} /* end of bb_simple_synchro_wait */
