/*

$Id: tsp_common.h,v 1.8 2006-04-05 08:10:31 erk Exp $

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

#ifndef _TSP_COMMON_H
#define _TSP_COMMON_H

#include "tsp_prjcfg.h"
#include "tsp_datastruct.h"

/**
 * @defgroup TSP_CommonLib TSP Common Library
 * The common librarie is the part of the TSP Core library
 * which is shared by provider and consumer.
 * It contains TSP datatypes definitions and helper functions.
 * @{
 */

typedef enum TSP_request_type {
  E_TSP_REQUEST_INVALID = 0,
  E_TSP_REQUEST_GENERIC,
  E_TSP_REQUEST_OPEN,
  E_TSP_REQUEST_CLOSE,
  E_TSP_REQUEST_INFORMATION,
  E_TSP_REQUEST_FILTERED_INFORMATION,
  E_TSP_REQUEST_FEATURE,
  E_TSP_REQUEST_SAMPLE,
  E_TSP_REQUEST_SAMPLE_INIT,
  E_TSP_REQUEST_SAMPLE_DESTROY,
  E_TSP_REQUEST_ASYNC_SAMPLE_WRITE,
  E_TSP_REQUEST_ASYNC_SAMPLE_READ,
  E_TSP_REQUEST_LAST
} TSP_request_type_t;

#ifdef TSP_COMMON_C
const char* tsp_reqname_tab[] = {"tsp_request_invalid",
				 "tsp_request_generic",
				 "tsp_request_open",
				 "tsp_request_close",
				 "tsp_request_information",
				 "tsp_request_filtered_information",
				 "tsp_request_feature",
				 "tsp_request_sample",
				 "tsp_request_sample_init",
				 "tsp_request_sample_destroy",
				 "tsp_request_async_sample_write",
				 "tsp_request_async_sample_read",
				 "tsp_request_last"
};

const char* tsp_reqhelp_tab[] = {"invalid tsp request",
				 "tsp_request_generic",
				 "tsp_request_open",
				 "tsp_request_close",
				 "tsp_request_information",
				 "tsp_request_filtered_information",
				 "tsp_request_feature",
				 "tsp_request_sample",
				 "tsp_request_sample_init",
				 "tsp_request_sample_destroy",
				 "tsp_request_async_sample_write",
				 "tsp_request_async_sample_read",
				 "TSP LAST REQUEST"
};
const int tsp_type_size[] = { 0,
			      sizeof(double),
			      sizeof(float),
			      sizeof(int8_t),
			      sizeof(int16_t),
			      sizeof(int32_t),
			      sizeof(int64_t),
			      sizeof(uint8_t),
			      sizeof(uint16_t),
			      sizeof(uint32_t),
			      sizeof(uint64_t),
			      sizeof(char),
			      sizeof(unsigned char),
			      sizeof(uint8_t),
			      0
				
};
#else
extern const char* tsp_reqname_tab[];
extern const char* tsp_reqhelp_tab[];
extern const int tsp_type_size[];
#endif

typedef struct TSP_request {
  int                 version_id;
  TSP_request_type_t    req_type;
  void*                 req_data;
} TSP_request_t;

/**
 * The filter kind used in 
 * TSP_request_filtered_information.
 * 
 */
typedef enum TSP_filter_kind {
  TSP_FILTER_NONE=0,
  TSP_FILTER_MINIMAL,
  TSP_FILTER_SIMPLE,
  TSP_FILTER_REGEX,
  TSP_FILTER_XPATH,
  TSP_FILTER_SQL,
  TSP_FILTER_LAST
} TSP_filter_kind_t;

#define MINIMAL_STRING           "minimal"
#define SIMPLE_CASE_SENSITIVE    "simple_withcase"
#define SIMPLE_CASE_INSENSITIVE  "simple_nocase"

BEGIN_C_DECLS

/**
 * Initialize a TSP request.
 * @param req 
 * @param req_type
 */
void TSP_request_create(TSP_request_t* req, TSP_request_type_t req_type);


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

#endif /* _TSP_COMMON_H */
