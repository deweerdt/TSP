/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.h,v 1.3 2002-10-09 07:45:21 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

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
  int len;

  char buf[TSP_STREAM_SENDER_RINBUF_ITEM_SIZE];
};

typedef struct TSP_stream_sender_item_t TSP_stream_sender_item_t;

/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_stream_sender_ringbuf_t,TSP_stream_sender_item_t);


TSP_stream_sender_t TSP_stream_sender_create(int fifo_size);

int TSP_stream_sender_send(TSP_stream_sender_t sender, const char *buffer, int bufferLen);

const char* TSP_stream_sender_get_data_address_string(TSP_stream_sender_t sender);

int TSP_stream_sender_is_client_connected(TSP_stream_sender_t sender);

TSP_stream_sender_ringbuf_t* TSP_stream_sender_get_ringbuf(TSP_stream_sender_t sender);

#endif /*TSP_STREAM_SENDER_H*/
