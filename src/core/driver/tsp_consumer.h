/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.h,v 1.5 2002-10-07 08:36:08 galles Exp $

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


/**
* End of TSP librairie use
* call this function when you are done with the librairie
*/
void TSP_consumer_end();


void TSP_consumer_open_all(const char* host_name,
			   TSP_provider_t** providers,
			   int* nb_providers);

void TSP_consumer_close_all(TSP_provider_t providers[]);
				  
void TSP_consumer_close(TSP_provider_t provider);


/** 
 * Send a TSP_REQUEST_OPEN to the provider.
 * This function must be called and must succed for the program to be able to call
 * other TSP_REQUEST_* functions.
 * @param provider The provider handle
 * @param custom_argc Custom provider data stream initialisation (optional, if so, set 0)
 * @param custom_argv Custom provider data stream initialisation (optional, is fo, set 0)
 * @return TRUE or FALSE. TRUE = OK.
 * custom_argc and custom_argv works like main argc and argv :
 * - custom_argv[0] is ignored but a real string must be provided 
 * (any string will do, but there MUST be a string, even an empty
 * string )
 * - custom_argc is the total number of elements in custom_argv (included custom_argc[0])
 * - if you do not want to provide custom data stream initialisation, call the function
 * with custom_argc=custom_argv=0 ( as a consequency if a stream is provided on the consumer
 * command line, it will be used )
 * If you provide your own custom_argc and custom_argv and the user provided data stream
 * initialisation arguments on the provider command line, this command line will be ignored
 * and your arguments call will be used
 * - If you call the function with custom_argc=1 (with any string in custom_argv[0]) this
 * means "overide consumer command line data stream initialisation, and set 0 parameter".
 * It means that the provided data stream initialisation will be done with 0 parameters.
 * It is strange but you might want to do that.
 */
int TSP_consumer_request_open(TSP_provider_t provider, int custom_argc, char* custom_argv[]);
				  

const TSP_otsp_server_info_t* TSP_consumer_get_server_info(TSP_provider_t provider);			  





int TSP_consumer_request_close(TSP_provider_t provider);

int TSP_consumer_request_information(TSP_provider_t provider);

TSP_sample_symbol_info_list_t* 
TSP_consumer_get_information(TSP_provider_t provider);

int TSP_consumer_request_sample(
				TSP_request_sample_t* req_sample,
				TSP_provider_t provider);
    
int TSP_consumer_request_sample_init(TSP_provider_t provider);    
    
int TSP_consumer_read_sample(TSP_provider_t provider,
                    TSP_sample_t* sample,
                    int* new_sample);

    
#endif /* _TSP_CONSUMER_H */
