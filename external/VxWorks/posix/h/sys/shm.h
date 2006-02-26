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
#ifndef _SYS_SHM_H
#define _SYS_SHM_H

#include <stdio.h>
#include <sys/ipc.h>


/* the ID value for POSIX SHM wrapper is fixed at 0 */
/* all project specific shm ids must be greater     */
#define SYS_SHM_ID		0

extern int   shm_putAddr(int id, int sid, void *lptr);
extern void *shm_getAddr(int id, int sid);


struct shmid_ds {
  /* void / used only for shmctl function */
};


int shmget(key_t key, size_t size, int shmflg);
void *shmat(int shmid, const void *shmaddr, int shmflg);
int shmctl(int shmid, int cmd, struct shmid_ds *buf);
int shmdt(const void *shmaddr);


#endif /* _SYS_SHM_H */
