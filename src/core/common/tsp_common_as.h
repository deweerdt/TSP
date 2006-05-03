/*

$Id: tsp_common_as.h,v 1.1 2006-05-03 21:13:34 erk Exp $

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

#ifndef _TSP_COMMON_AS_H
#define _TSP_COMMON_AS_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @defgroup TSP_CommonLib_SSI Answer Sample Library Handling
 * The Answer Sample  Handling part of the @ref TSP_CommonLib
 * regroup a set of functions which should be used to manipulate
 * the Answer Sample object.
 * @ingroup TSP_CommonLib
 * @{
 */

#ifdef TSP_COMMON_AS_C
#else
#endif

BEGIN_C_DECLS

/**
 * Create an Answer Sample.
 * @param[in] version_id the TSP protocol version identifier
 * @param[in] channel_id
 * @param[in] p_timeout
 * @param[in] pgn 
 * @param[in] base_frequency 
 * @param[in] max_period 
 * @param[in] max_consumer_number
 * @param[in] current_consumer_number 
 * @param[in] status
 * @return the new TSP_answer_sample_t
 */
TSP_answer_sample_t*
TSP_AS_new(int32_t version_id,
	   uint32_t channel_id,
	   int32_t p_timeout,
	   int32_t pgn,
	   double base_frequency,
	   int32_t max_period,
	   int32_t max_consumer_number,
	   int32_t current_consumer_number,
	   TSP_status_t status);

/**
 * Delete an Answer Sample.
 * @param[in] as  pointer to pointer to the Answer Sample to be destroy
 * return  TSP_STATUS_OK if ok
 */
int32_t
TSP_AS_delete(TSP_answer_sample_t** as);

/**
 * Initialize an Answer Sample.
 * @param[in,out] ssi  the Answer Sample to initialize
 * @param[in] version_id the TSP protocol version identifier
 * @param[in] channel_id
 * @param[in] p_timeout
 * @param[in] pgn 
 * @param[in] base_frequency 
 * @param[in] max_period 
 * @param[in] max_consumer_number
 * @param[in] current_consumer_number 
 * @param[in] status
 * return  TSP_STATUS_OK if ok
 */
int32_t
TSP_AS_initialize(TSP_answer_sample_t* as,
		  int32_t version_id,
		  uint32_t channel_id,
		  int32_t p_timeout,
		  int32_t pgn,
		  double base_frequency,
		  int32_t max_period,
		  int32_t max_consumer_number,
		  int32_t current_consumer_number,
		  TSP_status_t status);

/**
 * Destroy the content of an Answer Sample.
 * @param[in,out] as the Answer Sample whose content should be destroyed.
 * return  TSP_STATUS_OK if ok
 */
int32_t
TSP_AS_finalize(TSP_answer_sample_t* as);

/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_AS_H */
