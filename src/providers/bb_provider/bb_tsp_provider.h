/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.h,v 1.10 2006-01-22 09:35:15 erk Exp $

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

Purpose   : Blackboard TSP Provider

-----------------------------------------------------------------------
 */
#ifndef _BB_TSP_PROVIDER_H_
#define _BB_TSP_PROVIDER_H_

#include "tsp_abs_types.h"

#include <pthread.h>

/**
 * Initialize Blackboard TSP provider.
 * On doit passer à cette fonction d'initialisation les 
 * argument en provenance d'un main car la lib TSP peut
 * de cette manière gérer des paramètres spécifiques à TSP.
 * Les paramètres qui ne concernent pas TSP seront inchangés.
 * 
 * @param argc argc coming from main (before the main use it)
 * @param argv argv coming from main (before the main use it)
 * @param TSPRunMode running mode, 0 = non blocking, 1 = blocking (never return).
 * @param bbname the blackboard name we should attach to.
 * @param frequency the frequency advertised by the bb_provider 
 * @param acknowledgeCopy 0 if no acknowledgment is to be sent
 *                        by the provider to the simulation driving
 *                        sample, otherwise acknowledgment is sent.
 * @return E_OK if initialization is successfull  E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t
bb_tsp_provider_initialise(int* argc, char** argv[],int TSPRunMode, 
			   const char* bbname,
			   double frequency, 
			   int32_t acknowledgeCopy);

/**
 * Terminate bb_provider nicely.
 */
int32_t 
bb_tsp_provider_finalize();

/**
 * Authorize asynchronous write for BB symbols
 * specified by its PGI.
 */
int32_t 
bb_tsp_provider_allow_write_symbol(int provider_global_index);

/**
 * Forbid asynchronous write for BB symbols
 * specified by its PGI.
 */
int32_t 
bb_tsp_provider_forbid_write_symbol(int provider_global_index);

#endif /* _BB_TSP_PROVIDER_H_ */
