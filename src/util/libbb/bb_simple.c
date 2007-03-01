/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_simple.c,v 1.15 2007-03-01 18:45:13 deweerdt Exp $

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
#ifndef __KERNEL__
#include <sys/types.h>
#include <sys/msg.h>
#include <string.h>
#endif /* __KERNEL__ */

#include "bb_utils.h"
#include "bb_core.h"
#include "bb_alias.h"
#include "bb_simple.h"

static int bb_simple_synchro_type = BB_SIMPLE_SYNCHRO_PROCESS;

#ifndef __KERNEL__

pthread_cond_t  bb_simple_go_condvar       = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_go_mutex         = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  bb_simple_stockage_condvar = PTHREAD_COND_INITIALIZER;
pthread_mutex_t bb_simple_stcokage_mutex   = PTHREAD_MUTEX_INITIALIZER;

#endif /* __KERNEL__ */

static void gen_var_name(char *gen_var_name, const int module_instance,
			                   const char *module_name, const char *var_name)
{
  memset(gen_var_name,0,bb_varname_max_len()); 
  if (strcmp(module_name,"")==0) {
    snprintf(gen_var_name,bb_varname_max_len(), "%s", var_name);
  } else if (-1!=module_instance) {
    snprintf(gen_var_name,bb_varname_max_len(), "%s_%d_%s",
	           module_name,module_instance,var_name);
  } else {
    snprintf(gen_var_name,bb_varname_max_len(), "%s_%s", 
             module_name,var_name);
  }
}

void* bb_simple_publish(S_BB_T* bb_simple,
    const char* var_name, 
    const char* module_name,
    const int module_instance,
    E_BB_TYPE_T type,
    int type_size,
    int dimension) {

  void* retval;
  S_BB_DATADESC_T s_data_desc;
  char *new_name = alloca(bb_varname_max_len());
  int ret;

  gen_var_name(new_name, module_instance, module_name, var_name);
  ret = bb_set_varname(&s_data_desc, new_name);
	if (ret == BB_NOK)
		return NULL;
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
  char *new_name = alloca(bb_varname_max_len());
	int ret;

  gen_var_name(new_name, module_instance, module_name, var_name);
  ret = bb_set_varname(&s_data_desc, new_name);
 	if (ret == BB_NOK)
		return NULL;
 s_data_desc.type        = *type;
  s_data_desc.type_size   = *type_size;
  s_data_desc.dimension   = *dimension;
  retval = bb_subscribe(bb_simple,&s_data_desc);  
  return retval;
} /* end of bb_simple_subscribe */


void* bb_simple_alias_publish(S_BB_T* bb_simple,
			      const char* var_name,
			      const char* target_name, 
			      const char* module_name,
			      const int module_instance,
			      E_BB_TYPE_T type,
			      int type_size,
			      int dimension,
			      unsigned long offset) {
  
  void* retval=NULL;
  int32_t idx;
  char *new_target_name = alloca(bb_varname_max_len());
  char *alias_name = alloca(bb_varname_max_len());
	int ret;
  S_BB_DATADESC_T alias;
  S_BB_DATADESC_T target;
  
  gen_var_name(new_target_name, module_instance, module_name, target_name);
  snprintf(alias_name, bb_varname_max_len(), "%s_%s",
	   new_target_name, var_name);
  ret = bb_set_varname(&alias, alias_name);
  if (ret == BB_NOK)
    return NULL;

  alias.type        = type;
  alias.type_size   = type_size;
  alias.dimension   = dimension;
  alias.data_offset = offset;
  
  /* find the index of the target (it is the target for the aliases we want to publish) */                                                                                                                          
  idx = bb_find( bb_simple, new_target_name);
  if (idx ==-1) {
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_alias_publish", 
		"Cannot find the target <%s> of the alias <%s>",
		target_name, var_name);
  } else {
    char *n;
		int ret;
    target = bb_data_desc(bb_simple)[idx];

    n = bb_get_varname(&target);
    snprintf(alias_name,bb_varname_max_len(),
        "%s.%s", n, var_name);
    free(n);
    ret = bb_set_varname(&alias, alias_name);
		if (ret == BB_NOK)
			return NULL;

    retval=(void *)bb_alias_publish(bb_simple, &alias, &target);

    if (retval == NULL) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_simple_alias_publish", 
          "Cannot publish data <%s> instance <%d> module <%s>",
          var_name,module_instance,module_name);
    }
  } 
  return retval;
} /* end of bb_simple_alias_publish */



int32_t 
bb_simple_synchro_config(int synchro_type) {
  
  int32_t retcode=BB_OK;
  
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
  
  return bb_msgq_isalive(bb_simple);
} /* end of bb_simple_synchro_verify */

#ifdef __KERNEL__
int32_t
bb_simple_thread_synchro_go(int msg_type)
{
	return BB_OK;
}
#else
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
  retcode = BB_OK;
  
  return retcode;
} /* end of bb_simple_synchro_go */
#endif /* __KERNEL__ */

#ifdef __KERNEL__
int32_t 
bb_simple_thread_synchro_wait(int msg_type) 
{
	return BB_OK;
}
#else
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
  retcode = BB_OK;
  
  return retcode;
} /* end of bb_simple_synchro_wait */
#endif /* __KERNEL__ */

#ifdef __KERNEL__
EXPORT_SYMBOL_GPL(bb_simple_publish);
#endif

