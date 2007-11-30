/*

$Id: tsp_common_request.c,v 1.3 2007-11-30 15:42:01 erk Exp $

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
#define TSP_COMMON_REQUEST_C
#include <tsp_common_request.h>

#ifdef _WIN32
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif

void TSP_request_create(TSP_request_t* req, TSP_request_type_t req_type) {
  assert(req);
  req->version_id = TSP_PROTOCOL_VERSION;
  req->req_type   = req_type;
  req->req_data   = NULL;
} /* end of TSP_request_create */
