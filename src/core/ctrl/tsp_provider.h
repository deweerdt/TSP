/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.h,v 1.8 2002-11-29 17:29:57 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

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
