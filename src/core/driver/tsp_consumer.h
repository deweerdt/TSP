/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.h,v 1.3 2002-10-01 15:29:43 galles Exp $

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

/** Opaque handle type for provider */
typedef  void* TSP_provider_t;

/**
* Initialisation for TSP librarie.
* Call this function before using the command line string. This function
* removes the arguments it knows from the argument list, leaving anything
* it does not recognize for your application to parse or ignore. 
* This creates a set of standard arguments accepted by all TSP applications.
* @param argc Use the argc main arg before using it
* @param argc Use the argv main arg before using it
* @return TRUE = OK
*/
int TSP_consumer_init(int* argc, char** argv[]);



int TSP_consumer_is_command_ligne_stream_init(void);


/**
* End of TSP librairie use
* call this function when you are done with the librairie
*/
void TSP_consumer_end();


void TSP_open_all_provider(const char* target_name,
			   TSP_provider_t** providers,
			   int* nb_providers);

void TSP_close_all_provider(TSP_provider_t providers[]);
				  
void TSP_close_provider(TSP_provider_t provider);
				  
void TSP_print_provider_info(TSP_provider_t provider);	

const TSP_otsp_server_info_t* TSP_get_provider_simple_info(TSP_provider_t provider);			  

int TSP_request_provider_open(TSP_provider_t provider, char* stream_init);


int TSP_request_provider_close(TSP_provider_t provider);

int TSP_request_provider_information(TSP_provider_t provider);

TSP_sample_symbol_info_list_t* 
TSP_get_provider_information(TSP_provider_t provider);

int TSP_request_provider_sample(
				TSP_request_sample_t* req_sample,
				TSP_provider_t provider);
    
int TSP_request_provider_sample_init(TSP_provider_t provider);    
    
int TSP_read_sample(TSP_provider_t provider,
                    TSP_sample_t* sample,
                    int* new_sample);

    
#endif /* _TSP_CONSUMER_H */
