/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.h,v 1.2 2002-10-01 15:18:06 galles Exp $

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

typedef  void* TSP_data_sender_t;

/* FIXME : en release, ne pas faire la vérification de taille du buffer,
donc le prototype devra etre : typedef u_int(*TSP_data_encoder_t)(void* v, char* out_buf); */
/** Generic function to encode data */
typedef u_int(*TSP_data_encoder_t)(void* v, char* out_buf, u_int size); 

int TSP_data_sender_send(TSP_data_sender_t sender,
			 TSP_groups_t groups,
			 time_stamp_t time_stamp) ;

TSP_data_sender_t TSP_data_sender_create(void);

const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender);

TSP_data_encoder_t TSP_data_sender_get_double_encoder(void);

int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender);

int TSP_data_sender_send_eof(TSP_data_sender_t _sender);

#endif /* _TSP_DATA_SENDER_H */
