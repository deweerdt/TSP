/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/Attic/glue_sserver.c,v 1.5 2005-10-09 23:01:23 erk Exp $

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

#include <glue_sserver.h>


int32_t GLU_handle_create(GLU_handle_t** glu, const char* name, const GLU_server_type_t type, const double base_frequency) {
  int32_t retcode = FALSE;

  /* allocate structure */
  *glu = calloc(1,sizeof(GLU_handle_t));
  assert(*glu);
  /* set up field values */
  (*glu)->tid            = 0;
  (*glu)->name           = strdup(name);
  (*glu)->type           = type;
  (*glu)->base_frequency = base_frequency;
  /* provides default method implementation  and NULLIFY others */
  (*glu)->get_name           = &GLU_get_server_name_default;
  (*glu)->get_type           = &GLU_get_server_type_default;
  (*glu)->get_base_frequency = &GLU_get_base_frequency_default;

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
  (*glu)->start              = &GLU_start_default;
  (*glu)->get_pgi            = &GLU_get_pgi_default;
  (*glu)->get_filtered_ssi_list = &GLU_get_filtered_ssi_list_default;
  (*glu)->get_nb_symbols     = &GLU_get_nb_symbols_default;
  (*glu)->async_read         = &GLU_async_sample_read_default;
  (*glu)->async_write        = &GLU_async_sample_write_default;
  
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

/* ====== You'll find hereafter some default implementation GLU methods ====== */

char* 
GLU_get_server_name_default(struct GLU_handle_t* this) {
  return this->name;
}

GLU_server_type_t 
GLU_get_server_type_default(struct GLU_handle_t* this) {
  return this->type;
}

double 
GLU_get_base_frequency_default(struct GLU_handle_t* this) {
  return this->base_frequency;
}

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


int  
GLU_start_default(GLU_handle_t* this)
{
  if (0==this->tid) {
    return pthread_create(&(this->tid), NULL, this->run, this); 
  } else {
    return 1;
  }
}

int 
GLU_get_pgi_default(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes) {
  int retcode = TRUE;
  TSP_sample_symbol_info_list_t complete_symbol_list;
  int i;
  int j;

  this->get_ssi_list(this,&complete_symbol_list);
    
  /* now do the infamous double search loop */
  /* For each requested symbols, check by name, and find the provider global index */

  for( i = 0 ; i < symbol_list->TSP_sample_symbol_info_list_t_len ; i++)
     {
       int found = FALSE;
       TSP_sample_symbol_info_t* looked_for = &(symbol_list->TSP_sample_symbol_info_list_t_val[i]);
       
       for( j = 0 ; j < complete_symbol_list.TSP_sample_symbol_info_list_t_len ; j++)
	 {
	   TSP_sample_symbol_info_t* compared = &(complete_symbol_list.TSP_sample_symbol_info_list_t_val[j]);
	   if(!strcmp(looked_for->name, compared->name))
	     {
	       found = TRUE;
	       looked_for->provider_global_index = compared->provider_global_index;
	       pg_indexes[i]=looked_for->provider_global_index;
	     }
	   if(found) break;

	 }
       if(!found)
	 {
	   retcode = FALSE;
	   STRACE_INFO(("Unable to find symbol '%s'",  looked_for->name));
	   break;	   
	 }
     }
  return retcode;
} /* end of GLU_get_pgi_default */


int  
GLU_get_nb_symbols_default(GLU_handle_t* this)
{
  int retval = 0;
  TSP_sample_symbol_info_list_t complete_symbol_list;
  this->get_ssi_list(this,&complete_symbol_list);

  retval = complete_symbol_list.TSP_sample_symbol_info_list_t_len;
  return retval;
}

int GLU_get_filtered_ssi_list_default(GLU_handle_t* this, int filter_kind, char* filter_string, TSP_sample_symbol_info_list_t* symbol_list) {
  return this->get_ssi_list(this,symbol_list);
}

int 
GLU_async_sample_read_default(struct GLU_handle_t* this, int pgi, void* value_ptr, int* value_size) {
  /* default GLU does not authorize async read for any symbol */
  return -1;
}

int 
GLU_async_sample_write_default(struct GLU_handle_t* this, int pgi, void* value_ptr, int value_size) {
  /* default GLU does not authorize async write for any symbol */
  return -1;
}
