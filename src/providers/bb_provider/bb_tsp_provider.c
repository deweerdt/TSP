/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.c,v 1.6 2004-10-18 21:45:04 erk Exp $

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
#include "bb_core.h"
#include "bb_simple.h"
#define BB_TSP_PROVIDER_C
#include "bb_tsp_provider.h"

#include "tsp_provider_init.h"
#include "tsp_datapool.h"

/*
 * Declaration crado pour demarrage TSP en douceur
 */
int TSP_provider_rqh_manager_get_nb_running();

/* 
 * On inclu les en-têtes TSP
 * necessaire a l'implementation d'un GLU
 * car on doit réaliser ces fonctions dans
 * notre provider TSP.
 * Consulter le header 'glue_sserver.h' afin de comprendre
 * la réalisation des différentes fonctions.
 */
#include <tsp_sys_headers.h>
#include <glue_sserver.h>
#include <tsp_ringbuf.h>

#include <signal.h>

/* RINGBUF_DECLARE_TYPE_DYNAMIC(glu_ringbuf,glu_item_t); */

#define GLU_RING_BUFSIZE (1000 * 64 * 10)

/*
 * Quelques variables static
 */
/* le nom du provider */
static char* X_server_name = "BB-TSP-V0_2";
/* le ringbuffer entre le GLU et le reste du monde */
/* static glu_ringbuf* glu_ring = 0; */
/* la liste des symboles */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val = NULL;

/* The BB and its shadow BB */
static S_BB_T* the_bb = NULL;
static char* the_bbname = NULL;
static S_BB_T* shadow_bb = NULL;

/* Le time stamp time du GLU */
static time_stamp_t glu_time = 0;
/* Tableau des adresses des valeurs dans le shadow BB indexé par
 * le provider_global_index
 */
static void** value_by_pgi = NULL;
static E_BB_TYPE_T * bbtype_by_pgi = NULL;
/*
 * La frequence de base du provider devra être celle
 * du simulateur qui sera disponible dans le blackboard
 * a defaut on en fourni une fictive.
 */
static double frequence_provider = 64.0;
static double* bb_tsp_provider_frequency = &frequence_provider;

int GLU_set_base_frequency(double d_frequence_provider) {
  *bb_tsp_provider_frequency = d_frequence_provider;
  return 0;
}

char* 
GLU_get_server_name() {
  return X_server_name;
}


int 
GLU_init(int fallback_argc, char* fallback_argv[]) {

  int retcode;
  int i;
  int j;
  int i_temp;
  int i_pg_index;
  int i_nb_item_scalaire;

  retcode = TRUE;
  /* Le TID du GLU_thread */
  glu_thread_id = 0;
  /* On n'a rien a faire des fallback pour l'instant */
  /* 
   * !!! On n'a besoin de s'attacher au BB seulement 
   * !!! si on est un process separe de celui qui le crée
   */
  if (bb_attach(&the_bb,the_bbname) != E_OK) {
    bb_logMsg(BB_LOG_SEVERE,
	      "bb_tsp_provider::GLU_init","Cannot attach to BlackBoard <%s>!!",
	      the_bbname);
    retcode = FALSE;
  } 
  
  /* 
   * Récupère un acces direct au blackboard 
   * afin de construire la liste des variables
   * plus simplement.
   */
  if (TRUE == retcode) {
    /* Allocation du shadow blackboard */
    shadow_bb = malloc(bb_get_mem_size(the_bb));
    if (NULL == shadow_bb) {
      retcode = FALSE;
    }
  }
  if (TRUE == retcode) {
    /* Shadow BBPE */
    if (E_NOK == bb_shadow_get(shadow_bb, the_bb)) {
      retcode = FALSE;
    }  
  }
  if (TRUE == retcode) {
    /* Initial update (ce qui est inutile mais ça ne gate rien */
    if (E_NOK ==  bb_shadow_update_data(shadow_bb,the_bb)) {
      retcode = FALSE;
    } 
  }

  /* Construction de la liste des symboles */
  /* On doit calculer le nombre d'item SCALAIRE 
   * car TSP ne gère pas [encore] les tableaux
   */
  i_nb_item_scalaire = 0;
  for (i=0;i<bb_get_nb_item(shadow_bb);++i) {
    i_nb_item_scalaire += bb_data_desc(shadow_bb)[i].dimension;
  }
  /* On alloue une liste de symboles de la taille
   * correspondant au nombre de données (scalaire) enregistrées dans le BB
   */
  X_sample_symbol_info_list_val = calloc (i_nb_item_scalaire,
					  sizeof (TSP_sample_symbol_info_t));
  assert(X_sample_symbol_info_list_val);
  /* On alloue 
   * le tableau de pointeurs vers les données
   */
  value_by_pgi = (void **) calloc(i_nb_item_scalaire,sizeof(void*));
  bbtype_by_pgi = (E_BB_TYPE_T *) calloc(i_nb_item_scalaire,sizeof(E_BB_TYPE_T));

  assert(value_by_pgi);	
  /* on initialise le provider global index a 0 */
  i_pg_index = 0;
  for (i=0; i<bb_get_nb_item(shadow_bb);++i) {
    /* TSP ne gère pas encore les tableaux
     * donc on génère des noms de symboles postfixés par
     * l'index du tableau [%d]
     */
    if (bb_data_desc(shadow_bb)[i].dimension > 1) {
      for (j=0;j<bb_data_desc(shadow_bb)[i].dimension; ++j) {
	i_temp = strlen(bb_data_desc(shadow_bb)[i].name)+9;
	X_sample_symbol_info_list_val[i_pg_index].name = malloc(i_temp);
	assert(X_sample_symbol_info_list_val[i_pg_index].name);
	snprintf(X_sample_symbol_info_list_val[i_pg_index].name, 
		 i_temp,
		 "%s[%0d]",
		 bb_data_desc(shadow_bb)[i].name,
		 j);
	X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
	X_sample_symbol_info_list_val[i_pg_index].period = 1;
	/* on pointe vers la valeur adéquate */
	value_by_pgi[i_pg_index]  = ((void*) ((char*)bb_data(shadow_bb) + bb_data_desc(shadow_bb)[i].data_offset)) + j*bb_data_desc(shadow_bb)[i].type_size;
	bbtype_by_pgi[i_pg_index] = bb_data_desc(shadow_bb)[i].type;
	++i_pg_index;
      }
    } 
    /* creation simple pour les scalaires */
    else {
      X_sample_symbol_info_list_val[i_pg_index].name = strdup(bb_data_desc(shadow_bb)[i].name);
      X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
      X_sample_symbol_info_list_val[i_pg_index].period = 1;
      value_by_pgi[i_pg_index] = ((void*) ((char*)bb_data(shadow_bb) + bb_data_desc(shadow_bb)[i].data_offset));
      bbtype_by_pgi[i_pg_index] = bb_data_desc(shadow_bb)[i].type;
      ++i_pg_index;
    }
  }
  
  /* initialisation ringbuffer */
 /*  if (TRUE == retcode) { */
     /*    RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(GLU_RING_BUFSIZE)); */ 
/*     RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(i_pg_index*32*10));  */
/*     bb_logMsg(BB_LOG_CONFIG,"bb_tsp_provider::GLU_init","Ring Buffer Size is <%d>",glu_ring->size); */
/*     RINGBUF_PTR_RESET_CONSUMER (glu_ring); */
/*   } */
  return retcode;
} /* end of GLU_init */


/*
 * C'est un provider actif car il dépend
 * d'une simulation potentiellement temps réelle et 
 * n'attendra donc pas que les éventuels consumer
 * consomment ce qu'ils ont demandés pour produire.
 * Si les consumers sont trop lents ils perdront des données
 * le provider ne s'arrêtera pas.
 */ 
GLU_server_type_t GLU_get_server_type(void) {
  return GLU_SERVER_TYPE_ACTIVE;
}


int  GLU_get_symbol_number(void) {
  int i = 0;
  TSP_sample_symbol_info_t* p  = X_sample_symbol_info_list_val;
	
  for( p=X_sample_symbol_info_list_val; p->name!=0 ; ++p) {
    ++i;
  }
  return i;
}  /* end of GLU_get_symbol_number */

int  
GLU_get_sample_symbol_info_list(GLU_handle_t h_glu, 
				TSP_sample_symbol_info_list_t* symbol_list) {

  symbol_list->TSP_sample_symbol_info_list_t_len = GLU_get_symbol_number();
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  return TRUE;
}

double 
GLU_get_base_frequency(void) {
  return *bb_tsp_provider_frequency;
}

static void* GLU_thread(void* arg) {
  
  int i;
  glu_item_t item;
  sigset_t s_mask;
  int nb_consumed_symbols;
  int* ptr_consumed_index;
  int pgi;
  
  bb_logMsg(BB_LOG_INFO,
	      "bb_tsp_provider::GLU_thread",
	      "Provider thread started with <%d> symbols",
	      GLU_get_symbol_number());
  /*
   * On masque les signaux indesirables i.e. tous :))
   */
  sigfillset(&s_mask);
  pthread_sigmask(SIG_BLOCK,&s_mask,NULL);
  /* 
   * Initialise le temps propre du GLU 
   */
  glu_time      = 0;

  /* boucle infinie tant que le blackboard n'est pas detruit */
  while(1 != the_bb->destroyed) {
    /* On attend le déblocage du PE */
    if (E_NOK == bb_simple_synchro_wait(the_bb,BB_SIMPLE_MSGID_SYNCHRO_COPY)) {
      /* on sort car le BB a été détruit */
      break;
    }
    /* 
     * MAJ des donnees du shadow BB 
     */    
    bb_shadow_update_data(shadow_bb,the_bb);
    /* 
     * Refresh the [reverse list of consumed symbols]
     * Must be call at each step in case of new samples wanted 
     */
    TSP_datapool_get_reverse_list (&nb_consumed_symbols, &ptr_consumed_index); 
   /*  bb_simple_synchro_go(BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK); */
/*     while ( RINGBUF_PTR_ITEMS_LEFT(glu_ring) < (nb_consumed_symbols*sizeof(item)+64) ) { */
/*       usleep(300); */
/*       bb_logMsg(BB_LOG_WARNING,"bb_tsp_provider::GLU_thread", */
/* 		  "RINGBUF nb Item Left= <%d>",RINGBUF_PTR_ITEMS_LEFT(glu_ring)); */
/*       sched_yield(); */
/*     }  */

    /* PUSH des valeurs directement dans le datapool */
    for(i = 0 ; i <  nb_consumed_symbols ; ++i) {
      /* retrieve the pgi of the consumed symbol */
      pgi = ptr_consumed_index[i];
      item.time                  = glu_time;
      item.provider_global_index = pgi;
      /* we return a double value even if 
       * the blackboard type is different
       * since TSP only knows double ... till now */
      item.value                 = bb_double_of(value_by_pgi[pgi],bbtype_by_pgi[pgi]);
      TSP_datapool_push_next_item(&item);      
    }
    TSP_datapool_push_commit(glu_time, GLU_GET_NEW_ITEM);
      
    ++glu_time;
/*     if ( 0 == (glu_time%1000) ) { */
/*       bb_logMsg(BB_LOG_INFO,"bb_tsp_provider::GLU_thread", */
/* 		  "Time Stamp <%d>\n",glu_time); */
/*     } */

  }

  return NULL;
  
} /* end of GLU_thread */

int GLU_start(void)
{
  if (0==glu_thread_id) {
    return pthread_create(&glu_thread_id, NULL, GLU_thread, NULL); 
  } else {
    return 1;
  }
}

GLU_handle_t 
GLU_get_instance(int custom_argc,
		 char* custom_argv[],
		 char** error_info) {

  if(error_info)
    *error_info = "";

  return GLU_GLOBAL_HANDLE;
}  /* end of GLU_get_instance */


/* void  */
/* GLU_forget_data(GLU_handle_t h_glu) { */
/*   RINGBUF_PTR_RESET_CONSUMER(glu_ring); */
/* } */


int32_t 
bb_tsp_provider_initialise(int* argc, char** argv[],int i_mode, const char* bbname) {
  
  int32_t retcode;
  
  retcode = E_OK;
  the_bbname = strdup(bbname);
  /* Init LibTSP provider */
  TSP_provider_init(argc, argv);  
  /* demarrage provider */
  TSP_provider_run(i_mode);
  /* 
   * Si demarrage en mode non bloquant 
   * attendre demarrage thread provider
   * FIXME ce mode "d'attente" est pourlingue il faut une
   * API TSP pour gérer ces synchros de démarrage de thread.
   */
  if (0 == i_mode) {
    sleep(1);
    sched_yield();  
    while (TSP_provider_rqh_manager_get_nb_running()<1) {
      printf("Waiting TSP provider to start...\n");
      fflush(stdout);
      sleep(1);
      sched_yield();    
    } 
  }  
  return retcode;
} /* end of bb_tsp_provider_initialise */
