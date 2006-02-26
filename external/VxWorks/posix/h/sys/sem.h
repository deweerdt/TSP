/*

$Header $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2005 Cesare BERTONA

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
Maintainer: tsp@astrium.eads.net
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
*/
#ifndef _SYS_SEM_H
#define _SYS_SEM_H

#include <stdio.h>
#include <sys/ipc.h>

#define SEM_A   0200    /* alter permission */
#define SEM_R   0400    /* read permission */

/*
 * Semaphore Operation Flags.
 */

#define SEM_UNDO        010000  /* set up adjust on exit entry */

/*
 * Semctl Command Definitions.
 */

#define GETNCNT 3       /* get semncnt */
#define GETPID  4       /* get sempid */
#define GETVAL  5       /* get semval */
#define GETALL  6       /* get all semval's */
#define GETZCNT 7       /* get semzcnt */
#define SETVAL  8       /* set semval */
#define SETALL  9       /* set all semval's */

struct sembuf {
        ushort_t        sem_num;        /* semaphore # */
        short           sem_op;         /* semaphore operation */
        short           sem_flg;        /* operation flags */
};

union semun {
  int val;                           /* value for SETVAL */
  struct semid_ds *buf;              /* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;         /* array for GETALL & SETALL */
  struct seminfo *__buf;             /* buffer for IPC_INFO */
};


int semget(key_t key, int nsems, int semflg);
int semctl(int semid, int semnum, int cmd, ...);
int semop(int semid, struct sembuf *sops, size_t nsops);


#endif /* _SYS_SEM_H */
