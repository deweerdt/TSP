/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core.c,v 1.11 2005-02-23 09:18:09 dufy Exp $

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

Purpose   : Blackboard Idiom implementation

-----------------------------------------------------------------------
 */
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include <bb_core.h>
#include <bb_utils.h>
#include "tsp_abs_types.h"

/**
 * Convert type to string for display use.
 */
static const char* E_BB_2STRING[] = {"NoAType 0",
				     "double", 
				     "float", 
				     "int8_t",
				     "int16_t",
				     "int32_t",
				     "int64_t",
				     "uint8_t",
				     "uint16_t",
				     "uint32_t",
				     "uint64_t",
				     "char",
				     "uchar",
				     "UserType",
				     "NotAType end"};

int32_t 
bb_size(const int32_t n_data, const int32_t data_size) {
  /* The SHM segment is sized to:
   *  BB structure size +
   *  data descriptor array size +
   *  data zone size
   */
  return (sizeof(S_BB_T) + 
    sizeof(S_BB_DATADESC_T)*n_data +
    sizeof(char)*data_size);
} /* end of bb_size */

int32_t 
bb_find(volatile S_BB_T* bb, const char* var_name) {
  
  int32_t retval;  
  int32_t i;
    
  retval = -1;
  assert(bb);
  for (i=0; i< bb->n_data;++i) {
    if (!strncmp(var_name,(bb_data_desc(bb)[i]).name,VARNAME_MAX_SIZE+1)) {
      retval = i;
      break;
    }
  } /* end for */
  
  return retval;
} /* end of  bb_find */

S_BB_DATADESC_T* 
bb_data_desc(volatile S_BB_T* bb) {
  
  S_BB_DATADESC_T* retval;  
  
  retval = NULL;
  assert(bb);
  retval = (S_BB_DATADESC_T*) ((char*)(bb) + bb->data_desc_offset);
  
  return retval;
} /* end of bb_data_desc */

void* 
bb_data(volatile S_BB_T* bb) {
  
  void* retval;  
  
  retval = NULL;
  assert(bb);
  retval = (char*)(bb) + bb->data_offset;
  
  return retval;
} /* end of bb_data_desc */

double
bb_double_of(void* value, E_BB_TYPE_T bbtype) {

  double retval;

  retval = 0.0;

  switch (bbtype) {
  case E_BB_DOUBLE: 
    retval = *((double*)value);
    break;
  case E_BB_FLOAT:
    retval = *((float*)value);
    break;
  case E_BB_INT8:
    retval = *((int8_t*)value);
    break; 
  case E_BB_INT16:
    retval = *((int16_t*)value);
    break; 
  case E_BB_INT32:
    retval = *((int32_t*)value);
    break; 
  case E_BB_INT64:
    retval = *((int64_t*)value);
    break; 
  case E_BB_UINT8:
    retval = *((uint8_t*)value);
    break;  
  case E_BB_UINT16:
    retval = *((uint16_t*)value);
    break;
  case E_BB_UINT32:
    retval = *((uint32_t*)value);
    break;	
  case E_BB_UINT64:
    retval = *((uint64_t*)value);
    break;	
  case E_BB_CHAR:
  case E_BB_UCHAR:
  case E_BB_USER:
    /* FIXME could not convert set 0.0 */
    retval = 0.0;
    break; 
  default:
    retval = 0.0;
    break;
  }

  return retval;
} /* end of bb_double_of */

int32_t 
bb_data_initialise(volatile S_BB_T* bb, S_BB_DATADESC_T* data_desc,void* default_value) {
  
  int32_t retval;  
  char* data;
  int32_t i;
  
  assert(data_desc);
  /* on recupere l'adresse de la donnee dans le BB */
  data = (char*)bb_data(bb) + data_desc->data_offset;
  retval = E_OK;
  for (i=0; i< data_desc->dimension; ++i) {
    switch (data_desc->type) {
    case E_BB_DOUBLE: 
      ((double*) data)[i] = default_value ? *((double *) default_value) : 0.0;
      break;
    case E_BB_FLOAT:
      ((float*) data)[i] = default_value ? *((float *) default_value) : 0.0;
      break;
    case E_BB_INT16:
      ((int16_t*) data)[i] = default_value ? *((short *) default_value) : 0;
      break; 
    case E_BB_INT32:
      ((int32_t*) data)[i] = default_value ? *((int *) default_value) : 0;
      break; 
    case E_BB_INT64:
      ((int64_t*) data)[i] = default_value ? *((long *) default_value) : 0;
      break; 
    case E_BB_UINT16:
      ((uint16_t*) data)[i] = default_value ? *((unsigned short *) default_value) : 0;
      break;
    case E_BB_UINT32:
      ((uint32_t*) data)[i] = default_value ? *((unsigned int *) default_value) : 0;
      break;	
    case E_BB_UINT64:
      ((uint64_t*) data)[i] = default_value ? *((unsigned long *) default_value) : 0;
      break;	
    case E_BB_CHAR:
      ((char *) data)[i] = default_value ? *((char *) default_value) : '\0';
      break;
    case E_BB_USER:
      if (NULL == default_value) {
	memset(data + (data_desc->type)*i,0,data_desc->type);
      } else {
	memcpy(data + (data_desc->type)*i,default_value,data_desc->type);
      }
      break; 
    default:
      retval = E_NOK;
      break;
    }
  } 
  
  return retval;
} /* end of bb_data_initialise */

int32_t
bb_value_write(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc,const char* value, int32_t idx) {

  char* data;
  int retval;
  int hexval;
  assert(bb);
  
  retval = E_OK;

  if ((NULL != strstr(value,"0x")) | 
      (NULL != strstr(value,"0X"))
      ) {
    hexval = 1;
  } else {
    hexval = 0;
  }

  /* on recupere l'adresse de la donnee dans le BB */
  data = (char*)bb_data(bb) + data_desc.data_offset;
  switch (data_desc.type) {
    case E_BB_DOUBLE: 
      ((double *)data)[idx] = atof(value);
      break;
    case E_BB_FLOAT:
      ((float *)data)[idx] = atof(value);
      break;
    case E_BB_INT16:
      ((int16_t*)data)[idx] = strtol(value,(char **)NULL,hexval ? 16 : 10);
      break; 
    case E_BB_INT32:
      ((int32_t*)data)[idx] = strtol(value,(char **)NULL,hexval ? 16 : 10);
      break; 
    case E_BB_INT64:
      ((int64_t*)data)[idx] = strtoll(value,(char **)NULL,hexval ? 16 : 10);
      break; 
    case E_BB_UINT16:
      ((uint16_t*)data)[idx] = strtoul(value,(char **)NULL,hexval ? 16 : 10);
      break;
    case E_BB_UINT32:
      ((uint32_t*)data)[idx] = strtoul(value,(char **)NULL,hexval ? 16 : 10);
      break;	
    case E_BB_UINT64:
      ((uint64_t*)data)[idx] = strtoull(value,(char **)NULL,hexval ? 16 : 10);
      break;	
    case E_BB_CHAR:
      retval = E_NOK;
      break;
    case E_BB_USER:
      retval = E_NOK;
      break; 
    default:
      retval = E_NOK;
      break;
    }
  return retval;
}

int32_t
bb_data_header_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx) {

  fprintf(pf,"---------- < %s > ----------\n",data_desc.name);
  fprintf(pf,"  type        = %d  (%s)\n",data_desc.type,E_BB_2STRING[data_desc.type]);
  fprintf(pf,"  dimension   = %d  \n",data_desc.dimension);
  fprintf(pf,"  type_size   = %d  \n",data_desc.type_size);
  fprintf(pf,"  data_offset = %ld \n",data_desc.data_offset);
  if (idx>=0) {
    fprintf(pf,"  value[%d] = ",idx);
  } else {
    fprintf(pf,"  value = ");
  }
  if (data_desc.dimension > 1) {
    fprintf(pf," [ ");
  } 
  if (idx>=0) {
    fprintf(pf,"... ");
  }
  return 0;
} /* end of bb_data_header_print */

int32_t
bb_data_footer_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx) {

  if (idx>=0) {
    fprintf(pf,"... ");
  }  
  if (data_desc.dimension > 1) {
    fprintf(pf,"]");
  }  
  fprintf(pf,"\n");    
  fprintf(pf,"---------- ---------- ----------\n");

  return 0;
} /* end of bb_data_footer_print */

int32_t 
bb_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx) {
  
  int32_t i,j,ibeg,iend;
  char* data;
  assert(bb);
  /* on recupere l'adresse de la donnee dans le BB */
  data = (char*)bb_data(bb) + data_desc.data_offset;
  
  if (idx>=0) {
    ibeg=idx;
    iend=idx+1;
  } else {
    ibeg=0;
    iend=data_desc.dimension;
  }

  for (i=ibeg; i< iend; ++i) {
    
    switch (data_desc.type) {
    case E_BB_DOUBLE: 
      fprintf(pf,"%1.16f ",((double*) data)[i]);
      break;
    case E_BB_FLOAT:
      fprintf(pf,"%f ",((float*) data)[i]);
      break;
    case E_BB_INT16:
      fprintf(pf,"%d ",((int16_t*) data)[i]);
      break; 
    case E_BB_INT32:
      fprintf(pf,"%d ",((int32_t*) data)[i]);
      break; 
    case E_BB_INT64:
      fprintf(pf,"%lld ",((int64_t*) data)[i]);
      break; 
    case E_BB_UINT16:
      fprintf(pf,"0x%x ",((uint16_t*) data)[i]);
      break;
    case E_BB_UINT32:
      fprintf(pf,"0x%x ",((uint32_t*) data)[i]);
      break;	
    case E_BB_UINT64:
      fprintf(pf,"0x%llx ",((uint64_t*) data)[i]);
      break;	
    case E_BB_CHAR:
      fprintf(pf,"0x%02x<%c> ",((char*) data)[i],
	      isprint((int)data[i]) ? ((char*) data)[i] : '?');
      break;
    case E_BB_USER:
      for (j=0; j<data_desc.type_size; ++j) {
	fprintf(pf,"0x%02x ",((uint8_t*) data)[i*data_desc.type_size+j]);
      }
      break; 
    default:
      fprintf(pf,"0x%x ",((char*) data)[i]);
      break;
    }
  } 
  return 0;
} /* end of bb_value_print */

int32_t 
bb_data_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf) {

  bb_data_header_print(data_desc,pf,-1);
  bb_value_print(bb,data_desc,pf,-1);
  bb_data_footer_print(data_desc,pf,-1);
  return E_OK;
} /* end of bb_data_print */

int32_t 
bb_create(S_BB_T** bb, 
	       const char* pc_bb_name,
	       int32_t n_data,
	       int32_t data_size) {

  
  int32_t retcode;
  char* name_shm;
  char* name_sem;
  char* name_msg;
  int32_t  fd_shm;
  int32_t  mmap_size;
  char syserr[MAX_SYSMSG_SIZE];
  union semun u_sem_ctrl;
  struct sembuf s_semop;
  
  assert(bb);
  retcode = E_OK;  
  name_shm = bb_utils_build_shm_name(pc_bb_name);
  name_sem = NULL;
  name_msg = NULL;
  bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_create", 
	      "Create BB <%s>.",pc_bb_name);
  fd_shm      = 0;
  mmap_size = 0;

  /* open+create du segment SHM SysV */
  if (name_shm!=NULL) {
    /* On dimensionne le segment shm a
     * la taille d'une structure de BB +
     * la taille du tableau descripteur de donnee +
     * la taille de la zone de donnee
     */
    mmap_size = bb_size(n_data,data_size);    
    fd_shm = shmget(bb_utils_ntok(name_shm), mmap_size, IPC_CREAT|IPC_EXCL|BB_SHM_ACCESS_RIGHT);        
  } else {
    retcode = E_NOK;
  }
  if((retcode==E_OK) && (-1==fd_shm)) {
    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    /* Try to create an existing segment is NOT an error
     * its an info that should be handled by the caller.
     */
    if (EEXIST!=errno)  {

      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_create", 
		"Cannot open shm segment <%s key = 0x%x> (%s)",
		name_shm, bb_utils_ntok(name_shm), syserr);
    } else {
      bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_create", 
		"Cannot open shm segment <%s key = 0x%x> (%s)",
		name_shm, bb_utils_ntok(name_shm), syserr);

    }
    retcode = E_NOK; 
  }

  if (E_OK == retcode) {
    *bb = (S_BB_T*) shmat(fd_shm,NULL,0);
  }

  if ((E_OK == retcode) && ((void *)-1 == *bb)) {
    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_create", 
		"Cannot attach shm segment (%s)",syserr);
    retcode = E_NOK; 
  } 
  /* On initialise la structure comme il se doit */
  if (E_OK == retcode) {
    /* RAZ de la mémoire allouee */
    memset(*bb,0,mmap_size);        
    strncpy((*bb)->name,pc_bb_name,BB_NAME_MAX_SIZE+1);
    (*bb)->max_data_desc_size = n_data;
    (*bb)->data_desc_offset = sizeof(S_BB_T);
    
    (*bb)->n_data = 0;
    (*bb)->max_data_size = data_size;
    (*bb)->data_offset = (*bb)->data_desc_offset + 
                            ((*bb)->max_data_desc_size)*sizeof(S_BB_DATADESC_T);    
    (*bb)->data_free_offset = 0;  
    (*bb)->destroyed = 0;
  }
  /* On initialise la structure comme il se doit */

  /* Initialisation du semaphore d'acces au BB */
  if (E_OK == retcode) {
    name_sem = bb_utils_build_sem_name((*bb)->name);
    (*bb)->semid = semget(bb_utils_ntok(name_sem),1,
			       IPC_CREAT|IPC_EXCL|BB_SEM_ACCESS_RIGHT);
    
    if (-1==((*bb)->semid)) {
      strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_create", 
		"Cannot create semaphore <%s key = 0x%x> (%s)",
		name_sem, bb_utils_ntok(name_sem), syserr);
      retcode = E_NOK; 
    } else {
      u_sem_ctrl.val = 2;
      if (-1 == semctl((*bb)->semid,0,SETVAL,u_sem_ctrl)) {
	strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
	bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_create", 
		    "Cannot initialise semaphore <%s key = 0x%x> (%s)",
		    name_sem, bb_utils_ntok(name_sem), syserr);
	retcode = E_NOK;
      }
    }
    /* appeler 1 fois semop pour eviter
     * les races conditions entre le createur
     * du sémaphore et les clients (cf UNPV2 pages 284--285)
     */
    if (E_OK == retcode) {
      s_semop.sem_num = 0; 
      s_semop.sem_op  = -1; 
      s_semop.sem_flg = 0; 
      semop((*bb)->semid,&s_semop,1);
    }    
    free(name_sem);

    /* Initialisation de la queue de message liee au BB */
    if (E_OK == retcode) {     
      name_msg = bb_utils_build_msg_name((*bb)->name);
      (*bb)->msgid = msgget(bb_utils_ntok(name_msg),
				 IPC_CREAT|IPC_EXCL|BB_MSG_ACCESS_RIGHT);
      
      if (-1==((*bb)->msgid)) {
	strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
	bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_create", 
		    "Cannot message queue <%s key = 0x%x> (%s)",
		    name_msg, bb_utils_ntok(name_msg), syserr);
	retcode = E_NOK; 
      } 
    }
    free(name_msg);
  }
  free(name_shm);

  
  return retcode;
} /* end of bb_create */

int32_t 
bb_destroy(S_BB_T** bb) {
  
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  char* name_shm;
  int32_t fd_shm;
  int32_t local_semid;
  int32_t local_msgid;
  
  retcode = E_OK;
  assert(bb);
  assert(*bb);  
  bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_destroy", 
		"Destroy BB <%s>.",(*bb)->name);
  name_shm = bb_utils_build_shm_name((*bb)->name);
  /* 
   * On signale la destruction en cours pour les processes qui
   * resteraient attachés
   */
  (*bb)->destroyed = 1;

  /* On mémorise les IDs
   * des semaphore et autres queue de message
   * pour les détruire après la destruction de la SHM
   */
  local_semid = (*bb)->semid;
  local_msgid = (*bb)->msgid;
  
  /* On programme la destruction du segment SHM */
  fd_shm = shmget(bb_utils_ntok(name_shm),0,BB_SHM_ACCESS_RIGHT);
  if (-1 == shmctl(fd_shm,IPC_RMID,NULL)) {
    strncpy(&syserr[0],strerror(errno),MAX_SYSMSG_SIZE);
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_destroy", 
		"SHM Destroy failed (%s)",syserr);
    retcode = E_NOK;
  }
  free(name_shm);
  
  /* on se detache de façon a provoquer la destruction */
  if (E_OK == retcode) {
    if (-1 == shmdt((void*)*bb)) {
      strncpy(&syserr[0],strerror(errno),MAX_SYSMSG_SIZE);
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_destroy", 
		"SHM detach failed (%s)",syserr);
      retcode = E_NOK;
    }
  }
  /* FIXME doit-on prendre le semaphore avant de le détruire ?? */
  /* On programme la destruction du SEMAPHORE */
  semctl(local_semid,0,IPC_RMID);
  /* On programme la destruction de la QUEUE DE MESSAGE */
  msgctl(local_msgid,IPC_RMID,NULL);
  *bb = NULL;
    
  return retcode;
} /* end of bb_destroy */

int32_t
bb_data_memset(S_BB_T* bb, const char c) { 
  int32_t retcode;  
  assert(bb);
  retcode = 0;
  memset(bb_data(bb),c,bb->max_data_size);
  return retcode;
} /* end of bb_data_memset */

int32_t 
bb_lock(volatile S_BB_T* bb) {
  
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  struct sembuf s_semop;
  
  assert(bb);
  s_semop.sem_num = 0; 
  s_semop.sem_op  = -1; 
  s_semop.sem_flg = SEM_UNDO;   
  if (-1 == semop(bb->semid,&s_semop,1)) {
    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    if (EINVAL == errno) {
      bb_logMsg(BB_LOG_WARNING, "BlackBoard::bb_lock",
		  "Is BB semaphore destroyed?");
    }
    else { 
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_lock", 
		  "semop failed (%s)", syserr);
    }
    retcode = E_NOK;
  } else {
    retcode = E_OK; 
  }
  
  return retcode;
} /* end of bb_lock */

int32_t 
bb_unlock(volatile S_BB_T* bb) {
  
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  struct sembuf s_semop;
    
  s_semop.sem_num = 0; 
  s_semop.sem_op  = 1; 
  s_semop.sem_flg = SEM_UNDO;   
  if (-1 == semop(bb->semid,&s_semop,1)) {
    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    if (EINVAL == errno) {
      bb_logMsg(BB_LOG_WARNING, "BlackBoard::bb_unlock",
		  "Semaphore du BB detruit");
    }
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_unlock", 
		"semop failed (%s)", syserr);
    retcode = E_NOK;
  } else {
    retcode = E_OK; 
  }
  
  return retcode;
} /* end of bb_unlock */


int32_t 
bb_attach(S_BB_T** bb, const char* pc_bb_name) {
    
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  char* name_shm;
  int32_t  fd_shm;
  
  retcode = E_OK;
  fd_shm  = 0;
  assert(bb);
  
  name_shm = bb_utils_build_shm_name(pc_bb_name);
/*   bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_attach",  */
/* 	 "Attachement au BB <%s>.",pc_bb_name); */
  if (name_shm!=NULL) {
    /* On recupere l'acces au SHM */
    fd_shm = shmget(bb_utils_ntok(name_shm), 0, BB_SHM_ACCESS_RIGHT);        
  } else {
    retcode = E_NOK;
  }
  if((E_OK==retcode) && (-1==fd_shm)) {

    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    /* attaching to a non-existing segment is NOT an error
     * its an info that should be handled by the caller.
     */
    if (ENOENT!=errno) {

      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_attach", 
		"Cannot open shm segment <%s key = 0x%x> (%s)",
		name_shm, bb_utils_ntok(name_shm), syserr);
    } else {
      bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_attach", 
		"Cannot open shm segment <%s key = 0x%x> (%s)",
		name_shm, bb_utils_ntok(name_shm), syserr);

    }
    retcode = E_NOK; 
  }
  free(name_shm);

  if (E_OK == retcode) {
    *bb = (S_BB_T*) shmat(fd_shm,NULL,0);
  }
  if ((E_OK == retcode) && ((void *)-1 == *bb)) {
    strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_attach", 
		"Cannot attach shm segment (%s)",syserr);
    retcode = E_NOK; 
  }   
  
  return retcode;
} /* end of bb_attach */

int32_t 
bb_detach(S_BB_T** bb) {
  
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  
  retcode = E_OK;
  assert(bb);
  assert(*bb);
/*   bb_logMsg(BB_LOG_INFO,"BlackBoard::bb_detach",  */
/* 	      "Detachement du BB <%s>.",(*bb)->name); */

  if (-1 == shmdt((void*)*bb)) {
    strncpy(&syserr[0],strerror(errno),MAX_SYSMSG_SIZE);
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_detach", 
		"SHM detach failed (%s)",syserr);
    retcode = E_NOK;
  }

  *bb = NULL;
  
  return retcode;
} /* end of bb_detach */

void* 
bb_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc) {
  
  void* retval;
  int32_t needed_size;

  retval = NULL;
  assert(bb);
  assert(data_desc);
  
  /* Verify that the published data is not already published
   * (key unicity)
   */
  bb_lock(bb);
  if (bb_find(bb,data_desc->name) != -1) {
     bb_logMsg(BB_LOG_FINER,"BlackBoard::bb_publish",
	       "Key <%s> already exists in blackboard (automatic subscribe)!!",data_desc->name);
    bb_unlock(bb);
    retval = bb_subscribe(bb,data_desc);
    bb_lock(bb);
  } else {
    /* calcul de la taille demandee */
    needed_size = data_desc->type_size*data_desc->dimension;    
    /* verification espace disponible dans BB */
    if (bb->n_data >= bb->max_data_desc_size) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_publish", 
		"No more room in BB data descriptor!! [current n_data=%d]",
		bb->n_data);
      /* verification taille donnee allouable */
    } else if ((bb->max_data_size-bb->data_free_offset) < needed_size) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_publish", 
		"No more room in BB data zone!! [left <%d> byte(s) out of <%d> required]",
		bb->max_data_size-bb->data_free_offset,needed_size);
    } else {     
      /* Calcule de l'adresse libre */
      retval = (char*) bb_data(bb) + bb->data_free_offset;
      /* Mise a jour descripteur en retour */
      data_desc->data_offset = bb->data_free_offset;
      /* Mise a jour prochaine @ libre */
      bb->data_free_offset  = bb->data_free_offset + needed_size;
      /* Mise a jour descripteur de donnee */
      bb_data_desc(bb)[bb->n_data] = *data_desc;
      /* On augmente le nombre de donnees */
      bb->n_data++;
    }
    /* initialisation à des valeurs par defaut */
    bb_data_initialise(bb,data_desc,NULL);
  }    
  /* no init in case of automatic subscribe */  
  bb_unlock(bb);  
  return retval;
} /* end of bb_publish */


void* 
bb_subscribe(volatile S_BB_T *bb, 
	     S_BB_DATADESC_T* data_desc) {
  
  void* retval;
  int32_t  idx;
  
  retval = NULL;
  assert(bb);
  assert(data_desc);
  
  /* on cherche la donnee publiee par sa clef */
  bb_lock(bb);
  idx = bb_find(bb,data_desc->name);
  if (idx==-1) {
    retval = NULL;      
  } else {
    data_desc->type            = (bb_data_desc(bb)[idx]).type;
    data_desc->dimension       = (bb_data_desc(bb)[idx]).dimension;
    data_desc->type_size       = (bb_data_desc(bb)[idx]).type_size;
    data_desc->data_offset     = (bb_data_desc(bb)[idx]).data_offset;
    retval = (char*) bb_data(bb) + data_desc->data_offset;
  }
  bb_unlock(bb);

  return retval;
} /* end of bb_subscribe */


int32_t 
bb_dump(volatile S_BB_T *bb,FILE* p_filedesc) {  
  
  int32_t retcode;
  /* char syserr[MAX_SYSMSG_SIZE]; */
  int32_t i;

  
  retcode = E_OK;
  assert(bb);
  fprintf(p_filedesc,"============= <[begin] BlackBoard [%s] [begin] > ===============\n",
	  bb->name);
  fprintf(p_filedesc,"  @start blackboard    = 0x%x\n",(unsigned int) (bb));
  fprintf(p_filedesc,"  stored data          = %d / %d [max desc]\n",
	  bb->n_data,
	  bb->max_data_desc_size);
  fprintf(p_filedesc,"  free data size       = %ld / %ld\n",
	  bb->max_data_size - bb->data_free_offset + 1,
	  bb->max_data_size);  
  fprintf(p_filedesc,"  @data_desc           = 0x%x\n",
	  (unsigned int) (bb_data_desc(bb)));
  fprintf(p_filedesc,"  @data                = 0x%x\n",
	  (unsigned int) (bb_data(bb)));
  fprintf(p_filedesc,"================ < [begin] Data [begin] > ==================\n");
  for (i=0;i<bb->n_data;++i) {
    bb_data_print(bb,bb_data_desc(bb)[i],p_filedesc);
  }
  fprintf(p_filedesc,"================== < [end] Data [end] > ====================\n");
  fprintf(p_filedesc,"============== < [end] BlackBoard [%s] [end] > ================\n",
	  bb->name);
  
  return retcode;
} /* end of bb_dump */

int32_t
bb_get_nb_max_item(volatile S_BB_T *bb) {
  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb->max_data_desc_size;
  
  return retval;
}

int32_t
bb_get_nb_item(volatile S_BB_T *bb) {

  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb->n_data;
  
  return retval;
}

int32_t
bb_get_mem_size(volatile S_BB_T *bb) {

  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb_size(bb->max_data_desc_size,
			bb->max_data_size);
  
  return retval;
}

int32_t 
bb_shadow_get(S_BB_T *bb_shadow,
		   volatile S_BB_T *bb_src) {
  
  int32_t retcode;

  bb_lock(bb_src);

  
  assert(bb_src);
  assert(bb_shadow);
  retcode = E_OK;
  /* copie brutale du BB */
  memcpy(bb_shadow,
	 (void*)bb_src,
	 bb_get_mem_size(bb_src));
  /* On degage ce qui est inutilisable dans le shadow bb */
  bb_shadow->semid    = -1;
  
  bb_unlock(bb_src);
  
  return retcode;
}

int32_t 
bb_shadow_update_data(S_BB_T *bb_shadow,
		      volatile S_BB_T *bb_src) {  
  int32_t retcode;
  
  assert(bb_src);
  assert(bb_shadow);
  retcode = E_OK;
  bb_lock(bb_src);
  /* copie brutale du contenu la zone de donnée 
   * (utile, on ne copie pas la zone non utilisée) */
  memcpy(bb_data(bb_shadow),
	 bb_data(bb_src),
	 bb_shadow->data_free_offset);
  bb_unlock(bb_src);
  
  return retcode;
} /* end of bb_shadow_update_data */


int32_t
bb_msg_id(volatile S_BB_T *bb) {
    
  int32_t retval;
  
  assert(bb);
  retval = bb->msgid;  
  return retval;
}

int32_t 
bb_snd_msg(volatile S_BB_T *bb,
		S_BB_MSG_T* msg) {
  
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  struct msqid_ds mystat;
  
  retcode = E_OK;
  assert(bb);
  /* do not flood message queue if no one read it !!*/
  retcode  = msgctl(bb->msgid,IPC_STAT,&mystat);
  if (!(mystat.msg_cbytes > 2*MSG_BB_MAX_SIZE)) {
    /* Non blocking send */
    retcode = msgsnd(bb->msgid,msg,MSG_BB_MAX_SIZE,IPC_NOWAIT);
    if (-1 == retcode) {
      retcode = E_NOK;
      strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
      if (EAGAIN == errno) {
	/*bb_logMsg(BB_LOG_WARNING,
	  "Blackboard::bb_snd_msg",
	  "Envoi du message impossible (queue pleine) <%s>",
	  syserr);
	*/
      } else {
	bb_logMsg(BB_LOG_SEVERE,
		  "Blackboard::bb_snd_msg",
		  "Cannot send message <%s>",
		  syserr);
      } 
    }
  } else {
    /* NOP */
/*     bb_logMsg(BB_LOG_WARNING, */
/* 	      "Blackboard::bb_snd_msg", */
/* 	      "Cannot send msg <%d> bytes already queued>",mystat.msg_cbytes); */
  }
  
  return retcode;
} /* end of bb_snd_msg */


int32_t 
bb_rcv_msg(volatile S_BB_T *bb,
		S_BB_MSG_T* msg) {
    
  int32_t retcode;
  char syserr[MAX_SYSMSG_SIZE];
  int32_t  i_cont;
  
  retcode = E_OK;
  assert(bb);

  i_cont = 1;
  /* Réception bloquante */
  while (i_cont) {  
    retcode = msgrcv(bb->msgid,msg,MSG_BB_MAX_SIZE,msg->mtype,MSG_NOERROR);
    /* On sort de la boucle si on a pas pris un signal */
    if ((-1 != retcode) || (EINTR != errno)) {
      i_cont = 0;
    }
  }
  if (-1 == retcode) {
    retcode = E_NOK;
    /* The identifier removed case and INTR
     * should not generate a log message 
     * FIXME on devrait pouvoir faire mieux que ça.
     */
    if ((EIDRM != errno) && (EINTR != errno)) {
      strncpy(syserr,strerror(errno),MAX_SYSMSG_SIZE);
      bb_logMsg(BB_LOG_SEVERE,
		"Blackboard::bb_rcv_msg",
		"Cannot receive messsage <%s>",
		syserr);
    }
  } 
  
  return retcode;
} /* end of bb_rcv_msg */
