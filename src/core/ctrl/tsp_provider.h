/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.h,v 1.6 2002-10-09 07:37:48 galles Exp $

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



int TSP_provider_init(int* argc, char** argv[]);

int TSP_provider_run(int blocking);

void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open);
		      
void TSP_provider_request_close(const TSP_request_close_t* req_close);

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
 			      TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample(TSP_request_sample_t* req_info, 
			 TSP_answer_sample_t* ans_sample);

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_info, 
 			      TSP_answer_sample_init_t* ans_sample);

#endif /* _TSP_PROVIDER_H */
