/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_simple.h,v 1.1 2004-09-13 23:19:23 erk Exp $

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
 * L'identifiant du message
 * GO émis par la simulation afin de signifier
 * que le process Stockage peut historiser
 * (stocker) les variables qui doivent l'être.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY   1
/**
 * L'identifiant du message
 * GO émis par le process de 
 * Stockage afin de signifier à la simulation que la copie est terminée.
 */
#define BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK  2

/**
 * @defgroup SimpleBlackBoard
 * @ingroup BlackBoard
 * Definition et manipulation du SimpleBlackBoard.
 * Le blackboardSIMPLE est le moyen de communication inter-modèle
 * au niveau SIMPLE. Chaque modèle peut publish des sorties ou des
 * états dans ce blackboard et tout autre modèle peut s'subscribe
 * aux données publiées. Celui qui publie la donnée est écrivain
 * et lecteur de cette donnée ceux qui s'y abonnent sont uniquement
 * lecteurs.
 */

/**
 * Demande de publication d'une donnee dans le blackboard SIMPLE.
 * Cette demande réalise l'allocation de l'espace
 * nécessaire dans le blackboard et renvoi
 * l'adresse à laquelle la donnée à été alloué.
 * La zone de donnée allouée est initialisé à 0, octet
 * par octet. C'est donc de la responsabilité de l'appelant
 * d'initialiser la zone avec une valeur significative
 * du type de donnée alloué.
 * @param bb_simple a pointer to a valid BB.
 * @param var_name Le nom de la variable à allouer
 *                    ce nom sera un constituant de la clef de la donnée.
 * @param module_name Le nom du module enregistrant la donnée.
 * @param module_instance L'instance du module enregistrant la donnée.
 *                        -1 signifie par d'instance multiple.
 * @param i_type Le type de la donnée
 * @param i_taille_type La taille (en octet) du type de donnée.
 * @param i_dimension La dimension de la variable à allouer. 1 si scalaire, 
 *                    > 1 si tableau.
 * @return adresse de la donnée allouée, NULL
 *         si allocation impossible.
 * @ingroup SimpleBlackBoard
 */
void* bb_simple_publish(S_BB_T* bb_simple,
			const char* var_name, 
                        const char* module_name,
			const int module_instance,
			E_BB_TYPE_T i_type,
			int i_taille_type,
			int i_dimension);
/**
 * Demande d'abonnement à une donnee
 * du blackboard SIMPLE.
 * Cette demande réalise l'allocation de l'espace
 * nécessaire dans le blackboard et renvoi
 * l'adresse à laquelle la donnée à été alloué.
 * @param bb_simple a pointer to a valid BB.
 * @param var_name IN Le nom de la variable à allouer
 *                    ce nom sera la clef de la donnée.
 * @param module_name IN Le nom du module qui est censé avoir publié la donnée
 * @param module_instance IN numéro d'instance du module
 * @param i_type IN/OUT en entrée le type de donnée attendu  en sortie
 *                      le type de la donnée trouvée dans le BB.
 * @param i_taille_type OUT taille du type récupéré en octet
 * @param i_dimension IN/OUT en entrée la taille attendue en sortie
 *                           la taille effective de la donnée.
 * @return adresse de la donnée allouée, NULL
 *         si allocation impossible.
 * @ingroup SimpleBlackBoard
 */
void* bb_simple_subscribe(S_BB_T* bb_simple,
			  const char* var_name,
			  const char* module_name,
			  const int module_instance,
			  E_BB_TYPE_T  i_type,
			  int* i_taille_type,
			  int i_dimension);

/**
 * Configuration du type de synchronisation (thread ou process) 
 * @param i_type_synchro BB_SIMPLE_SYNCHRO_THREAD ou BB_SIMPLE_SYNCHRO_PROCESS.
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_config(int i_type_synchro);

/**
 * Envoi d'un message de déblocage
 * de synchronisation SIMPLE. 
 * @param bb_simple a pointer to a valid BB.
 * @param i_type_msg le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_go(S_BB_T* bb_simple,int i_type_msg);

/**
 * Attente d'un message de déblocage
 * de synchronisation SIMPLE.
 * @param bb_simple a pointer to a valid BB.
 * @param i_type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_wait(S_BB_T* bb_simple,int i_type_msg);

/**
 * Vérification de la synchronisation SIMPLE.
 * @param bb_simple a pointer to a valid BB.
 * @return E_OK si tous les messages de synchro ont été consommés, E_NOK sinon.
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_synchro_verify(S_BB_T* bb_simple);

/**
 * Envoi d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param bb_simple a pointer to a valid BB.
 * @param i_type_msg le type de message à envoyer pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_thread_synchro_go(int i_type_msg);

/**
 * Attente d'un message de déblocage
 * de synchronisation SIMPLE (version thread).
 * @param bb_simple a pointer to a valid BB.
 * @param i_type_msg le type de message à recevoir pour la synchro
 * @return E_OK si tout se passe bien
 * @ingroup SimpleBlackBoard
 */
int32_t bb_simple_thread_synchro_wait(int i_type_msg);
#endif /* _BB_SIMPLE_H_ */
