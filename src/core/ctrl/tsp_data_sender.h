/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.h,v 1.8 2002-12-24 14:14:18 tntdev Exp $

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


/** Generic function type to encode data */
typedef u_int(*TSP_data_encoder_t)(void* v, char* out_buf, u_int size); 

/**
 * Send data to the consumer.
 * @param sender The data sender handle.
 * @param groups The groups handle that will be used to encode the data stream
 * @param time_stamp Time stamp for these data
 * @return TRUE or FALSE. TRUE = OK
 */
int TSP_data_sender_send(TSP_data_sender_t sender,
			 TSP_groups_t groups,
			 time_stamp_t time_stamp) ;

/**
 * Send a message control to the consumer.
 * @param sender The data sender handle.
 * @param sender The message control the must be sent.
 * @return TRUE or FALSE. TRUE = OK
 */
int TSP_data_sender_send_msg_ctrl(TSP_data_sender_t _sender, TSP_msg_ctrl_t msg_ctrl);

/**
 * Create a data sender.
 * @param fifo_size If fifo_size > 0, a ringbuffer will be created with a fifo_size
 * depth, to send the data
 * @param max_group_size Size of the bigger group (used to calculate the buffer size )
 * @return The created data sender handle
 */
TSP_data_sender_t TSP_data_sender_create(int fifo_size, int max_group_size);

/**
 * Stop a data sender.
 * @param sender The data sender handle.
 */
void TSP_data_sender_stop(TSP_data_sender_t sender);

/**
 * Destroy a data sender.
 * @param sender The data sender handle.
 */
void TSP_data_sender_destroy(TSP_data_sender_t sender);

/**
 * Get the address string to which the client must connect
 * to receive data.(format depends on the used protocol ; for
 * TCP/IP it is 'hostname : port')
 * @param sender The data sender handle.
 * @return data address. An error returns 0;
 */
const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender);

/**
 * Returns the function used to encode a double type.
 * This value is stored in the group table to encode the data
 * as fast as possible. FIXME : manage more types : RAW, STRING ...
 * @return The double encoder function
 */
TSP_data_encoder_t TSP_data_sender_get_double_encoder(void);

/**
 * Is the consumer connected to the data sender ?
 * @param sender The data sender handle.
 * @return TRUE or FALSE. TRUE = OK
 */
int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender);




#endif /* _TSP_DATA_SENDER_H */
