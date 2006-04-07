/*

$Id: tsp_common_ssi.h,v 1.1 2006-04-07 09:30:36 erk Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Common

-----------------------------------------------------------------------

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_COMMON_SSI_H
#define _TSP_COMMON_SSI_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @defgroup TSP_CommonLib_SSI Sample Symbol Information Handling
 * The Sample Symbol Information Handling part of the @ref TSP_CommonLib
 * regroup a set of functions which should be used to manipulate
 * the Sample Symbol Information object and list of them.
 * @ingroup TSP_CommonLib
 * @{
 */

#ifdef TSP_COMMON_SSI_C
#else
#endif

BEGIN_C_DECLS

/**
 * Copy a Sample Symbol Information Structure.
 * @param[in,out] dest_symbol the (preallocated) sample symbol information structure. 
 *                The structure should be allocated but not the member of the structure.
 *                On entry should  be a non NULL pointer to  TSP_sample_symbol_info_t.
 *                On return contains the .
 * @param[in]     src_symbol 
 */
void
TSP_common_SSI_copy(TSP_sample_symbol_info_t* dest_symbol, 
		    TSP_sample_symbol_info_t src_symbol);

/**
 * Initialize a Sample Symbol Information Structure.
 * This does provide "reasonable" default values to all field.
 * BEWARE that the name field is nullified too.
 * @param[in,out] ssi the (preallocated) sample symbol information structure. 
 *                The structure should be allocated but not the member of the structure.
 *                On entry should  be a non NULL pointer to  TSP_sample_symbol_info_t.
 *                On return the initialized structure.
 */
void 
TSP_common_SSI_initialize(TSP_sample_symbol_info_t* ssi);


/**
 * Allocate a list of sample symbol information and initialize it.
 * @param[out] ssil pointer to pointer to the list to be create.
 * @param[in]  nbSSI the number of SSI in the to be created list.
 * @return TSP_STATUS_OK if allocation is OK. TSP_STATUS_ALLOC_FAILED otherwise.
 */
int32_t 
TSP_common_SSIList_create(TSP_sample_symbol_info_list_t** ssil, int32_t nbSSI);

/**
 * Initialize a preallocated list of sample symbol information.
 * @param[in] ssil the list to be initialized.
 */
void 
TSP_common_SSIList_initialize(TSP_sample_symbol_info_list_t* ssil, int32_t nbSSI);

/**
 * Copy a list of symbols from src_symbols to dest_symbols.
 * @param[out] dest_symbols the symbols list copy destination
 * @param[in] src_symbols the symbols list copy source
 */
void
TSP_common_SSIList_copy(TSP_sample_symbol_info_list_t* dest_symbols, 
			TSP_sample_symbol_info_list_t  src_symbols);

/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_SSI_H */
