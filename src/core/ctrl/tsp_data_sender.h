/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.h,v 1.6 2002-12-18 16:27:16 tntdev Exp $

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
Maintainer : tsp@astrium-space.com
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
void TSP_data_sender_stop(TSP_data_sender_t sender);
void TSP_data_sender_destroy(TSP_data_sender_t sender);

const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender);

TSP_data_encoder_t TSP_data_sender_get_double_encoder(void);

int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender);

int TSP_data_sender_send_msg_ctrl(TSP_data_sender_t _sender, TSP_msg_ctrl_t msg_ctrl);


#endif /* _TSP_DATA_SENDER_H */
