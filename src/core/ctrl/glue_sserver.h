/*!  \file 

$Id: glue_sserver.h,v 1.15 2003-03-13 18:04:04 yduf Exp $

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
Maintainer: tsp@astrium-space.com
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
 * the TSP provider lib int a real TSP provider.
 * @{
 */

/** GLU server type */
enum GLU_server_type_t
{
  /** GLU is active. Means that the data are continuously produced
      and must be read at the same pace (or faster)  by the provider */
  GLU_SERVER_TYPE_ACTIVE,

  /** GLU is active. Means that the data are produced only when the
      provider ask for them */
  GLU_SERVER_TYPE_PASSIVE
};
typedef enum GLU_get_state_t GLU_get_state_t;



enum GLU_get_state_t
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

};
typedef enum GLU_server_type_t GLU_server_type_t;

/** Item used to transmit a data sample */
struct glu_item_t
{
    time_stamp_t time;
    int provider_global_index;
    double value;

};

typedef struct glu_item_t glu_item_t;

/** Handle for a GLU instance */
typedef void* GLU_handle_t;


/** GLU global handle dummy value.
    For an active GLU, the function GLU_get_instance
    always returns GLU_GLOBAL_HANDLE, as for the active
    case there is always one single GLU in the system */ 
#define GLU_GLOBAL_HANDLE ((GLU_handle_t)0x1)



/** GLU server name.
    This string will be used by the consumers to get informations
    about a provider. It must be meaningfull (ex : "Temperatures Sampler" ) */
char* GLU_get_server_name(void);


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
* @param fallback_argc Number of fallback initialisation elements as used in main(argc, argv)
* @param fallback_argv Fallback initialisation elements as used in main(argc, argv)
* The first usefull parameter is always
* fallback_argv[1], so as the getopt* functions may be used to
* parse those parameters
* @return Return FALSE if a fatal error occur during initialisation
* or if there is a probleme with the provided parameters
*/
int GLU_init(int fallback_argc, char* fallback_argv[]);


/**
* GLU server type : ACTIVE or PASSIVE.
* @return GLU server type.
*/
GLU_server_type_t GLU_get_server_type(void);



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
 * @error_info When the function fail the GLU may use this parameter
 * to return an error string that will be transmited to the consumer
 * to get detailed information about the error. 
 * @return The created GLU handle. Returns 0 when the function fail
 */
GLU_handle_t GLU_get_instance(int custom_argc,
			      char* custom_argv[],
			      char** error_info);

/**
* List of symbols managed by the GLU.
* @param h_glu Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
* @param symbol_list List of symbols
* @return TRUE of FALSE. TRUE = OK;
*/
int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu, TSP_sample_symbol_info_list_t* symbol_list);


/**
* GLU base frequency
* @return GLU base frequency (Hz)
*/
double GLU_get_base_frequency(void);

/**
* Get next available sample item
* @param h_glu Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
* @param item The sample item
* @return status (see GLU_get_state_t values description)
*/
GLU_get_state_t GLU_get_next_item(GLU_handle_t h_glu,glu_item_t* item);

/**
* The GLU must forget all bufferized data.
* The provider may call this function to empty the GLU
* buffer data (it means that when GLU_get_next_item 
* is called just after this function, GLU_get_next_item
* cannot return GLU_GET_DATA_LOST as all the buffers of the GLU
* should be empty)
* This function is meaningless for a PASSIVE GLU (it should do nothing in this case)
* @param h_glu Handle for the GLU (when the GLU is ACTIVE, it is always equal to GLU_GLOBAL_HANDLE)
*/
void GLU_forget_data(GLU_handle_t h_glu);

/** @} */

#endif /*_TSP_GLUESERVER_H*/

/* Might be remove
int GLU_add_block(GLU_handle_t h_glu,int provider_global_index, xdr_and_sync_type_t type);

int GLU_commit_add_block(GLU_handle_t h_glu);
*/
