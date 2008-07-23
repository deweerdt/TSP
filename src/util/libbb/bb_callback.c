/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_callback.c,v 1.6 2008-07-23 15:18:06 jaggy Exp $

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
#include "bb_core_k.h"
#include <linux/connector.h>
#else /* __KERNEL__ */
#include <pthread.h>
#endif

struct S_BB_SUBSCRIBE {
	struct S_BB_MSG msg;
	void (*callback)(struct S_BB *bb, void *context, struct S_BB_MSG *msg);
	void *context;
	struct S_BB *bb;
	struct S_BB_SUBSCRIBE *next;

#ifndef __KERNEL__
	pthread_t thread;
#endif /* __KERNEL__ */
};


#ifdef __KERNEL__
static void bb_cn_callback (void *);
#else /* __KERNEL__ */
static void *thread_routine(void * arg);
#endif /* __KERNEL__ */

int32_t bb_msg_subscribe(struct S_BB *bb, void *context,
			 const struct S_BB_MSG *msg,
			 void (*callback)(struct S_BB *bb, void *context,
					  struct S_BB_MSG *msg))
{
	struct S_BB_SUBSCRIBE **last_subscribe;
	struct S_BB_LOCAL *local = bb_get_local(bb);

	/* mtype si stored on the NBIT_FOR_MTYPE LSB bits of cb_id.val */
	if (msg->mtype > MTYPE_MASK)
		goto err;

	if (!local)
		goto err;

	/* find the end of the list (or already subscribed msg)*/
	for (last_subscribe = &local->subscribed;
	     (*last_subscribe) !=  NULL &&
		     (*last_subscribe)->msg.mtype != msg->mtype;
	     last_subscribe = &(*last_subscribe)->next);

	/* mtype already subscribed*/
	if (*last_subscribe !=  NULL)
		goto err;

	*last_subscribe = malloc (sizeof(**last_subscribe));
	if (*last_subscribe == NULL)
		goto err;

	(*last_subscribe)->next = NULL;
	(*last_subscribe)->msg.mtype = msg->mtype;
	(*last_subscribe)->callback = callback;
	(*last_subscribe)->context = context;
	(*last_subscribe)->bb = bb;

#ifdef __KERNEL__
	{
		int ret;
		struct cb_id cb_id = {
			.idx = CN_IDX_BB,
			.val = MTYPE_INDEX_2_VAL((*last_subscribe)->msg.mtype,
						 bb->priv.k.index)
		};
		char cn_name[128];
		snprintf(cn_name, sizeof(cn_name), "%s_%d",
			 bb->name, cb_id.idx);
		cn_name[sizeof(cn_name)-1] = '\0';
		ret = cn_add_callback(&cb_id, cn_name, bb_cn_callback);
		if ( ret != 0) {
			printk("cn_add_callback failed ret %d\n",ret);
			goto err_free_mem;
		}
	}
#else /* __KERNEL__ */
	if (pthread_create(&(*last_subscribe)->thread, NULL,
			   thread_routine, (*last_subscribe)) != 0) {
		goto err_free_mem;
	}
#endif /* __KERNEL__ */

	return BB_OK;

err_free_mem:
	free (*last_subscribe);
	*last_subscribe = NULL;
err:
	return BB_NOK;
}
#ifdef __KERNEL__
EXPORT_SYMBOL_GPL(bb_msg_subscribe);
#endif /* __KERNEL__ */

#ifdef __KERNEL__
static void bb_cn_callback(void *arg)
{
	struct cn_msg *cn_msg = arg;
	struct S_BB *bb = present_bbs[VAL_2_INDEX(cn_msg->id.val)];
	struct S_BB_LOCAL *local = bb_get_local(bb);
	struct S_BB_SUBSCRIBE *subscribe;

	for (subscribe = local->subscribed;
	     subscribe !=  NULL &&
		     subscribe->msg.mtype != VAL_2_MTYPE(cn_msg->id.val);
	     subscribe = subscribe->next);

	if (subscribe == NULL) {
		printk("subscribed FAILED !! mtype (%d) bb (%s) val (0x%08x) index (%d)\n",
		       VAL_2_MTYPE(cn_msg->id.val), bb->name,
		       cn_msg->id.val, VAL_2_INDEX(cn_msg->id.val));
		return;
	}

	if (cn_msg->len > MAX_SYSMSG_SIZE) {
		printk("connector messsage too long (%d)\n", cn_msg->len);
		return;
	}

	memcpy(subscribe->msg.mtext, cn_msg->data, cn_msg->len);

	subscribe->callback(subscribe->bb, subscribe->context,
			    &subscribe->msg);
}

#else /* __KERNEL__ */
static void * thread_routine(void *arg)
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
			       &subscribe->msg) == BB_NOK) {
			bb_logMsg(BB_LOG_WARNING, MOD_NAME,
				  "bb_rcv_msg failed -> thread stoped\n");
			return NULL;
		}
		subscribe->callback(subscribe->bb, subscribe->context,
				    &subscribe->msg);
	}
}
#endif /* __KERNEL__ */

int32_t bb_msg_unsubscribe(struct S_BB *bb, const struct S_BB_MSG *msg)
{
	struct S_BB_SUBSCRIBE **previous_subscribe;
	struct S_BB_SUBSCRIBE *elem;
	struct S_BB_LOCAL *local = bb_get_local(bb);

	/* mtype si stored on the NBIT_FOR_MTYPE LSB bits of cb_id.val */
	if (msg->mtype > MTYPE_MASK)
		return BB_NOK;

	if (!local)
		return BB_NOK;

	/* find the end of the list (or subscribed msg)*/
	for (previous_subscribe = &local->subscribed;
	     (*previous_subscribe) !=  NULL &&
		     (*previous_subscribe)->msg.mtype != msg->mtype;
	     previous_subscribe = &(*previous_subscribe)->next);

	/* not found ? */
	if ((*previous_subscribe) ==  NULL)
		return BB_NOK;

#ifdef __KERNEL__
	{
		struct cb_id cb_id = {
			.idx = CN_IDX_BB,
			.val = MTYPE_INDEX_2_VAL((*previous_subscribe)->msg.mtype,
						 bb->priv.k.index)
		};
		cn_del_callback(&cb_id);
	}
#else /* __KERNEL__ */
	pthread_cancel((*previous_subscribe)->thread);
	pthread_join((*previous_subscribe)->thread, NULL);
#endif /* __KERNEL__ */

	elem = *previous_subscribe;
	(*previous_subscribe) = (*previous_subscribe)->next;
	free(elem);

	return BB_OK;
}
#ifdef __KERNEL__
EXPORT_SYMBOL_GPL(bb_msg_unsubscribe);
#endif /* __KERNEL__ */


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
#ifdef __KERNEL__
EXPORT_SYMBOL_GPL(bb_msg_unsubscribe_all);
#endif /* __KERNEL__ */


