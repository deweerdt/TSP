/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.h,v 1.2 2004-09-20 20:55:59 erk Exp $

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

#include <stdint.h>
#include <pthread.h>

pthread_t glu_thread_id;

/**
 * Indique la frequence de base du provider TSP
 * @param d_frequence_provider la frequence de base en Hz
 * @return toujours 0, cette fonction n'échoue jamais.
 * @ingroup BlackBoard
 */
int 
GLU_set_base_frequency(double d_frequence_provider);

/**
 * Initialisation du fournisseur de donnée TSP pour un BlackBoard TSP.
 * On doit passer à cette fonction d'initialisation les 
 * argument en provenance d'un main car la lib TSP peut
 * de cette manière gérer des paramètres spécifiques à TSP.
 * Les paramètres qui ne concernent pas TSP seront inchangés.
 * 
 * @param argc Le parametre argc d'un main avant que le main
 *             ne l'utilise.
 * @param argv Le parametre argc d'un main avant que le main
 *             ne l'utilise.
 * @param i_mode mode de démarrage 0 = non bloquant, 1 = bloquant.
 * @param bbname le nom du blackboard auquel s'attacher
 * @return E_OK si initialisation réalisée E_NOK sinon.
 * @ingroup BlackBoard
 */
int32_t
bb_tsp_provider_initialise(int* argc, char** argv[],int i_mode, const char* bbname);

#endif /* _BB_TSP_PROVIDER_H_ */
