/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_simple.h,v 1.10 2006-11-27 20:03:53 deweerdt Exp $

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

#ifdef __KERNEL__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/types.h>

#else

#include <stdio.h>
#include <pthread.h>
#include "bb_utils.h"
#include "bb_core.h"

pthread_cond_t  bb_simple_go_condvar;
pthread_mutex_t bb_simple_go_mutex;
pthread_cond_t  bb_simple_stockage_condvar;
pthread_mutex_t bb_simple_stockage_mutex;

#endif /* __KERNEL__ */

#define BB_SIMPLE_SYNCHRO_THREAD  1
#define BB_SIMPLE_SYNCHRO_PROCESS 2

/**
 * @addtogroup BBSimpleLib
 * @{
 */

/**
 * Message id used by an application to
 * notify BB tsp provider to shadow copy the blackboard.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY   1
/**
 * Message id used by BB tsp provider (or even
 * a specialized tsp consumer) in order to notify
 * the application that sampled symbols has been processed.
 * This is an ACKnowlegde message of Synchro COPY.
 * This should not be used by stringent realtime application.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK  2

/** @} */

/**
 * @defgroup BBSimpleLib The BB Simple Library
 * @ingroup BlackBoard
 * A simple way to use blackboard which may be simplier than @ref BBLib.
 * The simple blackboard API is an easy to use
 * publish subscribe interface using simple synchronization
 * primitives.
 * @{
 */

#ifndef __KERNEL__
BEGIN_C_DECLS
#endif

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
 * @param[in,out] bb_simple  a pointer to a valid BB.
 * @param[in] var_name  the name of the data
 * @param[in] module_name the module name
 * @param[in] module_instance  the module instance, -1 signify no instance.
 * @param[in] bb_type  BlackBoard data type.
 * @param[in] type_size  the data type size in byte (correspond to the size
 *        of an element of this type to be allocated).
 * @param[in] dimension  dimension of the variable
 *                    - 1 for scalar
 *                    - > 1 for array var.
 * @return address of the allocated data on success, NULL if allocation failed.
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
 * @param[in] bb_simple  a pointer to a valid BB.
 * @param[in] var_name  the variable name
 * @param[in] module_name  the name of the module who has published the data
 * @param[in] module_instance  module instance (-1 if no instance)
 * @param[in,out] bb_type en entrée le type de donnée attendu  en sortie
 *                      le type de la donnée trouvée dans le BB.
 * @param[out] type_size  taille du type récupéré en octet
 * @param[in,out] dimension en entrée la taille attendue en sortie
 *                           la taille effective de la donnée.
 * @return adresse de la donnée allouée, NULL
 *         si allocation impossible.
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
 * @param[in] synchro_type
 *        - BB_SIMPLE_SYNCHRO_THREAD for POSIX thread synchro
 *        - BB_SIMPLE_SYNCHRO_PROCESS for Sys V synchro
 * @return E_OK on success, E_NOK on failure.
 */
int32_t bb_simple_synchro_config(int synchro_type);

/**
 * Send a simple synchro message through the BB message queue.
 * @param[in,out] bb_simple a pointer to a valid BB.
 * @param[in] msg_type le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 */
int32_t bb_simple_synchro_go(S_BB_T* bb_simple,int msg_type);

/**
 * Attente d'un message de deblocage
 * de synchronisation SIMPLE.
 * @param[in,out] bb_simple a pointer to a valid BB.
 * @param[in] type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 */
int32_t bb_simple_synchro_wait(S_BB_T* bb_simple,int type_msg);

/**
 * Verification de la synchronisation SIMPLE.
 * @param[in,out] bb_simple  a pointer to a valid BB.
 * @return E_OK si tous les messages de synchro ont ete consommes, E_NOK sinon.
 */
int32_t bb_simple_synchro_verify(S_BB_T* bb_simple);

/**
 * Envoi d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param[in] type_msg le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 */
int32_t bb_simple_thread_synchro_go(int type_msg);

/**
 * Attente d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param[in] type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 */
int32_t bb_simple_thread_synchro_wait(int type_msg);

/**
 * Publish an alias in a simple BB.
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
 * @param[in,out] bb_simple a pointer to a valid BB.
 * @param[in] var_name  the name of the data
 * @param[in] target_var_name  the name of the target of the alias currently being defined
 * @param[in] module_name  the module name
 * @param[in] module_instance  the module instance, -1 signify no instance.
 * @param[in] bb_type  BlackBoard data type.
 * @param[in] type_size  the data type size in byte (correspond to the size
 *        of an element of this type to be allocated).
 * @param[in] dimension  dimension of the variable
 *                    - 1 for scalar
 *                    - > 1 for array var.
 * @param[in] offset  offset relative of the alias from the target 
 * @return address of the allocated data on success, NULL if allocation failed.
 */
void* bb_simple_alias_publish(S_BB_T* bb_simple,
			      const char* var_name,
			      const char* target_var_name, 
			      const char* module_name,
			      const int module_instance,
			      E_BB_TYPE_T bb_type,
			      int type_size,
			      int dimension,
			      unsigned long offset);


/** @} */

#ifndef __KERNEL__
END_C_DECLS
#endif

#endif /* _BB_SIMPLE_H_ */
