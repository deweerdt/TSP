/*

$Id: tsp_common_ssi.c,v 1.2 2006-04-12 13:06:10 erk Exp $

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
#include <tsp_common_ssi.h>

void
TSP_common_SSI_copy(TSP_sample_symbol_info_t* dest_symbol, 
		    TSP_sample_symbol_info_t src_symbol)
{
  if (NULL==dest_symbol) {
    return;
  }	
  /* brut memory copy */
  memcpy(dest_symbol,&src_symbol,sizeof(TSP_sample_symbol_info_t));
  /* then strdup string member */
  dest_symbol->name = strdup(src_symbol.name);
  
} /* end of TSP_common_SSI_copy */

void 
TSP_common_SSI_initialize(TSP_sample_symbol_info_t* ssi) {
  
  if (NULL==ssi) {
    STRACE_ERROR(("Trying to initialize NULL pointer ssi"));
    return;
  }
  
  memset(ssi,0,sizeof(ssi));
  ssi->name                   = NULL;
  ssi->provider_global_index  = 0;
  ssi->provider_group_index   = 0;
  ssi->provider_group_rank    = 0;
  ssi->type                   = TSP_TYPE_DOUBLE;
  ssi->dimension              = 1;
  ssi->offset                 = 0;
  ssi->nelem                  = 0;
  ssi->period                 = 1;
  ssi->phase                  = 0;
} /* end of TSP_common_SSI_initialize */

int32_t 
TSP_common_SSIList_create(TSP_sample_symbol_info_list_t* ssil, int32_t nbSSI) {
  int32_t retcode = TSP_STATUS_ERROR_UNKNOWN;

  assert(ssil);
  ssil->TSP_sample_symbol_info_list_t_len = nbSSI;
  ssil->TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*) calloc (nbSSI,sizeof(TSP_sample_symbol_info_t));
  assert(ssil->TSP_sample_symbol_info_list_t_val);
  TSP_common_SSIList_initialize(ssil);
  retcode = TSP_STATUS_OK;
  return retcode;
} /* end of TSP_common_SSIList_initialize */

void 
TSP_common_SSIList_initialize(TSP_sample_symbol_info_list_t* ssil) {

  int32_t i;
  assert(ssil);
  assert(ssil->TSP_sample_symbol_info_list_t_val);
  for (i=0;i<ssil->TSP_sample_symbol_info_list_t_len;++i) {
    TSP_common_SSI_initialize(&(ssil->TSP_sample_symbol_info_list_t_val[i]));
  }
} /* end of TSP_common_SSIList_initialize */

void
TSP_common_SSIList_copy(TSP_sample_symbol_info_list_t* dest_symbols, 
			TSP_sample_symbol_info_list_t  src_symbols) {
  int i;

  if (NULL==dest_symbols) {
    return;
  }

  if (0 == src_symbols.TSP_sample_symbol_info_list_t_len) {
    /* quick return nothing to copy from */
    return;
  }

  if (0 != dest_symbols->TSP_sample_symbol_info_list_t_len) {
    dest_symbols->TSP_sample_symbol_info_list_t_len = src_symbols.TSP_sample_symbol_info_list_t_len;
    dest_symbols->TSP_sample_symbol_info_list_t_val = 
      malloc(sizeof(TSP_sample_symbol_info_t)*(dest_symbols->TSP_sample_symbol_info_list_t_len));
    TSP_CHECK_ALLOC(dest_symbols->TSP_sample_symbol_info_list_t_val, );
    /* loop over symbol_info to copy */
    for (i=0;i<dest_symbols->TSP_sample_symbol_info_list_t_len;++i) {
      TSP_common_SSI_copy(&(dest_symbols->TSP_sample_symbol_info_list_t_val[i]),
			  src_symbols.TSP_sample_symbol_info_list_t_val[i]);
    }
  }
} /* end of TSP_common_SSIList_copy */
