/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.h,v 1.4 2002-11-19 13:11:45 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the functions used to encode and send the data
stream  for the asked symbols

-----------------------------------------------------------------------
 */

#ifndef _TSP_DATA_SENDER_H
#define _TSP_DATA_SENDER_H

#include "tsp_prjcfg.h"

#include "tsp_group_algo.h"


/** Anonymous data sender object */
typedef  void* TSP_data_sender_t;


/* FIXME : en release, ne pas faire la vérification de taille du buffer,
donc le prototype devra etre : typedef u_int(*TSP_data_encoder_t)(void* v, char* out_buf); */
/** Generic function to encode data */
typedef u_int(*TSP_data_encoder_t)(void* v, char* out_buf, u_int size); 

int TSP_data_sender_send(TSP_data_sender_t sender,
			 TSP_groups_t groups,
			 time_stamp_t time_stamp) ;

TSP_data_sender_t TSP_data_sender_create(int fifo_size);

const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender);

TSP_data_encoder_t TSP_data_sender_get_double_encoder(void);

int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender);

int TSP_data_sender_send_msg_ctrl(TSP_data_sender_t _sender, TSP_msg_ctrl_t msg_ctrl);


#endif /* _TSP_DATA_SENDER_H */
