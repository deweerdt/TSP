
#ifndef _BB_TSP_PROVIDER_H_
#define _BB_TSP_PROVIDER_H_

#include <stdint.h>
#include <pthread.h>

pthread_t glu_thread_id;

/**
 * Indique la frequence de base du provider TSP
 * @param d_frequence_provider la frequence de base en Hz
 * @return toujours 0, cette fonction n'�choue jamais.
 * @ingroup BlackBoard
 */
int 
GLU_set_base_frequency(double d_frequence_provider);

/**
 * Initialisation du fournisseur de donn�e TSP pour un BlackBoard TSP.
 * On doit passer � cette fonction d'initialisation les 
 * argument en provenance d'un main car la lib TSP peut
 * de cette mani�re g�rer des param�tres sp�cifiques � TSP.
 * Les param�tres qui ne concernent pas TSP seront inchang�s.
 * 
 * @param argc Le parametre argc d'un main avant que le main
 *             ne l'utilise.
 * @param argv Le parametre argc d'un main avant que le main
 *             ne l'utilise.
 * @param i_mode mode de d�marrage 0 = non bloquant, 1 = bloquant.
 * @param bbname le nom du blackboard auquel s'attacher
 * @return E_OK si initialisation r�alis�e E_NOK sinon.
 * @ingroup BlackBoard
 */
int32_t
bb_tsp_provider_initialise(int* argc, char** argv[],int i_mode, const char* bbname);

#endif /* _BB_TSP_PROVIDER_H_ */
