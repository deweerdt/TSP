
#include <bb_core.h>
#include <bb_simple.h>
#include <bb_tsp_provider.h>
#include <tsp_provider_init.h>

/*
 * Declaration crado pour demarrage TSP en douceur
 */
int TSP_provider_rqh_manager_get_nb_running();

/* 
 * On inclu les en-t�tes TSP
 * necessaire a l'implementation d'un GLU
 * car on doit r�aliser ces fonctions dans
 * notre provider TSP.
 * Consulter le header 'glue_sserver.h' afin de comprendre
 * la r�alisation des diff�rentes fonctions.
 */
#include <tsp_sys_headers.h>
#include <glue_sserver.h>
#include <tsp_ringbuf.h>

#include <signal.h>

RINGBUF_DECLARE_TYPE_DYNAMIC(glu_ringbuf,glu_item_t);

#define GLU_RING_BUFSIZE (1000 * 64 * 10)

/*
 * Declaration externe
 */
extern pthread_t glu_thread_id;

/*
 * Quelques variables static
 */
/* le nom du provider */
static char* X_server_name = "BB-TSP-V0_2";
/* le ringbuffer entre le GLU et le reste du monde */
static glu_ringbuf* glu_ring = 0;
/* la liste des symboles */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val = NULL;

/* The BB and its shadow BB */
static S_BB_T* the_bb = NULL;
static char* the_bbname = NULL;
static S_BB_T* shadow_bb = NULL;

/* Le time stamp time du GLU */
static time_stamp_t glu_time = 0;
/* Tableau des adresses des valeurs dans le shadow BB index� par
 * le provider_global_index
 */
static void** value_by_pgi = NULL;
static E_BB_TYPE_T * bbtype_by_pgi = NULL;
/*
 * La frequence de base du provider devra �tre celle
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
   * !!! si on est un process separe de celui qui le cr�e
   */
  if (bb_attach(&the_bb,the_bbname) != E_OK) {
    bb_logMsg(BB_LOG_SEVERE,
	      "bb_tsp_provider::GLU_init","Cannot attach to BlackBoard <%s>!!",
	      the_bbname);
    retcode = FALSE;
  } 
  
  /* 
   * R�cup�re un acces direct au blackboard 
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
    /* Initial update (ce qui est inutile mais �a ne gate rien */
    if (E_NOK ==  bb_shadow_update_data(shadow_bb,the_bb)) {
      retcode = FALSE;
    } 
  }

  /* Construction de la liste des symboles */
  /* On doit calculer le nombre d'item SCALAIRE 
   * car TSP ne g�re pas [encore] les tableaux
   */
  i_nb_item_scalaire = 0;
  for (i=0;i<bb_get_nb_item(shadow_bb);++i) {
    i_nb_item_scalaire += bb_data_desc(shadow_bb)[i].dimension;
  }
  /* On alloue une liste de symboles de la taille
   * correspondant au nombre de donn�es (scalaire) enregistr�es dans le BB
   */
  X_sample_symbol_info_list_val = calloc (i_nb_item_scalaire,
					  sizeof (TSP_sample_symbol_info_t));
  assert(X_sample_symbol_info_list_val);
  /* On alloue 
   * le tableau de pointeurs vers les donn�es
   */
  value_by_pgi = (void **) calloc(i_nb_item_scalaire,sizeof(void*));
  bbtype_by_pgi = (E_BB_TYPE_T *) calloc(i_nb_item_scalaire,sizeof(E_BB_TYPE_T));

  assert(value_by_pgi);	
  /* on initialise le provider global index a 0 */
  i_pg_index = 0;
  for (i=0; i<bb_get_nb_item(shadow_bb);++i) {
    /* TSP ne g�re pas encore les tableaux
     * donc on g�n�re des noms de symboles postfix�s par
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
	/* on pointe vers la valeur ad�quate */
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
  if (TRUE == retcode) {
    /*    RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(GLU_RING_BUFSIZE)); */
    RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(i_pg_index*32*10)); 
    bb_logMsg(BB_LOG_CONFIG,"bb_tsp_provider::GLU_init","Ring Buffer Size is <%d>",glu_ring->size);
    RINGBUF_PTR_RESET_CONSUMER (glu_ring);
  }
  return retcode;
} /* end of GLU_init */


/*
 * C'est un provider actif car il d�pend
 * d'une simulation potentiellement temps r�elle et 
 * n'attendra donc pas que les �ventuels consumer
 * consomment ce qu'ils ont demand�s pour produire.
 * Si les consumers sont trop lents ils perdront des donn�es
 * le provider ne s'arr�tera pas.
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

static int data_missed = FALSE;

static void* GLU_thread(void* arg) {
  
  int i;
  int i_nb_symbols;
  glu_item_t item;
  static int last_missed = 0;
  sigset_t s_mask;
  
  i_nb_symbols  = GLU_get_symbol_number();
  bb_logMsg(BB_LOG_INFO,
	      "bb_tsp_provider::GLU_thread",
	      "Provider thread started with <%d> symbols",
	      i_nb_symbols);
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
    /* On attend le d�blocage du PE */
    if (E_NOK == bb_simple_synchro_wait(the_bb,BB_SIMPLE_MSGID_SYNCHRO_COPY)) {
      /* on sort car le BB a �t� d�truit */
      break;
    }
    /* 
     * MAJ des donnees du shadow BB 
     */    
    bb_shadow_update_data(shadow_bb,the_bb);
   /*  bb_simple_synchro_go(BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK); */
    while ( RINGBUF_PTR_ITEMS_LEFT(glu_ring) < (i_nb_symbols*sizeof(item)+64) ) {
      usleep(300);
      bb_logMsg(BB_LOG_WARNING,"bb_tsp_provider::GLU_thread",
		  "RINGBUF nb Item Left= <%d>",RINGBUF_PTR_ITEMS_LEFT(glu_ring));
      sched_yield();
    } 

    /* PUSH des valeurs dans le RING BUF */
    for(i = 0 ; i <  i_nb_symbols ; ++i) {
      item.time                  = glu_time;
      item.provider_global_index = i;
      /* we return a double value even if 
       * the blackboard type is different
       * since TSP only knows double ... till now */
      item.value                 = bb_double_of(value_by_pgi[i],bbtype_by_pgi[i]);
      RINGBUF_PTR_PUT(glu_ring, item);
    }
      
    if(last_missed!=RINGBUF_PTR_MISSED(glu_ring)) {
      last_missed = RINGBUF_PTR_MISSED(glu_ring);
      data_missed = TRUE;
    }
    
    ++glu_time;
/*     if ( 0 == (glu_time%1000) ) { */
/*       bb_logMsg(BB_LOG_INFO,"bb_tsp_provider::GLU_thread", */
/* 		  "Time Stamp <%d>\n",glu_time); */
/*     } */

  }

  return NULL;
  
} /* end of GLU_thread */

GLU_handle_t 
GLU_get_instance(int custom_argc,
		 char* custom_argv[],
		 char** error_info) {

  if(error_info)
    *error_info = "";

  return GLU_GLOBAL_HANDLE;
}  /* end of GLU_get_instance */


GLU_get_state_t 
GLU_get_next_item(GLU_handle_t h_glu,glu_item_t* item) {

  GLU_get_state_t res = GLU_GET_NEW_ITEM;
  assert(h_glu == GLU_GLOBAL_HANDLE);
  
  if(!data_missed)
    {
      if (!RINGBUF_PTR_ISEMPTY(glu_ring))
	{
	  /* OK data found */
	  RINGBUF_PTR_NOCHECK_GET(glu_ring ,(*item));                  
	}
      else
	{
	  res = GLU_GET_NO_ITEM;

          /* Maybe there's no item coz the thread is not started : start it !
             And yes, this is ugly. In a perfect world, this should be in GLU_get_instance,
             but it does not work : the fifo may be full before the very first
             GLU_get_next_item
          */ 
	  if(!glu_thread_id) {
              pthread_create(&glu_thread_id, NULL, GLU_thread, NULL);
	  } 

	}
    }
  else
    {
      /* There's a huge problem, we were unable to put data
	 in the ringbuffer */
      res = GLU_GET_DATA_LOST;
    }

  return res;
}

void 
GLU_forget_data(GLU_handle_t h_glu) {
  RINGBUF_PTR_RESET_CONSUMER(glu_ring);
}


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
   * API TSP pour g�rer ces synchros de d�marrage de thread.
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
