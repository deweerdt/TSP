/*

$Id: tsp_common_ssi.c,v 1.9 2008-02-05 18:54:09 rhdv Exp $

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
#include <tsp_common_trace.h>
#include <tsp_const_def.h>
#define TSP_COMMON_SSI_C
#include <tsp_common_ssi.h>

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
	    int32_t phase) {
  TSP_sample_symbol_info_t* ssi;

  ssi = calloc(1,sizeof(TSP_sample_symbol_info_t));

  if(NULL!=ssi)
  {
    TSP_SSI_initialize(ssi,
		       name,
		       pgi,pgridx,pgrank,
		       type,
		       dimension,offset,nelem,
		       period,phase);
  }
  return ssi;
} /* end of TSP_SSI_new */

int32_t
TSP_SSI_delete(TSP_sample_symbol_info_t** ssi) {
  int32_t retcode;

  assert(ssi);

  retcode=TSP_STATUS_OK;

  if(NULL!=*ssi) {
    retcode=TSP_SSI_finalize(*ssi);
    if (TSP_STATUS_OK!=retcode){
      return retcode;
    }
    
    free(*ssi);
    *ssi=NULL;
  }
  return retcode;
} /* end of TSP_SSI_delete */

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
		   int32_t phase) {
  
  int32_t retcode = TSP_STATUS_OK;
  assert(ssi);  

  if (NULL!=ssi->name) {
    free(ssi->name);    
  }
  if (NULL!=name) {
    ssi->name                 = strdup(name);
    if (NULL==ssi->name) {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR("Cannot strdup <%s>",name);
    }
  } else {
    ssi->name                 = NULL;
  }
  ssi->provider_global_index  = pgi;
  ssi->provider_group_index   = pgridx;
  ssi->provider_group_rank    = pgrank;
  ssi->type                   = type;
  ssi->dimension              = dimension;
  ssi->offset                 = offset;
  ssi->nelem                  = nelem;
  ssi->period                 = period;
  ssi->phase                  = phase;

  return retcode;
} /* end of TSP_SSI_initialize */

int32_t
TSP_SSI_finalize(TSP_sample_symbol_info_t* ssi) {
  
  int32_t retcode = TSP_STATUS_OK;
  assert(ssi);

  TSP_SSI_initialize(ssi,
		     NULL,
		     -1,-1,-1,
		     -1,
		     -1,-1,-1,
		     -1,-1);
  
  return retcode;
} /* end of retcode */

int32_t 
TSP_SSI_initialize_default(TSP_sample_symbol_info_t* ssi) {
  
  return TSP_SSI_initialize(ssi,
			    NULL,
			    0,0,0,
			    TSP_TYPE_DOUBLE,
			    1,0,1,
			    0,0);
		     
} /* end of TSP_SSI_initialize_default */

int32_t 
TSP_SSI_initialize_request_minimal(TSP_sample_symbol_info_t* ssi,
				   const char* name,
				   int32_t period) {
  assert(name);
  return TSP_SSI_initialize(ssi,
			    name,
			    0,0,0,
			    TSP_TYPE_DOUBLE,
			    0,0,0,
			    period,0);
} /* TSP_SSI_initialize_request_minimal */

int32_t 
TSP_SSI_initialize_request_full(TSP_sample_symbol_info_t* ssi,
				const char* name,
				int32_t type,
				int32_t dimension,
				int32_t offset,
				int32_t nelem,
				int32_t period,
				int32_t phase) {
  assert(name);
  return TSP_SSI_initialize(ssi,
			    name,
			    0,0,0,
			    type,
			    dimension,offset,nelem,
			    period,phase);
} /* TSP_SSI_initialize_request_full */

int32_t
TSP_SSI_copy(TSP_sample_symbol_info_t* dst_ssi, 
	     const TSP_sample_symbol_info_t src_ssi) {
  
  int32_t retcode = TSP_STATUS_OK;

  assert(dst_ssi);
   
  /* free name if not null */
  if (NULL!=dst_ssi->name) { 
    free(dst_ssi->name);
  }
  /* Raw memory copy (should be faster than calling initialize) */
  memcpy(dst_ssi,&src_ssi,sizeof(TSP_sample_symbol_info_t));
  /* Then strdup string (name) member */
  if (NULL!=src_ssi.name) {
    dst_ssi->name = strdup(src_ssi.name);
  } else {
    dst_ssi->name = NULL;
  }

  return retcode;
} /* end of TSP_SSI_copy */

TSP_sample_symbol_info_list_t*
TSP_SSIList_new(int32_t nbSSI) {
 
  TSP_sample_symbol_info_list_t* ssil;

  ssil = (TSP_sample_symbol_info_list_t*) calloc(1,sizeof(TSP_sample_symbol_info_list_t));
  if (NULL!=ssil) {
    TSP_SSIList_initialize(ssil,nbSSI);
  }

  return ssil;
} /* end of TSP_SSIList_new */

int32_t
TSP_SSIList_delete(TSP_sample_symbol_info_list_t** ssil) {
  int32_t retcode;

  assert(ssil);

  retcode=TSP_STATUS_OK;

  if (NULL!=*ssil) {
    retcode=TSP_SSIList_finalize(*ssil);
    free(*ssil);
    *ssil=NULL;
  }

  return retcode;
} /* end of TSP_SSI_delete */

int32_t
TSP_SSIList_initialize(TSP_sample_symbol_info_list_t* ssil,int32_t nbSSI) {

  int32_t retcode = TSP_STATUS_OK;
  int32_t i;
  assert(ssil);
  
  ssil->TSP_sample_symbol_info_list_t_len = nbSSI;
  ssil->TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*) calloc (nbSSI,sizeof(TSP_sample_symbol_info_t));
  TSP_CHECK_ALLOC(ssil->TSP_sample_symbol_info_list_t_val,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

  for (i=0;i<ssil->TSP_sample_symbol_info_list_t_len;++i) {    
    retcode=TSP_SSI_initialize_default(&(ssil->TSP_sample_symbol_info_list_t_val[i]));
    /* give-up on first error */
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }
  return retcode;
} /* end of TSP_SSIList_initialize */

int32_t 
TSP_SSIList_finalize(TSP_sample_symbol_info_list_t* ssil) {
  int32_t retcode = TSP_STATUS_OK;
  int32_t i;

  assert(ssil);

  for (i=0;i<ssil->TSP_sample_symbol_info_list_t_len;++i) {
    retcode=TSP_SSI_finalize(&(ssil->TSP_sample_symbol_info_list_t_val[i]));
    /* Give-up on first error */
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }

  if (TSP_STATUS_OK==retcode) {
    free(ssil->TSP_sample_symbol_info_list_t_val);

    ssil->TSP_sample_symbol_info_list_t_val = NULL;
    ssil->TSP_sample_symbol_info_list_t_len = 0;
  }

  return retcode;
} /* end of TSP_SSIList_finalize */


int32_t
TSP_SSIList_copy(TSP_sample_symbol_info_list_t* dst_ssil, 
		 const TSP_sample_symbol_info_list_t  src_ssil) {
  int32_t retcode = TSP_STATUS_OK;
  int i;

  assert(dst_ssil);

  if (0 == src_ssil.TSP_sample_symbol_info_list_t_len) {
    /* Quick return nothing to copy from */
    return retcode;
  }

  TSP_SSIList_finalize(dst_ssil);
  TSP_SSIList_initialize(dst_ssil,src_ssil.TSP_sample_symbol_info_list_t_len);

  /* loop over symbol_info to copy */
  for (i=0;i<dst_ssil->TSP_sample_symbol_info_list_t_len;++i) {
    retcode= TSP_SSI_copy(&(dst_ssil->TSP_sample_symbol_info_list_t_val[i]),
			  src_ssil.TSP_sample_symbol_info_list_t_val[i]);
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }

  return retcode;
} /* end of TSP_SSIList_copy */

TSP_sample_symbol_info_t*
TSP_SSIList_getSSI(TSP_sample_symbol_info_list_t ssil, uint32_t index) {
  if ((index >= 0) &&
      (index < ssil.TSP_sample_symbol_info_list_t_len)) {
    return &(ssil.TSP_sample_symbol_info_list_t_val[index]);
  } else {
    return NULL;
  }
} /* end of TSP_SSIList_getSSI */

uint32_t
TSP_SSIList_getSize(const TSP_sample_symbol_info_list_t ssil) {
  return ssil.TSP_sample_symbol_info_list_t_len;
}
