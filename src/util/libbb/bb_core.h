/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core.h,v 1.4 2004-10-12 17:17:04 erk Exp $

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
#ifndef _BB_H_
#define _BB_H_

#include <stdio.h>
#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
#ifdef _SEM_SEMUN_UNDEFINED
#undef _SEM_SEMUN_UNDEFINED
/**
 * Union a definir et utiliser
 * pour les appels 'semop' SysV.
 * @ingroup BlackBoard
 */
union semun {
  int val;                           /* value for SETVAL */
  struct semid_ds *buf;              /* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;         /* array for GETALL & SETALL */
  struct seminfo *__buf;             /* buffer for IPC_INFO */
};
#endif
#endif


/**
 * @defgroup BlackBoard
 * BlackBoard idiom definition and functions.
 * A blackboard is an inter-process communication mean based
 * on the publish/subscribe principle.
 * Every process/thread attached to the blackboard may
 * publish or subscribe to a piece of data using a key
 * (string based in this implementation).
 */

#define MAX_SYSMSG_SIZE     255

#define BB_NAME_MAX_SIZE    255
#define VARNAME_MAX_SIZE    150
#define MSG_BB_MAX_SIZE     32
#define BB_SHM_ACCESS_RIGHT 0770
#define BB_SEM_ACCESS_RIGHT 0770
#define BB_MSG_ACCESS_RIGHT 0770

#define E_OK    0
#define E_NOK  -1

/**
 * BlackBoard publishable data type.
 * @ingroup BlackBoard
 */
typedef enum {E_BB_DOUBLE=1, 
	      E_BB_FLOAT,
	      E_BB_INT16, 
	      E_BB_INT32, 
	      E_BB_INT64, 
	      E_BB_UINT16, 
	      E_BB_UINT32, 
	      E_BB_UINT64,
	      E_BB_CHAR,
              E_BB_USER} E_BB_TYPE_T;
	      
/**
 * BlackBoard data descriptor.
 * @ingroup BlackBoard
 */     	      
typedef struct S_BB_DATADESC {
  /** Variable name */
  char name[VARNAME_MAX_SIZE+1];
  /** Variable (BlackBoard) type */
  E_BB_TYPE_T type;
  /** 
   * Dimension. 1 if scalar, > 1 for single dimension array.
   * There is no multidimensionnal array type.
   */
  int32_t dimension;
  /** 
   * Type size (in byte).
   * This size enables the appropriate computation
   * of the data offset in the data array section.
   */
  size_t type_size;
  /**
   * Data offset (in bytes) in the data array section.
   */
  unsigned long data_offset;
} S_BB_DATADESC_T;

/**
 * BlackBoard message definition.
 * This type must conform to the constraint of
 * a SysV message queue message used by msgsnd(2).
 * @ingroup BlackBoard
 */
typedef struct S_BB_MSG {
  /** 
   * Message type.
   * Should be strictly positive since negative or
   * zero value are used by the system.
   * Collaborative process should agree on the message type
   * they exchange, and what's inside those messages.
   */
  long mtype;
  /** 
   * Message content.
   * This is an opaque buffer.
   */
  unsigned char mtext[MSG_BB_MAX_SIZE];
} S_BB_MSG_T;

/**
 * BlackBoard definition.
 * A blackboard implements the publish/subsbcribe idiom.
 * @ingroup BlackBoard
 */
typedef struct S_BB {
  /** 
   * BB access semaphore. 
   * This SysV sempahore set contains only 1 semaphore
   * which should be taken before any structural BB 
   * modification and/or BB data zone copy.
   * @bb_shadow_get.
   */
  int semid;
  /**
   * Synchronisation message queue.
   * This is a simple inter-process synchronisation mean.
   * For example a simulation process may send end of cycle
   * message to the queue in order to signal another process
   * that he may shadow the BB for further safe use.
   */
  int msgid;
  /** BlackBoard name */
  char name[BB_NAME_MAX_SIZE+1];
  /** Maximum data descriptor size in bytes. */
  int32_t  max_data_desc_size;
  /** Data descriptor array offset */  
  unsigned long  data_desc_offset; 
  /** 
   * The number of data currently stored in the BB.
   * This is the used size of the data descriptor array.
   */
  int n_data;
  /** The maximum data size in BB. */
  unsigned long max_data_size;
  /** BlackBoard data zone offset (bytes) */
  unsigned long data_offset;
  /** 
   * Offset (octets) of the next free byte 
   * in the blackboard data zone.
   */
  unsigned long data_free_offset;
  /**
   * Destroyed state of a BB.
   * Should be used by processes to detach/re-attach
   * to a destroyed BB.
   */
  int destroyed;
} S_BB_T;


/**
 * Renvoie la taille occupée par un blackboard
 * contenant au plus 'n_data' et dont
 * la zone de donnée est de taille 'i_taille_donnees'
 * octet.
 * @param n_data IN le nombre de données maximum stockable
 *                     dans ce blackboard.
 * @param data_size IN la taille (en octet) de la zone de donnée
 *                         du blackboard
 * @return la taille en octet occupé par un blackboard
 *         de ce type
 * @ingroup BlackBoard
 */
int32_t
bb_size(const int32_t n_data, const int32_t data_size);

/**
 * Search a variable within a BlackBoard.
 * @param bb IN BlackBoard pointer
 * @param var_name IN the name of the searched variable
 * @return index of the variable in the BB descriptor array
 * @ingroup BlackBoard
 */
int32_t 
bb_find(volatile S_BB_T* bb, const char* var_name);

/**
 * Return a pointer to the beginning of the data 
 * descriptor array of the specified BB.
 * @param bb IN BlackBoard pointer
 * @return start address of the data descriptor array in bb
 * @ingroup BlackBoard
 */
S_BB_DATADESC_T* 
bb_data_desc(volatile S_BB_T* bb);

/**
 * Return a pointer to the beginning of the data array
 * of the specified BB.
 * @param bb IN BlackBoard pointer
 * @return start address of the data array in bb
 * @ingroup BlackBoard
 */
void* 
bb_data(volatile S_BB_T* bb);

/**
 * Return a double value from the pointer
 * value considered as the bbtype.
 * @param value IN pointer to the value
 * @param bbtype IN the type of the pointer
 * @return double value of the pointed value
 * @ingroup BlackBoard
 */
double
bb_double_of(void *value, E_BB_TYPE_T bbtype);

/**
 * Initialise une structure de donnée nouvellement
 * allouée à une valeur par défaut.
 * Si le pointeur vers la valeur par défaut est 
 * NULL on initialise à 0 pour tous les types
 * sauf le type E_BB_USER pour lequel on remplit
 * la mémoire allouée de zéro binaire.
 * @param bb IN pointeur vers le blackboard auquel est liée la donnée
 * @param ps_data INOUT pointeur vers la structure de description de la donnee
 *                doit être non NULL.
 * @param pv_valeur_defaut INOUT valeur par défaut pour l'initialisation.
 *                         Si NULL alors on initialise à zéro.
 * @return E_OK si initialisation réalisée E_NOK sinon.
 * @ingroup BlackBoard
 */
int32_t 
bb_data_initialise(volatile S_BB_T* bb, S_BB_DATADESC_T* ps_data,void* pv_valeur_defaut);

int32_t
bb_value_write(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc,const char* value, int32_t idx);

int32_t
bb_data_header_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t index);

int32_t
bb_data_footer_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t index);

int32_t 
bb_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf, int32_t index);

/**
 * Affiche le contenu d'un descripteur de donnée.
 * @param bb IN pointeur vers le blackboard auquel est liée la donnée
 * @param ps_data INOUT pointeur vers la structure de description de la donnee.
 * @param pf INOUT pointeur de fichier vers lequel on affiche. 
 *           Doit être non NULL.
 * @return renvoie toujours E_OK sauf si pf est NULL.
 * @ingroup BlackBoard
 */
int32_t 
bb_data_print(volatile S_BB_T* bb, S_BB_DATADESC_T ps_data, FILE* pf);

/**
 * Creation d'un blackboard.
 * Ouvre le segment de mémoire partagée et alloue la taille
 * demandée, mmap la zone sur une structure de donnée décrivant un blackboard et 
 * initialise cette dernière.
 * Seul un process doit appeler cette fonction.
 * Les autres processes doivent se contenter de s'attacher
 * au blackboard via bb_attach(S_BB_T**, const char*).
 * @param bb INOUT Pointeur de pointeur sur le BB.
 *                    En entree, ce pointeur doit être non nul.
 *                    En sortie, l'élément pointé est le blackboard nouvellement
 *                    crée.
 * @param pc_bb_name IN Nom du blackboard
 * @param n_data IN le nombre de données maximum du blackboard.
 *                        C'est-à-dire le nombre d'éléments différenciable
 *                        (par leur clef) du blackboard.
 * @param data_size IN la taille des données du blackboard (en octet).
 *                         C'est-à-dire la somme des tailles de toutes
 *                         les données à allouer dans le blackboard.
 * @return code de retour E_OK si creation reussie.
 * @ingroup BlackBoard
 */
int32_t 
bb_create(S_BB_T** bb, 
	  const char* pc_bb_name,
	  int n_data,
	  int data_size);
/**
 * Destruction d'un blackboard.
 * Détruit le contenu de la structure de donnée décrivant un blackboard.
 * @param bb INOUT Pointeur de pointeur sur le BB.
 *                    Ce pointeur doit être non nul.
 * @return code de retour E_OK si destruction reussie.
 * @ingroup BlackBoard
 */
int32_t 
bb_destroy(S_BB_T** bb);

/**
 * Verouillage d'un blackboard.
 * On doit verrouiller le blackboard avant
 * toute modification structurelle ou pour protéger
 * d'autres processus de modifications concurrentes.
 * Cet appel est bloquant.
 * Les interfaces de publication et d'abonnement au blackboard
 * vérouille automatiquement le blackboard.
 * @param bb INOUT Pointeur sur le BB.
 *                    Ce pointeur doit être non nul.
 * @return code de retour E_OK si lock reussi.
 * @ingroup BlackBoard
 */
int32_t 
bb_lock(volatile S_BB_T* bb);

/**
 * Déverouillage d'un blackboard.
 * @param bb INOUT Pointeur sur le BB.
 *                    Ce pointeur doit être non nul.
 * @return code de retour E_OK si unlock reussi.
 * @ingroup BlackBoard
 */
int32_t 
bb_unlock(volatile S_BB_T* bb);

/**
 * Connexion à un blackboard.
 * Se connecte à un blackboard existant.
 * @param bb OUT Pointeur de pointeur sur le BB.
 *                  Ce pointeur doit être non nul.
 *                  La valeur du pointeur est mise à jour si la connexion au 
 *                  BB a réussie.
 * @param pc_bb_name IN Nom du blackboard
 * @return code de retour E_OK si le blackboard existe et
 *                             que la connexion est acceptée E_NOK sinon.
 * @ingroup BlackBoard
 */
int32_t 
bb_attach(S_BB_T** bb, const char* pc_bb_name);

/**
 * Deconnexion à un blackboard.
 * @param bb INOUT Pointeur de pointeur sur le BB.
 *                  Ce pointeur doit être non nul.
 * @return code de retour E_OK si le blackboard existe et
 *                             que la connexion est accepté E_NOK sinon.
 * @ingroup BlackBoard
 */
int32_t 
bb_detach(S_BB_T** bb);

/**
 * Demande de publication d'une donnée dans un blackboard.
 * Cette demande réalise l'allocation de l'espace
 * nécessaire dans le blackboard et renvoie
 * l'adresse à laquelle la donnée à été alloué.
 * @param bb INOUT Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @param ps_data_desc INOUT Descripteur de la donnée à publish.
 *                          En sortie si la donnée a été allouée
 *                          le champ S_BB_DATADESC_T.pv_donnee
 *                          est mis à jour.
 * @return adresse de la donnée allouée, NULL
 *         si allocation impossible.
 * @ingroup BlackBoard
 */
void* 
bb_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* ps_data_desc);

/**
 * Demande d'abonnement à une donnée dans un blackboard.
 * Cette demande cherche la donnée dans le blackboard
 * et renvoie son adresse si elle est trouvée.
 * @param bb IN Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @param ps_data_desc INOUT Descripteur de la donnée à chercher.
 *                          En sortie si la donnée a été trouvée
 *                          le champ S_BB_DATADESC_T.pv_donnee
 *                          est mis à jour.
 * @return adresse de la donnée si elle est présente, NULL
 *         si la donnée est absente.
 * @ingroup BlackBoard
 */
void* 
bb_subscribe(volatile S_BB_T *bb, S_BB_DATADESC_T* ps_data_desc);

/**
 * Demande de dumper un blackboard.
 * On écrit dans un flux stdio la description et le 
 * contenu d'un blackboard.
 * @param bb INOUT Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @param p_filedesc INOUT Descripteur de flux stdio.
 * @return code de retour E_OK si dump reussi.
 * @ingroup BlackBoard
 */
int32_t 
bb_dump(volatile S_BB_T *bb, FILE* p_filedesc);

/**
 * Renvoi le nombre de données maximum
 * stockable dans le blackboard.
 * @param bb IN Pointeur sur le blackboard.
 *                 Ce pointeur doit être non nul.
 * @return Le nombre de données maximum
 *         stockable dans le blackboard.
 * @ingroup BlackBoard
 */
int
bb_get_nb_max_item(volatile S_BB_T *bb);

/**
 * Renvoi le nombre de données actuellement
 * stockées dans le blackboard.
 * @param bb IN Pointeur sur le blackboard.
 *                 Ce pointeur doit être non nul.
 * @return Le nombre de données actuellement
 *         stockées dans le blackboard.
 * @ingroup BlackBoard
 */
int
bb_get_nb_item(volatile S_BB_T *bb);

/**
 * Renvoi la taille mémoire occupée par un blackboard.
 * @param bb IN Pointeur sur le blackboard.
 *                 Ce pointeur doit être non nul.
 * @return La taille mémoire (en octets) occupé par le BB.
 * @ingroup BlackBoard
 */
int
bb_get_mem_size(volatile S_BB_T *bb);

/**
 * Demande l'ombre d'un blackboard.
 * L'ombre d'un blackboard est un blackboard dont on
 * sait qu'il ne sera partagé par aucun autre processus.
 * Les seules opérations valident sur une ombre de blackboard
 * sont (bb_shadow_get, bb_shadow_update_data) et l'accès
 * direct aux données du blackboard.
 * @param bb_shadow INOUT Pointeur sur la zone de donnée pré-allouée
 *                         qui doit recevoir le shadow blackboard
 * @param bb_src IN Pointeur sur le blackboard source
 *                     de l'ombre.
 * @return code de retour E_OK si l'ombre reussie.
 * @ingroup BlackBoard
 */
int32_t 
bb_shadow_get(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Demande la MAJ de la zone de données d'une ombre de blackboard.
 * @param bb_shadow INOUT Pointeur sur l'ombre de blackboard.
 * @param bb_src IN Pointeur sur le blackboard source
 *                     de l'ombre.
 * @return code de retour E_OK si l'update reussie.
 * @ingroup BlackBoard
 */
int32_t 
bb_shadow_update_data(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Renvoie l'identifiant de la queue de message lié au blackboard.
 * @param bb INOUT Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @return identifiant de la queue de message liée au blackboard
 * @ingroup BlackBoard
 */
int32_t 
bb_msg_id(volatile S_BB_T *bb);

/**
 * Demande d'envoi de message via le blackboard.
 * @param bb INOUT Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @param msg INOUT Pointeur sur le message à envoyer
 * @return code de retour E_OK si dump reussi.
 * @ingroup BlackBoard
 */
int32_t 
bb_snd_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);


/**
 * Demande de réception d'un message via le blackboard.
 * Cet appel est bloquant si aucun message n'est disponible.
 * @param bb INOUT Pointeur sur le blackboard.
 *                    Ce pointeur doit être non nul.
 * @param msg INOUT Pointeur sur le message à envoyer
 * @return code de retour E_OK si dump reussi.
 * @ingroup BlackBoard
 */
int32_t 
bb_rcv_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);
#endif /* _BB_H_ */
