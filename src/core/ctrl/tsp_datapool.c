/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.4 2002-09-18 08:14:52 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the function that read data from the sample
server, and for each opened session, ask the session to send its data to its
consumer

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>
#include <time.h>

#include "tsp_datapool.h"

#include "tsp_datastruct.h"
#include "glue_sserver.h"
#include "tsp_session.h"
#include "tsp_time.h"

/*-----------------------------------------------------*/

struct TSP_datapool_item_t 
{
	
  int user_counter_sync;
	
  int user_counter_async;
	
  double user_value;
	
};

typedef struct TSP_datapool_item_t TSP_datapool_item_t;



/*-----------------------------------------------------*/

static TSP_datapool_item_t* X_datapool_t = 0;
static int X_datapool_size = 0;
static glu_ringbuf* X_ring = 0;

/*-----------------------------------------------------*/

#define TSP_CHECK_PROVIDER_GLOBAL_INDEX(index, ret) \
{ \
	if( index >= X_datapool_size ) \
	{ \
		STRACE_ERROR(("ERROR :-->OUT : provider_global_index %d does not exist !", index)) \
		return ret; \
	} \
}

/*---------------------------------------------------------*/
/*                  FONCTIONS INTERNES  			   */
/*---------------------------------------------------------*/

int* TSP_datapool_get_symbol_usage_counter(int provider_global_index, xdr_and_sync_type_t type)
{
  SFUNC_NAME(TSP_datapool_get_symbol_counter);

	
  int* p = 0;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(provider_global_index, FALSE);
	
  /*FIXME : Remettre ca comme il faut */
  p = &(X_datapool_t[provider_global_index].user_counter_sync);
    
  /*
    
    switch(type)
    {
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_SYNC :
    p = &(X_datapool_t[provider_global_index].user_counter_sync);
    break;
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_ASYNC :
    p = &(X_datapool_t[provider_global_index].user_counter_async);
    break;
    default :
    STRACE_ERROR(("Unknown xdr_and_sync_type_t : %X", type));
    }
  */
  STRACE_IO(("-->OUT"));

	
  return p;
}

extern long count_add;
static void* TSP_worker(void* arg)
{
  SFUNC_NAME(TSP_worker);

  tsp_hrtime_t apres_fifo=0, avant_fifo=0, apres_send=0,avant_send=0;
  double total_send, total_fifo = 0;
  double very_total_send = 0;
  int nombre_send = 0;

  glu_item_t* item_ptr=NULL;
  glu_item_t item; 
  time_stamp_t time_stamp = 0;
  long count_remove = 0;

  STRACE_IO(("-->IN"));
    
  /* wait for data */
  while (RINGBUF_PTR_ISEMPTY(X_ring))
    {
      tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
    }
  RINGBUF_PTR_NOCHECK_GET(X_ring,item);

  time_stamp = item.time;
  count_remove ++;

  /* flush ringbuff until first time_stamp of a whole new data set   */
  do
    {
      if (!RINGBUF_PTR_ISEMPTY(X_ring))
	{
	  RINGBUF_PTR_NOCHECK_GET(X_ring, item); 
	  count_remove ++;
	}
      else
	{
	  tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
	}
        
    } while (item.time == time_stamp); 
     
  /*FIXME : gerer tous les machins avec les types user, raw...*/
  /* Resfresh data pool with new value */
  X_datapool_t[item.provider_global_index].user_value = item.value;     
  time_stamp = item.time;

  /* infinite loop on buff to update datapool */
  while(1)
    {
      avant_fifo = tsp_gethrtime();
      item_ptr = RINGBUF_PTR_GETBYADDR(X_ring);   

      /* while data with same time in buff, fill datapool */
      while(item_ptr && (time_stamp == item_ptr->time))
	{
	  count_remove ++;


	  /* time stamp change, need to send every things */
	  /*FIXME : gerer tous les machins avec les types user, raw...*/
	  /* Resfresh data pool with new value */
	  X_datapool_t[item_ptr->provider_global_index].user_value = item_ptr->value;
  
	  /*FIXME : gérer le fait qu'on peut perdre des données ! */            
	  RINGBUF_PTR_GETBYADDR_COMMIT(X_ring);
	  item_ptr = RINGBUF_PTR_GETBYADDR(X_ring);
	}
      
      /*---*/
      apres_fifo = tsp_gethrtime();
      total_fifo += (double)(apres_fifo - avant_fifo)/1e6;
      /*---*/

      	  
      if (item_ptr && (time_stamp != item_ptr->time))
	{
	  avant_send = tsp_gethrtime();
	  /* Send data to all clients  */	      
	  TSP_session_all_session_send_data(time_stamp);
	  apres_send = tsp_gethrtime();	 
	  total_send += (double)(apres_send - avant_send)/1e6;
	  nombre_send ++;
	  time_stamp = item_ptr->time;
	}

      if(!(time_stamp % 113))
	{
	  STRACE_INFO(("SEND_ALL  (tt=%d) msend=%f, mfifo=%f, nbsend=%d, diff=%d", 
		       time_stamp, total_send/nombre_send, total_fifo/nombre_send, nombre_send, count_add - count_remove));
	}
      
      /* wait for data */
      while (RINGBUF_PTR_ISEMPTY(X_ring))
	{
	  tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
	} 
    }
    
  STRACE_IO(("-->OUT"));

}

/*---------------------------------------------------------*/
/*                  FONCTIONS EXTERNE  			   */
/*---------------------------------------------------------*/

int TSP_datapool_init(void)
{
	 
  SFUNC_NAME(TSP_datapool_init);

	
  TSP_sample_symbol_info_list_t symbols;
  int i;
  int status;
  pthread_t thread_id;
     
  STRACE_IO(("-->IN"));
	
  GLU_get_sample_symbol_info_list(&symbols);

  X_datapool_size = symbols.TSP_sample_symbol_info_list_t_len;

  X_datapool_t = 
    (TSP_datapool_item_t*)calloc(X_datapool_size,
				 sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(X_datapool_t, FALSE);
    
  /* Get ring buffer */
  X_ring = GLU_get_ringbuf();
    
  /* Demarrage du thread */
  /* FIXME : faire l'arret du thread */
  /* FIXME : detacher le thread */
  status = pthread_create(&thread_id, NULL, TSP_worker,  NULL);
  TSP_CHECK_THREAD(status, FALSE);

    
  STRACE_IO(("-->OUT"));
  return TRUE;

}

int TSP_datapool_add_symbols(TSP_sample_symbol_info_list_t* symbols)
{
	
  SFUNC_NAME(TSP_datapool_add_symbols);

	
  guint32 i;
  int ret = TRUE;
  int* p_usage_counter;

	
  STRACE_IO(("-->IN"));

	
  for( i = 0 ; i < symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      gint32 index = symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index;
      xdr_and_sync_type_t type = *((xdr_and_sync_type_t*)symbols->TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t);
	
      /* We get the counter for this counter and this type */	
      p_usage_counter = TSP_datapool_get_symbol_usage_counter(index, type);
      if( 0 != p_usage_counter )
	{
	  /* Is the symbol already used ? */
	  if ( 0 == (*p_usage_counter))
	    {
				/* No. We had it to the list*/
	      if(!GLU_add_block(index,type))
		{
		  ret = FALSE;
		  STRACE_ERROR(("Unable to add symbol to sample_server for global_index=%d", index));

		  break;
		}
	    }
	}
      else
	{
	  ret = FALSE;
	  STRACE_ERROR(("Unable to get Usage Counter for symbol"));

	  break;	
	}
			
      (*p_usage_counter)++;
		
      if(ret)
	{
	  if(!GLU_commit_add_block())
	    {
	      ret = FALSE;
	      STRACE_ERROR(("GLU_commit_add_block failed"));

	    }
	}
		
    }
	
  STRACE_IO(("-->OUT"));

	
  return ret;

}

void* TSP_datapool_get_symbol_value(int provider_global_index, xdr_and_sync_type_t type)
{
  SFUNC_NAME(TSP_datapool_get_symbol_value);

	
  void* p = 0;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(provider_global_index, 0);

  /*FIXME !!!!!!!!!!!!!!!!!!!!!!!!!!!
    A remettre */
    
  /* Temporaire*/
  p = &(X_datapool_t[provider_global_index].user_value);
	
  /*switch(type & XDR_DATA_TYPE_MASK)
    {
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_SYNC :
    p = &(X_datapool_t[provider_global_index].user_value);
    break;
    default :
    STRACE_ERROR(("Unknown XDR type : %X", type));
    }
  */
    
  STRACE_IO(("-->OUT"));

	
  return p;
}
