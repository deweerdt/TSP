/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_default_glu.c,v 1.14 2006-04-13 21:22:46 erk Exp $

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

Project   : TSP
Maintainer : tsp@astrium.eads.net
Component : Provider / GLU server

-----------------------------------------------------------------------

Purpose   : Implementation for the object GLU_handle_t
            which contains some basic GLU informations and default
            function pointer values.

-----------------------------------------------------------------------
 */

#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

#include <tsp_glu.h>
#include <tsp_common.h>
#include <tsp_default_glu.h>


int32_t GLU_handle_create(GLU_handle_t** glu, const char* name, const GLU_server_type_t type, const double base_frequency) {
  int32_t retcode = FALSE;

  /* allocate structure */
  *glu = calloc(1,sizeof(GLU_handle_t));
  assert(*glu);
  /* set up field values */
  (*glu)->tid             = 0;
  (*glu)->name            = strdup(name);
  (*glu)->type            = type;
  (*glu)->base_frequency  = base_frequency;
  (*glu)->nb_max_consumer = TSP_MAX_CLIENT_NUMBER;
  (*glu)->nb_connected_consumer = 0;
  (*glu)->private_data    = NULL;
  /* provides default method implementation  and NULLIFY others */
  (*glu)->get_name            = &GLU_get_server_name_default;
  (*glu)->get_type            = &GLU_get_server_type_default;
  (*glu)->get_base_frequency  = &GLU_get_base_frequency_default;
  (*glu)->get_nb_max_consumer = &GLU_get_nb_max_consumer_default;

  if (GLU_SERVER_TYPE_ACTIVE==type) {
    (*glu)->get_instance = &GLU_get_instance_default;
    retcode = TRUE;
  } else if (GLU_SERVER_TYPE_PASSIVE==type) {
    /* we may not provide default get_instance for passive GLU */
    (*glu)->get_instance = NULL;    
    retcode = TRUE;
  } else {
    /* 
     * note that we have been providing an invalid type... 
     * retcode will be false.
     */    
  }

  /*
   * initialize, run and get_ssi_list do not have default
   * start and get_pgi does.
   */
  (*glu)->start                 = &GLU_start_default;
  (*glu)->get_pgi               = &GLU_get_pgi_default;
  (*glu)->get_filtered_ssi_list = &GLU_get_filtered_ssi_list_default;
  (*glu)->get_ssi_list_fromPGI  = &GLU_get_ssi_list_fromPGI_default;
  (*glu)->get_ssei_list_fromPGI = &GLU_get_ssei_list_fromPGI_default;
  (*glu)->get_nb_symbols        = &GLU_get_nb_symbols_default;
  (*glu)->async_read            = &GLU_async_sample_read_default;
  (*glu)->async_write           = &GLU_async_sample_write_default;
  
  return retcode;
} /* end if GLU_handle_create */

int32_t GLU_handle_destroy(GLU_handle_t** glu) {
  int32_t retcode = FALSE;
  
  free((*glu)->name);
  free((*glu));
  *glu = NULL;
  retcode = TRUE;
  return retcode;
} /* end if GLU_handle_destroy */

int32_t 
GLU_handle_check(GLU_handle_t* glu) {
  return TSP_STATUS_OK;
}


/* ====== You'll find hereafter some default implementation GLU methods ====== */

char* 
GLU_get_server_name_default(struct GLU_handle_t* this) {
  return this->name;
}

GLU_server_type_t 
GLU_get_server_type_default(struct GLU_handle_t* this) {
  return this->type;
} /* end of GLU_get_server_type_default */

double 
GLU_get_base_frequency_default(struct GLU_handle_t* this) {
  return this->base_frequency;
} /* end of GLU_get_base_frequency_default */

int32_t
GLU_get_nb_max_consumer_default(struct GLU_handle_t* this) {
  return this->nb_max_consumer;
} /* end of GLU_get_nb_max_consumer_default */

GLU_handle_t* 
GLU_get_instance_default(GLU_handle_t* this,
			 int custom_argc,
			 char* custom_argv[],
			 char** error_info) {
  
  if (GLU_SERVER_TYPE_ACTIVE == (this->type)) {
    return this;
  } else {
    return NULL;
  }
} /* end of GLU_get_instance_default */


int32_t
GLU_start_default(GLU_handle_t* this)
{
  if (0==this->tid) {
    return pthread_create(&(this->tid), NULL, this->run, this); 
  } else {
    return 1;
  }
} /* GLU_start_default */

int32_t
GLU_get_pgi_default(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes) {
  int retcode = TRUE;
  TSP_sample_symbol_info_list_t complete_symbol_list;
  TSP_sample_symbol_info_t* compared;
  int i;
  int j;

  this->get_ssi_list(this,&complete_symbol_list);
    
  /* now do the infamous double search loop */
  /* For each requested symbols, check by name, and find the provider global index */
  for( i = 0 ; i < symbol_list->TSP_sample_symbol_info_list_t_len ; i++) {
    int found = FALSE;
    TSP_sample_symbol_info_t* looked_for = &(symbol_list->TSP_sample_symbol_info_list_t_val[i]);
    
    for( j = 0 ; j < complete_symbol_list.TSP_sample_symbol_info_list_t_len ; j++) {
      compared = &(complete_symbol_list.TSP_sample_symbol_info_list_t_val[j]);
      if(!strcmp(looked_for->name, compared->name)) {
	found = TRUE;
	looked_for->provider_global_index = compared->provider_global_index;
	pg_indexes[i]=looked_for->provider_global_index;
      }
      if(found) break;
      
    }
    if(!found) {
      retcode = FALSE;
      pg_indexes[i]= -1;
      STRACE_INFO(("Unable to find symbol '%s'",  looked_for->name));	   
    }
    
    found = FALSE;     
    found = GLU_validate_sample_default(looked_for,compared,&pg_indexes[i]);
    
    if(!found) {
      retcode = FALSE;
      STRACE_INFO(("No good data in symbol '%s'",  looked_for->name));	   
    }
  }
  return retcode;
} /* end of GLU_get_pgi_default */


int32_t  
GLU_get_nb_symbols_default(GLU_handle_t* this) {
  int retval = 0;
  TSP_sample_symbol_info_list_t complete_symbol_list;
  this->get_ssi_list(this,&complete_symbol_list);

  retval = complete_symbol_list.TSP_sample_symbol_info_list_t_len;
  return retval;
} /* end of GLU_get_nb_symbols_default */

int32_t 
GLU_get_filtered_ssi_list_default(GLU_handle_t* this, int filter_kind, char* filter_string, TSP_answer_sample_t* answer_sample) {

  TSP_sample_symbol_info_list_t complete_symbol_list;
  int32_t nb_match;
  int32_t i;
  int32_t* matched_index;

  /* 
   * Get the complete list of symbols
   * this is sub-optimal but this GLU-side operation
   * should be made as a zero copy operation
   * with NO network transfert.
   * We will return a filtered list in answer_sample->symbols 
   * that will be transmitted to provider lib and then to consumer
   * side accross network.
   */
  this->get_ssi_list(this,&complete_symbol_list);

  switch (filter_kind) {
  case TSP_FILTER_SIMPLE:
    STRACE_INFO(("Requested filter kind <%d>, filter string = <%s>",filter_kind,filter_string));
    /* first loop to count matching symbols */
    nb_match      = 0;
    matched_index = calloc(complete_symbol_list.TSP_sample_symbol_info_list_t_len,sizeof(int32_t));
    for (i=0;i<complete_symbol_list.TSP_sample_symbol_info_list_t_len;i++) {
      if (NULL != strstr(complete_symbol_list.TSP_sample_symbol_info_list_t_val[i].name,filter_string)) {
	matched_index[nb_match] = i; 
	nb_match++;	
      }
    }
    STRACE_INFO(("Nb Match(es) is <%d>",nb_match));
    /* second loop if nb_match > 0 */
    if (nb_match>0) {
      free(answer_sample->symbols.TSP_sample_symbol_info_list_t_val);
      answer_sample->symbols.TSP_sample_symbol_info_list_t_val = calloc(nb_match,sizeof(TSP_sample_symbol_info_t));
      answer_sample->symbols.TSP_sample_symbol_info_list_t_len = nb_match;
      for (i=0;i<nb_match;i++) {
	STRACE_DEBUG(("Adding <%s> to answer_sample...",complete_symbol_list.TSP_sample_symbol_info_list_t_val[matched_index[i]].name));
	TSP_common_SSI_copy(&(answer_sample->symbols.TSP_sample_symbol_info_list_t_val[i]),
			    complete_symbol_list.TSP_sample_symbol_info_list_t_val[matched_index[i]]);
      }
    }
    free(matched_index);
    answer_sample->status = TSP_STATUS_OK;
    break;
  case TSP_FILTER_REGEX:
  case TSP_FILTER_XPATH:
  case TSP_FILTER_SQL:
    answer_sample->status = TSP_STATUS_ERROR_NOT_SUPPORTED;
  default:
    answer_sample->status = TSP_STATUS_ERROR_SYMBOL_FILTER;
    break;
  } /* end switch filter_kind */
  return -1;
}

int32_t
GLU_get_ssi_list_fromPGI_default(struct GLU_handle_t* this, 
				 int32_t* pgis, int32_t pgis_len, 
				 TSP_sample_symbol_info_list_t* SSI_list) {
  /* FIXME */
  int32_t i,ret;
  TSP_sample_symbol_info_list_t complete_symbol_list; 
 
  this->get_ssi_list(this,&complete_symbol_list);
  ret=TSP_STATUS_OK;

  /* Store all global indexes into list including NOT FOUND ones */
  for ( i=0 ; i < pgis_len;++i) {
    
    if(-1!=pgis[i]) {
      TSP_common_SSI_copy(&(SSI_list->TSP_sample_symbol_info_list_t_val[i]), 
			  complete_symbol_list.TSP_sample_symbol_info_list_t_val[pgis[i]]);
      
    }
    else {
      SSI_list->TSP_sample_symbol_info_list_t_val[i].provider_global_index=-1;      
      STRACE_INFO(("Unable to find symbol '%s'",  SSI_list->TSP_sample_symbol_info_list_t_val[i].name));      
      ret=TSP_STATUS_ERROR_SYMBOLS;      
    }
  }
  return  ret;
} /* end of GLU_get_ssi_list_fromPGI_default */

int32_t
GLU_get_ssei_list_fromPGI_default(struct GLU_handle_t* this, 
				  int32_t* pgis, int32_t pgis_len, 
				  TSP_sample_symbol_extended_info_list_t* SSEI_list) {

  int32_t i,ret;
  ret=TSP_STATUS_OK;

  /* Store all global indexes into list including NOT FOUND ones */
  for ( i=0 ; i < pgis_len;++i) {
    
    if (-1!=pgis[i]) {
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index=pgis[i]; 
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_len=0;
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val=NULL;
    } else {
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index=-1;      
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_len=0;
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val=NULL;
      ret=TSP_STATUS_ERROR_PGI_UNKNOWN;      
    }
  } /*end for*/
  return  ret;

} /* end of GLU_get_ssei_list_fromPGI_default */

int32_t
GLU_async_sample_read_default(struct GLU_handle_t* this, 
			      int32_t pgi, 
			      void* value_ptr, uint32_t* value_size) {
  /* default GLU does not authorize async read for any symbol */  
  return TSP_STATUS_ERROR_ASYNC_READ_NOT_SUPPORTED;
} /* end of GLU_async_sample_read_default */

int32_t
GLU_async_sample_write_default(struct GLU_handle_t* this, 
			       int pgi, 
			       void* value_ptr, uint32_t value_size) {
  /* default GLU does not authorize async write for any symbol */
  return TSP_STATUS_ERROR_ASYNC_WRITE_NOT_SUPPORTED;
} /* end of GLU_async_sample_write_default */


int32_t
GLU_validate_sample_default( TSP_sample_symbol_info_t* looked_for, 
			     TSP_sample_symbol_info_t* compared,
			     int* pg_indexes){

  /* validate period and phase range */	   
  if(looked_for->period < 1) 
  {
    *pg_indexes=-1;
    STRACE_DEBUG(("Invalid period"));
    return FALSE;
  }

  if(looked_for->phase < 0) 
  {
    *pg_indexes=-1;
    STRACE_DEBUG(("Invalid phase"));
    return FALSE;
  }
	  
      		     
 if(looked_for->type != TSP_TYPE_UNKNOWN)
 {
    if(looked_for->type != compared->type)
    {
      *pg_indexes=-1;
       STRACE_DEBUG(("Invalid type"));
      return FALSE;

    }
 }
 else
 {
   looked_for->type = compared->type;

 }

 if(looked_for->offset != 0)
 {	    
    if(looked_for->offset >= compared->dimension)
    {
      *pg_indexes=-1;
      STRACE_DEBUG(("Invalid offset"));
      return FALSE;
    }
 }

 if(looked_for->nelem != 0)
 {
    if((looked_for->offset + looked_for->nelem - 1) >= compared->dimension)
    {
      *pg_indexes=-1;
      STRACE_DEBUG(("Invalid nelem"));
      return FALSE;
    }
 }
 else
 {
   looked_for->nelem =compared->dimension;
 }


 if(looked_for->dimension != 0)
 {
    if(looked_for->dimension != compared->dimension)
    {
      *pg_indexes=-1;
      STRACE_DEBUG(("Invalid dimension"));
      return FALSE;
    }
 }
 else
 {
   looked_for->dimension =compared->dimension;
 }

 /* update PGI */
 looked_for->provider_global_index = *pg_indexes; 
 return TRUE;
} 
