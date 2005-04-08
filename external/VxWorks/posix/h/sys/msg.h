#ifndef _SYS_MSG_H
#define _SYS_MSG_H

#include <stdio.h>
#include <sys/ipc.h>

/*
 * Message Operation Flags.
 */

#define MSG_NOERROR     010000  /* no error if big message */

typedef unsigned long msgqnum_t;
typedef unsigned long msglen_t;

struct msqid_ds {
  msgqnum_t	msg_qnum;
  msglen_t	msg_cbytes;
};


int msgget(key_t key, int msgflg);
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
int msgsnd(int msqid, const void *msgp,  size_t  msgsz,  int msgflg);
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long int msgtyp, int msgflg);

#endif /* _SYS_MSG_H */
