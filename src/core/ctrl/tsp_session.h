/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the object TSP_session_t that embody the
opened session from a client

-----------------------------------------------------------------------
 */

#ifndef _TSP_SESSION_H
#define _TSP_SESSION_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"

int TSP_add_session(channel_id_t* new_channel_id);

void TSP_session_init(void);

int TSP_close_session_by_channel(channel_id_t channel_id);

int TSP_session_add_symbols(TSP_sample_symbol_info_list_t* symbols);

void TSP_session_free_create_symbols_table_call(TSP_answer_sample_t** ans_sample);

int TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
						TSP_answer_sample_t** ans_sample);

void TSP_session_all_session_send_data(time_stamp_t t);

const char* TSP_session_get_data_address_string_by_channel(channel_id_t channel_id);

#endif /* _TSP_SESSION_H */
