/*

$Id: tsp_common_status.c,v 1.3 2007-04-26 17:51:30 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MER%CHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
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
#define TSP_COMMON_STATUS_C
#include <tsp_common_status.h>
#ifdef WIN32
    #include <stdio.h>
    #define snprintf	(int) _snprintf
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif

#define TSP_STATUS_MSG_MAX_LENGTH 1024

int32_t
TSP_STATUS_print(FILE* fs, int32_t TSPStatus) {
  char statusMsg[TSP_STATUS_MSG_MAX_LENGTH];
  int32_t retcode = 0;

  retcode = TSP_STATUS_sprint(statusMsg,TSP_STATUS_MSG_MAX_LENGTH,TSPStatus);
  if (0==retcode) {
    retcode = fprintf(fs,"%s",statusMsg);
  }

  return retcode;
} /* end of TSP_STATUS_print */

int32_t
TSP_STATUS_sprint(char* buffer, int32_t buffer_len, int32_t TSPStatus) {
  int32_t retcode = 0;
  switch (TSPStatus) {
  case TSP_STATUS_OK:
    retcode = snprintf(buffer,buffer_len,"OK");
    break;
  case TSP_STATUS_NOK:
    retcode = snprintf(buffer,buffer_len,"NOK (generic error)");
    break;
  case TSP_STATUS_ERROR_PROVIDER_UNREACHABLE:
    retcode = snprintf(buffer,buffer_len,"TSP Provider unreachable or connection lost");
    break;
  case TSP_STATUS_ERROR_UNKNOWN:
    retcode = snprintf(buffer,buffer_len,"Unknow Error");
    break;
  case TSP_STATUS_ERROR_SEE_STRING:
    retcode = snprintf(buffer,buffer_len,"See Error String");
    break;
  case TSP_STATUS_ERROR_VERSION:
    retcode = snprintf(buffer,buffer_len,"TSP Version Mismatch");
    break;
  case TSP_STATUS_ERROR_SYMBOLS:
    retcode = snprintf(buffer,buffer_len,"Requested TSP Symbols may not be satisfied");
    break;
  case TSP_STATUS_ERROR_SYMBOL_FILTER:
    retcode = snprintf(buffer,buffer_len,"Filter string or filter kind is invalid (TSP_REQUEST_FILTERED)");
    break;
  case TSP_STATUS_ERROR_NOT_SUPPORTED:
    retcode = snprintf(buffer,buffer_len,"Requested feature Not Supported (provider specific)");
    break;
  case TSP_STATUS_ERROR_NOT_IMPLEMENTED:
    retcode = snprintf(buffer,buffer_len,"Requested feature Not Implemented (provider specific)");
    break;
  case TSP_STATUS_ERROR_PGI_UNKNOWN:
    retcode = snprintf(buffer,buffer_len,"Requested PGI does not exists");
    break;
  case TSP_STATUS_ERROR_ASYNC_READ_NOT_ALLOWED:
    retcode = snprintf(buffer,buffer_len,"ASYNC Read not allowed (for this PGI/Provider)");
    break;
  case TSP_STATUS_ERROR_ASYNC_WRITE_NOT_ALLOWED:
    retcode = snprintf(buffer,buffer_len,"ASYNC Write not allowed (for this PGI/Provider)");
    break;
  case TSP_STATUS_ERROR_ASYNC_READ_NOT_SUPPORTED:
    retcode = snprintf(buffer,buffer_len,"ASYNC Read not supported by this Provider");
    break;
  case TSP_STATUS_ERROR_ASYNC_WRITE_NOT_SUPPORTED:
    retcode = snprintf(buffer,buffer_len,"ASYNC Write not supported by this Provider");
    break;
  case TSP_STATUS_ERROR_MEMORY_ALLOCATION:
    retcode = snprintf(buffer,buffer_len,"MEMORY allocation failed");
    break;
  case TSP_STATUS_ERROR_INVALID_CHANNEL_ID:
    retcode = snprintf(buffer,buffer_len,"Channel Id is invalid");
    break;
  case TSP_STATUS_ERROR_NO_MORE_GLU:
    retcode = snprintf(buffer,buffer_len,"Cannot instantiate more GLU (provider-side)");
    break;
  case TSP_STATUS_ERROR_NO_MORE_SESSION:
    retcode = snprintf(buffer,buffer_len,"Maximum number of TSP session reached");
    break;
  case TSP_STATUS_ERROR_GLU_START:
    retcode = snprintf(buffer,buffer_len,"Cannot start GLU (provider-side)");
    break;
  case TSP_STATUS_ERROR_GLU_INITIALIZE:
    retcode = snprintf(buffer,buffer_len,"Cannot initialize GLU (provider-side)");
    break;
  case TSP_STATUS_ERROR_BAD_REQUEST_ORDER:
    retcode = snprintf(buffer,buffer_len,"Bad TSP request ordering");
    break;
  case TSP_STATUS_ERROR_DATAPOOL_INSTANTIATE:
    retcode = snprintf(buffer,buffer_len,"Cannot instantiate Datapool (provider-side)");
    break;
  case TSP_STATUS_ERROR_THREAD_CREATE:
    retcode = snprintf(buffer,buffer_len,"Cannot create thread");
    break;
  case TSP_STATUS_ERROR_NOT_INITIALIZED:
    retcode = snprintf(buffer,buffer_len,"TSP not initialized");
    break;
  case TSP_STATUS_ERROR_INVALID_REQUEST:
    retcode = snprintf(buffer,buffer_len,"TSP request content is invalid");
    break;
  case TSP_STATUS_ERROR_EMPTY_REQUEST_SAMPLE:
    retcode = snprintf(buffer,buffer_len,"0 symbol request sample");
    break;
  case TSP_STATUS_ERROR_CUSTOM_BEGIN:
    /* 
     * value greater than TSP_STATUS_ERROR_CUSTOM_BEGIN are custom TSP STATUS
     * values that may be used by specific consumer or provider
     */
  default:
    retcode = snprintf(buffer,buffer_len,"TSP unknown/unhandled/custom STATUS status <%d>",TSPStatus);
    break;
  }
  return retcode;
} /* end of TSP_STATUS_sprint */
