/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/Attic/glue_sserver.h,v 1.9 2002-11-19 13:09:23 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the glue server

-----------------------------------------------------------------------
 */

#ifndef _TSP_GLUESERVER_H
#define _TSP_GLUESERVER_H

#include "tsp_prjcfg.h"


#include "tsp_datastruct.h"

enum GLU_server_type_t
{
  GLU_SERVER_TYPE_ACTIVE,
  GLU_SERVER_TYPE_PASIVE
};

enum GLU_get_state_t
{
  GLU_GET_NEW_ITEM,
  GLU_GET_NO_ITEM,
  GLU_GET_EOF,
  GLU_GET_RECONF,

};

typedef enum GLU_get_state_t GLU_get_state_t;

typedef enum GLU_server_type_t GLU_server_type_t;

struct glu_item_t
{
    time_stamp_t time;
    int provider_global_index;
    double value;

};

typedef struct glu_item_t glu_item_t;

typedef void* GLU_handle_t;

#define GLU_GLOBAL_HANDLE ((GLU_handle_t)0x1)



/* Common functions*/

char* GLU_get_server_name(void);


/**
* GLU initialization function.
*
* The code in this function will be called once,
* and may do any usefull check and initialization. 
* Usually a datastream must not be started in this function
* as there is no connected consummer when this function
* called. To start your datastream, wait for the GLU_get_instance
* function
* @param fallback_stream_init This string is used by the GLU
* to initialize its data stream when the consumer does not
* provide its own stream_init. In this function, the GLU server
* may check that the fallback_stream_init string could work
* and helps start a new stream if needed.
* if it does not work, the GLU may then for this function return :
* - FALSE if it does not want to be started when the
* fallback stream does not work
* - TRUE if the fallback stream works, or if
* it is acceptable for the provider to be started
* with a wrong fallback stream.
* NOTE1 : Sometimes, the fallback stream can not be checked,
* and the only way is to start the data stream right now.
* If so, the data stream must not be started in the get_instance
* function.
* NOTE2 : This string is provided in this function for fallback
* test only and should not be memorized by the GLU server
* NOTE3 : When there is not fallback stream at all, the value
* of fallback_stream_init may be 0. It is up to the GLU server
* to decide if it is acceptable for the provider to be 
* started with no fallback stream at all (it usually is
* acceptable, but a consumer that does not provide its 
* own stream_init string will be rejected by the provider)
* @return Return FALSE if a fatal error occur during initialisation
* or if there is a probleme with the fallback_stream_init parameter
*/
int GLU_init(int fallback_argc, char* fallback_argv[]);

GLU_server_type_t GLU_get_server_type(void);

/* Functions with handle */

int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list);

int GLU_add_block(GLU_handle_t h_glu,int provider_global_index, xdr_and_sync_type_t type);

int GLU_commit_add_block(GLU_handle_t h_glu);

GLU_handle_t GLU_get_instance(int custom_argc, char* custom_argv[], char** error_info);

double GLU_get_base_frequency(void);


GLU_get_state_t GLU_get_next_item(GLU_handle_t h_glu,glu_item_t* item);





#endif /*_TSP_GLUESERVER_H*/
