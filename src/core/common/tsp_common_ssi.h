/*

$Id: tsp_common_ssi.h,v 1.4 2006-04-17 23:05:48 erk Exp $

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

TSP_sample_symbol_info_t*
TSP_SSI_new(const char* name,
	    int32_t pgi,
	    int32_t pgridx,
	    int32_t pgrank,
	    int32_t type,
	    int32_t dimension,
	    int32_t offset,
	    int32_t nelem,
	    int32_t period,
	    int32_t phase);

int32_t
TSP_SSI_delete(TSP_sample_symbol_info_t** ssi);


int32_t
TSP_SSI_initialize(TSP_sample_symbol_info_t* ssi,
		   const char* name,
		   int32_t pgi,
		   int32_t pgridx,
		   int32_t pgrank,
		   int32_t type,
		   int32_t dimension,
		   int32_t offset,
		   int32_t nelem,
		   int32_t period,
		   int32_t phase);

int32_t
TSP_SSI_finalize(TSP_sample_symbol_info_t* ssi);

/**
 * Initialize (default value) a Sample Symbol Information Structure.
 * This does provide "reasonable" default values to all field.
 * BEWARE that the name field is nullified too.
 * @param[in,out] ssi the (preallocated) sample symbol information structure. 
 *                The structure should be allocated but not the member of the structure.
 *                On entry should  be a non NULL pointer to  TSP_sample_symbol_info_t.
 *                On return the initialized structure.
 * @return TSP_STATUS_OK on success
 */
int32_t 
TSP_SSI_initialize_default(TSP_sample_symbol_info_t* ssi);

int32_t 
TSP_SSI_initialize_request_minimal(TSP_sample_symbol_info_t* ssi,
				   const char* name,
				   int32_t period);

int32_t 
TSP_SSI_initialize_request_full(TSP_sample_symbol_info_t* ssi,
				const char* name,
				int32_t type,
				int32_t dimension,
				int32_t offset,
				int32_t nelem,
				int32_t period,
				int32_t phase);

/**
 * Copy a Sample Symbol Information Structure.
 * @param[in,out] dst_ssi the (preallocated) sample symbol information structure. 
 *                The structure should be allocated but not the member of the structure.
 *                On entry should  be a non NULL pointer to  TSP_sample_symbol_info_t.
 *                On return contains the .
 * @param[in]     src_ssi 
 */
int32_t
TSP_SSI_copy(TSP_sample_symbol_info_t* dst_ssi, 
	     const TSP_sample_symbol_info_t src_ssi);


/**
 * Allocate a list of sample symbol information and initialize it.
 * @param[out] ssil pointer to the list to be create.
 * @param[in]  nbSSI the number of SSI in the to be created list.
 * @return TSP_STATUS_OK if allocation is OK. TSP_STATUS_ALLOC_FAILED otherwise.
 */
TSP_sample_symbol_info_list_t*
TSP_SSIList_new(int32_t nbSSI);

int32_t
TSP_SSIList_delete(TSP_sample_symbol_info_list_t** ssi);

/**
 * Initialize a preallocated list of sample symbol information.
 * @param[in] ssil the list to be initialized.
 */
int32_t 
TSP_SSIList_initialize(TSP_sample_symbol_info_list_t* ssil,
		       int32_t nbSSI);

int32_t 
TSP_SSIList_finalize(TSP_sample_symbol_info_list_t* ssil);

/**
 * Copy a list of symbols from src_symbols to dst_ssil.
 * @param[out] dst_ssil the symbols list copy destination
 * @param[in] src_ssil the symbols list copy source
 */
int32_t
TSP_SSIList_copy(TSP_sample_symbol_info_list_t* dst_ssil, 
		 const TSP_sample_symbol_info_list_t  src_ssil);

/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_SSI_H */
