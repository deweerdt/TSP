/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/Attic/glue_sserver.c,v 1.3 2002-09-05 13:29:54 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>
#include <math.h>
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"



#define TSP_NANOSLEEP_PERIOD_NS 10*1e6



#define GLU_MAX_SYMBOLS 1000

/*RINGBUF_DEFINE_DYNAMIC(glu_ringbuf, glu_ring,  RINGBUF_SZ(GLU_RING_BUFSIZE));*/

static struct timespec X_sleep_time = {0, TSP_NANOSLEEP_PERIOD_NS};

static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;

static tsp_hrtime_t X_lasttime;

static glu_ringbuf* glu_ring = 0;

/* =
{

{"Symbol1", 0, 0, 0, {0,0,0,0}, 0, 3, 0},
{"Symbol2", 1, 0, 0, {0,0,0,0}, 0, 2, 0},
{"Symbol3", 2, 0, 0, {0,0,0,0}, 0, 2, 0},
{0, 0, 0, 0, 0, 0 , 0,0, 0}
 
};*/

static char* X_server_name = "MonServeur";

static time_stamp_t my_time = 0;

glu_ringbuf* GLU_get_ringbuf(void)
{
  return glu_ring;
}

char* GLU_get_server_name(void)
{
  return X_server_name;
}

int  GLU_get_symbol_number(void)

{
  SFUNC_NAME(GLU_get_symbol_number);

	
  int i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = X_sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  STRACE_IO(("-->OUT : -Nombre symboles = %u", i));
    
  return i;
}

long count_add = 0;

static void* GLU_thread(void* arg)
{
  
  SFUNC_NAME(GLU_thread);

  static int last_missed = 0;
  int symbols_nb;
  int i;
  tsp_hrtime_t current_time;
  
  glu_item_t item;

  symbols_nb  = GLU_get_symbol_number();
    
  STRACE_IO(("-->IN"));
    
  current_time = X_lasttime = tsp_gethrtime();  
  while(1)
    {
      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  if(my_time%X_sample_symbol_info_list_val[i].period == 0)
	    {

	      item.time = my_time;
	      item.provider_global_index = i;
	      item.value = sin(my_time + i);
                
	      /* STRACE_DEBUG(("IN->T=%d, Id=%d, V=%f",
		 item.time,
		 item.provider_global_index,
		 item.value))*/
                
	      RINGBUF_PTR_PUT(glu_ring, item);
	      count_add ++;
                     
	    }
	}
      
      if( current_time <= X_lasttime )
	{ 
	  nanosleep(&X_sleep_time,NULL);
	}
      X_lasttime += TSP_NANOSLEEP_PERIOD_NS;
      current_time = tsp_gethrtime();
      
      if(last_missed!=RINGBUF_PTR_MISSED(glu_ring))
	{
	  STRACE_ERROR(("RingBuffer full : %d missed data", RINGBUF_PTR_MISSED(glu_ring)));
	  last_missed = RINGBUF_PTR_MISSED(glu_ring);
	  sleep(0);
	}
      
      my_time++;    
      
      if(! (my_time%100))  STRACE_INFO(("TOP !"));
      
			     }
  

  
  STRACE_IO(("-->OUT"));
    
  return arg;

}

int TSP_glue_sserver_init(void)
{
  SFUNC_NAME(TSP_glue_sserver_init);

  
  pthread_t thread_id;	
  int i;
  
  STRACE_IO(("-->IN"));
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS+1, sizeof (TSP_sample_symbol_info_t)) ;
  for (i=0; i<GLU_MAX_SYMBOLS; i++)
    {      
      X_sample_symbol_info_list_val[i].name = "symbol";
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
    }
  
  RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  RINGBUF_SZ(GLU_RING_BUFSIZE));
  RINGBUF_PTR_RESET (glu_ring);
  TSP_CHECK_THREAD( (pthread_create(&thread_id, NULL, GLU_thread, NULL)), FALSE);
  
  STRACE_IO(("-->OUT"));
  
  return TRUE;
  
}

void  GLU_get_sample_symbol_info_list
(TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(TSP_DTP_get_sample_symbol_info_list);

	
  gint32 i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = X_sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  STRACE_IO(("-->OUT : -Nombre symboles = %u", symbol_list->TSP_sample_symbol_info_list_t_len));
}

int GLU_add_block(int provider_global_index, xdr_and_sync_type_t type)
{

  SFUNC_NAME(GLU_add_block);
  STRACE_IO(("-->IN index=%d type=%X", provider_global_index, type));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  /* FIXME : gérer ca ! */
  STRACE_IO(("-->OUT"));
	
  return TRUE;
}

int GLU_commit_add_block(void)
{
  SFUNC_NAME(GLU_commit_add_block);
  STRACE_IO(("-->IN"));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  STRACE_IO(("-->OUT"));
	
  return TRUE;
	
}
