/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.h,v 1.4 2002-11-29 17:26:16 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the functions that read data from the sample
server, and for each opened session, ask the session to send its data to its
consumer

-----------------------------------------------------------------------
 */

#ifndef _TSP_DATAPOOL_H
#define _TSP_DATAPOOL_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include "glue_sserver.h"

typedef  void* TSP_datapool_t;

/*int TSP_global_datapool_init(void);*/

TSP_datapool_t TSP_local_datapool_allocate(channel_id_t session_channel_id, int symbols_number, GLU_handle_t h_glu );
void TSP_local_datapool_destroy(TSP_datapool_t datapool);

TSP_datapool_t TSP_global_datapool_get_instance(void);

int TSP_global_datapool_add_symbols(TSP_sample_symbol_info_list_t* symbols);

void*
TSP_datapool_get_symbol_value(TSP_datapool_t datapool, 
			      int provider_global_index,
			      xdr_and_sync_type_t type);


int TSP_local_datapool_start_thread(TSP_datapool_t datapool);

#endif _TSP_DATAPOOL_H
