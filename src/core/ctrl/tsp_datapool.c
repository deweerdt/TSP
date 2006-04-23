/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_datapool.c,v 1.27 2006-04-23 20:06:48 erk Exp $

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

#include <tsp_sys_headers.h>
#include <tsp_datapool.h>
#include <tsp_datastruct.h>
#include <tsp_session.h>
#include <tsp_time.h>

/* Poll time waiting for  consumer connection (us) */
#define TSP_LOCAL_WORKER_CONNECTION_POLL_TIME ((int)(1e5))

/* Data Poll period (us) */
#define TSP_DATAPOOL_POLL_PERIOD ((int)(1e4))


/*-----------------------------------------------------*/



/*-----------------------------------------------------*/

TSP_datapool_t X_global_datapool = {FALSE,FALSE,0,0,0};

/*-----------------------------------------------------*/

#define TSP_CHECK_PROVIDER_GLOBAL_INDEX(obj, index, ret) \
{ \
	if( (index) >= (obj).size ) \
	{ \
		STRACE_ERROR(("ERROR :-->OUT : provider_global_index %d does not exist !",(index))) \
		return ret; \
	} \
}


void 
TSP_datapool_get_reverse_list (TSP_datapool_t* datapool, int *nb, int **list) {
  assert(nb!=NULL);
  assert(list!=NULL);
  assert(datapool);
  *nb = datapool->nb_wanted_items;
  *list = datapool->reverse_index;
}


inline int 
TSP_datapool_push_next_item(TSP_datapool_t* datapool, glu_item_t* item) {
  assert(datapool);
  memcpy((*datapool).items[item->provider_global_index].raw_value,
	 item->raw_value,
	 item->size);
  return 0;
}

int 
TSP_datapool_push_commit(TSP_datapool_t* datapool, time_stamp_t time_stamp, GLU_get_state_t state) {

  /* Send end status message */
  switch(state) {
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
  X_global_datapool.initialized = TRUE;
  X_global_datapool.terminated = FALSE;
    
  return TRUE;
}

static void 
TSP_datapool_internal_free(TSP_datapool_t* obj_datapool) {
     free(obj_datapool->items);
     obj_datapool->items = 0;
     free(obj_datapool->reverse_index);
     obj_datapool->reverse_index = 0;
     obj_datapool->nb_wanted_items = 0;
}

void 
TSP_datapool_delete(TSP_datapool_t** datapool) {

   TSP_datapool_internal_free(*datapool);
   free(*datapool);
   *datapool=NULL;
} /* End of TSP_datapool_delete */


TSP_datapool_t* 
TSP_datapool_new(int symbols_number, GLU_handle_t* h_glu ) {
  TSP_datapool_t* datapool;
     
  datapool = (TSP_datapool_t*)calloc(1,sizeof(TSP_datapool_t));
  TSP_CHECK_ALLOC(datapool, 0);
	
  datapool->size = symbols_number;
  datapool->items = (TSP_datapool_item_t*)calloc(datapool->size,sizeof(TSP_datapool_item_t));
  TSP_CHECK_ALLOC(datapool->items, 0);
    
  datapool->h_glu = h_glu;
  datapool->initialized = TRUE;
  datapool->terminated = FALSE;
    
  return datapool;
} /* end of TSP_datapool_new */


void* 
TSP_datapool_get_symbol_value(TSP_datapool_t* datapool, int provider_global_index) {
	
  TSP_CHECK_PROVIDER_GLOBAL_INDEX(*datapool, provider_global_index, 0);

  /* Someone will ask for this symbols */
  if (datapool->items[provider_global_index].is_wanted == FALSE) {
    datapool->items[provider_global_index].is_wanted = TRUE; 
    datapool->reverse_index[datapool->nb_wanted_items++] = provider_global_index;
  }
  
  /* 
   * History; we add planned to 
   * handle raw --> user calibration
   * here this is NOT done for now
   * FIXME: we should re-think about this.
   */
  return datapool->items[provider_global_index].raw_value;
} /* End of TSP_datapool_get_symbol_value */


TSP_datapool_t* 
TSP_global_datapool_instantiate(GLU_handle_t* glu) {
  /* Act like a singleton. */ 
  
  /* Was the datapool terminated ? */
  if( TRUE == X_global_datapool.terminated ) {
    /* Yes, clean up previous datapool */
    TSP_datapool_internal_free(&X_global_datapool);
    /* Now, the datapool is not initialized anymore */
    X_global_datapool.initialized = FALSE;
  }

  /* Was the datapool initialized  ? */
  if(FALSE == X_global_datapool.initialized) {
    /* Now initialize */
    TSP_global_datapool_init(glu);
  }
  
  /* link to provided GLU */
  glu->datapool = &X_global_datapool;

  return &X_global_datapool;
} /* end of TSP_global_datapool_get_instance */

