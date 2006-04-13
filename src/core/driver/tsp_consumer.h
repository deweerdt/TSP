/*

$Id: tsp_consumer.h,v 1.31 2006-04-13 23:05:18 erk Exp $

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
Component  : Consumer

-----------------------------------------------------------------------

Purpose   : Main interface for the TSP consumer library

-----------------------------------------------------------------------
 */

#ifndef _TSP_CONSUMER_H
#define _TSP_CONSUMER_H

#include <stdio.h>

#include <tsp_abs_types.h>
#include <tsp_common.h>

/**
 * @defgroup TSP_ConsumerLib Consumer Core Library
 * @ingroup TSP_CoreLib
 * The Consumer module is the set of all 
 * consumer library interface.
 * @{
 */

/*------------------------------ ENUM ---------------------------------*/
 
/**
 * Status for a futur get_last_error function.
 * FIXME : not used for now. Anyway, this enum
 * must be completed
 */
enum TSP_consumer_status_t {
	TSP_CONSUMER_STATUS_OK,
	TSP_CONSUMER_STATUS_ERROR_UNKNOWN,
	TSP_CONSUMER_STATUS_ERROR_VERSION
};

/*-------------------------- STRUCTURES -------------------------------*/



/** Structure to get each sample */
struct TSP_sample_t
{
  int time;
  int provider_global_index;
  int32_t array_index;
  TSP_datatype_t type; 
 
  union {
    double        double_value;
    float         float_value;
    int8_t        int8_value;
    int16_t       int16_value;
    int32_t       int32_value;
    int64_t       int64_value;    
    uint8_t       uint8_value;
    uint16_t      uint16_value;
    uint32_t      uint32_value;
    uint64_t      uint64_value;
    char          char_value;
    unsigned char uchar_value;
    int8_t        raw_value;
  } uvalue;

};

typedef struct TSP_sample_t TSP_sample_t;

/** Callback function type used to receive a sample item.
FIXME : The callback function should transmit the error code
too ( ex :  typedef void (*TSP_sample_callback_t) (TSP_sample_t* sample, TSP_consumer_status_t status);  */
typedef void (*TSP_sample_callback_t) (TSP_sample_t* sample, void* user_data); 

typedef struct TSP_consumer_async_sample_t
{
    int provider_global_index;
    void * value_ptr;
    int    value_size;
} TSP_consumer_async_sample_t;



/** Opaque handle type for provider */
typedef  void* TSP_provider_t;

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

/*--------------------------------- FUNCTIONS -------------------------------*/

/** @} */

BEGIN_C_DECLS

/**
 * @addtogroup TSP_ConsumerLib
 * @{
 */

/**
 * Initialisation for TSP library.
 * Call this function before using main(argc, argv) function arguments. This function
 * removes the arguments it knows from the argument list, leaving anything
 * it does not recognize for your application to parse or ignore. 
 * This creates a set of standard arguments accepted by all TSP applications.
 * This function must be called once.
 * @param argc Use the argc main arg before using it
 * @param argv Use the argv main arg before using it
 * @return TRUE = OK
 */
int TSP_consumer_init(int* argc, char** argv[]);

/**
* End of TSP library use
* call this function when you are done with the librairy.
* This function must be called once.
*/
void TSP_consumer_end();


/*----
  TSP_consumer_connect_all
  TSP_consumer_disconnect_all
                       deprecated, use URL connection instead 
---*/

/**
* Connects to all found providers on the given host.
* The consumer may use the TSP_consumer_get_connected_name to retreive information about
* each provider, so as to choose what provider(s) will be left opened.
* @param host_name Name of host on which the providers must be opened
* @param providers Array of found providers
* @param nb_providers Total number of providers in 'providers' array
*/
void TSP_consumer_connect_all(const char* host_name,
			   TSP_provider_t** providers,
			   int* nb_providers);

/**
* Disconnected all found providers.
* @param providers Array of providers from which we want to be disconnected
*/
void TSP_consumer_disconnect_all(TSP_provider_t providers[]);				 

/**
* Connects to a provider based on a given URL.
* @param url   URL to the provider that must be opened
*              Possible formats for URL are defined in following macro

*/
#define TSP_URL_FORMAT_USAGE  "\
TSP URL format is defined by <PROTOCOL://HOST/SERVER:PORT>           \n\
\tPROTOCOL   if omitted, is defaulted to 'rpc'                       \n\
\tHOST       if omitted, is defaulted to 'localhost'                 \n\
\tSERVER     if omitted, find any TSP server name                    \n\
\tPORT       if omitted, find first TSP server number                \n\
\tSimplified possible syntaxes :                                     \n\
\t\tPROTOCOL://HOST/SERVER     = PROTOCOL://HOST/SERVER:(find first)         \n\
\t\tPROTOCOL://HOST/:0         = PROTOCOL://HOST/(find any):0                \n\
\t\tPROTOCOL://HOST            = PROTOCOL://HOST/(find any):(find first)     \n\
\t\tPROTOCOL:// or PROTOCOL    = PROTOCOL://localhost/(find any):(find first)\n\
\t\tvoid, /, //, /// or :///   = rpc://localhost/(find any):(find first)     \n\
\tOthers may have unpredictable results .. \n"

TSP_provider_t TSP_consumer_connect_url(const char *url);


/** 
 * Request provider name.
 * This string should provide some information for a given  provider.
 * See it as a naming service.
 * @param provider The provider handle
 * @return The provider name
 */				  
const char* TSP_consumer_get_connected_name(TSP_provider_t provider);		

/**
 * Disconnect one given provider.
 * @param provider IN, the provider to be disconnected from.
 */ 
void TSP_consumer_disconnect_one(TSP_provider_t provider);


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
 * Return the TSP channel Id associated with this
 * TSP session if a TSP Request Open has already been sent to this
 * provider.
 * @param provider the TSP session to provider
 * @return the channel Id  if the session is open -1 if session not opened. 
 */
uint32_t
TSP_consumer_get_channel_id(TSP_provider_t provider);

/**
 * Close the session.
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */
int TSP_consumer_request_close(TSP_provider_t provider);


/** 
 * Request provider information.
 * Ask the provider informations about several parameters, including
 * the available symbol list that can be asked.
 * This function should be called multiple times only to refresh
 * the structure returned by the TSP_consumer_get_information function.
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */				  
int TSP_consumer_request_information(TSP_provider_t provider);

/** 
 * Request provider information.
 * Ask the provider informations about several parameters, including
 * the available symbol list that can be asked.
 * This function should be called multiple times only to refresh
 * the structure returned by the TSP_consumer_get_information function.
 * @param provider The provider handle
 * @param filter_kind the kind of filter
 * @param filter_string the filter string
 * @return TRUE or FALSE. TRUE = OK.
 */				  
int TSP_consumer_request_filtered_information(TSP_provider_t provider, int filter_kind, char* filter_string);

/** 
 * Request provider extended information.
 * Ask the provider informations about several parameters, including
 * the available symbol list that can be asked.
 * This function should be called multiple times only to refresh
 * the structure returned by the TSP_consumer_get_extended_information function.
 * @param provider The provider handle
 * @param filter_kind the kind of filter
 * @param filter_string the filter string
 * @return TRUE or FALSE. TRUE = OK.
 */
int32_t 
TSP_consumer_request_extended_information(TSP_provider_t provider, int32_t* pgis, int32_t pgis_len);

/** 
 * Retrieve the provider extended information list.
 * Get the provider information asked by TSP_consumer_request_information
 * This function may be called multiple times per session
 * @param provider The provider handle
 * @return the provider extended information structure
 */
const TSP_sample_symbol_extended_info_list_t*  
TSP_consumer_get_extended_information(TSP_provider_t provider);

/** 
 * Retrieve the provider list.
 * Get the provider information asked by TSP_consumer_request_information
 * This function may be called multiple times per session
 * @param provider The provider handle
 * @return the provider information structure
 */				  
const TSP_answer_sample_t* TSP_consumer_get_information(TSP_provider_t provider);

/** 
 * Ask the provider for a list of symbols.
 * When the list of available symbols was retreived via TSP_consumer_request_information,
 * and TSP_consumer_get_information, the consumer may choose some symbols, set their phase
 * and period and call this function to prepare the provider to sample these symbols.
 * The symbol is retrieved by name. The 'index' member of the symbols->val structure is not used
 * (to retreive the provider global index of a symbol uses the TSP_consumer_get_requested_sample
 * function after this function call).
 * This function should be called multiple times only to refresh
 * the structure returned by the TSP_consumer_get_requested_sample function.
 * @param provider The provider handle
 * @param symbols The request symbols list
 * @return TRUE or FALSE. TRUE = OK.
 */				  
int TSP_consumer_request_sample(TSP_provider_t provider,
				TSP_sample_symbol_info_list_t* symbols);

/** 
 * Retrieve the symbols requested list.
 * The function TSP_consumer_request_sample must be called first.
 * This function may be called multiple times per session.
 * @param provider The provider handle
 * @return the requested symbols list.
 */				  
const TSP_sample_symbol_info_list_t* TSP_consumer_get_requested_sample(TSP_provider_t provider);

/** 
 * Prepare and start the sampling sequence.
 * @param provider The provider handle
 * @param callback Address of a callback function that must be called
 * @param user_data pointer on any custom user data ( set 0 if not used)
 * do not want to use any callback function, and use the TSP_consumer_read_sample
 * function instead. DO NOT USE BOTH. Using the TSP_consumer_read_sample function
 * is easier as you do not have to deal with multi-thread problems, but the callback
 * function is CPU friendlier (theoricaly at least...)
 * @return TRUE or FALSE. TRUE = OK.
 */				      
int TSP_consumer_request_sample_init(TSP_provider_t provider, TSP_sample_callback_t callback, void* user_data);    

/** 
 * Stop and destroy the sampling sequence
 * @param provider The provider handle
 * @return TRUE or FALSE. TRUE = OK.
 */				      
int TSP_consumer_request_sample_destroy(TSP_provider_t provider);    

/** 
 * Read a sample symbol.
 * @param provider The provider handle
 * @param sample The returned symbol if there is one
 * @param new_sample When TRUE, there is a new sample, else the sample value is
 *                    meaningless
 * @return TRUE or FALSE. FALSE = There is an error (but we can not know it for now).
 * @todo
 *  - Some kind of get_last_error func must be implemented to read
 *    the error codes (EOF, RECONF ... ).
 *  - When the other types will be implemented (RAW, STRING) the TSP_sample_t
 *    type will not work anymore as it is double specific for now.
 */				          
int TSP_consumer_read_sample(TSP_provider_t provider,
			     TSP_sample_t* sample,
			     int* new_sample);


/**
 * Send a TSP asynchronous sample write request.
 * @param[in] provider the TSP session onto we want to send the request
 * @param[in] async_sample_write the async write message itself
 * @return TSP_STATUS_OK on success other TSP_STATUS_ERROR on failure.
 */
int32_t 
TSP_consumer_request_async_sample_write(TSP_provider_t provider,
					TSP_consumer_async_sample_t* async_sample_write);

/**
 * Send a TSP asynchronous sample read request.
 * @param[in] provider the TSP session onto we want to send the request
 * @param[in,out] async_sample_read the async read message itself
 *                on entry describes the symbol to read
 *                on return contains the read value (if success)
 * @return TSP_STATUS_OK on success other TSP_STATUS_ERROR on failure.
 */
int32_t 
TSP_consumer_request_async_sample_read(TSP_provider_t provider,
				       TSP_consumer_async_sample_t* async_sample_read);

/**
 * Print the symbols flagged as invalid
 * in the provided sample symbol info list.
 * @param[in]  fs the STDIO file stream
 * @param[in]  symbol the list of symbol to analyse
 * @param[in]  tsp_provider_url the provider TSP URL.
 */
void TSP_consumer_print_invalid_symbols(FILE* fs, 
					TSP_sample_symbol_info_list_t* symbols, 
					const char* tsp_provider_url);

/** @} end group TSP_ConsumerLib */ 

END_C_DECLS
    
#endif /* _TSP_CONSUMER_H */
