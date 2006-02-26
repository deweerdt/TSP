/*

$Id: tsp_common.c,v 1.4 2006-02-26 13:36:05 erk Exp $

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

#include "tsp_sys_headers.h"
#include "tsp_abs_types.h"
#include "tsp_datastruct.h"
#include "tsp_simple_trace.h"
#include "tsp_const_def.h"
#define TSP_COMMON_C
#include "tsp_common.h"

void TSP_request_create(TSP_request_t* req, TSP_request_type_t req_type) {
  assert(req);
  req->version_id = TSP_PROTOCOL_VERSION;
  req->req_type   = req_type;
  req->req_data   = NULL;
}

void
TSP_common_sample_symbol_info_list_copy(TSP_sample_symbol_info_list_t* dest_symbols, 
					TSP_sample_symbol_info_list_t  src_symbols)
{
  int i;
  STRACE_IO(("-->IN"));

  if (NULL==dest_symbols) {
    return;
  }

  if (0 == src_symbols.TSP_sample_symbol_info_list_t_len) {
    /* quick return nothing to copy from */
    return;
  }

  if (0 == dest_symbols->TSP_sample_symbol_info_list_t_len) {
    dest_symbols->TSP_sample_symbol_info_list_t_len = src_symbols.TSP_sample_symbol_info_list_t_len;
    dest_symbols->TSP_sample_symbol_info_list_t_val = 
      malloc(sizeof(TSP_sample_symbol_info_t)*(dest_symbols->TSP_sample_symbol_info_list_t_len));
    TSP_CHECK_ALLOC(dest_symbols->TSP_sample_symbol_info_list_t_val, );
    /* loop over symbol_info to copy */
    for (i=0;i<dest_symbols->TSP_sample_symbol_info_list_t_len;++i) {
      TSP_common_sample_symbol_copy(&(dest_symbols->TSP_sample_symbol_info_list_t_val[i]),
				    src_symbols.TSP_sample_symbol_info_list_t_val[i]);
    }
  }
  STRACE_IO(("-->OUT"));
}

void
TSP_common_sample_symbol_copy(TSP_sample_symbol_info_t* dest_symbol, 
			      TSP_sample_symbol_info_t src_symbol)
{
  STRACE_IO(("-->IN"));
  if (NULL==dest_symbol) {
    return;
  }	
  /* brut memory copy */
  memcpy(dest_symbol,&src_symbol,sizeof(TSP_sample_symbol_info_t));
  /* then strdup string member */
  dest_symbol->name = strdup(src_symbol.name);
  
  STRACE_IO(("-->OUT"));
}
