/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.h,v 1.10 2006-02-26 13:36:05 erk Exp $

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

Purpose   : Interface for the low level functions that send the data
stream  to the consumers. 

-----------------------------------------------------------------------
 */

#ifndef TSP_STREAM_SENDER_H
#define TSP_STREAM_SENDER_H

#include "tsp_prjcfg.h"
#include "tsp_ringbuf.h"

typedef  void* TSP_stream_sender_t;


/* ringbuf struct */
struct TSP_stream_sender_item_t
{
  int len;

  /** Do not remove pad, else the memory following
     the structure will not be 'double aligned'
     (FIXME : use an union ? )*/
  int pad;

};

typedef struct TSP_stream_sender_item_t TSP_stream_sender_item_t;

/** samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_stream_sender_ringbuf_t,TSP_stream_sender_item_t);

/** Get the memory following a TSP_stream_sender_item_t item */
#define TSP_STREAM_SENDER_ITEM_BUF(stream_sender_item) ((char*)( (stream_sender_item) + 1 ))

/**
 * Create a stream sender.
 * @param fifo_size Size of ringbuffer. Set fifo_size = 0 if you do not
 * want any ringbuffer
 * @param buffer_size Size of buffer used to prepare the data
 * @return The stream sender handle
 */
TSP_stream_sender_t TSP_stream_sender_create(int fifo_size, int buffer_size);

/**
 * Stop the stream sender.
 * @param sender The stream sender handle
 */
void TSP_stream_sender_stop(TSP_stream_sender_t sender);

/**
 * Destroy the stream sender.
 * @param sender The stream sender handle
 */
void TSP_stream_sender_destroy(TSP_stream_sender_t sender);

/**
 * Send some data on thrue data stream.
 * @param sender The stream sender handle.
 * @param buffer The data that must be sent
 * @param bufferLen The length of the data in buffer (bytes)
 * @return TRUE or FALSE. TRUE = OK
 */
int TSP_stream_sender_send(TSP_stream_sender_t sender, const char *buffer, int bufferLen);

/**
 * Get the address string to which the client must connect
 * to receive data.(format depends on the used protocol ; for
 * TCP/IP it is 'hostname : port')
 * @param sender The stream sender handle.
 * @return data address. An error return 0;
 */
const char* TSP_stream_sender_get_data_address_string(TSP_stream_sender_t sender);

/**
 * Is the client connected ?
 * @param sender The stream sender handle.
 * TRUE = client connected
 */
int TSP_stream_sender_is_client_connected(TSP_stream_sender_t sender);

/**
 * Is the client still connected ?
 * @param sender The stream sender handle.
 * TRUE = client still connected
 */
int TSP_stream_sender_is_connection_ok(TSP_stream_sender_t sender);

/**
 * The ringbuffer object.
 * @param sender The stream sender handle.
 * @return The address of the created ringbuf (meaningful when fifo_size > 0)
 */
TSP_stream_sender_ringbuf_t* TSP_stream_sender_get_ringbuf(TSP_stream_sender_t sender);

/**
 * The buffer object.
 * @param sender The stream sender handle.
 * @return The address of the created buffer (meaningful when fifo_size = 0)
 */
TSP_stream_sender_item_t* TSP_stream_sender_get_buffer(TSP_stream_sender_t sender);

#endif /*TSP_STREAM_SENDER_H*/
