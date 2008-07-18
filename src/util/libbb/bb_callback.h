/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_callback.h,v 1.1 2008-07-18 15:09:53 jaggy Exp $

-----------------------------------------------------------------------

BlackBoard Library - API to subscribe/unsubscribe callback on messages
receiving

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

#ifndef _BB_CALLBACK_H_
#define _BB_CALLBACK_H_

#ifdef __KERNEL__

#include <linux/types.h>

#else /* __KERNEL__ */

#include <inttypes.h>

#endif /* __KERNEL__ */

struct S_BB;
struct S_BB_MSG;

/**
 * Launch a thread which waits in endless loop for a message on the BB
 * message queue, and then calls the callback.
 *
 * @param[in,out] bb  pointer to BB.
 * @param[in,out] msg  the message to be subcribed on.
 *                  The type of the message to be received should be
 *                  be specified on entry in the message structure
 *                  msg->mtype.
 * @param[in,out] callback The callback wich will be called each time
 *                   the message is received.
 * @return BB_OK on success, BB_NOK otherwise
 */
int32_t bb_msg_subscribe(struct S_BB *bb, const struct S_BB_MSG *msg,
			 void (*callback)(struct S_BB *bb, struct S_BB_MSG *msg));

/**
 * Stops the thread which is waiting for a message on the BB message
 * queue.
 *
 * @param[in,out] bb  pointer to BB.
 * @param[in,out] msg  the message to be subcribed on.
 *                  The type of the message to be received should be
 *                  be specified on entry in the message structure
 *                  msg->mtype.
 * @return BB_OK on success, BB_NOK otherwise
 */
int32_t bb_msg_unsubscribe(struct S_BB *bb, const struct S_BB_MSG *msg);

/**
 * unsubscribe to all subscribed msg
 * queue.
 *
 * @param[in,out] bb  pointer to BB.
 * @return BB_OK on success, BB_NOK otherwise
 */
int32_t bb_msg_unsubscribe_all(struct S_BB *bb);


#endif /* _BB_CALLBACK_H_ */
