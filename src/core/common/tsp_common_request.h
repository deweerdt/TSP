/*

$Id: tsp_common_request.h,v 1.3 2006-10-18 09:58:48 erk Exp $

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

#ifndef _TSP_COMMON_REQUEST_H
#define _TSP_COMMON_REQUEST_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @define TSP_CommonLib_Request Generic Request Handling
 * The generic request handling API.
 * @ingroup TSP_CommonLib
 * @{
 */

/**
 * The list of request type.
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
  E_TSP_REQUEST_EXTENDED_INFORMATION,
  E_TSP_REQUEST_LAST
} TSP_request_type_t;

#ifdef TSP_COMMON_REQUEST_C
_EXPORT_TSP_COMMON const char* tsp_reqname_tab[] = {"tsp_request_invalid",
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
				 "tsp_request_extended_information",
				 "tsp_request_last"
};

_EXPORT_TSP_COMMON const char* tsp_reqhelp_tab[] = {"invalid tsp request",
				 "Generic interface to TSP request",
				 "Send a TSP request open then close",
				 "Same as request_open",
				 "Get TSP symbols information from provider using Request Information",
				 "Get TSP symbols information from provider using Request Filtered Information",
				 "tsp_request_feature (not implemented)",
				 "tsp_request_sample",
				 "tsp_request_sample_init",
				 "tsp_request_sample_destroy",
				 "Send TSP Asynchronous write request",
				 "Send TSP Asynchronous read request",
				 "tsp_request_extended_information",
				 "TSP LAST REQUEST"
};
#else
_EXTERN_TSP_COMMON const char* tsp_reqname_tab[];
_EXTERN_TSP_COMMON const char* tsp_reqhelp_tab[];
#endif

typedef struct TSP_request {
  int                 version_id;
  TSP_request_type_t    req_type;
  void*                 req_data;
} TSP_request_t;

BEGIN_C_DECLS

/**
 * Initialize a TSP request.
 * @param req 
 * @param req_type
 */
_EXPORT_TSP_COMMON void TSP_request_create(TSP_request_t* req, TSP_request_type_t req_type);


/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_REQUEST_H */
