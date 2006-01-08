/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_alias.h,v 1.1 2006-01-08 19:22:27 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */
#ifndef _BB_ALIAS_H_
#define _BB_ALIAS_H_

#include <bb_core.h>
#define ALIAS_SEPARATOR     "."
#define MAX_ALIAS_LEVEL     10

BEGIN_C_DECLS

/**
 * True if the described data is an alias.
 * @param data_desc IN, the data descriptor.
 * @return 1 if the specified data is an alias 0 otherwise.
 */
int32_t bb_isalias(const S_BB_DATADESC_T* data_desc);

int32_t bb_find_aliastack(volatile S_BB_T* bb,       
			  S_BB_DATADESC_T* data_desc_stack, 
			  int32_t* stack_max_size);

unsigned long bb_aliasstack_offset(S_BB_DATADESC_T* data_desc_stack, 
				   int32_t* index_stack,
				   int32_t stack_size);
/**
 * Publish an aliased data in a blackboard.
 * This request allocate space for the specified data in blackboard
 * and return the address of the newly allocated space.
 * This function has the same semantic as malloc(3).
 * @param bb INOUT, BB pointer (should not be NULL).
 * @param data_desc INOUT, Data descriptor of the data to be published.
 *                         OUT, if data has been properly allocated
 *                         the S_BB_DATADESC_T.data_offset is updated
 *                         and S_BB_DATADESC_T.alias_target too.
 * @param data_desc_target IN, Data descriptor of the data to be aliased.
 * @return address of the (first) aliased data, NULL
 *         if alias failed.
 * @ingroup BlackBoard
 */
void* 
bb_alias_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc, 
		 const S_BB_DATADESC_T* data_desc_target);


void* 
bb_alias_subscribe(volatile S_BB_T *bb, 
	          		S_BB_DATADESC_T* data_desc,
				 		const int32_t* indexstack,
						const int32_t indexstack_len);

/**
 * increment indexstack from the left most
 * element (alias) to the right most element (target)
 * 
 * @return E_OK on success, E_NOK otherwise
 * @ingroup BlackBoard
 */					
int32_t 
bb_alias_increment_idxstack(S_BB_DATADESC_T * aliasstack, int32_t aliasstack_size,
			    int32_t * idxstack, int32_t idxstack_len); 



END_C_DECLS
#endif /* _BB_ALIAS_H_ */
