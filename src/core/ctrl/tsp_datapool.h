/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.h,v 1.1 2002-08-27 08:56:09 galles Exp $

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

int TSP_datapool_init(void);

int TSP_datapool_add_symbols(TSP_sample_symbol_info_list_t* symbols);

void* TSP_datapool_get_symbol_value
(int provider_global_index,
 xdr_and_sync_type_t type);

#endif _TSP_DATAPOOL_H
