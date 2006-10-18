/*

$Id: tsp_default_glu.h,v 1.8 2006-10-18 09:58:48 erk Exp $

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
 * @defgroup TSP_DefaultGLU GLU Default Instance
 * @ingroup TSP_GLULib
 * The default GLU API implements
 * default function that are suitable for a 'simple'
 * GLU object.
 * @{
 */

BEGIN_C_DECLS

/** 
 * Default GLU server name.
 * @param cthis the GLU object
 * @return GLU name.
 */
char* GLU_get_server_name_default(GLU_handle_t* cthis);

/**
 * Default GLU server type.
 * @param[in] cthis the GLU object
 * @return GLU server type.
 */
GLU_server_type_t GLU_get_server_type_default(GLU_handle_t* cthis);

/**
 * Default GLU base frequency.
 * @param[in] cthis the GLU object
 * @return GLU base frequency (Hz)
 */
double GLU_get_base_frequency_default(GLU_handle_t* cthis);

int32_t 
GLU_get_nb_max_consumer_default(struct GLU_handle_t* cthis);

/**
 * Default GLU start.
 * @param[in] cthis the GLU object
 * @return true or false
 */
int32_t 
GLU_start_default(GLU_handle_t* cthis);

/** 
 * Default GLU_get_pgi.
 * The default implementation use the mandatory GLU_get_sample_symbol_info_list
 * and does a linear search in it.
 * @param[in] cthis the GLU object 
 * @param[in] symbol_list the symbol list to validate
 * @param[out] pg_indexes array containing corresponding provider global indexes or -1 if not found 
 * @return TRUE if all symbol found, else return FALSE 
 */
int32_t
GLU_get_pgi_default(GLU_handle_t* cthis, 
		    TSP_sample_symbol_info_list_t* symbol_list, 
		    int* pg_indexes);

/**
 * Default GLU_get_instance.
 * The default implementation is only valid for an ACTIVE GLU.
 * In this case this function always return this.
 * PASSIVE GLU should reimplement this function.
 * @param[in,out] cthis  the GLU object
 * @param[in] custom_argc
 * @param[in] custom_argv
 * @param[in,out] error_info
 * @return the GLU handle  
 */ 
GLU_handle_t* 
GLU_get_instance_default(GLU_handle_t* cthis,
			 int custom_argc,
			 char* custom_argv[],
			 char** error_info);

/**
 * Return the number of (static) available symbols.
 * @param[in] cthis the GLU object
 * @return the number of (static) available symbols.
 */
int32_t
GLU_get_nb_symbols_default(GLU_handle_t* cthis);

/**
 * Return the filtered list of sample symbols.
 * @param[in] cthis the GLU object
 * @param[in] filter_kind the filter kind
 * @param[in] filter_string the filter string
 * @param[out] answer_sample the Answer Sample containing the list of symbols
 *             matching specified filter.
 * @return the number of (static) available symbols.
 */
int32_t
GLU_get_filtered_ssi_list_default(GLU_handle_t* cthis, 
				  int filter_kind, char* filter_string, 
				  TSP_answer_sample_t* answer_sample);

int32_t
GLU_get_ssi_list_fromPGI_default(struct GLU_handle_t* cthis, 
				 int* pgis, int pgis_len, 
				 TSP_sample_symbol_info_list_t* SSI_list);

int32_t
GLU_get_ssei_list_fromPGI_default(struct GLU_handle_t* cthis, 
				  int* pgis, int pgis_len, 
  				  TSP_sample_symbol_extended_info_list_t* SSEI_list);

int32_t
GLU_async_sample_read_default(struct GLU_handle_t* cthis, 
			      int pgi, 
			      void* value_ptr, uint32_t* value_size);

int32_t
GLU_async_sample_write_default(struct GLU_handle_t* cthis, 
			       int pgi, 
			       void* value_ptr, uint32_t value_size);

/**
 * Test if the asked symbol is the same symbol of the complete symbol list.
 * @param[in] looked_for asked symbol
 * @param[in] compared symbol of the complete list
 * @param[out] pg_indexes provider global index (-1 if symbol is NOK)
 *             matching specified filter.
 * @return true if symbol is correct.
 */
int32_t
GLU_validate_sample_default( TSP_sample_symbol_info_t* looked_for, 
			     TSP_sample_symbol_info_t* compared,
			     int* pg_indexes);

/** @} */

END_C_DECLS

#endif /* _TSP_DEFAULT_GLU_H */
