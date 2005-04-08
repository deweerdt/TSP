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
