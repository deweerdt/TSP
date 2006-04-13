/*

$Id: tsp_common_ssei.c,v 1.4 2006-04-13 21:22:46 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD,Robert PAGNOT and Arnaud MORVAN

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
#define TSP_COMMON_SSEI_C
#include <tsp_common_ssei.h>

/* function to use thsi struct
struct TSP_extended_info_t {
	char *key;
	char *value;
};
*/

int32_t
TSP_EI_initialize(TSP_extended_info_t* ei, const char* key, const char* value) {

  assert(ei);  
  
  if(NULL!=key)
     ei->key = strdup(key);

  if(NULL!=value)
    ei->value = strdup(value);

  return TSP_STATUS_OK;
} /* end of TSP_EI_initialize */

int32_t
TSP_EI_update(TSP_extended_info_t* ei, const char* key, const char* value){

  assert(ei); 

  if(NULL!=ei->key)
  {
    free(ei->key); 
    ei->key = NULL;
  }

  if(NULL!=ei->value)
  {
    free(ei->value); 
    ei->value = NULL;
  }

  if(NULL!=key)
    ei->key   = strdup(key);

  if(NULL!=value)
    ei->value = strdup(value);

  return TSP_STATUS_OK;
}

int32_t
TSP_EI_copy(TSP_extended_info_t* dest_EI, const TSP_extended_info_t src_EI){

  if (NULL==dest_EI) {
    return TSP_STATUS_ERROR_UNKNOWN;
  }	

  /* brut memory copy */
  memcpy(dest_EI,&src_EI,sizeof(TSP_extended_info_t));

  /* then strdup the member */
  if(NULL!=src_EI.key)
    dest_EI->key = strdup(src_EI.key);

  if(NULL!=src_EI.value)
    dest_EI->value = strdup(src_EI.value);

  return TSP_STATUS_OK;
}

int32_t
TSP_EI_finalize(TSP_extended_info_t* ei) {

  assert(ei);

  if(NULL!=ei->key)
  {
    free(ei->key); 
    ei->key = NULL;
  }

  if(NULL!=ei->value)
  {
    free(ei->value); 
    ei->value = NULL;
  }

  return TSP_STATUS_OK;
} /* end of TSP_EI_initialize */



/* function to use this struc
typedef struct {
	u_int TSP_extended_info_list_t_len;
	TSP_extended_info_t *TSP_extended_info_list_t_val;
} TSP_extended_info_list_t;
*/

int32_t
TSP_EIList_initialize(TSP_extended_info_list_t* eil, const int32_t len){
  int32_t i;
  
  assert(eil);

  eil->TSP_extended_info_list_t_val = malloc(sizeof(TSP_extended_info_t)*(len));
  eil->TSP_extended_info_list_t_len=len;

  TSP_CHECK_ALLOC(eil->TSP_extended_info_list_t_val,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

  for (i=0;i<len;++i) {
    TSP_EI_initialize(&(eil->TSP_extended_info_list_t_val[i]),"","");
  }
  
  return TSP_STATUS_OK;
}

const TSP_extended_info_t*
TSP_EIList_findEIByKey(const TSP_extended_info_list_t* eil, const char* key){

  int32_t i;

  for (i=0;i<eil->TSP_extended_info_list_t_len;++i)
  {
    if(!strcmp(eil->TSP_extended_info_list_t_val[i].key,key))
    {
      return &(eil->TSP_extended_info_list_t_val[i]);
    }
  }
  return NULL;

}

int32_t
TSP_EIList_copy(TSP_extended_info_list_t* eil_dest,
		const TSP_extended_info_list_t eil_src){
  int i;

  if (NULL==eil_dest) {
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  if (0 == eil_src.TSP_extended_info_list_t_len) {
    /* quick return nothing to copy from */
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  if (0 != eil_dest->TSP_extended_info_list_t_len) {

    eil_dest->TSP_extended_info_list_t_len = eil_src.TSP_extended_info_list_t_len;

    eil_dest->TSP_extended_info_list_t_val = 
      malloc(sizeof(TSP_extended_info_t)*(eil_dest->TSP_extended_info_list_t_len));

    TSP_CHECK_ALLOC(eil_dest->TSP_extended_info_list_t_val,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

    /* loop over symbol_info to copy */
    for (i=0;i<eil_dest->TSP_extended_info_list_t_len;++i) {
      TSP_EI_copy(&(eil_dest->TSP_extended_info_list_t_val[i]),
			  eil_src.TSP_extended_info_list_t_val[i]);
    }
  }

  return TSP_STATUS_OK;
}

int32_t
TSP_EIList_finalize(TSP_extended_info_list_t* eil){
  
  assert(eil);

  int32_t i;

  for (i=0;i<eil->TSP_extended_info_list_t_len;++i) {
    TSP_EI_finalize(&(eil->TSP_extended_info_list_t_val[i]));

  }

  eil->TSP_extended_info_list_t_len=0;
  free(eil->TSP_extended_info_list_t_val);

  return TSP_STATUS_OK;

}





/* function to use thus struct
struct TSP_sample_symbol_extended_info_t {
	int provider_global_index;
	TSP_extended_info_list_t info;
};
typedef struct TSP_sample_symbol_extended_info_t
*/

int32_t
TSP_SSEI_initialize(TSP_sample_symbol_extended_info_t* ssei, const int32_t pgi, const int32_t nei) {
  
  assert(ssei);

  ssei->provider_global_index=pgi;

  TSP_EIList_initialize(&(ssei->info), nei);

  return TSP_STATUS_OK;

}


int32_t
TSP_SSEI_finalize(TSP_sample_symbol_extended_info_t* ssei){
  assert(ssei);

  ssei->provider_global_index=-1;

  TSP_EIList_finalize(&(ssei->info));

  return TSP_STATUS_OK;

}

int32_t
TSP_SSEI_copy(TSP_sample_symbol_extended_info_t* ssei_dest,
  	      const TSP_sample_symbol_extended_info_t ssei_src){

  if (NULL==ssei_dest) {
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  ssei_dest->provider_global_index=ssei_src.provider_global_index;

  TSP_EIList_copy( &(ssei_dest->info),ssei_src.info);
 
  return TSP_STATUS_OK;
}




/* function to use this struct
typedef struct {
	u_int TSP_sample_symbol_extended_info_list_t_len;
	TSP_sample_symbol_extended_info_t   *TSP_sample_symbol_extended_info_list_t_val;
}TSP_sample_symbol_extended_info_list_t;
*/

int32_t
TSP_SSEIList_create(TSP_sample_symbol_extended_info_list_t** ssei_list){

    *ssei_list = malloc(sizeof(TSP_sample_symbol_extended_info_list_t));

    TSP_CHECK_ALLOC(ssei_list,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

    TSP_SSEIList_initialize(*ssei_list,0);

    return TSP_STATUS_OK;
}

int32_t
TSP_SSEIList_initialize(TSP_sample_symbol_extended_info_list_t* ssei_list,int32_t len){

  assert(ssei_list);

  ssei_list->TSP_sample_symbol_extended_info_list_t_len=len;

  ssei_list->TSP_sample_symbol_extended_info_list_t_val=malloc(sizeof(TSP_sample_symbol_extended_info_t)*len);

  return TSP_STATUS_OK;

}

int32_t
TSP_SSEIList_finalize(TSP_sample_symbol_extended_info_list_t* ssei_list){
  
  assert(ssei_list);

  TSP_SSEI_finalize(ssei_list->TSP_sample_symbol_extended_info_list_t_val);

  free(ssei_list->TSP_sample_symbol_extended_info_list_t_val);
  free(ssei_list);

  return TSP_STATUS_OK;
}


int32_t
TSP_SSEIList_copy(TSP_sample_symbol_extended_info_list_t* dest_ssei_list,
                  const TSP_sample_symbol_extended_info_list_t src_ssei_list){

  int i;

  if (NULL==dest_ssei_list) {
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  if (0 == src_ssei_list.TSP_sample_symbol_extended_info_list_t_len) {
    /* quick return nothing to copy from */
    return TSP_STATUS_ERROR_UNKNOWN;
  }

  if (0 != dest_ssei_list->TSP_sample_symbol_extended_info_list_t_len) {

    dest_ssei_list->TSP_sample_symbol_extended_info_list_t_len=src_ssei_list.TSP_sample_symbol_extended_info_list_t_len;

    dest_ssei_list->TSP_sample_symbol_extended_info_list_t_val = 
      malloc(sizeof(TSP_sample_symbol_extended_info_t)*(dest_ssei_list->TSP_sample_symbol_extended_info_list_t_len));

    TSP_CHECK_ALLOC(dest_ssei_list->TSP_sample_symbol_extended_info_list_t_val,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

    /* loop over symbol_info to copy */
    for (i=0;i<dest_ssei_list->TSP_sample_symbol_extended_info_list_t_len;++i) {
      TSP_SSEI_copy(&(dest_ssei_list->TSP_sample_symbol_extended_info_list_t_val[i]),
			  src_ssei_list.TSP_sample_symbol_extended_info_list_t_val[i]);
    }
  }

  return TSP_STATUS_OK;


}





