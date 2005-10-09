/*!  \file 

$Id: glue_sserver.h,v 1.20 2005-10-09 23:01:23 erk Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the glue server : the data producer

-----------------------------------------------------------------------
 */

#ifndef _TSP_GLUESERVER_H
#define _TSP_GLUESERVER_H

#include "tsp_prjcfg.h"
#include "tsp_datastruct.h"  /* FIXME : just for TSP_sample_symbol_info_list_t, and shouldn't use generated rpc struct */


/**
 * @defgroup GLUServer
 * @ingroup Provider
 * The GLU server is the part of the TSP provider that should be
 * coded each time the underlying 'real' sampled system 
 * (hardware card, simulator, ...) change.
 * The GLU interface should be implemented in order to combine
 * the TSP provider lib in a real TSP provider.
 * @{
 */

#define TSP_ASYNC_WRITE_ALLOWED   1
#define TSP_ASYNC_WRITE_FORBIDDEN 0


/** GLU server type */
typedef enum GLU_server_type_t
{
  /** 
   * GLU is active. Means that the data are continuously produced
   * and must be read at the same pace (or faster) by the provider.
   * When GLU is active their shouldn'tr be more that one 
   * GLU instance running by provider.
   * @see GLU_get_instance.
   */
  GLU_SERVER_TYPE_ACTIVE,

  /** 
   * GLU is passive. Means that the data are produced only when the
   * provider ask for them (typically File Based Glu/Provider)
   */
  GLU_SERVER_TYPE_PASSIVE
} GLU_server_type_t;


/**
 * The state of the GLU.
 */
typedef enum GLU_get_state_t
{
  /** new item available */
  GLU_GET_NEW_ITEM,

  /** no item available */
  GLU_GET_NO_ITEM,

  /** end of data stream */
  GLU_GET_EOF,

  /** end of data stream, and GLU list of symbols changed, all
      client will have to ask fo the new symbol list */
  GLU_GET_RECONF,

  /** Data were lost in GLU internals ; 
      The provider might be too slow and/or the GLU too fast, 
      and/or the CPU overloaded : all clients will be notified
      by the provider */
  GLU_GET_DATA_LOST

} GLU_get_state_t;

/** 
 * Item used to transmit a data sample 
 */
typedef struct glu_item_t
{
    time_stamp_t time;
    int          provider_global_index;
    double       value;

} glu_item_t;

/*
 * Define the GLU handle data type
 */
struct GLU_handle_t; /* necessary forward declaration */
/** 
 * GLU server name getter.
 * This string will be used by the consumers to get informations
 * about a provider. It must be meaningfull (ex : "Temperatures Sampler" ) 
 */
typedef char*              (* GLU_get_server_name_ft   )(struct GLU_handle_t* this);
/**
 * GLU server type getter.
 * Will get the GLU type i.e. ACTIVE or PASSIVE.
 * @return GLU server type.
 */
typedef GLU_server_type_t  (* GLU_get_server_type_ft   )(struct GLU_handle_t* this);
/**
 * Default GLU base frequency.
 * @return GLU base frequency (Hz)
 */
typedef double             (* GLU_get_base_frequency_ft)(struct GLU_handle_t* this);
/**
 * GLU instance creation.
 * This function will be called by the provider for each consumer
 * connection.
 * This function is the place where the GLU must be instanciated.
 * 
 * - When the GLU is type ACTIVE, it must be a singleton and must
 * be instanciated once ( i.e. the first time this function is called).
 * When the instanciation of an ACTIVE GLU succed this function always
 * returns the value GLU_GLOBAL_HANDLE (and for all the subsequent calls
 * that should not create a new GLU, and always should always succed).
 * 
 * - When the GLU is type PASSIVE, for each consumer connection a new
 * GLU must be instanciated ; use the GLU_handle_t type to reference your
 * GLU object and return this handle when the instanciation succed
 * 
 * @param custom_argc Number of consumer initialisation elements
 * (when the consumer do not provide any parameters
 * this parameter is equal to fallback_argc of GLU_init)
 * @param custom_argv Initialisation elements
 * The first usefull parameter is always
 * fallback_argv[1], so as the getopt* functions may be used to
 * parse those parameters (when the consumer do not provide any parameters
 * this parameter is equal to fallback_argv of GLU_init)
 * @param error_info When the function fail the GLU may use this parameter
 * to return an error string that will be transmited to the consumer
 * to get detailed information about the error. 
 * @return The created GLU handle. Returns 0 when the function fail
 */
typedef struct GLU_handle_t*      (* GLU_get_instance_ft      )(struct GLU_handle_t* this, int custom_argc, char* custom_argv[], char** error_info);
/**
 * GLU initialization function.
 *
 * The code in this function will be called once,
 * and may do any usefull global check and initialization. 
 * Usually the data stream should not be started in this function
 * as there is no connected consumer when this function is
 * called. To start your data stream, wait for the first GLU_get_instance
 * function that will be called for each consumer connection
 *
 * The parameters for this function come from the parameters
 * that may have been provided thrue the command line that was used
 * to launch the provider. In this function the GLU should not use these
 * parameters, but it should check their value, as these value
 * will be used as fallback parameters for the GLU_get_instance function
 * each time a consumer connect to the provider and does not provide
 * its own parameters
 *
 * the GLU may then for this function return :
 * - FALSE if it does not want to be started ( ex : when the
 * parameters are incorrect, or when there is no parameters
 * at all and this GLU exepects parameters). The provider
 * will stop.
 * - TRUE if everything is OK
 *
 * NOTE1 : Sometimes, the the only way to check the provided
 * parameters is to start the data stream. If so,
 * the data stream must not be started a second time 
 * in the get_instance function.
 * 
 * NOTE2 : This string is provided in this function for fallback
 * test only and should not be memorized by the GLU server
 *
 * NOTE3 : When there is not fallback stream at all, the value
 * of fallback_argv  is 0. It is up to the GLU server
 * to decide if it is acceptable for the provider to be 
 * started with no fallback stream at all (it usually is
 * acceptable)
 *
 * @param this, the GLU handle
 * @param fallback_argc Number of fallback initialisation elements as used in main(argc, argv)
 * @param fallback_argv Fallback initialisation elements as used in main(argc, argv)
 * The first usefull parameter is always
 * fallback_argv[1], so as the getopt* functions may be used to
 * parse those parameters
 * @return Return FALSE if a fatal error occur during initialisation
 * or if there is a probleme with the provided parameters
 */
typedef int                (* GLU_init_ft              )(struct GLU_handle_t* this,int fallback_argc, char* fallback_argv[]);
/**
 * Start the loop that will push data to datapool with push_next_item
 * 
 * @return status 
 */
typedef void*              (* GLU_run_ft               )(void* this);
/**
 * Start the loop that will push data to datapool with push_next_item
 * 
 * @return status 
 */
typedef int                (* GLU_start_ft             )(struct GLU_handle_t* this);
/** 
 * GLU_get_provider_global_indexes provider global indexes corresponding
 * to given symbol list. Used to validate client provided symbol list
 * @param symbol_list IN the symbol list to validate
 * @param pg_indexes OUT array containing corresponding provider global indexes or -1 if not found 
 * @return TRUE if all symbol found, else return FALSE 
 */
typedef int                (* GLU_get_pgi_ft           )(struct GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes);
/**
 * List of symbols managed by the GLU.
 * @param this Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param symbol_list List of symbols
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int                (* GLU_get_ssi_list_ft  )(struct GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list);

/**
 * Filtered list of symbols managed by the GLU.
 * @param this Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @param filter_kind the filter kind
 * @param filter_string the filter string
 * @param symbol_list List of symbols
 * @return TRUE of FALSE. TRUE = OK;
 */
typedef int                (* GLU_get_filtered_ssi_list_ft)(struct GLU_handle_t* this, int filter_kind, char* filter_string, TSP_sample_symbol_info_list_t* symbol_list);

/**
 * Get the number of symbols managed by the GLU.
 * @param this Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
 * @return the number of symbols;
 */
typedef int                (* GLU_get_nb_symbols_ft  )(struct GLU_handle_t* this);

/**
 * GLU asynchronous sample write.
 * This function will be called by the provider for TSP_async_sample_write.
 * The convention fixed is : if we can write with this function the sample
 * represented by its provider_global_index (correct pgi and authorization by
 * the GLU), the number returned is >0; else the number returned is <= 0
 *
 * @param pgi        IN, the index of the symbol concerned by the writing
 * @param value_ptr  IN, the new value of the symbol
 * @param value_size IN, the size of the value.
 * @return >0 on success <=0 on failure
 */
typedef int                (* GLU_async_sample_write_ft)(struct GLU_handle_t* this, int pgi, void* value_ptr, int value_size);

/**
 * GLU asynchronous sample read.
 * This function will be called by the provider for each TSP_async_sample_read.
 * The convention fixed is : if we can write with this function the sample
 * represented by its provider_global_index (correct pgi and authorization by
 * the GLU), the number returned is >0; else the number returned is <= 0
 *
 * @param pgi        IN,  the index of the symbol concerned by the writing
 * @param value_ptr  OUT, the new value of the symbol
 * @param value_size INOUT, the size of the value.
 * @return >0 on success <=0 on failure
 */
typedef int                (* GLU_async_sample_read_ft)(struct GLU_handle_t* this, int pgi, void* value_ptr, int* value_size);

                
/** 
 * GLU handle object.
 * This object represents a GLU instance.
 * It contains some minimal data and 
 * function pointer which are 'methods' 
 * for this object.
 * GLU creation function provides a pre-initialize GLU_handle_t 
 * structure with default implementation for some methods.
 * Specific GLU implementation may overrides thoses methods
 * with more efficient ones.
 */
typedef struct GLU_handle_t {

  pthread_t                 tid;            /**< The GLU thread Id */
  char*                     name;           /**< The GLU name */
  GLU_server_type_t         type;           /**< The GLU type */
  double                    base_frequency; /**< The provider base frequency */
  

  GLU_get_server_name_ft    get_name;           /**< name getter */
  GLU_get_server_type_ft    get_type;           /**< type getter */
  GLU_get_base_frequency_ft get_base_frequency; /**< base frequency getter */
  GLU_get_instance_ft       get_instance;       /**< instance getter */
  GLU_init_ft               initialize;
  GLU_run_ft                run;
  GLU_start_ft              start;
  GLU_get_pgi_ft                 get_pgi;
  GLU_get_ssi_list_ft            get_ssi_list;
  GLU_get_filtered_ssi_list_ft   get_filtered_ssi_list;
  GLU_get_nb_symbols_ft          get_nb_symbols; 
  GLU_async_sample_read_ft       async_read;
  GLU_async_sample_write_ft      async_write;
  
} GLU_handle_t;

/**
 * Create a GLU_handle. 
 * This will provide some default implementation for member function.
 * @param glu OUT, pointer to a GLU_handle pointer that will be allocated
 * @return TRUE if ok FALSE otherwise
 */
int32_t GLU_handle_create(GLU_handle_t** glu, const char* name, const GLU_server_type_t type, const double base_frequency);

/**
 * Destroy a GLU_handle.
 * for member function.
 * @param glu OUT, pointer to a GLU_handle pointer that will be allocated
 * @return TRUE if ok FALSE otherwise
 */
int32_t GLU_handle_destroy(GLU_handle_t** glu);

/* ====== You'll find hereafter some default implementation GLU methods ====== */

/** 
 * Default GLU server name.
 * @param this the GLU structure
 * @return GLU name.
 */
char* GLU_get_server_name_default(GLU_handle_t* this);

/**
 * Default GLU server type.
 * @param this the GLU structure
 * @return GLU server type.
 */
GLU_server_type_t GLU_get_server_type_default(GLU_handle_t* this);

/**
 * Default GLU base frequency.
 * @param this the GLU structure
 * @return GLU base frequency (Hz)
 */
double GLU_get_base_frequency_default(GLU_handle_t* this);

/**
 * Default GLU start.
 * @param this the GLU structure
 * @return true or false
 */
int GLU_start_default(GLU_handle_t* this);

/** 
 * Default GLU_get_pgi.
 * The default implementation use the mandatory GLU_get_sample_symbol_info_list
 * and does a linear search in it.
 * @param this IN, 
 * @param symbol_list IN the symbol list to validate
 * @param pg_indexes OUT array containing corresponding provider global indexes or -1 if not found 
 * @return TRUE if all symbol found, else return FALSE 
 */
int GLU_get_pgi_default(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes);

/**
 * Default GLU_get_instance.
 * The default implementation is only valid for an ACTIVE GLU.
 * In this case this function always return this.
 * PASSIVE GLU should reimplement this function.
 */ 
GLU_handle_t* GLU_get_instance_default(GLU_handle_t* this,
                                       int custom_argc,
	 		               char* custom_argv[],
			               char** error_info);

int  
GLU_get_nb_symbols_default(GLU_handle_t* this);

int 
GLU_get_filtered_ssi_list_default(GLU_handle_t* this, int filter_kind, char* filter_string, TSP_sample_symbol_info_list_t* symbol_list);

int 
GLU_async_sample_read_default(struct GLU_handle_t* this, int pgi, void* value_ptr, int* value_size);

int 
GLU_async_sample_write_default(struct GLU_handle_t* this, int pgi, void* value_ptr, int value_size);

/** @} */

#endif /*_TSP_GLUESERVER_H*/

/* Might be remove
int GLU_add_block(GLU_handle_t h_glu,int provider_global_index, xdr_and_sync_type_t type);
int GLU_commit_add_block(GLU_handle_t h_glu);
*/

