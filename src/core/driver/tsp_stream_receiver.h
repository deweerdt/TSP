/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_stream_receiver.h,v 1.3 2002-12-18 16:27:28 tntdev Exp $

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
