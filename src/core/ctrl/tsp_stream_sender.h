/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.h,v 1.7 2002-12-18 16:27:18 tntdev Exp $

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

Purpose   : Implementation for the functions that send the data
stream  from the producer for the asked symbols. This layer is the network layer and uses sockets

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
  /* buf must be the first element of this structure (alignment stuff )*/
  char buf[TSP_STREAM_SENDER_RINBUF_ITEM_SIZE];

  int len;

  /* Do not remove that, else the next buf item will not 'double aligned'*/
  int pad;

};

typedef struct TSP_stream_sender_item_t TSP_stream_sender_item_t;

/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_stream_sender_ringbuf_t,TSP_stream_sender_item_t);


TSP_stream_sender_t TSP_stream_sender_create(int fifo_size);
void TSP_stream_sender_stop(TSP_stream_sender_t sender);
void TSP_stream_sender_destroy(TSP_stream_sender_t sender);

int TSP_stream_sender_send(TSP_stream_sender_t sender, const char *buffer, int bufferLen);

const char* TSP_stream_sender_get_data_address_string(TSP_stream_sender_t sender);

int TSP_stream_sender_is_client_connected(TSP_stream_sender_t sender);
int TSP_stream_sender_is_connection_ok(TSP_stream_sender_t sender);

TSP_stream_sender_ringbuf_t* TSP_stream_sender_get_ringbuf(TSP_stream_sender_t sender);

#endif /*TSP_STREAM_SENDER_H*/
