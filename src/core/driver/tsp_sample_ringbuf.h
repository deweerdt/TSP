/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_sample_ringbuf.h,v 1.2 2002-10-01 15:36:20 galles Exp $

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

#include "tsp_ringbuf.h"


/* Used to add a dummy item in fifo for an eof notification */
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF   0xFFFFFFFF

/* Struct to get each sample */
struct TSP_sample_t
{
  time_stamp_t time;
  int provider_global_index;
  double user_value;


};

typedef struct TSP_sample_t TSP_sample_t;

/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_sample_ringbuf_t,TSP_sample_t);

                                       
#endif /* _TSP_SAMPLE_RINGBUF_H */
