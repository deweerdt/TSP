/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_local.h,v 1.2 2008-07-21 11:55:09 jaggy Exp $

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

#ifndef _BB_LOCAL_H_
#define _BB_LOCAL_H_

#include "bb_core.h"

struct S_BB_SUBSCRIBE;

typedef struct S_BB_LOCAL {
#ifdef __KERNEL__
#else /* __KERNEL__ */
	struct S_BB_SUBSCRIBE * subscribed;
#endif /* __KERNEL__ */
} S_BB_LOCAL_T;


/**
 * A function to create and initiliaze a local structure
 * 
 * @return a valid pointer if the operation succeeded, NULL otherwise.
 */
struct S_BB_LOCAL *bb_local_new(void);

/**
 * A function to delete a local structure
 *
 * @param[in] local a BB local pointer
 */
void bb_local_delete(struct S_BB_LOCAL *local);

/**
 * A function to attach a local structure to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
int32_t bb_attach_local(struct S_BB *bb, struct S_BB_LOCAL *local);

/**
 * A function to clear local structure attached to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
int32_t bb_detach_local(struct S_BB *bb);

/**
 * A function to get local structure attached to a BlackBoard
 * @param[in] bb a BlackBoard pointer
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
struct S_BB_LOCAL *bb_get_local(const S_BB_T *bb);

#endif /* _BB_LOCAL_H_ */
