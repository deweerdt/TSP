/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core_sysv.c,v 1.1 2006-07-22 16:52:27 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD, Frederik DEWEERDT

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

Purpose   : Blackboard SysV implementation

Note	  : Most of the code was written by Eric, and was historically
	    in bb_core.c
-----------------------------------------------------------------------
 */


#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include "bb_core_sysv.h"
#include "bb_core.h"
#include "bb_utils.h"

#define LOG(x, ...)

static int sysv_bb_msgq_get(S_BB_T * bb, int create)
{
	unsigned long flags = 0;
	char *name_msg;
	int retcode = BB_OK;


	if (create)
		flags = IPC_CREAT | IPC_EXCL;
	name_msg = bb_utils_build_msg_name(bb->name);
	bb->priv.sysv.msg_id = msgget(bb_utils_ntok(name_msg),
			      flags | BB_MSG_ACCESS_RIGHT);

	if (bb->priv.sysv.msg_id < 0) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
		    "Cannot message queue <%s key = 0x%x>",
		    name_msg, bb_utils_ntok(name_msg));
		retcode = BB_NOK;
	}
	free(name_msg);
	return retcode;
}

static int sysv_bb_lock(volatile S_BB_T * bb)
{
	int32_t retcode;
	struct sembuf s_semop;

	assert(bb);

	s_semop.sem_num = 0;
	s_semop.sem_op = -1;
	s_semop.sem_flg = SEM_UNDO;
	if (-1 == semop(bb->priv.sysv.sem_id, &s_semop, 1)) {
		if (EINVAL == errno) {
			LOG(BB_LOG_WARNING, "BlackBoard::bb_lock",
				  "Is BB semaphore destroyed?");
		} else {
			LOG(BB_LOG_SEVERE, "BlackBoard::bb_lock",
				  "semop failed");
		}
		retcode = BB_NOK;
	} else {
		retcode = BB_OK;
	}

	return retcode;
}
static int sysv_bb_unlock(volatile S_BB_T *bb)
{
	int32_t retcode;
	struct sembuf s_semop;

	/* shadow do not honor locking
	 * they have neither semaphore nor msgqueue attached
	 */
	if (BB_STATUS_SHADOW == bb->status) {
		return BB_OK;
	}

	s_semop.sem_num = 0;
	s_semop.sem_op = 1;
	s_semop.sem_flg = SEM_UNDO;
	if (-1 == semop(bb->priv.sysv.sem_id, &s_semop, 1)) {
		if (EINVAL == errno) {
			bb_logMsg(BB_LOG_WARNING, "BlackBoard::bb_unlock",
				  "Is BB semaphore destroyed?");
		}
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_unlock",
			  "semop failed");
		retcode = BB_NOK;
	} else {
		retcode = BB_OK;
	}

	return retcode;
}
static int sysv_bb_sem_get(S_BB_T * bb, int create)
{
	char *name_sem;
	unsigned long flags = 0;
	union semun u_sem_ctrl;
	struct sembuf s_semop;

	if (create)
		flags = IPC_CREAT | IPC_EXCL;

	name_sem = bb_utils_build_sem_name(bb->name);
	bb->priv.sysv.sem_id = semget(bb_utils_ntok(name_sem), 1,
			      flags | BB_SEM_ACCESS_RIGHT);

	if ((bb->priv.sysv.sem_id) < 0) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
		    "Cannot create semaphore <%s key = 0x%x>",
		    name_sem, bb_utils_ntok(name_sem));
		goto err;
	}

	u_sem_ctrl.val = 2;
	if (semctl(bb->priv.sysv.sem_id, 0, SETVAL, u_sem_ctrl) < 0) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
		    "Cannot initialise semaphore <%s key = 0x%x>",
		    name_sem, bb_utils_ntok(name_sem));
		goto err;
	}
	/* call semop at least once,
	 * so that we avoid possible race conditions between the
	 * creator and the clients (cf UNPV2 pages 284--285)
	 */
	s_semop.sem_num = 0;
	s_semop.sem_op = -1;
	s_semop.sem_flg = 0;
	semop(bb->priv.sysv.sem_id, &s_semop, 1);

	free(name_sem);
	return BB_OK;
      err:
	free(name_sem);
	return BB_NOK;
}

static const char *sysv_strip_name(const char *name)
{
	char *p;
	p = strchr(name, ':');
	if (!p)
		return name;
	return p + 1;
}

static int sysv_bb_shmem_detach(S_BB_T ** bb)
{
	int32_t retcode;

	retcode = BB_OK;
	assert(bb);
	assert(*bb);

	if (-1 == shmdt((void *) *bb)) {
		LOG(bb_log_severe, "blackboard::bb_detach",
			  "shm detach failed");
		retcode = BB_NOK;
	}

	*bb = NULL;
	return retcode;
}
static int sysv_bb_shmem_attach(S_BB_T ** bb, const char *name)
{
	int fd_shm, retcode;
	char *name_shm;

	retcode = BB_OK;
	fd_shm  = 0;

	name_shm = bb_utils_build_shm_name(name);
	if (name_shm != NULL) {
		/* On recupere l'acces au SHM */
		fd_shm = shmget(bb_utils_ntok(name_shm), 0, BB_SHM_ACCESS_RIGHT);
	} else {
		retcode = BB_NOK;
	}
	if ((BB_OK == retcode) && (-1 == fd_shm)) {
		/* attaching to a non-existing segment is NOT an error
		 * its an info that should be handled by the caller.
		 */
		if (ENOENT != errno) {
			LOG(BB_LOG_SEVERE, "BlackBoard::bb_attach",
				  "Cannot open shm segment <%s key = 0x%x>",
				  name_shm, bb_utils_ntok(name_shm));
		} else {
			LOG(BB_LOG_INFO, "BlackBoard::bb_attach",
				  "Cannot open shm segment <%s key = 0x%x>",
				  name_shm, bb_utils_ntok(name_shm));

		}
		retcode = BB_NOK;
	}
	free(name_shm);

	if (BB_OK == retcode) {
		*bb = (S_BB_T *) shmat(fd_shm, NULL, 0);
	}
	if ((BB_OK == retcode) && ((void *) -1 == *bb)) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_attach",
			  "Cannot attach shm segment (%s)");
		retcode = BB_NOK;
	}

	return retcode;

}
static int sysv_bb_shmem_get(S_BB_T ** bb, const char *name, int n_data,
			     int data_size, int create)
{
	unsigned long flags = 0;
	char *shm_name;
	int shm_size, fd;

	shm_name = bb_utils_build_shm_name(name);
	if (!shm_name)
		goto err;
	shm_size = bb_size(n_data, data_size);
	if (create)
		flags = IPC_CREAT | IPC_EXCL;
	fd =
	    shmget(bb_utils_ntok(shm_name), shm_size,
		   flags | BB_SHM_ACCESS_RIGHT);

	if (fd < 0) {
		goto free_shm_name;
	}

	*bb = (S_BB_T *) shmat(fd, NULL, 0);
	if (!(*bb)) {
		goto free_shm_name;
	}

	if (create) {
		/* Init the freshly allocated structure */
		memset(*bb, 0, shm_size);
		(*bb)->bb_version_id = BB_VERSION_ID;
		strncpy((*bb)->name, name,
			BB_NAME_MAX_SIZE + 1);
		(*bb)->max_data_desc_size = n_data;
		(*bb)->data_desc_offset = sizeof(S_BB_T);

		(*bb)->n_data = 0;
		(*bb)->max_data_size = data_size;
		(*bb)->data_offset = (*bb)->data_desc_offset +
		    ((*bb)->max_data_desc_size) * sizeof(S_BB_DATADESC_T);
		(*bb)->data_free_offset = 0;
		(*bb)->status = BB_STATUS_GENUINE;
		(*bb)->priv.sysv.shm_size = shm_size;
		(*bb)->priv.sysv.fd = fd;
	}
	(*bb)->type = BB_SYSV;

	free(shm_name);
	return BB_OK;

free_shm_name:
	free(shm_name);
err:
	*bb = NULL;
	return BB_NOK;
}

static int32_t sysv_bb_shmem_destroy(S_BB_T ** bb)
{
	int32_t retcode;
	char *name_shm;
	int32_t fd_shm;
	/*S_BB_MSG_T bb_msg;*/

	retcode = BB_OK;
	name_shm = bb_utils_build_shm_name((*bb)->name);
	/* 
	 * On signale la destruction en cours pour les processes qui
	 * resteraient attachés
	 */

	/* FIXME should reserve a message type for signaling destroy */
	/*   bb_msg.mtype = 1; */
	/*   bb_snd_msg(*bb,&bb_msg); */
	/*   sleep(1); */

	/* On programme la destruction du segment SHM */
	fd_shm = shmget(bb_utils_ntok(name_shm), 0, BB_SHM_ACCESS_RIGHT);
	if (-1 == shmctl(fd_shm, IPC_RMID, NULL)) {
		bb_logMsg(BB_LOG_SEVERE, "BlackBoard::bb_destroy",
			  "SHM Destroy failed");
		retcode = BB_NOK;
	}
	free(name_shm);

	/* on se detache de façon a provoquer la destruction */
	if (BB_OK == retcode) {
		if (-1 == shmdt((void *) *bb)) {
			bb_logMsg(BB_LOG_SEVERE, "BlackBoard::bb_destroy",
				  "SHM detach failed");
			retcode = BB_NOK;
		}
	}
	*bb = NULL;

	return retcode;
} /* end of sysv_bb_shmem_destroy */

static int32_t sysv_bb_msgq_recv(volatile S_BB_T * bb, S_BB_MSG_T * msg)
{
	int32_t retcode;
	int32_t i_cont;

	retcode = BB_OK;
	assert(bb);

	i_cont = 1;
	/* Réception bloquante */
	while (i_cont) {
		retcode =
		    msgrcv(bb->priv.sysv.msg_id, msg, MSG_BB_MAX_SIZE, msg->mtype,
			   MSG_NOERROR);
		/* On sort de la boucle si on a pas pris un signal */
		if ((-1 != retcode) || (EINTR != errno)) {
			i_cont = 0;
		}
	}
	if (-1 == retcode) {
		retcode = BB_NOK;
		/* The identifier removed case and INTR
		 * should not generate a log message 
		 * FIXME on devrait pouvoir faire mieux que ça.
		 */
		if ((EIDRM != errno) && (EINTR != errno)) {
			bb_logMsg(BB_LOG_SEVERE,
				  "Blackboard::bb_rcv_msg",
				  "Cannot receive messsage");
		}
	}
	return retcode;
}

static int32_t sysv_bb_msgq_send(volatile S_BB_T * bb, S_BB_MSG_T * msg)
{

	int32_t retcode;
	struct msqid_ds mystat;

	retcode = BB_OK;
	assert(bb);
	/* do not flood message queue if no one read it !! */
	retcode = msgctl(bb->priv.sysv.msg_id, IPC_STAT, &mystat);
	if (!(mystat.msg_cbytes > 2 * MSG_BB_MAX_SIZE)) {
		/* Non blocking send */
		retcode =
		    msgsnd(bb->priv.sysv.msg_id, msg, MSG_BB_MAX_SIZE, IPC_NOWAIT);
		if (-1 == retcode) {
			retcode = BB_NOK;
			if (EAGAIN == errno) {
				/*bb_logMsg(BB_LOG_WARNING,
				   "Blackboard::bb_snd_msg",
				   "Envoi du message impossible (queue pleine) <%s>",
				   syserr);
				 */
			} else {
				LOG(BB_LOG_SEVERE,
				    "Blackboard::bb_snd_msg",
				    "Cannot send message");
			}
		}
	} else {
		/* NOP */
/*     LOG(BB_LOG_WARNING, */
/* 	      "Blackboard::bb_snd_msg", */
/* 	      "Cannot send msg <%d> bytes already queued>",mystat.msg_cbytes); */
	}
}

static int32_t sysv_bb_msgq_isalive(S_BB_T * bb)
{
	int32_t retcode;
	struct msqid_ds s_msgq_stat;

	msgctl(bb->priv.sysv.msg_id, IPC_STAT, &s_msgq_stat);

	if (s_msgq_stat.msg_qnum > 0) {
		retcode = BB_NOK;

	} else {
		retcode = BB_OK;
	}

	return retcode;
}

static int sysv_bb_sem_destroy(S_BB_T *bb)
{
	/* FIXME should the sem be taken before being removed? */
	/* Remove the bb's semaphore */
	return semctl(bb->priv.sysv.sem_id, 0, IPC_RMID);
}

static int sysv_bb_msgq_destroy(S_BB_T *bb)
{
	/* Destroy the bb's message queue */
	return msgctl(bb->priv.sysv.msg_id, IPC_RMID, NULL);
}

struct bb_operations sysv_bb_ops = {
	.bb_shmem_get = sysv_bb_shmem_get,
	.bb_shmem_attach = sysv_bb_shmem_attach,
	.bb_shmem_detach = sysv_bb_shmem_detach,
	.bb_shmem_destroy = sysv_bb_shmem_destroy,

	.bb_sem_get = sysv_bb_sem_get,
	.bb_lock = sysv_bb_lock,
	.bb_unlock = sysv_bb_unlock,
	.bb_sem_destroy = sysv_bb_sem_destroy,

	.bb_msgq_get = sysv_bb_msgq_get,
	.bb_msgq_send = sysv_bb_msgq_send,
	.bb_msgq_recv = sysv_bb_msgq_recv,
	.bb_msgq_isalive = sysv_bb_msgq_isalive,
	.bb_msgq_destroy = sysv_bb_msgq_destroy,
};
