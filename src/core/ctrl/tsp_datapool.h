/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.h,v 1.13 2006-02-26 13:36:05 erk Exp $

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

typedef  void* TSP_datapool_t;

BEGIN_C_DECLS

/**
 * Get the reverse list of global_index wanted by some consumers
 * @param nb   : pointer on where to store the number of items
 * @param list : pointer on list of global_index
 */ 
void TSP_datapool_get_reverse_list (int *nb, int **list);

/**
 * Insted of thread created, we push directly the data
 * @param item : what to push
 */ 
/*inline*/ int TSP_datapool_push_next_item(glu_item_t* item);

/**
 * End of push, we commit the whole
 * @param time : date of datapool items
 * @param state: ok or error (reconf,eof, ...)
 */ 
int TSP_datapool_push_commit(time_stamp_t time_stamp, GLU_get_state_t state);


/**
 * Allocation of a local datapool.
 * Only used when the sample server is a passive one.
 * @param symbols_number Total number of symbols for this datapool
 * @param h_glu Handle for the GLU that mus be linked to this datapool
 * @return The datapool handle
 */ 
TSP_datapool_t TSP_local_datapool_allocate(int symbols_number, GLU_handle_t* h_glu );

/**
 * Destroy a local datapool.
 * Only used when the sample server is a passive one.
 * @param datapool The datapool handle
 */ 
void TSP_local_datapool_destroy(TSP_datapool_t datapool);

/**
 * Get the global datapool instance.
 * Only used when the sample server is an active one.
 * This function work as a singleton. If the global datapool
 * does not exist or was destroyed, a new datapool is created.
 * @return The datapool handle
 */ 
TSP_datapool_t TSP_global_datapool_get_instance(GLU_handle_t* glu);

/**
 * Get the address of a value in the datapool
 * @param datapool The datapool handle
 * @param provider_global_index The index of the symbol that is searched
 * @param type FIXME : this parameter was used to ask for a given type of
 * symbol. As the datapool items should be RAW (and converted when needed),
 * this parameter will be useless.
 * @return The data address
 */ 
void* TSP_datapool_get_symbol_value(TSP_datapool_t datapool, 
			      int provider_global_index,
			      xdr_and_sync_type_t type);


/**
 * For a local datapool, starts the associated thread.
 * Only used when the GLU is pasive
 * @param datapool The datapool handle
 * @return TRUE or FALSE. TRUE = OK
 */ 
int TSP_local_datapool_start_thread(TSP_datapool_t datapool);

/**
 * For a local datapool, stops the associated thread.
 * Only used when the GLU is pasive
 * @param datapool The datapool handle
 * @return TRUE or FALSE. TRUE = OK
 */ 
int TSP_local_datapool_wait_for_end_thread(TSP_datapool_t datapool);
 
END_C_DECLS

#endif /* _TSP_DATAPOOL_H */
