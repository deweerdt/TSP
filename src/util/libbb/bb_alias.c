/*
  
$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_alias.c,v 1.5 2006-11-27 19:55:14 deweerdt Exp $

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

Purpose   : Blackboard (alias handling)

-----------------------------------------------------------------------
 */

#ifndef __KERNEL__

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include <tsp_abs_types.h>

#endif /* __KERNEL__ */

#include "bb_core.h"
#include "bb_alias.h"
#include "bb_utils.h"

int32_t 
bb_isalias(const S_BB_DATADESC_T* data_desc) {
  assert(data_desc);
  return (data_desc->alias_target != -1);
} /* end of bb_isalias */

int32_t 
bb_find_aliastack(volatile S_BB_T* bb,       
		  S_BB_DATADESC_T* data_desc_stack, 
		  int32_t* stack_max_size) {
  int32_t retval             = 0;
  int32_t current_stack_size = 1;

  assert(bb);
  assert(data_desc_stack);
  assert(stack_max_size);
  
  while ((current_stack_size<(*stack_max_size)) &&
	 bb_isalias(&data_desc_stack[current_stack_size-1])) {

    data_desc_stack[current_stack_size] = 
      bb_data_desc(bb)[data_desc_stack[current_stack_size-1].alias_target];
    ++current_stack_size;
  }
  if ((current_stack_size==(*stack_max_size)) && bb_isalias(&data_desc_stack[current_stack_size-1])) {
    bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_find_aliastack", 
	      "MAX alias stacksize exceeded <%d>",
	      *stack_max_size);
    retval = -1;
  } else {
    *stack_max_size = current_stack_size;
  }
  return retval;
} /* end of bb_find_aliastack */

unsigned long 
bb_aliasstack_offset(S_BB_DATADESC_T* data_desc_stack, 
		     int32_t* index_stack,
		     int32_t stack_size) {
  long retval;
  int32_t i;
  retval  = 0;
  
  assert(data_desc_stack);
  assert(index_stack);

  for (i=0;i<stack_size;++i) {
    retval += data_desc_stack[i].data_offset+index_stack[i]*data_desc_stack[i].type_size;
  }
  return retval;
} /* end of bb_aliasstack_offset */

void* 
bb_alias_publish(volatile S_BB_T *bb, 
		 S_BB_DATADESC_T* data_desc,
		 const S_BB_DATADESC_T* data_desc_target) {
  
  void* retval;
  S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
  int32_t          indexstack[MAX_ALIAS_LEVEL];
  int32_t          aliasstack_size = MAX_ALIAS_LEVEL;

  retval = NULL;
  assert(bb);
  assert(data_desc);
  assert(data_desc_target);
  /* zero out indexstack */
  memset(indexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
  
  /* Verify that the published data is not already published
   * (key unicity) and trigger automatic subscribe
   * if key already exists.
   */
  bb_lock(bb);
  if (bb_find(bb,data_desc->name) != -1) {
     bb_logMsg(BB_LOG_FINER,"BlackBoard::bb_publish",
	       "Key <%s> already exists in blackboard (automatic subscribe)!!",data_desc->name);
    bb_unlock(bb);
    retval = bb_subscribe(bb,data_desc);
    bb_lock(bb);
  } 
  
  else {
    /* verify available space in BB data descriptor zone */
    if (bb->n_data >= bb->max_data_desc_size) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_alias_publish", 
		"No more room in BB data descriptor!! [current n_data=%d]",
		bb->n_data);
         
    } 
    else {
      /* find the target */
      data_desc->alias_target = bb_find(bb,data_desc_target->name);
      if (-1 != data_desc->alias_target) {
	      /* 
	       * check described offset is not
	       * out of target range 
	       */
	      if ((data_desc->data_offset) + (data_desc->type_size) <=
	          (data_desc_target->type_size)*(data_desc_target->dimension)) {
	        /* Compute the first alias address */
	        aliasstack[0] = *data_desc_target;
	        if (!bb_find_aliastack(bb,aliasstack,&aliasstack_size)) {
	          retval = (char*) bb_data(bb) + bb_aliasstack_offset(aliasstack,indexstack,aliasstack_size)
	            + data_desc->data_offset;
	          /* Update data descriptor zone */
	          bb_data_desc(bb)[bb->n_data] = *data_desc;
	          /* Increment number of published data */
	          bb->n_data++;
	        } else {	    
	          bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_alias_publish",
		            "Cannot resolve alias stack");
	        }
	      } else {
	        bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_alias_publish", 
		          "Alias dim * size <%d> * <%d> and offset <%d> goes out of target range <%d> (%s)",
		          data_desc->dimension,
		          data_desc->type_size,
		          data_desc->data_offset,
		          data_desc_target->type_size,
		          data_desc_target->name);
	      }
      } 
      else {
	        bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_alias_publish", 
		          "Target <%s> does not exists",
		          data_desc_target->name);
      }
    }
    /* DO NOT initialize publish data zone with default value */
    /* You dont want an alias to be initialized since it may
     * destroy data previously initialized with the genuine (non-aliased)
     * structure.
     */
    /* bb_data_initialise(bb,data_desc,NULL); */
  }    
  /* no init in case of automatic subscribe */  
  bb_unlock(bb);  
  return retval;
} /* end of bb_alias_publish */

void* 
bb_alias_subscribe(volatile S_BB_T* bb, 
		   S_BB_DATADESC_T* data_desc,
		   const int32_t*   indexstack,
		   const int32_t    indexstack_len) {
  
  void* retval;
  int32_t  idx;
  
  retval = NULL;
  assert(bb);
  assert(data_desc);

   /* We seek the data using its key (name) */
  bb_lock(bb);
  idx = bb_find(bb,data_desc->name);
  if (idx==-1) {
    retval = NULL;      
  } else {
    if (E_BB_DISCOVER == data_desc->type) {
      data_desc->type            = (bb_data_desc(bb)[idx]).type;
    }

    if (0 == data_desc->dimension) {
      data_desc->dimension       = (bb_data_desc(bb)[idx]).dimension;
    }

    if (0 == data_desc->type_size) {
      data_desc->type_size       = (bb_data_desc(bb)[idx]).type_size;
    }
    data_desc->data_offset     = (bb_data_desc(bb)[idx]).data_offset;
    data_desc->alias_target    = (bb_data_desc(bb)[idx]).alias_target;
    /* return NULL pointer if symbol signature does not match */
    if ((data_desc->type      !=  (bb_data_desc(bb)[idx]).type)     ||
	(data_desc->dimension != (bb_data_desc(bb)[idx]).dimension) ||
	(data_desc->type_size != (bb_data_desc(bb)[idx]).type_size)) {
      retval = NULL;

    } else {
	    retval=bb_item_offset(bb,data_desc,indexstack,indexstack_len);
    }
  }
  bb_unlock(bb);
  return retval;
} /* end of bb_alias_subscribe */

int32_t 
bb_alias_increment_idxstack(S_BB_DATADESC_T * aliasstack, int32_t aliasstack_size,
			    int32_t * indexstack, int32_t indexstack_len) {
					
  int j = 0;
  int32_t indexstack_curr = indexstack_len-1;
  int32_t return_value = BB_NOK;
  
  while (j<aliasstack_size && return_value == BB_NOK){
    if (aliasstack[j].dimension > 1) {
      if (indexstack[indexstack_curr] < (aliasstack[j].dimension - 1) ) {
	++indexstack[indexstack_curr];
	return_value = BB_OK;
      }
      else {
	indexstack[indexstack_curr] = 0;
	--indexstack_curr;	
      }
    }
    ++j;
  }
  return return_value;
} /* end of bb_alias_increment_idxstack */


