/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_sample_ringbuf.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Interface for the creation of the n ring_buf used by the user of
the consumer to retreive the n asked symbols

-----------------------------------------------------------------------
 */

#ifndef _TSP_SAMPLE_RINGBUF_H
#define _TSP_SAMPLE_RINGBUF_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include "tsp_ringbuf.h"

/* Struct to get each sample */
struct TSP_sample_t
{
  time_stamp_t time;
  double user_value;

};

typedef struct TSP_sample_t TSP_sample_t;

/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_sample_ringbuf_t,TSP_sample_t);

void TSP_sample_ringbuf_create(
			       const TSP_sample_symbol_info_list_t* symbols, 
			       TSP_sample_ringbuf_t** sample_ringbuf);

                                       
#endif /* _TSP_SAMPLE_RINGBUF_H */
