/*

$Header: /home/def/zae/tsp/tsp/src/consumers/generic/generic_consumer.h,v 1.6 2006-04-13 22:28:25 erk Exp $

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
Maintainer : enoulard@free.fr
Component : Generic Consumer

-----------------------------------------------------------------------

Purpose   : A generic command line consumer

-----------------------------------------------------------------------
 */
#ifndef _GENERIC_CONSUMER_H_
#define _GENERIC_CONSUMER_H_

#include <stdio.h>
#include <tsp_abs_types.h>
#include <tsp_common.h>
#include <tsp_consumer.h>

/**
 * @defgroup TSP_GenericConsumerLib Generic Consumer Library
 * The generic consumer library API.
 * @ingroup TSP_GenericConsumer
 * @ingroup TSP_Libraries
 */

typedef struct generic_consumer_request {
  int32_t          verbose;
  int32_t          silent;
  int32_t          help;
  int32_t          nb_global_opt;
  int32_t          argc;
  char**           argv;
  TSP_request_t    request;
  FILE*            stream;
  char*            provider_url;
  TSP_provider_t   the_provider;
  char             newline[2];
} generic_consumer_request_t;

BEGIN_C_DECLS

/**
 * @addtogroup  TSP_GenericConsumerLib
 * @{
 */

/**
 * Initialise GenericConsumer request.
 * @param req OUT, the request to initialize.
 */
void 
generic_consumer_request_create(generic_consumer_request_t* req);

/**
 * The GenericConsumer command generic API.
 * @param req INOUT, The TSP request to be honored
 */
int32_t 
generic_consumer(generic_consumer_request_t* req);

/**
 * Return the TSP request command type
 * from the string passed as parameter
 * @param generic_consumer_request_string the name of the generic_consumer command
 */
TSP_request_type_t
generic_consumer_cmd(const char* generic_consumer_request_string);

/**
 * Check arguments.
 * @param req INOUT, req the TSP request for which the arguments should be analyzed
 */
TSP_request_type_t
generic_consumer_checkargs(generic_consumer_request_t* req);

/**
 * Print usage of the specified generic_consumer request.
 * @param req IN, the generic_consumer request.
 */
void 
generic_consumer_usage(generic_consumer_request_t* req);

/**
 * When unimplemented call this.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_unimplemented_cmd(generic_consumer_request_t* req);

/**
 * The TSP request open handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_open(generic_consumer_request_t* req);

/**
 * The TSP request close handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_close(generic_consumer_request_t* req);

/**
 * The TSP request information handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_information(generic_consumer_request_t* req);

/**
 * The TSP request filtered information handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_filtered_information(generic_consumer_request_t* req);

/**
 * The TSP request asynchronous read handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_async_read(generic_consumer_request_t* req);

/**
 * The TSP request asynchronous write handler.
 * @param req IN, the generic_consumer request.
 */
int32_t 
generic_consumer_async_write(generic_consumer_request_t* req);


int32_t 
generic_consumer_extended_information(generic_consumer_request_t* req);

/**
 * @}
 */

END_C_DECLS

#endif
