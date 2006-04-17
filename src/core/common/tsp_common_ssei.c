/*

$Id: tsp_common_ssei.c,v 1.7 2006-04-17 22:27:35 erk Exp $

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

TSP_extended_info_t*
TSP_EI_new(const char *key,const char *value) {
  TSP_extended_info_t* ei;
  
  ei = calloc(1,sizeof(TSP_extended_info_t));
 
  if(NULL!=ei)
    TSP_EI_initialize(ei,key,value);

  return ei;
}  /* end of TSP_EI_new */

int32_t
TSP_EI_delete(TSP_extended_info_t** ei) {
  int32_t ret;

  assert(ei); 

  ret=TSP_STATUS_OK;

  if(NULL==*ei)
  {

    ret=TSP_EI_finalize(*ei);
    free(*ei);
    *ei=NULL;
  }

  return ret;
} /* end of TSP_EI_delete */

int32_t
TSP_EI_initialize(TSP_extended_info_t* ei, const char* key, const char* value) {

  assert(ei);  
  
  if(NULL!=ei->key)
  {
    free(ei->key);
    ei->key=NULL;
  }

  if(NULL!=key)
     ei->key = strdup(key);

 if(NULL!=ei->value)
  {
    free(ei->value);
    ei->value=NULL;
  }

  if(NULL!=value)
    ei->value = strdup(value);

  return TSP_STATUS_OK;
} /* end of TSP_EI_initialize */

int32_t
TSP_EI_update(TSP_extended_info_t* ei, const char* key, const char* value){

  return TSP_EI_initialize(ei,key,value);

} /* end of TSP_EI_update */

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

} /* end of TSP_EI_finalize */

int32_t
TSP_EI_copy(TSP_extended_info_t* dest_EI, const TSP_extended_info_t src_EI){
  int32_t ret;

  assert(dest_EI);

  ret= TSP_STATUS_OK;

  
  ret=TSP_EI_finalize(dest_EI);
  
  if (TSP_STATUS_OK!=ret) {
    return ret; 
  }  

  ret=TSP_EI_initialize(dest_EI,src_EI.key, src_EI.value);

  return ret;
} /* end of TSP_EI_copy */


TSP_extended_info_list_t*
TSP_EIList_new(int32_t len) {

  TSP_extended_info_list_t* ei_list;

  ei_list = (TSP_extended_info_list_t*) calloc(1,sizeof(TSP_extended_info_list_t));
  if(NULL!=ei_list)
  {
    TSP_EIList_initialize(ei_list,len);
  }

  return ei_list;
} /* end TSP_EIList_new */

int32_t
TSP_EIList_delete(TSP_extended_info_list_t** eil) {
  int32_t ret;

  assert(eil);

  ret=TSP_STATUS_OK;

  if ( NULL!=*eil ) {
    ret=TSP_EIList_finalize(*eil);
    free(*eil);
    *eil=NULL;
  }
  return ret;
} /* end TSP_EIList_delete */


int32_t
TSP_EIList_initialize(TSP_extended_info_list_t* eil, const int32_t len){
  int32_t ret;
  int32_t i;

  assert(eil);

  ret=TSP_STATUS_OK;

  if(NULL!=eil->TSP_extended_info_list_t_val)
  {
    for(i=0;i<eil->TSP_extended_info_list_t_len;++i)
    {
       ret=TSP_EI_finalize(&(eil->TSP_extended_info_list_t_val[i]));
      
    }
  }

  eil->TSP_extended_info_list_t_len=len;
  eil->TSP_extended_info_list_t_val=calloc(len,sizeof(TSP_extended_info_t));
  
  if(NULL!=eil->TSP_extended_info_list_t_val)
  {
    for (i=0;i<len;++i) {
       ret=TSP_EI_initialize(&(eil->TSP_extended_info_list_t_val[i]),"","");
    }
  }
  else
  {
    ret=TSP_STATUS_ERROR_MEMORY_ALLOCATION;
  }
 
  return ret;
 
} /* end TSP_EIList_initialize */

int32_t
TSP_EIList_finalize(TSP_extended_info_list_t* eil) {
  int32_t ret;
  int32_t i;

  assert(eil);

  ret= TSP_STATUS_OK;

  for (i=0;i<eil->TSP_extended_info_list_t_len;++i) {
    ret=TSP_EI_finalize(&(eil->TSP_extended_info_list_t_val[i]));
  }

  eil->TSP_extended_info_list_t_len=0;
  eil->TSP_extended_info_list_t_val=NULL;

  return ret;

} /* end of TSP_EIList_finalize */

const TSP_extended_info_t*
TSP_EIList_findEIByKey(const TSP_extended_info_list_t* eil, const char* key) {

  int32_t i;

  for (i=0;i<eil->TSP_extended_info_list_t_len;++i)
  {
    if(!strcmp(eil->TSP_extended_info_list_t_val[i].key,key))
    {
      return &(eil->TSP_extended_info_list_t_val[i]);
    }
  }
  return NULL;
} /* end of TSP_EIList_findEIByKey */

int32_t
TSP_EIList_copy(TSP_extended_info_list_t* eil_dest,
		const TSP_extended_info_list_t eil_src){
  int32_t i;
  int32_t ret;

  assert(eil_dest);

  ret= TSP_STATUS_OK;
 
  ret=TSP_EIList_finalize(eil_dest);
  if(TSP_STATUS_OK!=ret){
    return ret;
  }

  ret=TSP_EIList_initialize(eil_dest,eil_src.TSP_extended_info_list_t_len);
  if(TSP_STATUS_OK!=ret){
    return ret;
  }

  /* loop over symbol_info to copy */
  for (i=0;i<eil_dest->TSP_extended_info_list_t_len;++i) {
      ret=TSP_EI_copy(&(eil_dest->TSP_extended_info_list_t_val[i]),
			  eil_src.TSP_extended_info_list_t_val[i]);
  }
  return ret;
} /* end of TSP_EIList_copy */

TSP_sample_symbol_extended_info_t*
TSP_SSEI_new(const int32_t pgi, const int32_t nei) {

  TSP_sample_symbol_extended_info_t* ssei;

  ssei = calloc(1,sizeof(TSP_sample_symbol_extended_info_t));

  if(NULL!=ssei)
  {
    TSP_SSEI_initialize(ssei,pgi,nei);
  }
  return ssei;
} /* end of TSP_SSEI_new */

int32_t
TSP_SSEI_delete(TSP_sample_symbol_extended_info_t** ssei) {
  int32_t ret;

  assert(ssei);

  ret=TSP_STATUS_OK;

  if(NULL!=*ssei)
    {
      ret=TSP_SSEI_finalize(*ssei);
      if(TSP_STATUS_OK!=ret){
	return ret;
      }

      free(*ssei);
      *ssei=NULL;
    }

    return ret;
} /* end of TSP_SSEI_delete */

int32_t
TSP_SSEI_initialize(TSP_sample_symbol_extended_info_t* ssei, const int32_t pgi, const int32_t nei) {
  
  assert(ssei);
  ssei->provider_global_index=pgi;
  return TSP_EIList_initialize(&(ssei->info), nei);

} /* end of TSP_SSEI_initialize */


int32_t
TSP_SSEI_finalize(TSP_sample_symbol_extended_info_t* ssei){

  assert(ssei);
  return  TSP_EIList_finalize(&(ssei->info));
} /* end of TSP_SSEI_finalize */

int32_t
TSP_SSEI_copy(TSP_sample_symbol_extended_info_t* ssei_dest,
  	      const TSP_sample_symbol_extended_info_t ssei_src){
  int32_t ret;

  assert(ssei_dest);

  ret=TSP_STATUS_OK;

  ret=TSP_SSEI_finalize(ssei_dest);
  if(TSP_STATUS_OK!=ret){
    return ret;
  }

  ret=TSP_SSEI_initialize(ssei_dest,ssei_src. provider_global_index,ssei_src.info.TSP_extended_info_list_t_len);
  if(TSP_STATUS_OK!=ret){
    return ret;
  }
  ssei_dest->provider_global_index=ssei_src.provider_global_index;

  ret=TSP_EIList_copy( &(ssei_dest->info),ssei_src.info);
 
  return TSP_STATUS_OK;
} /* end TSP_SSEI_copy */

TSP_sample_symbol_extended_info_list_t*
TSP_SSEIList_new(int32_t len) {
  TSP_sample_symbol_extended_info_list_t* ssei_list;

  ssei_list = calloc(1,sizeof(TSP_sample_symbol_extended_info_list_t));
  

  if(NULL!=ssei_list)
  {    
    TSP_SSEIList_initialize(ssei_list,len);
  }
  return ssei_list;
} /* end of TSP_SSEIList_new */

int32_t
TSP_SSEIList_delete(TSP_sample_symbol_extended_info_list_t** ssei_list) {

  int32_t ret;

  assert(ssei_list);

  ret=TSP_STATUS_OK;

  if(NULL!=*ssei_list)
    {
      ret=TSP_SSEIList_finalize(*ssei_list);
      free(*ssei_list);
      *ssei_list=NULL;
    }

    return ret;
} /* end of TSP_SSEI_delete */

int32_t
TSP_SSEIList_initialize(TSP_sample_symbol_extended_info_list_t* ssei_list,int32_t len) {
  int32_t i;
  int32_t ret;

  assert(ssei_list);
  ret=TSP_STATUS_OK;

  if(NULL!=ssei_list->TSP_sample_symbol_extended_info_list_t_val)
  {
    for(i=0;i<ssei_list->TSP_sample_symbol_extended_info_list_t_len;++i)
    {
      ret=TSP_SSEI_finalize(&(ssei_list->TSP_sample_symbol_extended_info_list_t_val[i]));
    }
  }

  ssei_list->TSP_sample_symbol_extended_info_list_t_len=len;
  ssei_list->TSP_sample_symbol_extended_info_list_t_val=calloc(len,sizeof(TSP_sample_symbol_extended_info_t));
  TSP_CHECK_ALLOC(ssei_list->TSP_sample_symbol_extended_info_list_t_val,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

  return ret;

} /* end of TSP_SSEI_initialize */

int32_t
TSP_SSEIList_finalize(TSP_sample_symbol_extended_info_list_t* ssei_list){
  int32_t ret;
  int32_t i;

  assert(ssei_list);
  ret=TSP_STATUS_OK;

  for (i=0;i<ssei_list->TSP_sample_symbol_extended_info_list_t_len;++i) {
    ret=TSP_SSEI_finalize(&(ssei_list->TSP_sample_symbol_extended_info_list_t_val[i]));
  }

  free(ssei_list->TSP_sample_symbol_extended_info_list_t_val);

  ssei_list->TSP_sample_symbol_extended_info_list_t_val = NULL;
  ssei_list->TSP_sample_symbol_extended_info_list_t_len = 0;

  return ret;
} /* end of TSP_SSEI_finalize */


int32_t
TSP_SSEIList_copy(TSP_sample_symbol_extended_info_list_t* dest_ssei_list,
                  const TSP_sample_symbol_extended_info_list_t src_ssei_list){

  int32_t ret;
  int32_t i;

  assert(dest_ssei_list);
 
  ret=TSP_SSEIList_finalize(dest_ssei_list);
  if(TSP_STATUS_OK!=ret){
    return ret;
  }

  ret= TSP_SSEIList_initialize(dest_ssei_list,src_ssei_list.TSP_sample_symbol_extended_info_list_t_len); 
  if(TSP_STATUS_OK!=ret){
    return ret;
  }
  /* loop over symbol_info to copy */
  for (i=0;i<dest_ssei_list->TSP_sample_symbol_extended_info_list_t_len;++i) {
     ret= TSP_SSEI_copy(&(dest_ssei_list->TSP_sample_symbol_extended_info_list_t_val[i]),
			  src_ssei_list.TSP_sample_symbol_extended_info_list_t_val[i]);
  }

  return ret;

} /* end of TSP_SSEI_copy */





