/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.26 2006-04-23 15:50:42 erk Exp $

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
  void* raw_value;
  int   is_wanted;	/* Is the symbol wanted by some consummers */
};

typedef struct TSP_datapool_item_t TSP_datapool_item_t;

struct TSP_datapool_table_t
{

  /** Is the datapool initialized ? */
  int initialized;

  /** Tells if the datapool was used and destroyed */
  int terminated;
  
  /** handle on GLU */
  GLU_handle_t* h_glu;

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
  memcpy(X_global_datapool.items[item->provider_global_index].raw_value,item->raw_value,item->size);
  /* X_global_datapool.items[item->provider_global_index].user_value = item->value; */
  return 0;
}

/**
 * End of push, we commit the whole
 * @param time_stamp date of datapool items
 * @param state ok or error (reconf,eof, ...)
 */ 
int TSP_datapool_push_commit(time_stamp_t time_stamp, GLU_get_state_t state)
{
  /*
    that's too much of trace here
    STRACE_DEBUG(("Datapool push new item time %d",time_stamp)); 
  */

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

/**
 * Start global datapool thread.
 * Only used when the sample server is an active one
 * @return TRUE = OK
 */ 
static int TSP_global_datapool_init(GLU_handle_t* glu)
{
	 
  TSP_sample_symbol_info_list_t symbols;
  int32_t i;
	
  /* Here the datapool is global */
  X_global_datapool.h_glu = glu;   

  glu->get_ssi_list(X_global_datapool.h_glu, &symbols);

  X_global_datapool.size = symbols.TSP_sample_symbol_info_list_t_len;

  X_global_datapool.items = 
    (TSP_datapool_item_t*)calloc(X_global_datapool.size, sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(X_global_datapool.items, FALSE);


  for(i=0;i<symbols.TSP_sample_symbol_info_list_t_len;++i)
  {

    X_global_datapool.items[i].raw_value = 
                       calloc(symbols.TSP_sample_symbol_info_list_t_val[i].dimension,sizeof(symbols.TSP_sample_symbol_info_list_t_val[i].type));
    TSP_CHECK_ALLOC(X_global_datapool.items[i].raw_value, FALSE); 


  }



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


TSP_datapool_t 
TSP_local_datapool_allocate(int symbols_number, GLU_handle_t* h_glu ) {
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


/*  p = &(obj_datapool->items[provider_global_index].raw_value);*/

    p = obj_datapool->items[provider_global_index].raw_value;

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


TSP_datapool_t TSP_global_datapool_get_instance(GLU_handle_t* glu)
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
      TSP_global_datapool_init(glu);
    }

  return &X_global_datapool;
}

