/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.h,v 1.3 2002-11-29 17:33:31 tntdev Exp $

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

TSP_data_receiver_t TSP_data_receiver_create(const char* data_address);
void TSP_data_receiver_prepare_stop(TSP_data_receiver_t _receiver);
void TSP_data_receiver_stop(TSP_data_receiver_t _receiver);
void TSP_data_receiver_destroy(TSP_data_receiver_t _receiver);

TSP_data_decoder_t TSP_data_receiver_get_double_decoder(void);

int TSP_data_receiver_get_double_encoded_size(void);

#endif /* _TSP_DATA_RECEIVER_H */
