/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_local.c,v 1.3 2008-07-23 15:18:05 jaggy Exp $

-----------------------------------------------------------------------

TSP Library - A way to attach local context to a Blackboard.

Copyright (c) 2008 Jerome Arbez-Gindre

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
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : BlackBoard Local Data

-----------------------------------------------------------------------
*/

#include "bb_local.h"
#include "bb_callback.h"

#define MOD_NAME "BB_LOCAL"

#ifdef __KERNEL__

#include "bb_core_k.h"
#define fprintf(fd,fmt,pr...) printk(fmt, ## pr)

struct S_BB_LOCAL *present_locals[BB_DEV_MAX] =
{ [0]			= NULL,
  [BB_DEV_MAX-1]	= NULL};

#else /* __KERNEL__ */
struct S_BB_2_LOCAL
{
	const struct S_BB *bb;
	struct S_BB_LOCAL *local;
};

#define MAX_LOCAL (255)
static struct S_BB_2_LOCAL bb2local[MAX_LOCAL] =
{[0]			= {NULL,NULL},
 [MAX_LOCAL - 1]	= {NULL,NULL}};

#endif /* __KERNEL__ */

static void bb_local_initialize(struct S_BB_LOCAL *local)
{
#ifdef __KERNEL__
	local->kmalloc_ptr = NULL;
	local->dev = NULL;
#endif /* __KERNEL__ */
	local->subscribed = NULL;

	return;
}

struct S_BB_LOCAL *bb_local_new(void)
{
	struct S_BB_LOCAL *local = malloc(sizeof(*local));

	if (local)
		bb_local_initialize (local);
	return local;
}

void bb_local_delete (struct S_BB_LOCAL *local)
{
	free(local);
}

#ifdef __KERNEL__

int32_t bb_attach_local(struct S_BB *bb, struct S_BB_LOCAL *local)
{
	if (present_locals[bb->priv.k.index] != NULL)
		return BB_NOK;

	present_locals[bb->priv.k.index] = local;
	return BB_OK;
}

struct S_BB_LOCAL *bb_get_local(const S_BB_T *bb)
{
	return present_locals[bb->priv.k.index];
}

int32_t bb_detach_local(struct S_BB *bb)
{
	/* this must be done first (can still use
	   bb_get_local)*/
	bb_msg_unsubscribe_all(bb);
	present_locals[bb->priv.k.index] = NULL;
	return BB_OK;
}

#else /* __KERNEL__ */

int32_t bb_attach_local(struct S_BB *bb, struct S_BB_LOCAL *local)
{
	int i;
	struct S_BB_2_LOCAL *free_entry = NULL;

	/* find if a local is already attached to this bb and search a
	 * free entry */
	for (i = 0 ; i < MAX_LOCAL ; i++) {
		if (bb2local[i].bb == bb) {
			return BB_NOK;
		}
		if (bb2local[i].bb == NULL && free_entry == NULL) {
			free_entry = &bb2local[i];
		}
	}

	/* if there's no free_entry... create one */
	if (free_entry == NULL) {
		bb_logMsg(BB_LOG_WARNING, MOD_NAME,
			  "Fail to find a free entry... "
			  "Consider increase MAX_LOCAL (actually %d)",
			  MAX_LOCAL);
		return BB_NOK;
	}

	free_entry->bb = bb;
	free_entry->local = local;

	return BB_OK;
}

struct S_BB_LOCAL *bb_get_local(const S_BB_T *bb)
{
	int i;
	for (i = 0 ; i < MAX_LOCAL ; i++)
		if (bb2local[i].bb == bb)
			return bb2local[i].local;
	return NULL;
}

int32_t bb_detach_local(struct S_BB *bb)
{
#ifndef __KERNEL__
#define printk printf
#endif
	int i;
	for (i = 0 ; i < MAX_LOCAL ; i++) {
		if (bb2local[i].bb == bb) {
			/* this must be done first (can still use
			 bb_get_local)*/
			bb_msg_unsubscribe_all(bb);
			bb2local[i].bb = NULL;
			return BB_OK;
		}
	}
	return BB_NOK;
}

#endif /* __KERNEL__ */
