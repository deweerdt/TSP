/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.h,v 1.8 2002-12-05 10:54:13 tntdev Exp $

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
#include "glue_sserver.h"




int TSP_add_session(channel_id_t* new_channel_id, GLU_handle_t glu_h);

void TSP_session_init(void);

void TSP_session_close_session_by_channel(channel_id_t channel_id);

int TSP_session_add_symbols(TSP_sample_symbol_info_list_t* symbols);

int TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
						TSP_answer_sample_t* ans_sample,
						int use_global_datapool);
void TSP_session_create_symbols_table_by_channel_free_call(TSP_answer_sample_t* ans_sample);
void TSP_session_destroy_symbols_table_by_channel(channel_id_t channel_id);


/* FIXME : en faire un return int ? */
void TSP_session_all_session_send_data(time_stamp_t t);
void TSP_session_all_session_send_msg_ctrl(TSP_msg_ctrl_t msg_ctrl);

int TSP_session_send_data_by_channel(channel_id_t channel_id, time_stamp_t t);
int TSP_session_send_msg_ctrl_by_channel(channel_id_t channel_id, TSP_msg_ctrl_t msg_ctrl);

const char* TSP_session_get_data_address_string_by_channel(channel_id_t channel_id);

int TSP_session_create_data_sender_by_channel(channel_id_t channel_id, int no_fifo);
int TSP_session_destroy_data_sender_by_channel(channel_id_t channel_id, int stop_local_thread);

int  TSP_session_get_sample_symbol_info_list_by_channel(channel_id_t channel_id,
							TSP_sample_symbol_info_list_t* symbol_list);

int TSP_session_get_symbols_global_index_by_channel(channel_id_t channel_id,
						   TSP_sample_symbol_info_list_t* symbol_list);

int TSP_session_is_consumer_connected_by_channel(channel_id_t channel_id);

int TSP_session_get_garbage_session(channel_id_t* channel_id);




/**
 * Get current total number of session.
 * @return Total number of session
 */
int TSP_session_get_nb_session(void);

#endif /* _TSP_SESSION_H */
