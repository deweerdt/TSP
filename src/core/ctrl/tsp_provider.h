/*!  \file 

$Id: tsp_provider.h,v 1.14 2005-10-23 13:15:21 erk Exp $

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

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_PROVIDER_H
#define _TSP_PROVIDER_H

#include <tsp_prjcfg.h>
#include <tsp_abs_types.h>
#include <tsp_datastruct.h>
#include <tsp_glu.h>

/**
 * @defgroup Provider
 * @ingroup  Core
 * The Provider module is the set of all 
 * provider library interface.
 * @{
 */

BEGIN_C_DECLS

/**
 * Initialize TSP provider library.
 * One should call this before any other TSP provider lib call.
 * @param theGLU INOUT, the GLU to be used by this provider.
 * @param argc INOUT, the number of argument of the main
 * @param argv INOUT, array of argument of size argc.
 */
int TSP_provider_private_init(GLU_handle_t* theGLU, int* argc, char** argv[]);

const char* TSP_provider_get_name();


void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open);
		      
void TSP_provider_request_close(const TSP_request_close_t* req_close);

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
				       TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_filtered_information(TSP_request_information_t* req_info, 
						int filter_kind, char* filter_string,
						TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample(TSP_request_sample_t* req_sample, 
			 TSP_answer_sample_t* ans_sample);
void TSP_provider_request_sample_free_call(TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_sample_init, 
 			      TSP_answer_sample_init_t* ans_sample);

void  TSP_provider_request_sample_destroy(TSP_request_sample_destroy_t* req_sample_destroy, 
					  TSP_answer_sample_destroy_t* ans_sample);

int TSP_provider_is_initialized(void);

int TSP_provider_get_server_number(void);
int TSP_provider_get_server_base_number(void );

int TSP_provider_request_async_sample_write(TSP_async_sample_t* async_sample_write);

int TSP_provider_request_async_sample_read(TSP_async_sample_t* async_sample_read);

END_C_DECLS
/** @} end group Provider */ 

#endif /* _TSP_PROVIDER_H */
