/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.20 2004-09-16 09:38:42 tractobob Exp $

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
Maintainer: tsp@astrium.eads.net 
Component : Provider

-----------------------------------------------------------------------

Purpose   : Datapool implementation
FIXME : the global/local datapool cases could have been avoided. Somehow plenty of code
may be unified in this source .

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_datapool.h"

#include "tsp_datastruct.h"

#include "tsp_session.h"
#include "tsp_time.h"

/* Poll time waiting for  consumer connection (us) */
#define TSP_LOCAL_WORKER_CONNECTION_POLL_TIME ((int)(1e5))

/* Data Poll period (us) */
#define TSP_DATAPOOL_POLL_PERIOD ((int)(1e4))


/*-----------------------------------------------------*/

struct TSP_datapool_item_t 
{
  /** One Item in the datapool.
   * FIXME : The values should be saved in raw format in the datapool,
   * and must be calculated for their final value when they are sent, depending
   * on the data format that each consumer requested (double, raw, string).
   * Somehow the GLU should provide its own functions to transform a RAW in double or
   * string.
   */
  double user_value;
  int is_wanted;	/* Is the symbol wanted by some consummers */
};

typedef struct TSP_datapool_item_t TSP_datapool_item_t;

struct TSP_datapool_table_t
{

  /** Is the datapool initialized ? */
  int initialized;

  /** Tells if the datapool was used and destroyed */
  int terminated;
  
  /** handle on GLU */
  GLU_handle_t h_glu;

  /** List of items in the datapool */
  TSP_datapool_item_t* items;
  int size;

  /** Reverse list of wanted items index */
  int *reverse_index;
  int nb_wanted_items;

};

typedef struct TSP_datapool_table_t TSP_datapool_table_t;

/*-----------------------------------------------------*/

TSP_datapool_table_t X_global_datapool = {FALSE,FALSE,0,0,0};

/*-----------------------------------------------------*/

#define TSP_CHECK_PROVIDER_GLOBAL_INDEX(obj, index, ret) \
{ \
	if( (index) >= (obj).size ) \
	{ \
		STRACE_ERROR(("ERROR :-->OUT : provider_global_index %d does not exist !",(index))) \
		return ret; \
	} \
}


/**
 * Get the reverse list of global_index wanted by some consumers
 * @param nb   : pointer on where to store the number of items
 * @param list : pointer on list of global_index
 */ 
void TSP_datapool_get_reverse_list (int *nb, int **list)
{
  assert(nb!=NULL);
  assert(list!=NULL);
  *nb = X_global_datapool.nb_wanted_items;
  *list = X_global_datapool.reverse_index;
}


/**
 * Insted of thread created, we push directly the data
 * @param item : what to push
 */ 
inline int TSP_datapool_push_next_item(glu_item_t* item)
{
  X_global_datapool.items[item->provider_global_index].user_value = item->value;
  return 0;
}

/**
 * End of push, we commit the whole
 * @param time : date of datapool items
 * @param state: ok or error (reconf,eof, ...)
 */ 
int TSP_datapool_push_commit(time_stamp_t time_stamp, GLU_get_state_t state)
{
  STRACE_DEBUG(("Datapool push new item time %d",time_stamp)); 

   /* Send end status message */
  switch(state)
    {
    case GLU_GET_NO_ITEM :
      /* nothing to do */
      break;

    case GLU_GET_NEW_ITEM :
      /* Yep ! throw data to all clients */
      /* For a global datapool, the thread must not end, even if a client is disconnected, so
	 we do not check any returned value from TSP_session_all_session_send_data */
      TSP_session_all_session_send_data(time_stamp);		  
      break;

    case GLU_GET_EOF :
      TSP_session_all_session_send_msg_ctrl(TSP_MSG_CTRL_EOF);
      STRACE_INFO(("GLU sent EOF"));
      /* End of flow. Our datapool is dead */
      X_global_datapool.terminated = TRUE;
      break;

    case GLU_GET_RECONF :
      TSP_session_all_session_send_msg_ctrl(TSP_MSG_CTRL_RECONF);
      STRACE_INFO(("GLU sent RECONF"));
      /* End of flow. Our datapool is dead */
      X_global_datapool.terminated = TRUE;
      break;

    case GLU_GET_DATA_LOST :
      TSP_session_all_session_send_msg_ctrl(TSP_MSG_CTRL_GLU_DATA_LOST);
      STRACE_INFO(("GLU sent DATA_LOST"));
      break;

    default:
      STRACE_ERROR(("?"));
      assert(0);
    }


  return 0;  
}

/* DON'T WANT ANYMORE this fucking thread passive callback f....cking philo */
#define USE_DATA_POOL_THREAD 0 
#if USE_DATA_POOL_THREAD
/**
 * Thread created per session when the sample server is a pasive one.
 * @param datapool The datapool object instance that will be linked to the thread
 */ 
void* TSP_datapool_thread(void* datapool)
{

  time_stamp_t time_stamp;
  glu_item_t item;
  int more_items;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;  
  GLU_get_state_t state;
  TSP_msg_ctrl_t msg_ctrl;  
  int data_link_ok = TRUE; /**< Used to store the  data link state for a local thread (broken or ok) */

  /* FIXME : The datapool might not be coherent when a client is already
     connected and this thread starts after the connection
     for the client */
  STRACE_DEBUG(("Data Pool is GLOBAL (active GLU)"));

  /* get first item */
  STRACE_DEBUG(("Waiting for First Item from GLU..."));
  while( ( GLU_GET_NEW_ITEM != (state=GLU_get_next_item(obj_datapool->h_glu, &item)))  )
    {
      tsp_usleep(TSP_DATAPOOL_POLL_PERIOD);
    }

  STRACE_DEBUG(("First Item from GLU received!"));

  time_stamp = item.time;
  /* Update datapool */
  obj_datapool->items[item.provider_global_index].user_value = item.value;           
  
  while( ((GLU_GET_NEW_ITEM == state) || (GLU_GET_NO_ITEM == state)) && data_link_ok )
    {
      while( (GLU_GET_NEW_ITEM == (state=GLU_get_next_item(obj_datapool->h_glu, &item))) && data_link_ok )
	{       
	  /* is the datapool coherent ? */
	  if( time_stamp != item.time )
	    {
	      /* Yep ! throw data to client */
	      /* For a global datapool, the thread must not end, even if a client is disconnected, so
		 we do not check any returned value from TSP_session_all_session_send_data */
	      TSP_session_all_session_send_data(time_stamp);		  
	      time_stamp = item.time;
	    }
	  /* Update datapool */
	  obj_datapool->items[item.provider_global_index].user_value = item.value;     
	}

       tsp_usleep(TSP_DATAPOOL_POLL_PERIOD);
    }      

   /* Send end status message */
  if(data_link_ok)
    {
      switch(state)
	{
	case   GLU_GET_EOF :
	  msg_ctrl = TSP_MSG_CTRL_EOF;
	  STRACE_INFO(("GLU sent EOF"));
	  break;
	case   GLU_GET_RECONF :
	  msg_ctrl = TSP_MSG_CTRL_RECONF;
	  STRACE_INFO(("GLU sent RECONF"));
	  break;
	case   GLU_GET_DATA_LOST :
	  msg_ctrl = TSP_MSG_CTRL_GLU_DATA_LOST;
	  STRACE_INFO(("GLU sent DATA_LOST"));
	  break;
	default:
	  STRACE_ERROR(("?"));
	  assert(0);
	}

      /* The lastest data were not sent coz we did not compare the latest timestamp. So, send them !*/
      /* Send data to all clients  */	      
      TSP_session_all_session_send_data(time_stamp);		  
      TSP_session_all_session_send_msg_ctrl(msg_ctrl);		  
    }

  /* End of thread. Our datapool is dead */
  obj_datapool->terminated = TRUE;
}



/**
 * Wait for the local datapool thread end per session.
 * Only used when the sample server is a pasive one
 * @param datapool The datapool instance linked to the thread
 */ 
int TSP_local_datapool_wait_for_end_thread(TSP_datapool_t datapool)
{
  int status;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

  status = pthread_join(obj_datapool->worker_id, NULL);
  TSP_CHECK_THREAD(status, FALSE);

  return TRUE;
}


/**
 * Start local datapool thread be per session.
 * Only used when the sample server is a passive one
 * @param datapool The datapool instance that will be linked to the thread
 */ 
int TSP_local_datapool_start_thread(TSP_datapool_t datapool)
{
  int status;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

  STRACE_ERROR(("Func is deprecated, use global datapool"));
  return -1;

  status = pthread_create(&(obj_datapool->worker_id), NULL, TSP_datapool_thread,  datapool);
  TSP_CHECK_THREAD(status, FALSE);

  return TRUE;

}
#endif

/**
 * Start global datapool thread.
 * Only used when the sample server is an active one
 * @return TRUE = OK
 */ 
static int TSP_global_datapool_init(void)
{
	 
  TSP_sample_symbol_info_list_t symbols;
	
  /* Here the datapool is global */
  X_global_datapool.h_glu = GLU_GLOBAL_HANDLE;   

  GLU_get_sample_symbol_info_list(X_global_datapool.h_glu, &symbols);

  X_global_datapool.size = symbols.TSP_sample_symbol_info_list_t_len;

  X_global_datapool.items = 
    (TSP_datapool_item_t*)calloc(X_global_datapool.size, sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(X_global_datapool.items, FALSE);

  X_global_datapool.reverse_index = (int*)calloc(X_global_datapool.size, sizeof(int));
  TSP_CHECK_ALLOC(X_global_datapool.reverse_index, FALSE);
  X_global_datapool.nb_wanted_items = 0;

  STRACE_INFO(("No More datapool thread "));
  /*status = pthread_create(&(X_global_datapool.worker_id), NULL, TSP_datapool_thread,  &X_global_datapool);
  TSP_CHECK_THREAD(status, FALSE);*/

  X_global_datapool.initialized = TRUE;
  X_global_datapool.terminated = FALSE;
    
  return TRUE;

}

static void TSP_datapool_internal_free(TSP_datapool_table_t* obj_datapool)
{
     free(obj_datapool->items);
     obj_datapool->items = 0;
     free(obj_datapool->reverse_index);
     obj_datapool->reverse_index = 0;
     obj_datapool->nb_wanted_items = 0;
}

void TSP_local_datapool_destroy(TSP_datapool_t datapool)
{
   TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

   TSP_datapool_internal_free(obj_datapool);
   free(obj_datapool);
}


TSP_datapool_t TSP_local_datapool_allocate(int symbols_number, GLU_handle_t h_glu )
{
  TSP_datapool_table_t* datapool;
     
  datapool = (TSP_datapool_table_t*)calloc(1,sizeof(TSP_datapool_table_t));
  TSP_CHECK_ALLOC(datapool, 0);
	
  datapool->size = symbols_number;

  datapool->items = (TSP_datapool_item_t*)calloc(datapool->size,sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(datapool->items, 0);
    
  datapool->h_glu = h_glu;
  datapool->initialized = TRUE;
  datapool->terminated = FALSE;
    
  return datapool;
}


void* TSP_datapool_get_symbol_value(TSP_datapool_t datapool, int provider_global_index, xdr_and_sync_type_t type)
{
  void* p = 0;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;
	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(*obj_datapool, provider_global_index, 0);

  /* Someone will ask for this symbols */
  if (obj_datapool->items[provider_global_index].is_wanted == FALSE)
    {
      obj_datapool->items[provider_global_index].is_wanted = TRUE; 
      obj_datapool->reverse_index[obj_datapool->nb_wanted_items++] = provider_global_index;
    }

  p = &(obj_datapool->items[provider_global_index].user_value);

  /* FIXME : manages different types RAW, DOUBLE, STRING...,
     see how to implement this when the data will be RAW in the datapool */
  /*switch(type & XDR_DATA_TYPE_MASK)
    {
    case XDR_DATA_TYPE_USER | TSP_DATA_TYPE_SYNC :
    p = &(X_global_datapool.items[provider_global_index].user_value);
    break;
    default :
    STRACE_ERROR(("Unknown XDR type : %X", type));
    }
  */
    
  return p;
}


TSP_datapool_t TSP_global_datapool_get_instance(void)
{
  /* Act like a singleton. The first consumer that
 calls us, init us */

  /* Was the datapool terminated ? */
  if( TRUE == X_global_datapool.terminated )
    {
      /* Wait for thread to end */
      /* pthread_join(X_global_datapool.worker_id, NULL);*/
      /* Yes, clean up previous datapool */
      TSP_datapool_internal_free(&X_global_datapool);
      /* Now, the datapool is not initialized anymore */
      X_global_datapool.initialized = FALSE;
    }

  /* Was the datapool initialized  ? */
  if(FALSE == X_global_datapool.initialized)
    {
      /* Now initialize */
      TSP_global_datapool_init();
    }

  return &X_global_datapool;
}

