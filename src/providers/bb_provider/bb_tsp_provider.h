/*

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.h,v 1.11 2006-02-26 13:36:06 erk Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Blackboard TSP Provider

-----------------------------------------------------------------------
 */
#ifndef _BB_TSP_PROVIDER_H_
#define _BB_TSP_PROVIDER_H_

#include <tsp_abs_types.h>
#include <pthread.h>

/**
 * @defgroup BB_TSPProviderLib BB TSP Provider Library
 * The BlackBoard TSP provider library API.
 * The library is used to build the @ref BB_TSPProvider application.
 * It can be used to build its own customized BlackBoard provider.
 * @ingroup BB_TSPProvider
 * @ingroup TSP_Libraries
 * @{
 */

/**
 * Initialize Blackboard TSP provider Library.
 * 
 * @param argc argc coming from main (before the main use it)
 * @param argv argv coming from main (before the main use it)
 * @param TSPRunMode running mode, 0 = non blocking, 1 = blocking (never return).
 * @param bbname the blackboard name we should attach to.
 * @param frequency the frequency advertised by the bb_tsp_provider 
 * @param acknowledgeCopy 0 if no acknowledgment is to be sent
 *                        by the provider to the simulation driving
 *                        sample, otherwise acknowledgment is sent.
 * @return E_OK if initialization is successfull  E_NOK otherwise.
 */
int32_t
bb_tsp_provider_initialise(int* argc, char** argv[],int TSPRunMode, 
			   const char* bbname,
			   double frequency, 
			   int32_t acknowledgeCopy);

/**
 * Terminate the bb_tsp_provider nicely.
 * The function end up in a call to TSP_provider_finalize().
 */
int32_t 
bb_tsp_provider_finalize();

/**
 * Authorize asynchronous write for BB symbols
 * specified by its PGI.
 * After a call to this asynchronous write request to this PGI
 * will be honored gracefully (if valid though) by BB provider.
 * @param provider_global_index IN, the PGI of the symbol we want to authorize
 *                                  asynchronous write.
 * @return TRUE if authorization succeed FALSE otherwise.
 */
int32_t 
bb_tsp_provider_allow_write_symbol(int provider_global_index);

/**
 * Forbid asynchronous write for BB symbols specified by its PGI.
 * After a call to this asynchronous write request to this PGI
 * will be refused by BB provider.
 * @param provider_global_index IN, the PGI of the symbol we want to forbid
 *                                  the asynchronous write.
 * @return TRUE if authorization succeed FALSE otherwise.
 */
int32_t 
bb_tsp_provider_forbid_write_symbol(int provider_global_index);

/** @} */

#endif /* _BB_TSP_PROVIDER_H_ */
