/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_stream_receiver.h,v 1.2 2002-11-29 17:33:31 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Interface for the functions that receive the data
stream  from the procuder for the asked symbols. This layer is the network layer and uses sockets
-----------------------------------------------------------------------
 */

#ifndef TSP_STREAM_RECEIVER_H
#define TSP_STREAM_RECEIVER_H

#include "tsp_prjcfg.h"

typedef  void* TSP_stream_receiver_t;

TSP_stream_receiver_t TSP_stream_receiver_create(const char* data_address);
void TSP_stream_receiver_destroy(TSP_stream_receiver_t receiver);

void TSP_stream_receiver_prepare_stop(TSP_stream_receiver_t receiver);
void TSP_stream_receiver_stop(TSP_stream_receiver_t receiver);
int TSP_stream_receiver_is_stopped(TSP_stream_receiver_t receiver);

int TSP_stream_receiver_receive(TSP_stream_receiver_t receiver, char *buffer, int bufferLen);

#endif /*TSP_STREAM_RECEIVER_H*/
