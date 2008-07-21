/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core_posix.c,v 1.3 2008-07-21 08:45:17 jaggy Exp $

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

Purpose   : Blackboard POSIX implementation

Note	  : Most of the code was written by Eric, and was historically
	    in bb_core.c
-----------------------------------------------------------------------
 */


#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <mqueue.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>
#if !defined(__rtems__)
#  include <sys/mman.h>
#endif

#include "bb_core_posix.h"
#include "bb_core.h"
#include "bb_utils.h"


#define LOG(x, ...)


#if defined(__rtems__)
/* FIXME to implement */
static int shm_open(const char *name, int oflag, mode_t mode)
{
	return 0;
}
#endif

struct bb_used_t bb_used[MAX_NUM_OF_BB];

int bb_count = 0;

/* posix rtems ok */
static int posix_bb_msgq_get(S_BB_T *bb, int create)
{
	char *name_msg;
	int retcode = BB_OK;
	int oflag = 0 ;

	if (create)
		oflag = O_CREAT | O_EXCL;
		name_msg = bb_utils_build_msg_name(bb->name);
		bb->priv.posix.msg_id = (mqd_t) mq_open( bb_utils_build_msg_name(bb->name),
											oflag | BB_MSG_ACCESS_RIGHT );

	if (bb->priv.posix.msg_id < 0) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_create", "Cannot message queue %s ", name_msg);
		retcode = BB_NOK;
	}
	//free(name_msg);
	return retcode;
}

/* posix rtems ok */
static int posix_bb_lock(volatile S_BB_T *bb)
{
	int32_t retcode;

	assert(bb);

	if (-1 == sem_wait(bb->priv.posix.sem_id)) {
		if (EINVAL == errno) {
			LOG(BB_LOG_WARNING, "BlackBoard::bb_lock",
				  "Is BB semaphore destroyed?");
		} else {
			LOG(BB_LOG_SEVERE, "BlackBoard::bb_lock",
				  "sem_open failed");
		}
		retcode = BB_NOK;
	} else {
		retcode = BB_OK;
	}

	return retcode;
}

/* posix rtems ok */
static int posix_bb_unlock(volatile S_BB_T *bb)
{
	int32_t retcode;


	/* shadow do not honor locking
	 * they have neither semaphore nor msgqueue attached
	 */
	if (BB_STATUS_SHADOW == bb->status) {
		return BB_OK;
	}

	if (-1 == sem_post(bb->priv.posix.sem_id)) {
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

/*
 *	posix_bb_sem_get : posix rtems ok
 */
static int posix_bb_sem_get(S_BB_T *bb, int create)
{
	int32_t retcode = BB_OK;
	//char* name_sem = NULL;
	int oflag = 0;

	if (create)
		oflag = O_CREAT | O_EXCL;

	//name_sem = bb_utils_build_sem_name(bb->name);

	bb->priv.posix.sem_id = (sem_t *)sem_open(bb_utils_build_sem_name(bb->name),
											oflag | BB_SEM_ACCESS_RIGHT);

	if ( bb->priv.posix.sem_id < 0) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
		    "Cannot create semaphore < %s >", name_sem);
		retcode = BB_NOK;
	} else {
		retcode = BB_OK;
	}
	return retcode;
}

/*
 * Detache le segment de memoire partagee
 * posix rtems ok
 */
static int posix_bb_shmem_detach(S_BB_T ** bb)
{
	int32_t retcode = BB_OK;
	assert(bb);
	assert(*bb);

#if defined(__rtems__)
	retcode = shm_unlink((char *)*bb);
#else
	free((char *)*bb);
#endif
	*bb = NULL;
	if (!retcode)
		return BB_OK;
	else
		LOG(bb_log_severe, "blackboard::bb_detach","shmem detach failed");
		return BB_NOK;
}

/*
 * posix_bb_shmem_attach : posix Rtems ok
 */
static int posix_bb_shmem_attach(S_BB_T ** bb, const char *name)
{
	int i_bb, fd_shm, retcode;
	int success = FALSE;

	retcode = BB_OK;
	fd_shm  = 0;

	for (i_bb = 0; i_bb < bb_count; i_bb++){
		if ( strncmp((char *)bb_used[i_bb].name, name, BB_NAME_MAX_SIZE + 1) == 0){
			*bb = bb_used[i_bb].fd;
			success = TRUE;
			retcode = BB_OK;
		}
	}

	if (!success){
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_attach", "Cannot find the BB named :%s ", name);
		retcode = BB_NOK;
	}

	if ((retcode == BB_OK) && ((void *) -1 == *bb)) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_attach", "Cannot attach shm segment (%s)");
		retcode = BB_NOK;
	}
	return retcode;
}

/*
 *	posix_bb_shmem_get : posix rtems ok
 */
static int posix_bb_shmem_get(S_BB_T ** bb,
			      const char *name,
			      int n_data,
			      int data_size,
			      int create)
{
	int i_bb, shm_size;
	int success = FALSE;

	shm_size = bb_size(n_data, data_size);

	if (create) {
		if (bb_count == MAX_NUM_OF_BB-1) {
			LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
			  "Max number of BB reached !");
			return BB_NOK;
		}

		*bb = (S_BB_T *)malloc(shm_size);

		if (*bb == NULL) {
			LOG(BB_LOG_SEVERE, "BlackBoard::bb_create",
			  "Cannot allow memory ");
			return BB_NOK;
		}

		/* Init the freshly allocated structure */
		memset(*bb, 0, shm_size);
		(*bb)->bb_version_id = BB_VERSION_ID;
		strncpy((char *)(*bb)->name, name, BB_NAME_MAX_SIZE + 1);
		(*bb)->max_data_desc_size = n_data;
		(*bb)->data_desc_offset = sizeof(S_BB_T);
		(*bb)->n_data = 0;
		(*bb)->max_data_size = data_size;
		(*bb)->data_offset = (*bb)->data_desc_offset + ((*bb)->max_data_desc_size) * sizeof(S_BB_DATADESC_T);
		(*bb)->data_free_offset = 0;
		(*bb)->status = BB_STATUS_GENUINE;
		(*bb)->priv.posix.shm_size = shm_size;
		(*bb)->priv.posix.fd = (int)*bb;
		(*bb)->type = BB_POSIX;

		/* created BB memorisation */
		strncpy((char *)bb_used[bb_count].name, name, BB_NAME_MAX_SIZE + 1);
		bb_used[bb_count].fd   = (S_BB_T *)*bb;
		bb_count++;

		return BB_OK;
	}

	/* else look for the corresponding fd */
	for (i_bb = 0; i_bb < bb_count; i_bb++) {
		if (!strncmp((char *)bb_used[i_bb].name, name, BB_NAME_MAX_SIZE + 1)) {
			*bb = bb_used[i_bb].fd;
			success = TRUE;
		}
	}

	if (!success) {
		LOG(BB_LOG_SEVERE, "BlackBoard::bb_get", "Cannot find the BB named :%s ", name);
		return BB_NOK;
	}

	return BB_OK;
}

/*
 *	posix_bb_shmem_destroy
 */
static int32_t posix_bb_shmem_destroy(S_BB_T ** bb)
{
	int32_t retcode = BB_OK;
	int i_bb, j_bb;

	/*S_BB_MSG_T bb_msg;*/

	/*
	 * On signale la destruction en cours pour les processes qui
	 * resteraient attachés
	 */

	/* FIXME should reserve a message type for signaling destroy */
	/*   bb_msg.mtype = 1; */
	/*   bb_snd_msg(*bb,&bb_msg); */
	/*   sleep(1); */

	/* Look for the corresponding BB */
	for (i_bb = 0; i_bb < bb_count; i_bb++){
		if ( (S_BB_T *)bb_used[i_bb].fd == ((S_BB_T *)(*bb)->priv.posix.fd) ) {
			if ( i_bb == (MAX_NUM_OF_BB - 1)){
				/* cas en fin de tab*/
				bb_used[i_bb].name[0] = '\0';
				bb_used[i_bb].fd = NULL;
				bb_count--;
			} else { // decallage
				for (j_bb = i_bb; j_bb < bb_count-1; j_bb++){
					strncpy((char *)bb_used[j_bb].name, (char *)bb_used[j_bb+1].name, BB_NAME_MAX_SIZE + 1);
					bb_used[j_bb].fd = bb_used[j_bb+1].fd;
				}
				bb_count--;
			}
		}
	}
	free(*bb);
	*bb = NULL;
	return retcode;
} /* end of posix_bb_shmem_destroy */

/*
 * posix_bb_msgq_recv : posix rtems ok
 */
static int32_t posix_bb_msgq_recv(volatile S_BB_T * bb, S_BB_MSG_T * msg)
{
	int32_t retcode;
	int32_t i_cont;

	retcode = BB_OK;
	assert(bb);

	i_cont = 1;
	/* Réception bloquante */
	while (i_cont) {

		retcode = mq_receive(bb->priv.posix.msg_id, (char *)msg->mtext, MAX_SYSMSG_SIZE, NULL );

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

/*
 * posix_bb_msgq_send : posix rtems ok
 */
static int32_t posix_bb_msgq_send(volatile S_BB_T * bb, S_BB_MSG_T * msg)
{
	int32_t retcode;
	struct mq_attr mqstat;

	retcode = BB_OK;
	assert(bb);
	/* do not flood message queue if no one read it !! */
	retcode =  mq_getattr(bb->priv.posix.msg_id, &mqstat);

	if ( !(mqstat.mq_curmsgs * mqstat.mq_msgsize > 2 * MAX_SYSMSG_SIZE) ) {
		/* Non blocking send */
		retcode = mq_send( bb->priv.posix.msg_id, (char *)msg->mtext, MAX_SYSMSG_SIZE, 0);
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
	}
	return retcode;
}

/*
 * posix_bb_msgq_isalive : posix rtems ok
 */
static int32_t posix_bb_msgq_isalive(S_BB_T * bb)
{
	int32_t retcode;
	struct mq_attr mqstat;

	retcode =  mq_getattr( bb->priv.posix.msg_id , &mqstat );

	if ( !retcode && mqstat.mq_curmsgs > 0 ) {
		retcode = BB_NOK;
	} else {
		retcode = BB_OK;
	}

	return retcode;
}

/*
 * posix_bb_sem_destroy : posix rtems ok
 */
static int posix_bb_sem_destroy(S_BB_T *bb)
{
	/* FIXME should the sem be taken before being removed? */
	/* Remove the bb's semaphore */
	return sem_destroy( bb->priv.posix.sem_id );
}

/*
 * posix_bb_msgq_destroy : posix rtems ok
 */
static int posix_bb_msgq_destroy(S_BB_T *bb)
{
	char *name_msg;
	name_msg = bb_utils_build_msg_name(bb->name);

	/* Destroy the bb's message queue */
	return mq_unlink(name_msg);
}


/*
 * Structure bb_operations : pointeurs de fonctions
 */
struct bb_operations posix_bb_ops = {
	.bb_shmem_get = posix_bb_shmem_get,
	.bb_shmem_attach = posix_bb_shmem_attach,
	.bb_shmem_detach = posix_bb_shmem_detach,
	.bb_shmem_destroy = posix_bb_shmem_destroy,

	.bb_sem_get = posix_bb_sem_get,
	.bb_lock = posix_bb_lock,
	.bb_unlock = posix_bb_unlock,
	.bb_sem_destroy = posix_bb_sem_destroy,

	.bb_msgq_get = posix_bb_msgq_get,
	.bb_msgq_send = posix_bb_msgq_send,
	.bb_msgq_recv = posix_bb_msgq_recv,
	.bb_msgq_isalive = posix_bb_msgq_isalive,
	.bb_msgq_destroy = posix_bb_msgq_destroy,
};

