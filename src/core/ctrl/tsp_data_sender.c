/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.c,v 1.30 2007-11-30 15:42:00 erk Exp $

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

Purpose   :  Implementation for the functions used to encode and send the data
stream  for the asked symbols

-----------------------------------------------------------------------
 */

#ifdef _WIN32
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif

#include <tsp_sys_headers.h>
#include <tsp_abs_types.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <tsp_data_sender.h>
#include <tsp_datastruct.h>
#include <tsp_group_algo.h>
#include <tsp_group_algo_data.h>
#include <tsp_stream_sender.h>
#include <tsp_const_def.h>

#define TSP_XDR_BUF_SIZE 8192

struct TSP_struct_data_sender_t {
  /** The stream sender object used to send data thrue the network*/
  TSP_stream_sender_t stream_sender;  

  /** do we use the fifo or a simple buffer ? */
  int use_fifo;

  /** FIFO used to send the data stream when there is no buffer (threaded send)*/
  TSP_stream_sender_ringbuf_t* out_fifo;

  /** Buffer used to create the data stream when there is no fifo*/
  TSP_stream_sender_item_t* out_item;

  /** Max buffer size available to send data */
  int buffer_size;

  /** flag that tells if data were lost for the consumer 'coz' the fifo was full */
  int fifo_full;
  
};

typedef struct TSP_struct_data_sender_t TSP_struct_data_sender_t;


TSP_data_sender_t 
TSP_data_sender_create(int fifo_size, uint32_t group_max_byte_size) {
  
  TSP_struct_data_sender_t* sender;
  
  sender = (TSP_struct_data_sender_t*)calloc(1, sizeof(TSP_struct_data_sender_t));
  TSP_CHECK_ALLOC(sender, NULL);

  /* init */
  sender->fifo_full   = FALSE;
  sender->buffer_size = TSP_DATA_STREAM_MAX_BUFFER_SIZE(group_max_byte_size);

  STRACE_DEBUG(("Data Sender buffer size is: <%d> byte(s)",sender->buffer_size));
  /* Create the sender stream with its fifo size*/
  sender->stream_sender = (TSP_data_sender_t)TSP_stream_sender_create(fifo_size, sender->buffer_size);
  if (sender->stream_sender) {      
    /* Check if the user wants any fifo */
    sender->use_fifo = fifo_size > 0 ? TRUE : FALSE ;
    if (sender->use_fifo) {
      sender->out_item = 0;
      sender->out_fifo = TSP_stream_sender_get_ringbuf(sender->stream_sender);
      assert(sender->out_fifo);
    }
    else {
      sender->out_fifo = 0;
      sender->out_item = TSP_stream_sender_get_buffer(sender->stream_sender);
      assert(sender->out_item);      
    }
  }
  else {
    STRACE_ERROR(("Function TSP_stream_sender_create failed"));
    free(sender);
    sender = NULL;
  }
  return sender;
} /* end of TSP_data_sender_create */

void 
TSP_data_sender_destroy(TSP_data_sender_t sender) {

  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  TSP_stream_sender_destroy(data_sender->stream_sender);
  free(data_sender);
}

void 
TSP_data_sender_stop(TSP_data_sender_t sender) {

  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  TSP_stream_sender_stop(data_sender->stream_sender);
}


static TSP_stream_sender_item_t* TSP_data_sender_get_out_item(TSP_struct_data_sender_t* data_sender)
{

  TSP_stream_sender_item_t* ret_item = 0;
  if(data_sender->use_fifo)
    {
      /* Check if fifo was full last time */
      if(!(data_sender->fifo_full))
	{
	  /* Was not full. We try to take an element */
	  ret_item = RINGBUF_PTR_PUTBYADDR(data_sender->out_fifo);
	  if(!ret_item)
	    {
	      /* Arg! It is is full now */
	      data_sender->fifo_full = TRUE;
	    }
	}
      else
	{
	  /* FIFO was full. We must wait for two available items :
	     one for the error message, one for the next item */
	  if( RINGBUF_PTR_ITEMS_LEFT(data_sender->out_fifo) > 2 )
	    {

	      /* not full anymore */
	      data_sender->fifo_full = FALSE; 

	      /* First, add the error message :  we do some recursive magic here ;),
		 there must not be any infinite loop...*/
	      TSP_data_sender_send_msg_ctrl(data_sender,  TSP_MSG_CTRL_CONSUMER_DATA_LOST);

	      /* And now there must be one item left */
	      ret_item = RINGBUF_PTR_PUTBYADDR(data_sender->out_fifo);
	      assert(ret_item);
	    }
	  else
	    {
	      data_sender->fifo_full = TRUE;	  
	    }
	  
	}
    }
  else  /* not ringbuffer, use direct buffer */
    {
      ret_item = data_sender->out_item;
    }

  return ret_item;
}

static int 
TSP_data_sender_to_stream_sender(TSP_struct_data_sender_t* data_sender,
				 TSP_stream_sender_item_t* tosend) {

  /* First check for a valid connection */
  int ret = TSP_stream_sender_is_connection_ok(data_sender->stream_sender);
  if(ret) {
    if (data_sender->use_fifo) {
      /* we use a fifo */
      RINGBUF_PTR_PUTBYADDR_COMMIT(data_sender->out_fifo);
    }
    else {            
      /* 
       * no fifo. send data now (SEND should block if socket buffer is full) 
       * this should be the case for PASSIVE GLU.
       */
      if(!TSP_stream_sender_send(data_sender->stream_sender,
				 TSP_STREAM_SENDER_ITEM_BUF(tosend),
				 tosend->len) ) {
	  STRACE_WARNING(("Function TSP_stream_sender_send failed "));
	  ret = FALSE;
	}  
    }
  }
  return ret;
} /* end of TSP_data_sender_to_stream_sender */


/**
 * Send message control.
 * @param sender sender used to send the data
 * The special group ID groupe EOF indicate the End of Stream for the consumer
 * @param msg_ctrl The message that must be sent
 * @return TRUE = OK
 */
int TSP_data_sender_send_msg_ctrl(TSP_data_sender_t sender, TSP_msg_ctrl_t msg_ctrl)
{
  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  int* buf_int = 0;
  int tsp_reserved_group;
  int ret = TRUE;
  TSP_stream_sender_item_t* tosend;

  /*---------------------------*/
  /* Traduce enum */
  switch(msg_ctrl)
    {
    case TSP_MSG_CTRL_EOF :
      tsp_reserved_group = TSP_RESERVED_GROUP_EOF;
      break;
    case TSP_MSG_CTRL_RECONF :
      tsp_reserved_group = TSP_RESERVED_GROUP_RECONF;
      break;
    case TSP_MSG_CTRL_GLU_DATA_LOST :
      tsp_reserved_group = TSP_RESERVED_GROUP_GLU_DATA_LOST;
      break;
    case TSP_MSG_CTRL_CONSUMER_DATA_LOST :
      tsp_reserved_group = TSP_RESERVED_GROUP_CONSUMER_DATA_LOST;
      break;
    default:
      STRACE_ERROR(("We should not be there..."));
      assert(0);
      return FALSE;
    }

  /* get out buffer if it is available */
  tosend = TSP_data_sender_get_out_item(data_sender);
    
  /* If it is not available, try net time */

  /* FIXME : uses packet format as described in the protocol spec */
  if(tosend)
    {
      buf_int = (int*)(TSP_STREAM_SENDER_ITEM_BUF(tosend));
      buf_int[0] = TSP_ENCODE_INT(-1); /* Dummy time stamp */
      buf_int[1] = TSP_ENCODE_INT(tsp_reserved_group);
  
      tosend->len = sizeof(int)*2;

      /* We send it */
      ret = TSP_data_sender_to_stream_sender(data_sender, tosend);
    }
  
  return ret; 
}

/*
 * For a given time stamp, send data to a client.
 * @param _sender sender used to send the data
 * @param _groups groups used to calculate the data
 * @param time_stamp sent with the data
 * @return TRUE = OK
 */
int 
TSP_data_sender_send(TSP_data_sender_t _sender, TSP_groups_t _groups, time_stamp_t time_stamp) {    

  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)_sender;
  TSP_algo_table_t* groups_table = (TSP_algo_table_t*) _groups;

  char* buf_main;
  int* buf_int;
  char* buf_char;    
  TSP_algo_group_t* group;
  int group_index;
  int i;
  int ret = TRUE;
  int size;
  TSP_stream_sender_item_t* tosend;

  /* 
   * Time stamp is the TSP time stamp 
   * cycle counter
   * so that the TSP group index 
   * whose data is to be sent at this "times_tamp"  
   * only depends on the current time stamp.
   */
  group_index = time_stamp % groups_table->table_len;
  group       = &(groups_table->groups[group_index]);

  /* get out buffer if it is available */
  tosend = TSP_data_sender_get_out_item(data_sender);
    
  /* 
   * If it is not available, try next time 
   * FIXME erk: why? could we just skip a cycle?
   */

  /* FIXME : uses packet format as described in the protocol spec */
  if (tosend) {
      buf_main = TSP_STREAM_SENDER_ITEM_BUF(tosend);
      buf_int = (int*)(buf_main);
      *( buf_int++ ) = TSP_ENCODE_INT(time_stamp);
      *( buf_int++ ) = TSP_ENCODE_INT(group_index);
      buf_char = (char*)(buf_int);
  
      if( group->group_len > 0) {
	for ( i = 0 ; i < group->group_len ; i++) {
	  /* 
	   * FIXME : we cannot STRACE_DEBUG value simply due
	   * to multi-type handling 
	   */
	  /* STRACE_DEBUG(("Gr=%d V=%f", group_index, *(double*)(group->items[i].data))); */
	  STRACE_DEBUG_MORE(("Gr=%d Ne=%d", group_index, group->items[i].nelem));
	  
	  /* Call encode function */
	  assert(group->items[i].data_encoder);
	  size = (group->items[i].data_encoder)(group->items[i].data,
						group->items[i].nelem,
						buf_char,
						data_sender->buffer_size - ( buf_char - buf_main) );
	  if ( 0 == size ) {
	    STRACE_ERROR(("data_encoder failed"));	    
	    ret = FALSE;
	    break;
	  }
	  
	  buf_char += size;
	  
	} /*for*/
	
	  /* We send it */
	tosend->len = buf_char - buf_main;
	ret = TSP_data_sender_to_stream_sender(data_sender, tosend);
      }
  }
  
  return ret;        
} /* TSP_data_sender_send */

const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender)
{
  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  return TSP_stream_sender_get_data_address_string(data_sender->stream_sender);
}



int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender)
{
   TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;

   return TSP_stream_sender_is_client_connected(data_sender->stream_sender);
}
