/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.17 2004-07-28 13:05:37 mia Exp $

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

/* Poll time waiting for  consumer connection (µs) */
#define TSP_LOCAL_WORKER_CONNECTION_POLL_TIME ((int)(1e5))

/* Data Poll period (µs) */
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
  
  pthread_t worker_id;

  /** Only for local thread id.
   * The id of the session linked to the local datapool
   */
  channel_id_t session_channel_id;


  /** is the datapool local or global ? */
  int is_global;

};

typedef struct TSP_datapool_table_t TSP_datapool_table_t;



/*-----------------------------------------------------*/

TSP_datapool_table_t X_global_datapool = {FALSE,FALSE,0,0,0,0};

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
 * Thread created per session when the sample server is a pasive one.
 * @param datapool The datapool object instance that will be linked to the thread
 */ 
void* TSP_datapool_thread(void* datapool)
{

  SFUNC_NAME(TSP_local_worker);

  time_stamp_t time_stamp;
  glu_item_t item;
  int more_items;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;  
  GLU_get_state_t state;
  TSP_msg_ctrl_t msg_ctrl;  
  int data_link_ok = TRUE; /**< Used to store the  data link state for a local thread (broken or ok) */

  STRACE_IO(("-->IN"));
  
  STRACE_DEBUG(("Datapool thread started for session %d",obj_datapool->session_channel_id)); 
  

  if( obj_datapool->is_global )
    {
      /* FIXME : The datapool might not be coherent when a client is already
	 connected and this thread starts after the connection
	 for the client */
      STRACE_DEBUG(("Data Pool is GLOBAL (active GLU)"));
    }
  else
    {
      /* Wait for consumer connection before we send data */    
      while(!TSP_session_is_consumer_connected_by_channel(obj_datapool->session_channel_id))
	{
	  tsp_usleep(TSP_LOCAL_WORKER_CONNECTION_POLL_TIME);
	}
      STRACE_INFO(("Consumer connected for session id %d",obj_datapool->session_channel_id)); 

    }

  /* Flush old data, we do not want to get a GLU_GET_DATA_LOST now */
  GLU_forget_data(obj_datapool->h_glu);

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
	      if( obj_datapool->is_global )
		{
		  assert(obj_datapool->h_glu == GLU_GLOBAL_HANDLE );
		  /* Send data to all clients  */	      		  
		  /* For a global datapool, the thread must not end, even if a client is disconnected, so
		     we do not check any returned value from TSP_session_all_session_send_data */
		  TSP_session_all_session_send_data(time_stamp);		  
		}
	      else
		{
		  /* send data to one client, and check client state */
		  data_link_ok = TSP_session_send_data_by_channel(obj_datapool->session_channel_id, time_stamp);	  
		}
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
      if( obj_datapool->is_global )
	{
	  /* Send data to all clients  */	      
	  assert(obj_datapool->h_glu == GLU_GLOBAL_HANDLE );
	  TSP_session_all_session_send_data(time_stamp);		  
	  TSP_session_all_session_send_msg_ctrl(msg_ctrl);		  
	}
      else
	{
	  /* send data to one client */
	  TSP_session_send_data_by_channel(obj_datapool->session_channel_id, time_stamp);	  
	  TSP_session_send_msg_ctrl_by_channel(obj_datapool->session_channel_id, msg_ctrl);
	}
    }

  /* End of thread. Our datapool is dead */
  obj_datapool->terminated = TRUE;

  STRACE_IO(("-->OUT"));

}


/**
 * Wait for the local datapool thread end per session.
 * Only used when the sample server is a pasive one
 * @param datapool The datapool instance linked to the thread
 */ 
int TSP_local_datapool_wait_for_end_thread(TSP_datapool_t datapool)
{
  SFUNC_NAME(TSP_local_datapool_wait_for_end_thread);
  int status;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

  STRACE_IO(("-->IN"));

  status = pthread_join(obj_datapool->worker_id, NULL);
  TSP_CHECK_THREAD(status, FALSE);

  STRACE_IO(("-->OUT"));
  return TRUE;

}


/**
 * Start local datapool thread be per session.
 * Only used when the sample server is a passive one
 * @param datapool The datapool instance that will be linked to the thread
 */ 
int TSP_local_datapool_start_thread(TSP_datapool_t datapool)
{
  SFUNC_NAME(TSP_local_datapool_start_thread);
  int status;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

  STRACE_IO(("-->IN"));

  status = pthread_create(&(obj_datapool->worker_id), NULL, TSP_datapool_thread,  datapool);
  TSP_CHECK_THREAD(status, FALSE);

  STRACE_IO(("-->OUT"));
  return TRUE;

}

/**
 * Start global datapool thread.
 * Only used when the sample server is an active one
 * @return TRUE = OK
 */ 
static int TSP_global_datapool_init(void)
{
	 
  SFUNC_NAME(TSP_global_datapool_init);

	
  TSP_sample_symbol_info_list_t symbols;
  int i;
  int status;
 
     
  STRACE_IO(("-->IN"));
	
  /* Here the datapool is global */

  /* FIXME : remettre l'assertion */
  /*assert(0 == X_global_datapool.h_glu);*/


  X_global_datapool.h_glu = GLU_GLOBAL_HANDLE;   

  GLU_get_sample_symbol_info_list(X_global_datapool.h_glu, &symbols);

  X_global_datapool.size = symbols.TSP_sample_symbol_info_list_t_len;

  X_global_datapool.items = 
    (TSP_datapool_item_t*)calloc(X_global_datapool.size,
				 sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(X_global_datapool.items, FALSE);

  /* Actually all session are link to a global datapool */
  X_global_datapool.session_channel_id = -1;
  X_global_datapool.is_global = TRUE;

  status = pthread_create(&(X_global_datapool.worker_id), NULL, TSP_datapool_thread,  &X_global_datapool);
  TSP_CHECK_THREAD(status, FALSE);

  X_global_datapool.initialized = TRUE;
  X_global_datapool.terminated = FALSE;
    
  STRACE_IO(("-->OUT"));
  return TRUE;

}

static void TSP_datapool_internal_free(TSP_datapool_table_t* obj_datapool)
{
     free(obj_datapool->items);obj_datapool->items = 0;
}

void TSP_local_datapool_destroy(TSP_datapool_t datapool)
{
   TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;

   TSP_datapool_internal_free(obj_datapool);
   free(obj_datapool);
   
}


TSP_datapool_t TSP_local_datapool_allocate(channel_id_t session_channel_id, int symbols_number, GLU_handle_t h_glu )
{
	 
  SFUNC_NAME(TSP_local_datapool_allocate);

	
  TSP_sample_symbol_info_list_t symbols;
  TSP_datapool_table_t* datapool;
     
  STRACE_IO(("-->IN"));

  datapool = (TSP_datapool_table_t*)calloc(1,sizeof(TSP_datapool_table_t));
  TSP_CHECK_ALLOC(datapool, 0);
	
  datapool->size = symbols_number;

  datapool->items = (TSP_datapool_item_t*)calloc(datapool->size,sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(datapool->items, 0);
    
  /* set session linked to this datapool*/
  datapool->session_channel_id = session_channel_id;
  datapool->is_global = FALSE;

  datapool->h_glu = h_glu;
  datapool->initialized = TRUE;
  datapool->terminated = FALSE;
    
  STRACE_IO(("-->OUT"));
  return datapool;

}


void* TSP_datapool_get_symbol_value(TSP_datapool_t datapool, int provider_global_index, xdr_and_sync_type_t type)
{
  SFUNC_NAME(TSP_datapool_get_symbol_value);

	
  void* p = 0;
  TSP_datapool_table_t* obj_datapool = (TSP_datapool_table_t*)datapool;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(*obj_datapool, provider_global_index, 0);


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
    
  STRACE_IO(("-->OUT"));

	
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
      pthread_join(X_global_datapool.worker_id, NULL);
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

  assert( GLU_GLOBAL_HANDLE == X_global_datapool.h_glu);
  return &X_global_datapool;
}
