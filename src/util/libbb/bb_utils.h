/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_utils.h,v 1.1 2004-09-13 23:19:23 erk Exp $

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

Purpose   : Blackboard Idiom utilities

-----------------------------------------------------------------------
 */
#ifndef _BB_UTILS_H_
#define _BB_UTILS_H_

#include <sys/ipc.h>
#include <sys/types.h>

/**
 * @defgroup BBUtils
 * @ingroup BlackBoard
 * D�finition de diff�rentes fonctions d'utilit�es g�n�rales.
 */

/**
 * BlackBoard Log message level
 * @ingroup BBUtils
 */
typedef enum BB_LOG_LEVEL {
  BB_LOG_ABORT,
  BB_LOG_SEVERE,
  BB_LOG_WARNING,
  BB_LOG_INFO,
  BB_LOG_CONFIG,
  BB_LOG_FINE,
  BB_LOG_FINER
} BB_LOG_LEVEL_T;


/**
 * Construit le nom � utiliser pour la cr�ation
 * d'un segment de m�moire partag�e � partir d'un nom
 * 'utilisateur'.
 * @param shm_name IN le nom utilisateur
 * @return le nom � utiliser pour cr�er le segment SHM
 *         La chaine de retour est allou�e par la fonction
 *         et DOIT donc �tre d�sallou�e par l'appelant.
 * @ingroup BBUtils
 */
char* 
bb_utils_build_shm_name(const char* shm_name);

/**
 * Construit le nom � utiliser pour la cr�ation
 * d'un s�maphore � partir d'un nom 'utilisateur'.
 * @param sem_name IN le nom utilisateur
 * @return le nom � utiliser pour cr�er le s�maphore
 *         La chaine de retour est allou�e par la fonction
 *         et DOIT donc �tre d�sallou�e par l'appelant.
 * @ingroup BBUtils
 */
char*
bb_utils_build_sem_name(const char* sem_name);

/**
 * Construit le nom � utiliser pour la cr�ation
 * d'une queue de message � partir d'un nom 'utilisateur'.
 * @param sem_name IN le nom utilisateur
 * @return le nom � utiliser pour cr�er la queue de message
 *         La chaine de retour est allou�e par la fonction
 *         et DOIT donc �tre d�sallou�e par l'appelant.
 * @ingroup BBUtils
 */
char*
bb_utils_build_msg_name(const char* sem_name);

/**
 * Construit une clef IPC SysV � partir d'un nom.
 * L'objectif de cette fonction est de g�n�rer des
 * clefs diff�rentes pour des noms diff�rents, � la mani�re
 * d'une fonction de hachage ou checksum parfaite.
 * Cette fonction vise les m�me objectifs que ftok(3)
 * avec les noms de fichiers mais avec une chaine
 * de caractere quelconque.
 * @param name un nom repr�sentant l'�l�ment IPC pour lequel on
 *                veut g�n�rer une clef.
 * @return la clef SysV correspondante au nom
 * @ingroup BBUtils
 */
key_t
bb_utils_ntok(const char* name);

int32_t 
bb_logMsg(const BB_LOG_LEVEL_T e_level, const char* pc_who, char* pc_fmt, ...);

#endif /* _BB_UTILS_H_ */
