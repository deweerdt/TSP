/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.h,v 1.9 2002-12-18 16:27:17 tntdev Exp $

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
Maintainer : tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_PROVIDER_H
#define _TSP_PROVIDER_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"



int TSP_provider_private_init(int* argc, char** argv[]);


void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open);
		      
void TSP_provider_request_close(const TSP_request_close_t* req_close);

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
 			      TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample(TSP_request_sample_t* req_info, 
			 TSP_answer_sample_t* ans_sample);
void TSP_provider_request_sample_free_call(TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_info, 
 			      TSP_answer_sample_init_t* ans_sample);

void  TSP_provider_request_sample_destroy(TSP_request_sample_destroy_t* req_info, 
					  TSP_answer_sample_destroy_t* ans_sample);

int TSP_provider_is_initialized(void);

int TSP_provider_get_server_number(void);

#endif /* _TSP_PROVIDER_H */
