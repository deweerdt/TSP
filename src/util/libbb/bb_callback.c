/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_callback.c,v 1.1 2008-07-18 15:09:53 jaggy Exp $

-----------------------------------------------------------------------

BlackBoard Library - Implementation of the subscribe/unsubscribe
callback on messages receiving

Copyright (c) 2008 Jerome ARBEZ-GINDRE

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
Component : Blackboard

-----------------------------------------------------------------------

Purpose   : BlackBoard Message Callback implementation

-----------------------------------------------------------------------
 */

#include "bb_callback.h"
#include "bb_core.h"
#include "bb_local.h"

#define MOD_NAME "BB_CALLBACK"

#ifdef __KERNEL__
#else /* __KERNEL__ */
#include <pthread.h>

struct S_BB_SUBSCRIBE {
	struct S_BB_MSG msg;
	void (*callback)(struct S_BB *bb, struct S_BB_MSG *msg);
	struct S_BB *bb;
	pthread_t thread;
	struct S_BB_SUBSCRIBE *next;
};
#endif /* __KERNEL__ */

#ifdef __KERNEL__

int32_t bb_msg_subscribe(struct S_BB *bb, const struct S_BB_MSG *msg,
			 void (*callback)(struct S_BB *bb, struct S_BB_MSG *msg))
{
	return BB_NOK;
}
EXPORT_SYMBOL_GPL(bb_msg_subscribe);

int32_t bb_msg_unsubscribe(struct S_BB *bb, const struct S_BB_MSG *msg)
{
	return BB_NOK;

}
EXPORT_SYMBOL_GPL(bb_msg_unsubscribe);

#else /* __KERNEL__ */
static void *thread_routine(void * arg);

int32_t bb_msg_subscribe(struct S_BB *bb, const struct S_BB_MSG *msg,
			 void (*callback)(struct S_BB *bb, struct S_BB_MSG *msg))
{
	struct S_BB_SUBSCRIBE **last_subscribe;
	struct S_BB_LOCAL *local = bb_get_local(bb);

	if (!local)
		return BB_NOK;

	/* find the end of the list (or already subscribed msg)*/
	for (last_subscribe = &local->subscribed;
	     (*last_subscribe) !=  NULL &&
		     (*last_subscribe)->msg.mtype != msg->mtype;
	     last_subscribe = &(*last_subscribe)->next);

	/* mtype already subscribed*/
	if (*last_subscribe !=  NULL) {
		return BB_NOK;
	}

	*last_subscribe = malloc (sizeof(**last_subscribe));
	if (*last_subscribe == NULL)
		return BB_NOK;

	(*last_subscribe)->next = NULL;
	(*last_subscribe)->msg.mtype = msg->mtype;
	(*last_subscribe)->callback = callback;
	(*last_subscribe)->bb = bb;

	if (pthread_create(&(*last_subscribe)->thread, NULL,
			   thread_routine, (*last_subscribe)) != 0) {
		free (*last_subscribe);
		*last_subscribe = NULL;
		return BB_NOK;
	}

	return BB_OK;
}

static void * thread_routine(void * arg)
{
	struct S_BB_SUBSCRIBE * subscribe = (struct S_BB_SUBSCRIBE *) arg;
	int retcode;

	retcode = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	if (retcode != 0) {
		bb_logMsg(BB_LOG_WARNING, MOD_NAME,
			  "pthread_setcanceltype failed (retcode %d)",
			  retcode);
		return NULL;
	} ;

	while (1) {
		if (bb_rcv_msg(subscribe->bb,
			       &subscribe->msg) != BB_OK) {
			bb_logMsg(BB_LOG_WARNING, MOD_NAME,
				  "bb_rcv_msg failed -> thread stoped\n",
				  retcode);
			return NULL;
		}
		subscribe->callback(subscribe->bb, &subscribe->msg);
	}
}

int32_t bb_msg_unsubscribe(struct S_BB *bb, const struct S_BB_MSG *msg)
{
	struct S_BB_SUBSCRIBE **previous_subscribe;
	struct S_BB_SUBSCRIBE *elem;
	struct S_BB_LOCAL *local = bb_get_local(bb);

	if (!local)
		return BB_NOK;

	/* find the end of the list (or subscribed msg)*/
	for (previous_subscribe = &local->subscribed;
	     (*previous_subscribe) !=  NULL &&
		     (*previous_subscribe)->msg.mtype != msg->mtype;
	     previous_subscribe = &(*previous_subscribe)->next);

	/* not found ? */
	if ( (*previous_subscribe) ==  NULL ||
	     (*previous_subscribe)->next == NULL)
		return BB_NOK;


	pthread_cancel((*previous_subscribe)->thread);
	pthread_join((*previous_subscribe)->thread, NULL);

	elem = *previous_subscribe;
	(*previous_subscribe) = (*previous_subscribe)->next;
	free(elem);

	return BB_OK;
}

int32_t bb_msg_unsubscribe_all(struct S_BB *bb)
{
	struct S_BB_SUBSCRIBE *subscribe;
	struct S_BB_SUBSCRIBE *next;
	struct S_BB_LOCAL *local = bb_get_local(bb);

	if (!local)
		return BB_NOK;

	/* find the end of the list (or subscribed msg)*/
	for (subscribe = local->subscribed;
	     subscribe != NULL;
	     subscribe = next) {
		/* we are calculating the next now... because
		   subscribe will not exist any more after the call to
		   bb_msg_unsubscribe */
		next = subscribe->next;
		bb_msg_unsubscribe (bb, &subscribe->msg);
	}

	return BB_OK;
}

#endif /* __KERNEL__ */


