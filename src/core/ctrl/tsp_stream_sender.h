/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.h,v 1.1 2002-08-27 08:56:09 galles Exp $

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

typedef  void* TSP_stream_sender_t;

TSP_stream_sender_t TSP_stream_sender_create(void);

int TSP_stream_sender_send(TSP_stream_sender_t sender, const char *buffer, int bufferLen);

const char* TSP_stream_sender_get_data_address_string(TSP_stream_sender_t sender);

#endif /*TSP_STREAM_SENDER_H*/
