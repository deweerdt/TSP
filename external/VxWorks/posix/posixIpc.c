/*!  \file 

$Header: /home/def/zae/tsp/tsp/external/VxWorks/posix/posixIpc.c,v 1.1 2005-04-08 14:45:20 le_tche Exp $

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
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
 */

#include <stdio.h>
#include <semLib.h>
#include <errnoLib.h>
#include <taskLib.h>
#include <intLib.h>
#include <msgQLib.h>
#include <stdarg.h>

#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#include "../../../src/core/include/tsp_abs_types.h"
#include "../../../src/util/libbb/bb_core.h"

/* declare vme_malloc function : must be provided in BSP or user application */
extern void * vme_malloc(unsigned long size);


char * shmget_ptr_global = NULL;

int shmget(key_t key, size_t size, int shmflg)
{

  if(shmget_ptr_global == NULL) {

    if((shmget_ptr_global = (char *) vme_malloc(size)) == NULL) {
      errnoSet(ENOMEM);
      return -1;
    }
  }

  printf("shmget : ptr = 0x%x, size = 0x%x\n", shmget_ptr_global, size);

  return (int ) shmget_ptr_global;
}

void *shmat(int shmid, const void *shmaddr, int shmflg)
{
printf("shmat : ptr = 0x%x\n", shmid);
  return (char *) shmid;
}

int shmctl(int shmid, int cmd, struct shmid_ds *buf)
{
  /* not implemented on tornado !! */
  return 0;
}

int shmdt(const void *shmaddr)
{
  /* $TBD$ */
  return 0;
}

/*-------------------------------------------------------*/

int semget(key_t key, int nsems, int semflg)
{
  SEM_ID id;

  /* FIXME : alow multiple semaphore to be created */
  if(nsems != 1) {
    errnoSet(EINVAL);
    return -1;
  }

  /* FIXME : implement all semflg */
  if(semflg & IPC_CREAT) {
    id = semCCreate ( SEM_Q_FIFO, 0);

    if(id == NULL) {
      errnoSet(ENOSPC);
      return -1;
    }
printf("semget : SEM_ID=0x%x\n", id);    
    return (int ) id;
  }
  else {
    errnoSet(ENOENT);
    return -1;
  }
}

int semctl(int semid, int semnum, int cmd, ...)
{
  int	lockKey;
  int	num;
  union semun arg;
  va_list	ap;



  switch (cmd) {
  case SETVAL:
    if (taskLock() == OK) {
      va_start(ap, cmd);
      arg = va_arg(ap, union semun);
      lockKey = intLock ();
      
      for(num=0; num<arg.val; num++) semGive((SEM_ID ) semid);

      intUnlock (lockKey);
      taskUnlock();
      va_end(ap);
printf("semctl :  SEM_ID=0x%x, val=%d\n", (SEM_ID ) semid, arg.val);    
    }
    else {
      errnoSet(EACCES);
      return -1;
    }
    break;

  case IPC_RMID:
    if(semDelete((SEM_ID ) semid) != OK) {
      errnoSet(EINVAL);
      return -1;
    }
    break;

  default:
      errnoSet(EACCES);
      return -1;
    break;
  }

  return 0;
}

int semop(int semid, struct sembuf *sops, size_t nsops)
{
  /* not implemented */
  return 0;
}

/*-------------------------------------------------------*/

int msgget(key_t key, int msgflg)
{
  MSG_Q_ID	msqid;

  msqid = msgQCreate ( 3, MSG_BB_MAX_SIZE, MSG_Q_FIFO);

  if(msqid == NULL) {
    errnoSet(ENOSPC);
    return -1;
  }

  return (int ) msqid;
}

int msgctl(int msqid, int cmd, struct msqid_ds *buf)
{
  int	nb;

  switch (cmd) {
  case IPC_STAT:
    if((nb = msgQNumMsgs ((MSG_Q_ID ) msqid)) == -1) {
      errnoSet(EINVAL);
      return -1;
    }
    
    return (nb*MSG_BB_MAX_SIZE);
    break;

  case IPC_RMID:
    if(msgQDelete((MSG_Q_ID ) msqid) != OK) {
      errnoSet(EINVAL);
      return -1;
    }
    break;

  default:
      errnoSet(EACCES);
      return -1;
    break;
  }

  return 0;
}

int msgsnd(int msqid, const void *msgp,  size_t  msgsz,  int msgflg)
{
  int      timeout;

  if(msgflg & IPC_NOWAIT) {
    timeout = NO_WAIT ;
  }
  else {
    timeout = WAIT_FOREVER;
  }

  if(msgQSend ((MSG_Q_ID ) msqid, (char *) msgp, msgsz, timeout, MSG_PRI_NORMAL) != 0) {
      errnoSet(EAGAIN);
      return -1;
  }

  return 0;
}

ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg)
{

  if(msgQReceive((MSG_Q_ID ) msqid, (char *) msgp, msgsz, WAIT_FOREVER) != 0) {
      errnoSet(EFAULT);
      return -1;
  }

  return 0;
}
