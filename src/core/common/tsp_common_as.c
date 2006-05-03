/*

$Id: tsp_common_as.c,v 1.1 2006-05-03 21:13:34 erk Exp $

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
Component  : Provider

-----------------------------------------------------------------------

Purpose   : Main implementation for the producer module

-----------------------------------------------------------------------
 */
#include <string.h>

#include <tsp_sys_headers.h>
#include <tsp_abs_types.h>
#include <tsp_datastruct.h>
#include <tsp_simple_trace.h>
#include <tsp_const_def.h>
#include <tsp_common_macros.h>
#define TSP_COMMON_AS_C
#include <tsp_common_as.h>
#include <tsp_common_ssi.h>

TSP_answer_sample_t*
TSP_AS_new(int32_t version_id,
	   uint32_t channel_id,
	   int32_t p_timeout,
	   int32_t pgn,
	   double base_frequency,
	   int32_t max_period,
	   int32_t max_consumer_number,
	   int32_t current_consumer_number,
	   TSP_status_t status) {
  
  TSP_answer_sample_t* as;
  
  as = calloc(1,sizeof(TSP_answer_sample_t));

  if (NULL!=as) {
    TSP_AS_initialize(as,
		      version_id,channel_id,
		      p_timeout,
		      pgn,
		      base_frequency,max_period,max_consumer_number,
		      current_consumer_number,
		      status);
  }
  return as;
} /* end of TSP_AS_new */

TSP_TYPE_DELETE_IMPLEMENT(TSP_answer_sample_t,AS)

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
		  TSP_status_t status) {
  assert(as);
  int32_t retcode = TSP_STATUS_OK;

  as->version_id             = version_id;
  as->channel_id             = channel_id;
  as->provider_timeout       = p_timeout;
  as->provider_group_number  = pgn;
  retcode = TSP_SSIList_finalize(&(as->symbols));    
  as->base_frequency         = base_frequency;
  as->max_period             = max_period;
  as->max_client_number      = max_consumer_number;
  as->current_client_number  = current_consumer_number;
  as->status                 = status;

  return retcode;
} /* end of TSP_AS_initialize */

int32_t
TSP_AS_finalize(TSP_answer_sample_t* as) {
  assert(as);
  int32_t retcode = TSP_STATUS_OK;
  
  TSP_AS_initialize(as,
		    TSP_UNDEFINED_VERSION_ID,
		    -1,-1,-1,
		    0.0,
		    -1,-1,-1,
		    TSP_STATUS_ERROR_UNKNOWN);
  
  return retcode;
} /* end of retcode */
