/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Main interface for the TSP consumer library

-----------------------------------------------------------------------
 */

#ifndef _TSP_CONSUMER_H
#define _TSP_CONSUMER_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include "tsp_sample_ringbuf.h"

/* Handle sur provider */
typedef  void* TSP_provider_t;

/* Fontions */

void TSP_open_all_provider(const char* target_name,
			   TSP_provider_t** providers,
			   int* nb_providers);

void TSP_close_all_provider(TSP_provider_t providers[]);
				  
void TSP_close_provider(TSP_provider_t provider);
				  
void TSP_print_provider_info(TSP_provider_t provider);	

const TSP_otsp_server_info_t* TSP_get_provider_simple_info(TSP_provider_t provider);			  

int TSP_request_provider_open(TSP_provider_t provider);

int TSP_request_provider_close(TSP_provider_t provider);

int TSP_request_provider_information(TSP_provider_t provider);

TSP_sample_symbol_info_list_t* 
TSP_get_provider_information(TSP_provider_t provider);

int TSP_request_provider_sample(
				TSP_request_sample_t* req_sample,
				TSP_provider_t provider);
    
int TSP_request_provider_sample_init(TSP_provider_t provider);    
    
int TSP_read_sample(TSP_provider_t provider,
                    int provider_global_index,
                    TSP_sample_t* sample,
                    int* new_sample);

    
#endif /* _TSP_CONSUMER_H */
