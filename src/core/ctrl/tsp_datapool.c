/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.6 2002-10-09 07:41:17 galles Exp $

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

#include "tsp_session.h"
#include "tsp_time.h"

/* Pool time waiting for  consumer connection µs */
#define TSP_LOCAL_WORKER_CONNECTION_POOL_TIME ((int)(1e5))

/*-----------------------------------------------------*/

struct TSP_datapool_item_t 
{
	
  int user_counter_sync;
	
  int user_counter_async;
	
  double user_value;
	
};

typedef struct TSP_datapool_item_t TSP_datapool_item_t;

struct TSP_datapool_table_t
{
  TSP_datapool_item_t* items;
  int size;
  
  pthread_t worker_id;

  /** Only for global thread id*/
  glu_ringbuf* ring;

  /** Only for local thread id.
   * The id of the session linked to the local datapool
   */
  channel_id_t session_channel_id;

  /** handle on GLU */
  GLU_handle_t h_glu;

};

typedef struct TSP_datapool_table_t TSP_datapool_table_t;



/*-----------------------------------------------------*/

TSP_datapool_table_t X_global_datapool = {0,0,0};

/*static TSP_datapool_item_t* X_datapool_t = 0;
static int X_datapool_size = 0;
static glu_ringbuf* X_ring = 0;*/

/*-----------------------------------------------------*/

#define TSP_CHECK_PROVIDER_GLOBAL_INDEX(obj, index, ret) \
{ \
	if( (index) >= (obj).size ) \
	{ \
		STRACE_ERROR(("ERROR :-->OUT : provider_global_index %d does not exist !",(index))) \
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

	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(X_global_datapool, provider_global_index, FALSE);
	
  /*FIXME : Remettre ca comme il faut */
  p = &(X_global_datapool.items[provider_global_index].user_counter_sync);
    
  /*
    
    switch(type)
    {
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_SYNC :
    p = &(X_global_datapool.items[provider_global_index].user_counter_sync);
    break;
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_ASYNC :
    p = &(X_global_datapool.items[provider_global_index].user_counter_async);
    break;
    default :
    STRACE_ERROR(("Unknown xdr_and_sync_type_t : %X", type));
    }
  */
  STRACE_IO(("-->OUT"));

	
  return p;
}


/**
 * Thread created per session when the sample server is a pasive one.
 * @param datapool The datapool object instance that will be linked to the thread
 */ 
static void* TSP_local_worker(void* datapool)
{

  /* FIXME : WARNING : la datapool a un pointeur sur le GLU, donc, ne pas détruire le
     GLU avant d'avoir arreté ce Thread */

  SFUNC_NAME(TSP_local_worker);
  time_stamp_t time_stamp;
  glu_item_t item;
  int more_items;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;  

  STRACE_IO(("-->IN"));
  
  STRACE_INFO(("Local datapool thread started for session %d",obj_datapool->session_channel_id)); 
  
  /* Wait for consumer connection before we send data */  
  while(!TSP_session_is_consumer_connected_by_channel(obj_datapool->session_channel_id))
    {
      tsp_usleep(TSP_LOCAL_WORKER_CONNECTION_POOL_TIME);
    }
  STRACE_INFO(("Consumer connected for session id %d",obj_datapool->session_channel_id)); 
  /* FIXME : s'occuper des divers types raw, string... */

  /* get first item */
  more_items = GLU_pasive_get_next_item(obj_datapool->h_glu, &item);
  if (more_items)
    {
      time_stamp = item.time;
      /* Update datapool */
      obj_datapool->items[item.provider_global_index].user_value = item.value;           

      while(GLU_pasive_get_next_item(obj_datapool->h_glu, &item))
	{       
	  /* is the datapool coherent ? */
	  if( time_stamp != item.time )
	    {
	      /* Yep ! throw data to client */
	      TSP_session_send_data_by_channel(obj_datapool->session_channel_id, time_stamp);	  
	      time_stamp = item.time;
 
	    }
	  /* Update datapool */
	  obj_datapool->items[item.provider_global_index].user_value = item.value;     
	}      

      /* The lastest data were not sent coz we did not compare the latest timestamp*/
      TSP_session_send_data_by_channel(obj_datapool->session_channel_id, time_stamp);

      /* Send group EOF */
      TSP_session_send_data_eof_by_channel(obj_datapool->session_channel_id);
    }
      
  STRACE_IO(("-->OUT"));

}

extern long count_add;

/**
 * Thread created when the sample server is an active one
 * @param dummy Not used
 */ 
static void* TSP_global_worker(void* dummy)
{
  SFUNC_NAME(TSP_global_worker);

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
  while (RINGBUF_PTR_ISEMPTY(X_global_datapool.ring))
    {
      tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
    }
  RINGBUF_PTR_NOCHECK_GET(X_global_datapool.ring,item);

  time_stamp = item.time;
  count_remove ++;

  /* flush ringbuff until first time_stamp of a whole new data set   */
  do
    {
      if (!RINGBUF_PTR_ISEMPTY(X_global_datapool.ring))
	{
	  RINGBUF_PTR_NOCHECK_GET(X_global_datapool.ring, item); 
	  count_remove ++;
	}
      else
	{
	  tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
	}
        
    } while (item.time == time_stamp); 
     
  /*FIXME : gerer tous les machins avec les types user, raw...*/
  /* Resfresh data pool with new value */
  X_global_datapool.items[item.provider_global_index].user_value = item.value;     
  time_stamp = item.time;

  /* infinite loop on buff to update datapool */
  while(1)
    {
      avant_fifo = tsp_gethrtime();
      item_ptr = RINGBUF_PTR_GETBYADDR(X_global_datapool.ring);   

      /* while data with same time in buff, fill datapool */
      while(item_ptr && (time_stamp == item_ptr->time))
	{
	  count_remove ++;


	  /* time stamp change, need to send every things */
	  /*FIXME : gerer tous les machins avec les types user, raw...*/
	  /* Resfresh data pool with new value */
	  X_global_datapool.items[item_ptr->provider_global_index].user_value = item_ptr->value;
  
	  /*FIXME : gérer le fait qu'on peut perdre des données ! */            
	  RINGBUF_PTR_GETBYADDR_COMMIT(X_global_datapool.ring);
	  item_ptr = RINGBUF_PTR_GETBYADDR(X_global_datapool.ring);
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
      while (RINGBUF_PTR_ISEMPTY(X_global_datapool.ring))
	{
	  tsp_usleep(TSP_DATAPOOL_POOL_PERIOD);
	} 
    }
    
  STRACE_IO(("-->OUT"));

}

/*---------------------------------------------------------*/
/*                  FONCTIONS EXTERNE  			   */
/*---------------------------------------------------------*/

/**
 * Start local datapool thread be per session.
 * Only used when the sample server is a pasive one
 * @param datapool The datapool instance that will be linked to the thread
 */ 
int TSP_local_datapool_start_thread(TSP_datapool_t datapool)
{
  SFUNC_NAME(TSP_global_datapool_init);
  int status;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

  STRACE_IO(("-->IN"));

  status = pthread_create(&(obj_datapool->worker_id), NULL, TSP_local_worker,  datapool);
  TSP_CHECK_THREAD(status, FALSE);

  STRACE_IO(("-->OUT"));
  return TRUE;

}

/**
 * Start global datapool thread.
 * Only used when the sample server is an active one
 * @return TRUE = OK
 */ 
int TSP_global_datapool_init(void)
{
	 
  SFUNC_NAME(TSP_global_datapool_init);

	
  TSP_sample_symbol_info_list_t symbols;
  int i;
  int status;
 
     
  STRACE_IO(("-->IN"));
	
  /* Here the datapool is global */
  X_global_datapool.h_glu = GLU_GLOBAL_HANDLE;   

  GLU_get_sample_symbol_info_list(X_global_datapool.h_glu, &symbols);

  X_global_datapool.size = symbols.TSP_sample_symbol_info_list_t_len;

  X_global_datapool.items = 
    (TSP_datapool_item_t*)calloc(X_global_datapool.size,
				 sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(X_global_datapool.items, FALSE);
    
  /* Get ring buffer */
  X_global_datapool.ring = GLU_active_get_ringbuf(X_global_datapool.h_glu);
    
  /* Demarrage du thread */
  /* FIXME : faire l'arret du thread */
  /* FIXME : detacher le thread */
  status = pthread_create(&(X_global_datapool.worker_id), NULL, TSP_global_worker,  NULL);
  TSP_CHECK_THREAD(status, FALSE);

    
  STRACE_IO(("-->OUT"));
  return TRUE;

}


TSP_datapool_t TSP_local_datapool_allocate(channel_id_t session_channel_id, int symbols_number, GLU_handle_t h_glu )
{
	 
  SFUNC_NAME(TSP_local_datapool_allocate);

	
  TSP_sample_symbol_info_list_t symbols;
  TSP_datapool_table_t* datapool;
     
  STRACE_IO(("-->IN"));

  /* FIXME : fuite, fuite, fuite ...*/

  datapool = (TSP_datapool_table_t*)calloc(1,sizeof(TSP_datapool_table_t));
  TSP_CHECK_ALLOC(datapool, 0);
	
  datapool->size = symbols_number;

  datapool->items = (TSP_datapool_item_t*)calloc(datapool->size,sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(datapool->items, 0);
    
  /* set session linked to this datapool*/
  datapool->session_channel_id = session_channel_id;

  datapool->h_glu = h_glu;

  /* No ring buffer at all for a local datapool*/
  datapool->ring = 0;
    
  STRACE_IO(("-->OUT"));
  return datapool;

}


int TSP_global_datapool_add_symbols(TSP_sample_symbol_info_list_t* symbols)
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
	      /* FIXME : faire un appel a une fonction de session GLU_session_add_block_per_channel */
	      if(!GLU_add_block(GLU_GLOBAL_HANDLE, index,type))
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
	      /* FIXME : faire un appel a une fonction de session GLU_session_add_block_per_channel */
	  if(!GLU_commit_add_block(GLU_GLOBAL_HANDLE))
	    {
	      ret = FALSE;
	      STRACE_ERROR(("GLU_commit_add_block failed"));

	    }
	}
		
    }
	
  STRACE_IO(("-->OUT"));

	
  return ret;

}

void* TSP_datapool_get_symbol_value(TSP_datapool_t datapool, int provider_global_index, xdr_and_sync_type_t type)
{
  SFUNC_NAME(TSP_datapool_get_symbol_value);

	
  void* p = 0;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(*obj_datapool, provider_global_index, 0);

  /*FIXME !!!!!!!!!!!!!!!!!!!!!!!!!!!
    A remettre */
    
  /* Temporaire*/
  p = &(obj_datapool->items[provider_global_index].user_value);
	
  /*switch(type & XDR_DATA_TYPE_MASK)
    {
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_SYNC :
    p = &(X_global_datapool.items[provider_global_index].user_value);
    break;
    default :
    STRACE_ERROR(("Unknown XDR type : %X", type));
    }
  */
    
  STRACE_IO(("-->OUT"));

	
  return p;
}


TSP_datapool_t TSP_global_datapool_get_instance(void)
{
  return &X_global_datapool;
}
