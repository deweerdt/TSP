/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.h,v 1.15 2006-04-23 20:06:48 erk Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Datapool implementation

-----------------------------------------------------------------------
 */

#ifndef _TSP_DATAPOOL_H
#define _TSP_DATAPOOL_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include <tsp_glu.h>


typedef struct TSP_datapool_item  {
  /** One Item in the datapool.
   * FIXME : The values should be saved in raw format in the datapool,
   * and must be calculated for their final value when they are sent, depending
   * on the data format that each consumer requested (double, raw, string).
   * Somehow the GLU should provide its own functions to transform a RAW in double or
   * string.
   */
  void* raw_value;
  int   is_wanted;	/* Is the symbol wanted by some consumers */
} TSP_datapool_item_t 
;

typedef struct TSP_datapool {

  /** Is the datapool initialized ? */
  int initialized;

  /** Tells if the datapool was used and destroyed */
  int terminated;
  
  /** handle on GLU */
  GLU_handle_t* h_glu;

  /** List of items in the datapool */
  TSP_datapool_item_t* items;
  int size;

  /** Reverse list of wanted items index */
  int *reverse_index;
  int nb_wanted_items;

} TSP_datapool_t;


BEGIN_C_DECLS

/**
 * Get the reverse list of global_index wanted by some consumers
 * @param nb   : pointer on where to store the number of items
 * @param list : pointer on list of global_index
 */ 
void TSP_datapool_get_reverse_list (TSP_datapool_t* datapool, int *nb, int **list);

/**
 * Instead of thread created, we push directly the data
 * @param item : what to push
 */ 
/*inline*/ int TSP_datapool_push_next_item(TSP_datapool_t* datapool, glu_item_t* item);

/**
 * End of push, we commit the whole
 * @param time : date of datapool items
 * @param state: ok or error (reconf,eof, ...)
 */ 
int TSP_datapool_push_commit(TSP_datapool_t* datapool, time_stamp_t time_stamp, GLU_get_state_t state);


/**
 * Allocation of a datapool.
 * Only used when the sample server is a passive one.
 * @param symbols_number Total number of symbols for this datapool
 * @param h_glu Handle for the GLU that mus be linked to this datapool
 * @return The datapool handle
 */ 
TSP_datapool_t* TSP_datapool_new(int symbols_number, GLU_handle_t* h_glu );

/**
 * Destroy a datapool.
 * Only used when the sample server is a passive one.
 * @param datapool The datapool handle
 */ 
void TSP_datapool_delete(TSP_datapool_t** datapool);

/**
 * Get the global datapool instance.
 * Only used when the sample server is an active one.
 * This function work as a singleton. If the global datapool
 * does not exist or was destroyed, a new datapool is created.
 * @return The datapool handle
 */ 
TSP_datapool_t* TSP_global_datapool_instantiate(GLU_handle_t* glu);

/**
 * Get the address of a value in the datapool
 * @param datapool The datapool handle
 * @param provider_global_index The index of the symbol that is searched
 * @return The data address
 */ 
void* 
TSP_datapool_get_symbol_value(TSP_datapool_t* datapool, 
			      int provider_global_index);

 
END_C_DECLS

#endif /* _TSP_DATAPOOL_H */
