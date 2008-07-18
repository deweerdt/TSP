/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_local.c,v 1.1 2008-07-18 15:09:53 jaggy Exp $

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
#define fprintf(fd,fmt,pr...) printk(fmt, ## pr)
#endif /* __KERNEL__*/

struct S_BB_2_LOCAL
{
	const struct S_BB *bb;
	struct S_BB_LOCAL *local;
};

#define MAX_LOCAL (255)
static struct S_BB_2_LOCAL bb2local[MAX_LOCAL] = {{NULL,NULL},};

static void bb_local_initialize(struct S_BB_LOCAL *local)
{
#ifdef __KERNEL__
#else /* __KERNEL__ */
	local->subscribed = NULL;
#endif /* __KERNEL__ */

	return;
}

int32_t bb_init_local(struct S_BB *bb)
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
	free_entry->local = malloc(sizeof(*(free_entry->local)));

	if (!free_entry->local) {
		free_entry->bb = NULL;
		return BB_NOK;
	}

	bb_local_initialize(free_entry->local);

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

int32_t bb_clear_local(struct S_BB *bb)
{
	int i;
	for (i = 0 ; i < MAX_LOCAL ; i++) {
		if (bb2local[i].bb == bb) {
			bb2local[i].bb = NULL;
			bb_msg_unsubscribe_all(bb);
			free(bb2local[i].local);
			return BB_OK;
		}
	}

	return BB_NOK;
}

