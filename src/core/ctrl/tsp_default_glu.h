/*!  \file 

$Id: tsp_default_glu.h,v 1.2 2005-11-29 22:08:53 erk Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the glue server : the data producer

-----------------------------------------------------------------------
 */

#ifndef _TSP_DEFAULT_GLU_H
#define _TSP_DEFAULT_GLU_H

#include <tsp_glu.h>
/**
 * @defgroup DefaultGLU
 * @ingroup GLUServer
 * The default GLU API implements
 * default function that are suitable for a 'simple'
 * @{
 */

BEGIN_C_DECLS

/** 
 * Default GLU server name.
 * @param this the GLU structure
 * @return GLU name.
 */
char* GLU_get_server_name_default(GLU_handle_t* this);

/**
 * Default GLU server type.
 * @param this the GLU structure
 * @return GLU server type.
 */
GLU_server_type_t GLU_get_server_type_default(GLU_handle_t* this);

/**
 * Default GLU base frequency.
 * @param this the GLU structure
 * @return GLU base frequency (Hz)
 */
double GLU_get_base_frequency_default(GLU_handle_t* this);

int 
GLU_get_nb_max_consumer_default(struct GLU_handle_t* this);

/**
 * Default GLU start.
 * @param this the GLU structure
 * @return true or false
 */
int GLU_start_default(GLU_handle_t* this);

/** 
 * Default GLU_get_pgi.
 * The default implementation use the mandatory GLU_get_sample_symbol_info_list
 * and does a linear search in it.
 * @param this IN, 
 * @param symbol_list IN the symbol list to validate
 * @param pg_indexes OUT array containing corresponding provider global indexes or -1 if not found 
 * @return TRUE if all symbol found, else return FALSE 
 */
int GLU_get_pgi_default(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes);

/**
 * Default GLU_get_instance.
 * The default implementation is only valid for an ACTIVE GLU.
 * In this case this function always return this.
 * PASSIVE GLU should reimplement this function.
 */ 
GLU_handle_t* GLU_get_instance_default(GLU_handle_t* this,
                                       int custom_argc,
	 		               char* custom_argv[],
			               char** error_info);

int  
GLU_get_nb_symbols_default(GLU_handle_t* this);

int 
GLU_get_filtered_ssi_list_default(GLU_handle_t* this, int filter_kind, char* filter_string, TSP_answer_sample_t* answer_sample);

int 
GLU_async_sample_read_default(struct GLU_handle_t* this, int pgi, void* value_ptr, uint32_t* value_size);

int 
GLU_async_sample_write_default(struct GLU_handle_t* this, int pgi, void* value_ptr, uint32_t value_size);

/** @} */

END_C_DECLS

#endif /* _TSP_DEFAULT_GLU_H */
