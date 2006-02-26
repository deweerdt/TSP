/*

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_sample_ringbuf.h,v 1.8 2006-02-26 13:36:05 erk Exp $

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

Purpose   : Interface for the creation of the ringbuf used
to store the incoming samples and read thrue the consumer API

-----------------------------------------------------------------------
 */

#ifndef _TSP_SAMPLE_RINGBUF_H
#define _TSP_SAMPLE_RINGBUF_H

#include "tsp_prjcfg.h"

#include "tsp_consumer.h"
#include "tsp_ringbuf.h"

/* FIXME : remove DUMMY from the names ... It is historical */

/* These defined are use to insert message in the ringbuf that the API
consumer reads, unsing the TSP_consumer_read_sample function */

/** Used to add a dummy item in fifo for an eof notification (sent by the provider)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF   0xFFFFFFFF

/** Used to add a dummy item in fifo for an reconf notification (sent by the provider)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECONF   0xFFFFFFFE

/** Used to tell that a connection problem occured (added in the ringbuf by the data_receiver)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECEIVER_ERROR   0xFFFFFFFD

/** Used to tell that on the provider side, the GLU lost some data (provider too slow ?)*/
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_GLU_DATA_LOST   0xFFFFFFFC

/** Used to tell that on the provider side, some data were lost for this consumer (consumer too slow ?),
   or network overload ? */
#define TSP_DUMMY_PROVIDER_GLOBAL_INDEX_CONSUMER_DATA_LOST   0xFFFFFFFB



/* samples ring buf */
RINGBUF_DECLARE_TYPE_DYNAMIC(TSP_sample_ringbuf_t,TSP_sample_t);

                                       
#endif /* _TSP_SAMPLE_RINGBUF_H */
