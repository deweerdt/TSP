/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.h,v 1.5 2002-12-18 16:27:27 tntdev Exp $

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

Purpose   : Interface for the functions used to receive and decode the data
stream  for the asked symbols
-----------------------------------------------------------------------
 */

#ifndef _TSP_DATA_RECEIVER_H
#define _TSP_DATA_RECEIVER_H

#include "tsp_prjcfg.h"

#include "tsp_sample_ringbuf.h"
#include "tsp_group.h"

typedef  void* TSP_data_receiver_t;

/** Generic function to decode data */
typedef int(*TSP_data_decoder_t)(void* v, char* in_buf); 

int TSP_data_receiver_receive(TSP_data_receiver_t _receiver,
                              TSP_groups_t _groups,
                              TSP_sample_ringbuf_t* sample_fifo,
			      int* fifo_full) ;

TSP_data_receiver_t TSP_data_receiver_create(const char* data_address, TSP_sample_callback_t callback);
void TSP_data_receiver_prepare_stop(TSP_data_receiver_t _receiver);
void TSP_data_receiver_stop(TSP_data_receiver_t _receiver);
void TSP_data_receiver_destroy(TSP_data_receiver_t _receiver);

TSP_data_decoder_t TSP_data_receiver_get_double_decoder(void);

int TSP_data_receiver_get_double_encoded_size(void);


int TSP_data_receiver_set_read_callback(TSP_data_receiver_t _receiver,
					TSP_sample_callback_t callback);


#endif /* _TSP_DATA_RECEIVER_H */
