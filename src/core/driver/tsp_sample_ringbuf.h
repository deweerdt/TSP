/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_sample_ringbuf.h,v 1.5 2002-12-03 16:14:18 tntdev Exp $

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

#include "tsp_consumer.h"
#include "tsp_ringbuf.h"


/* Used to add a dummy item in fifo for an eof notification (sent by the provider)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF   0xFFFFFFFF

/* Used to add a dummy item in fifo for an reconf notification (sent by the provider)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECONF   0xFFFFFFFE

/* Used to tell that a connection problem occured (added in the ringbuf by the data_receiver)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECEIVER_ERROR   0xFFFFFFFD

/* Used to tell that on the provider side, the GLU lost some data (provider too slow ?)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_GLU_DATA_LOST   0xFFFFFFFC

/* Used to tell that on the provider side, some data were lost for this consumer (consumer too slow ?),
   or network overload ? */
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_CONSUMER_DATA_LOST   0xFFFFFFFB

 



/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_sample_ringbuf_t,TSP_sample_t);

                                       
#endif /* _TSP_SAMPLE_RINGBUF_H */
