/*

$Id: tsp_common_ssei.c,v 1.1 2006-04-07 09:30:36 erk Exp $

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
#define TSP_COMMON_SSI_C
#include <tsp_common_ssei.h>

int32_t
TSP_ei_initialize(TSP_extended_info_t* ei, const char* key, const char* value) {
  assert(ei);  
  ei->key   = strdup(key);
  ei->value = strdup(value);
  return TSP_STATUS_OK;
} /* end of TSP_ei_initialize */

int32_t
TSP_ei_finalize(TSP_extended_info_t* ei, const char* key, const char* value) {
  assert(ei);  
  free(ei->key); 
  ei->key = NULL;
  free(ei->value); 
  ei->value = NULL;
  return TSP_STATUS_OK;
} /* end of TSP_ei_initialize */


