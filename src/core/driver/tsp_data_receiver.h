/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.h,v 1.4 2002-12-05 10:55:22 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

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
