/*

$Id: tsp_common_ssei.h,v 1.8 2006-05-03 21:13:34 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD ,Robert PAGNOT and Arnaud MORVAN

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Common

-----------------------------------------------------------------------

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_COMMON_SSEI_H
#define _TSP_COMMON_SSEI_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @defgroup TSP_CommonLib_SSEI Sample Symbol Extended Information Library Handling
 * The Sample Symbol Extended Information Library part of the @ref TSP_CommonLib
 * regroup a set of functions which should be used to manipulate
 * the Sample Symbol Extended Information object and list of them.
 * @ingroup TSP_CommonLib
 * @{
 */

#ifdef TSP_COMMON_SSEI_C
#else
#endif

BEGIN_C_DECLS

/**
 * create a new ei
 * @param[in] key key of the ei
 * @param[in] value value of the extended info
 * @return the new ei
 */
TSP_extended_info_t*
TSP_EI_new(const char *key,const char *value);

/**
 * destruct an ei
 * @param[in] ei ei to destruct
 * @return  TSP_STATUS_OK if ok
 */
int32_t
TSP_EI_delete(TSP_extended_info_t** ei);

/**
 * Initialize an extended information item.
 * @param[in,out] ei the extended info structure, the structure should
 *                be allocated but the field may not
 * @param[in] key the key/name of the extended information
 * @param[in] value the value of the extended information
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EI_initialize(TSP_extended_info_t* ei, const char* key, const char* value);

/**
 * Udpate an extended information item.
 * @param[in,out] ei the extended info structure, the structure should
 *                be allocated including the field
 * @param[in] key the key/name of the extended information
 * @param[in] value the value of the extended information
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EI_update(TSP_extended_info_t* ei, const char* key, const char* value);

/**
 * Finalize an extended information item.
 * @param[in,out] ei the extended info structure, the structure should
 *                be allocated but the field may not
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EI_finalize(TSP_extended_info_t* ei);

/**
 * copy a TSP Extended Info .
 * @param[in,out] dest_EI destination
 * @param[in]     src_EI  source
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EI_copy(TSP_extended_info_t* dest_EI, const TSP_extended_info_t src_EI);

/**
 * create a new eil
 * @param[in] len length of the eil
 * @return the new eil
 */
TSP_extended_info_list_t*
TSP_EIList_new(int32_t len);

/**
 * destruct an eil
 * @param[in] eil eil to destruct
 * @return  TSP_STATUS_OK if ok
 */
int32_t
TSP_EIList_delete(TSP_extended_info_list_t** eil);

/**
 * Initialize an extended information list.
 * @param[in,out] eil the extended info list structure, the structure should
 *                be allocated but the field may not
 *                On input the pre-allocated structure
 *                On output the content of the structure has been allocated 
 *                and initialized.
 * @param[in] len the length of the extended info list
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EIList_initialize(TSP_extended_info_list_t* eil, const int32_t len);

/**
 * Finalize an extended information list.
 * @param[in,out] eil the extended info list structure
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EIList_finalize(TSP_extended_info_list_t* eil);

/**
 * Find a extended information in a list by key.
 * @param[in] eil the extended information list to look in
 * @param[in] key the key of the extended information to look for 
 * @return a TSP_extended_info_t if found else NULL 
 */
const TSP_extended_info_t*
TSP_EIList_findEIByKey(const TSP_extended_info_list_t* eil, const char* key);

/**
 * copy a TSP Extended Info list.
 * @param[in,out] eil_dest destination list
 * @param[in]     eil_src  source list
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_EIList_copy(TSP_extended_info_list_t* eil_dest,
		const TSP_extended_info_list_t eil_src);

/**
 * create a new ssei
 * @param[in] pgi pgi of the ei
 * @param[in] nei number of extended info
 * @return the new ssei
 */
TSP_sample_symbol_extended_info_t*
TSP_SSEI_new(const int32_t pgi, const int32_t nei);

/**
 * destruct an ssei
 * @param[in] ssei ssei to destruct
 * @return TSP_STATUS_OK if ok
 */
int32_t
TSP_SSEI_delete(TSP_sample_symbol_extended_info_t** ssei);

/**
 * Initialize a TSP Sample Symbol Extended Info structure.
 * @param[in,out] ssei the SSEI structure 
 * @param[in] pgi the provider global index
 * @param[in] nei the number of extended informations to be attached to this PGI
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEI_initialize(TSP_sample_symbol_extended_info_t* ssei, const int32_t pgi, const int32_t nei);

/**
 * destruction of a TSP Sample Symbol Extended Info structure.
 * @param[in,out] ssei struct to destroy
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEI_finalize(TSP_sample_symbol_extended_info_t* ssei);

/**
 * copy a TSP Sample Symbol Extended Info structure.
 * @param[in,out] ssei_dest destination  
 * @param[in]     ssei_src  source 
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEI_copy(TSP_sample_symbol_extended_info_t* ssei_dest,
  	      const TSP_sample_symbol_extended_info_t ssei_src);

/**
 * create a new sseil
 * @param[in] len length of the sseil
 * @return the new sseil
 */
TSP_sample_symbol_extended_info_list_t*
TSP_SSEIList_new(int32_t len);

/**
 * destruct an sseil
 * @param[in] ssei_list  sseil to destruct
 * @return  TSP_STATUS_OK if ok
 */
int32_t
TSP_SSEIList_delete(TSP_sample_symbol_extended_info_list_t** ssei_list);

/**
 * initialize a TSP Sample Symbol Extended Info list structure.
 * @param[in,out] ssei_list list to initialize
 * @param[in]     len length of the list
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEIList_initialize(TSP_sample_symbol_extended_info_list_t* ssei_list,int32_t len);

/**
 * Destruction of a TSP Sample Symbol Extended Info list structure.
 * @param[in,out] ssei_list list to destroy
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEIList_finalize(TSP_sample_symbol_extended_info_list_t* ssei_list);

/**
 * copy a TSP Sample Symbol Extended Info list structure.
 * @param[in,out] dest_ssei_list destination list 
 * @param[in] src_ssei_list source list
 * @return TSP_STATUS_OK on success 
 */
int32_t
TSP_SSEIList_copy(TSP_sample_symbol_extended_info_list_t* dest_ssei_list,
                  const TSP_sample_symbol_extended_info_list_t src_ssei_list);


TSP_extended_info_list_t*
TSP_SSEIList_getEIList(const TSP_sample_symbol_extended_info_list_t SSEIList, uint32_t index);

END_C_DECLS

/** @} */
 

#endif /* _TSP_COMMON_SSEI_H */
