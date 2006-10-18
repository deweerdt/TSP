/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.h,v 1.18 2006-10-18 09:58:48 erk Exp $

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

/**
 * @defgroup TSP_DatapoolLib Datapool Library
 * @ingroup TSP_ProviderLib
 * The datapool library is an internal provider-side TSP API
 * which is used by the GLU in order to push sample data
 * to the \ref TSP_ProviderLib.
 * @{
 */

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
 * @param[in] cthis the datapool object
 * @param[in]  nb   pointer on where to store the number of items
 * @param[out] list pointer on list of global_index
 */ 
_EXPORT_TSP_PROVIDER void TSP_datapool_get_reverse_list (TSP_datapool_t* cthis, int *nb, int **list);

/**
 * Push GLU data into datapool.
 * @param[in,out] cthis  the datapool object
 * @param[in]     item  the GLU item to push in the datapool. The item is copied
 *                      and may be re-used on return.
 */ 
_EXPORT_TSP_PROVIDER int TSP_datapool_push_next_item(TSP_datapool_t* cthis, glu_item_t* item);

/**
 * End of push, we commit the whole
 * @param[in,out] cthis the datapool object to commit in.
 * @param[in] time_stamp  date of datapool items
 * @param[in] state ok or error (reconf,eof, ...)
 */ 
_EXPORT_TSP_PROVIDER int TSP_datapool_push_commit(TSP_datapool_t* cthis, time_stamp_t time_stamp, GLU_get_state_t state);


/**
 * Allocation of a datapool.
 * @param[in] glu Handle for the GLU that mus be linked to this datapool
 *                  datapool size will be GLU provided number of symbol(s).
 * @return The datapool object created on success, NULL on failure.
 */ 
TSP_datapool_t* TSP_datapool_new(GLU_handle_t* glu);

/**
 * Destroy a datapool.
 * Only used when the sample server is a passive one.
 * @pre pthis != NULL
 * @pre (*pthis) != NULL
 * @param[in,out] pthis Pointer to the datapool object to be destroyed. The pointee
 *                      will be nullified on return.
 * @post (*pthis) == NULL.
 */ 
void TSP_datapool_delete(TSP_datapool_t** pthis);

/**
 * Initialize datapool with provided GLU.
 * This function will allocate internal datapool structure.
 * After this datapool is ready to be used by the refered GLU.
 * @param[in,out] cthis the datapool to be initialized
 * @param[in,out] glu the GLU to be linked with the datapool
 * @return TSP_STATUS_OK on success.
 */
int32_t 
TSP_datapool_initialize(TSP_datapool_t* cthis, GLU_handle_t* glu);

/**
 * Initialize datapool with provided GLU.
 * @param[in,out] cthis the datapool to be finalized
 * @return TSP_STATUS_OK on success.
 */
int32_t 
TSP_datapool_finalize(TSP_datapool_t* cthis);

/**
 * Get the datapool instance for this GLU.
 * <ul>
 * <li>ACTIVE   GLU will get global datapool singleton instance</li>
 * <li>PASSIVE  GLU will get their own local datapool instance</li>
 * </ul>
 * When the sample server is an active one, this function work as 
 * a singleton. If the global datapool does not exist or was destroyed, 
 * a new datapool is created. 
 * @param[in,out] glu the GLU object. On return the datapool member
 *                of the GLU is updated with the retrieved datapool.
 * @return The datapool handle
 */ 
TSP_datapool_t* TSP_datapool_instantiate(GLU_handle_t* glu);


/**
 * Get the address of a value in the datapool
 * @param cthis The datapool handle
 * @param provider_global_index The index of the symbol that is searched
 * @return The data address
 */ 
void* 
TSP_datapool_get_symbol_value(TSP_datapool_t* cthis, 
			      int provider_global_index);

 
END_C_DECLS

/** @} */

#endif /* _TSP_DATAPOOL_H */
