/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_stream_receiver.h,v 1.4 2002-12-24 14:14:25 tntdev Exp $

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
stream  from the producer for the asked symbols. This layer is the network layer and uses sockets
-----------------------------------------------------------------------
 */

#ifndef TSP_STREAM_RECEIVER_H
#define TSP_STREAM_RECEIVER_H

#include "tsp_prjcfg.h"

/** The receiver handle */
typedef  void* TSP_stream_receiver_t;

/**
 * Creation of a stream receiver object.
 * @param data_address The data adresse encoder by the stream_sender on the
 * provider side (actually this string is 'hostname:port', but could be anything
 * else whith an other protocol)
 * @return The receiver handle. O when an error occured.
 */
TSP_stream_receiver_t TSP_stream_receiver_create(const char* data_address);

/**
 * Destroy a stream receiver object.
 * @param receiver handle that must be destroyed
 */
void TSP_stream_receiver_destroy(TSP_stream_receiver_t receiver);

/**
 * Prepare the receiver to stop.
 * This function set the receiver in a state, where the it does not
 * complain if the socket is broken by the provider stream sender.
 * @param receiver handle
 */
void TSP_stream_receiver_prepare_stop(TSP_stream_receiver_t receiver);

/**
 * Stop receiving data.
 * @param receiver handle
 */
void TSP_stream_receiver_stop(TSP_stream_receiver_t receiver);

/**
 * Tell if a receiver was stopped.
 * @param receiver handle
 * @return TRUE of FALSE. TRUE = receiver stopped
 */
int TSP_stream_receiver_is_stopped(TSP_stream_receiver_t receiver);

/**
 * Receiver a data packet.
 * @param receiver handle
 * @param buffer The data reception buffer
 * @param bufferLen The length of data (in bytes) in the buffer
 * @return TRUE of FALSE. TRUE = OK.
 */
int TSP_stream_receiver_receive(TSP_stream_receiver_t receiver, char *buffer, int bufferLen);

#endif /*TSP_STREAM_RECEIVER_H*/
