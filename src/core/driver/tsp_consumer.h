/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.h,v 1.6 2002-10-24 13:28:29 galles Exp $

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


#include "tsp_sample_ringbuf.h"

/*------------------------------ ENUM ---------------------------------*/

enum TSP_consumer_status_t {
	TSP_CONSUMER_STATUS_OK,
	TSP_CONSUMER_STATUS_ERROR_UNKNOWN,
	TSP_CONSUMER_STATUS_ERROR_VERSION
};

/*-------------------------- STRUCTURES -------------------------------*/

struct TSP_consumer_symbol_requested_t
{
  int index;
  int period;
  int phase;
  /* FIXME : char xdr_tsp_t[4]; */

};

typedef struct TSP_consumer_symbol_requested_t TSP_consumer_symbol_requested_t;

struct TSP_consumer_symbol_requested_list_t
{
  int len;
  TSP_consumer_symbol_requested_t* val;
};

typedef struct TSP_consumer_symbol_requested_list_t TSP_consumer_symbol_requested_list_t;

struct TSP_consumer_symbol_info_t
{
  char* name;
  int index;
  /* FIXME : char xdr_tsp_t[4]; */
  /* FIXME :  u_int dimension */
};

typedef struct TSP_consumer_symbol_info_t TSP_consumer_symbol_info_t;


struct TSP_consumer_symbol_info_list_t
{
  int len;
  TSP_consumer_symbol_info_t* val;
};

typedef struct TSP_consumer_symbol_info_list_t TSP_consumer_symbol_info_list_t;

struct TSP_consumer_information_t
{
  /* FIXME : int provider_timeout;*/
  double base_frequency;
  int max_period;
  int max_client_number;
  int current_client_number;
  TSP_consumer_symbol_info_list_t symbols;
};

typedef struct TSP_consumer_information_t TSP_consumer_information_t;

/*----------------------------------- TYPES ---------------------------------*/

/** Opaque handle type for provider */
typedef  void* TSP_provider_t;

/*--------------------------------- FUNCTIONS -------------------------------*/

/**
* Initialisation for TSP librarie.
* Call this function before using main(argc, argv) function arguments. This function
* removes the arguments it knows from the argument list, leaving anything
* it does not recognize for your application to parse or ignore. 
* This creates a set of standard arguments accepted by all TSP applications.
* @param argc Use the argc main arg before using it
* @param argc Use the argv main arg before using it
* @return TRUE = OK
*/
int TSP_consumer_init(int* argc, char** argv[]);

/**
* Open all found providers on the given host.
* This function allocate teh provider array, that's why TSP_consumer_close_all
* must be symetrically called at the end of the program to free the allocated resources.
* The consumer may use the TSP_consumer_get_server_info to retreive information about
* each provider, so as to choose what provider(s) will be left opened.
* The consumer may then close providers with the TSP_consumer_close function.
* @param host_name Name of host on which the providers must be opened
* @param providers Array of found providers
* @param nb_providers Total number of providers in 'providers' array
*/
void TSP_consumer_open_all(const char* host_name,
			   TSP_provider_t** providers,
			   int* nb_providers);

/** 
 * Request provider information.
 * Ask the provider information including the list of symbols
 * that can be asked.
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */				  
const TSP_otsp_server_info_t* TSP_consumer_get_server_info(TSP_provider_t provider);			  

/** 
 * Ask the provider for a new consumer session.
 * This function must be called and must succed for the program to be able to call
 * other TSP_REQUEST_* functions.
 * @param provider The provider handle
 * @param custom_argc Custom provider data stream initialisation (optional : set 0)
 * @param custom_argv Custom provider data stream initialisation (optional : set 0)
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
 * initialisation arguments on the provider command line, the user command line will be ignored
 * and your arguments call will be used
 * - If you call the function with custom_argc=1 (with any string in custom_argv[0]) this
 * means "overide consumer command line data stream initialisation, and set 0 parameters".
 * It means that the provided data stream initialisation will be done with 0 parameters.
 * It is a way to prevent the user from providing initialisation parameters thrue the command
 * line.
 */
int TSP_consumer_request_open(TSP_provider_t provider, int custom_argc, char* custom_argv[]);

/** 
 * Request provider information.
 * Ask the provider informations about several parameters, including
 * the available symbol list that can be asked.
 * This function should be called once per session
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */				  
int TSP_consumer_request_information(TSP_provider_t provider);


/** 
 * Retrieve the provider list.
 * Get the provider information asked by TSP_consumer_request_information
 * This function may be called multiple times per session
 * @param provider The provider handle
 * @return the provider information structure
 */				  
const TSP_consumer_information_t* TSP_consumer_get_information(TSP_provider_t provider);

/** 
 * Ask the provider for a list of symbols.
 * When the list of available symbols was retreived via TSP_consumer_request_information,
 * and TSP_consumer_get_information, the consumer may choose some symbols, set their phase
 * and period and call this function to prepare the provider to sample these symbols
 * @param provider The provider handle
 * @param symbols The request symbols list
 * @return TRUE or FALSE. TRUE = OK.
 */				  
int TSP_consumer_request_sample(TSP_provider_t provider,
				TSP_consumer_symbol_requested_list_t* symbols);

/** 
 * Prepare the sampling sequence.
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */				      
int TSP_consumer_request_sample_init(TSP_provider_t provider);    


/*FIXME : ajouter des codes d'erreur (eof, symbol manqué, symbol ne sera plus recu ),
virer le new_sample quand il y aura un code de retour !!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/** 
 * Read a symbol.
 * @param provider The provider handle
 * @param sample The retreived symbol
 * @return TRUE or FALSE. TRUE = OK. !!!!!!!!!!!!! A MODIFIER
 */				          
int TSP_consumer_read_sample(TSP_provider_t provider,
                    TSP_sample_t* sample,
                    int* new_sample);


int TSP_consumer_request_close(TSP_provider_t provider);


void TSP_consumer_close_all(TSP_provider_t providers[]);				  
void TSP_consumer_close(TSP_provider_t provider);

/**
* End of TSP librairie use
* call this function when you are done with the librairie
*/
void TSP_consumer_end();

    
#endif /* _TSP_CONSUMER_H */
