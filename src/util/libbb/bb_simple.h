/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_simple.h,v 1.4 2005-10-23 12:28:56 erk Exp $

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

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */
#ifndef _BB_SIMPLE_H_
#define _BB_SIMPLE_H_

#include <stdio.h>
#include <pthread.h>
#include <bb_utils.h>
#include <bb_core.h>

pthread_cond_t  bb_simple_go_condvar;
pthread_mutex_t bb_simple_go_mutex;
pthread_cond_t  bb_simple_stockage_condvar;
pthread_mutex_t bb_simple_stockage_mutex;

#define BB_SIMPLE_SYNCHRO_THREAD  1
#define BB_SIMPLE_SYNCHRO_PROCESS 2

/**
 * Message id used by an application to
 * notify BB tsp provider to shadow copy the blackboard.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY   1
/**
 * Message id used by BB tsp provider (or even
 * a specialized tsp consumer) in order to notify
 * the application that sampled symbols has been processed.
 * This is an ACKnolegde message of Synchro COPY.
 * This should not be used by stringent realtime application.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK  2

/**
 * @defgroup SimpleBlackBoard
 * @ingroup BlackBoard
 * A simplest way to use blackboard.
 * The simple blackboard interface is an easy to use
 * publish subscribe interface using simple synchronization
 * primitive.
 */

BEGIN_C_DECLS

/**
 * Publish data in a simple BB.
 * This is a normal BB publish operation with an
 * added automatic subscribe facility. If the to 
 * be published data already exists in BB then the publish operation
 * automatically triggers the corresponding subscribe operation.
 * If first publish, the data is initialized to 0 if automatically
 * subscribed the data is not initialized by the call and one
 * obtain the previously set value.
 * The name of the variable to be published is mangled using
 * 3 field:
 *   - variable name
 *   - the module name whose variable belongs to
 *   - the module instance in case there may be
 *     several instance of the same moule.
 * @param bb_simple INOUT, a pointer to a valid BB.
 * @param var_name IN, the name of the data
 * @param module_name IN, the module name
 * @param module_instance IN, the module instance, -1 signify no instance.
 * @param bb_type IN, BlackBoard data type.
 * @param type_size IN, the data type size in byte (correspond to the size
 *        of an element of this type to be allocated).
 * @param dimension IN, dimension of the variable
 *                    - 1 for scalar
 *                    - > 1 for array var.
 * @return address of the allocated data on success, NULL if allocation failed.
 * @ingroup SimpleBlackBoard
 */
void* bb_simple_publish(S_BB_T* bb_simple,
			const char* var_name, 
                        const char* module_name,
			const int module_instance,
			E_BB_TYPE_T bb_type,
			int type_size,
			int dimension);
/**
 * Subscribe to a data in simple BB.
 * This a normal BB subscribe with name
 * mangling just the same as  @see bb_simple_publish.
 * @param bb_simple IN, a pointer to a valid BB.
 * @param var_name IN, the variable name
 * @param module_name IN, the name of the module who has published the data
 * @param module_instance IN, module instance (-1 if no instance)
 * @param bb_type IN/OUT, en entrée le type de donnée attendu  en sortie
 *                      le type de la donnée trouvée dans le BB.
 * @param type_size OUT, taille du type récupéré en octet
 * @param dimension IN/OUT, en entrée la taille attendue en sortie
 *                           la taille effective de la donnée.
 * @return adresse de la donnée allouée, NULL
 *         si allocation impossible.
 * @ingroup SimpleBlackBoard
 */
void* bb_simple_subscribe(S_BB_T* bb_simple,
			  const char* var_name,
			  const char* module_name,
			  const int module_instance,
			  E_BB_TYPE_T*  bb_type,
			  int* type_size,
			  int* dimension);

/**
 * Configure synchronization type (thread or process).
 * @param synchro_type IN,
 *        - BB_SIMPLE_SYNCHRO_THREAD for POSIX thread synchro
 *        - BB_SIMPLE_SYNCHRO_PROCESS for Sys V synchro
 * @return E_OK on success, E_NOK on failure.
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_config(int synchro_type);

/**
 * Send a simple synchro message through the BB message queue.
 * @param bb_simple INOUT, a pointer to a valid BB.
 * @param msg_type IN, le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_go(S_BB_T* bb_simple,int msg_type);

/**
 * Attente d'un message de deblocage
 * de synchronisation SIMPLE.
 * @param bb_simple a pointer to a valid BB.
 * @param type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_wait(S_BB_T* bb_simple,int type_msg);

/**
 * Verification de la synchronisation SIMPLE.
 * @param bb_simple INOUT, a pointer to a valid BB.
 * @return E_OK si tous les messages de synchro ont ete consommes, E_NOK sinon.
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_verify(S_BB_T* bb_simple);

/**
 * Envoi d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param type_msg le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_thread_synchro_go(int type_msg);

/**
 * Attente d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_thread_synchro_wait(int type_msg);

END_C_DECLS

#endif /* _BB_SIMPLE_H_ */
