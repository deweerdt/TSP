/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.c,v 1.5 2002-11-19 13:11:45 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   :  Implementation for the functions used to encode and send the data
stream  for the asked symbols

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <rpc/types.h>
#include <rpc/xdr.h>

 
#include "tsp_data_sender.h"

#include "tsp_datastruct.h"

#include "tsp_group_algo.h"
#include "tsp_group_algo_data.h"

#include "tsp_stream_sender.h"

#define TSP_XDR_BUF_SIZE 8192

struct TSP_struct_data_sender_t
{
  /** The stream sender object used to send data thrue the network*/
  TSP_stream_sender_t stream_sender;
  
  /** Buffer used to create the data stream */
  char* buf                                    ;

  /** is there a fifo in the strea sender ? */
  int use_stream_sender_fifo;

  /** the stream sender fifo */
  TSP_stream_sender_ringbuf_t* stream_sender_fifo;

  
};

typedef struct TSP_struct_data_sender_t TSP_struct_data_sender_t;

static u_int TSP_data_sender_double_encoder(void* v_double,  char* out_buf, u_int size)
{

#ifndef TSP_NO_XDR_ENCODE

  SFUNC_NAME(TSP_data_sender_double_encoder with XDR);

  XDR xhandle;
   
  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);
  if( xdr_double(&xhandle, (double*)v_double) != TRUE)
    {
      STRACE_ERROR(("Function xdr_double failed"));
      return 0;
    }
  else
    {
      return xdr_getpos(&xhandle);
    }

#else

  SFUNC_NAME(TSP_data_sender_double_encoder withOUT XDR);

  
  if(size < sizeof(double) )
    {
      STRACE_ERROR(("buffer is too small"));
      return 0;
    }
  else
    {
      *(guint64*)out_buf = TSP_ENCODE_DOUBLE_TO_GUINT64(v_double);
      return (u_int)sizeof(double);
    }

#endif /*TSP_NO_XDR_ENCODE*/

}

TSP_data_sender_t TSP_data_sender_create(int fifo_size)
{
  SFUNC_NAME(TSP_data_sender_create);

    
  TSP_struct_data_sender_t* sender;
    
  STRACE_IO(("-->IN"));

  sender = (TSP_struct_data_sender_t*)calloc(1, sizeof(TSP_struct_data_sender_t));
  TSP_CHECK_ALLOC(sender, 0);

  /* FIXME : a desallouer */
  

  /* Create the sender stream */
  sender->stream_sender = (TSP_data_sender_t)TSP_stream_sender_create(fifo_size);

  if(sender->stream_sender)
    {
      sender->use_stream_sender_fifo = fifo_size > 0 ? TRUE : FALSE ;
      if(sender->use_stream_sender_fifo)
	{
	  sender->buf = 0;
	  sender->stream_sender_fifo = TSP_stream_sender_get_ringbuf(sender->stream_sender);
	}
      else
	{
	  sender->buf = (char*)calloc(TSP_DATA_STREAM_CREATE_BUFFER_SIZE, sizeof(char) );
	  TSP_CHECK_ALLOC(sender->buf, 0);
	  sender->stream_sender_fifo = 0;
	}
    }
  else
    {
      STRACE_ERROR(("Function TSP_stream_sender_create failed"));
      free(sender);
      sender = 0;
    }
  

    
  STRACE_IO(("-->OUT"));

    
  return sender;

}

/**
 * Send message control.
 * @param sender sender used to send the data
 * The special group ID groupe EOF indicate the End of Stream for the consumer
 * @param msg_ctrl The message that must be sent
 * @return TRUE = OK
 */
int TSP_data_sender_send_msg_ctrl(TSP_data_sender_t sender, TSP_msg_ctrl_t msg_ctrl)
{
  SFUNC_NAME(TSP_data_sender_send);

  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  TSP_stream_sender_item_t* fifo_item = 0;
  char* buf_main = 0;
  int* buf_int = 0;
  int tsp_reserved_group;
  int ret = TRUE;
  
  STRACE_IO(("-->IN"));

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
    default:
      STRACE_ERROR(("We should not be there..."));
      assert(0);
      return FALSE;
    }


  /*-----------------------*/
  /* check if we must use our own direct buffer and send the data, or
     put it in a ringbuffer */
  if(data_sender->use_stream_sender_fifo)
    {
      /* use ringbuffer */
      fifo_item = RINGBUF_PTR_PUTBYADDR(data_sender->stream_sender_fifo);
      if(fifo_item)
	{
	  buf_main = fifo_item->buf;
	  fifo_item->len = 0;
	}
      else
	{	  
	  /* FIXME : gerer l'empilement d'un message d'erreur des qu'il y a de la place dans le ringbuf */
	  ret = FALSE;
	  STRACE_WARNING(("Stream sender ringbuf full"));
	}
    }
  else  /* not ringbuffer, use direct buffer */
    {
      buf_main = (char*)(data_sender->buf);
    }
  
    
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
  /* FIXME : gere l'empilement du eof dans le ringbuf */
  /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/

 
  /*------------------------*/
  /*Encode data */

  if(ret)
    {
      buf_int = (int*)(buf_main);
      buf_int[0] = TSP_ENCODE_INT(-1);
      buf_int[1] = TSP_ENCODE_INT(tsp_reserved_group);
  
      /*------------------------ */
      /* use ringbuffer or not ? */
      if(! (data_sender->use_stream_sender_fifo) )
	{
	  /* not ringbuffer. send data now */
	  if( ret && (FALSE == TSP_stream_sender_send(data_sender->stream_sender,
						      buf_main,
						      sizeof(int)*2)))
	    {
	      STRACE_ERROR(("Function TSP_stream_sender_send failed "));
	      ret = FALSE;
	    }
	}
      else
	{
	  /* use ringbuffer */
	  fifo_item->len = sizeof(int)*2;
	  RINGBUF_PTR_PUTBYADDR_COMMIT(data_sender->stream_sender_fifo);
	}
    }
  
  STRACE_IO(("-->OUT"));
  
  return ret;
 
}

/**
 * For a given time stamp, send data to a client.
 * @param _sender sender used to send the data
 * @param _groups groups used to calculate the data
 * @param t time stamp sent with the data
 * @return TRUE = OK
 */
int TSP_data_sender_send(TSP_data_sender_t _sender, TSP_groups_t _groups, time_stamp_t time_stamp) 
{
    
  SFUNC_NAME(TSP_data_sender_send);


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
  TSP_stream_sender_item_t* fifo_item = 0;


  STRACE_IO(("-->IN"));
  group_index = time_stamp % groups_table->table_len;
  group = &(groups_table->groups[group_index]);
    
  /* check if we must use our own direct buffer and send the data, or
     put it in a ringbuffer */
  if(data_sender->use_stream_sender_fifo)
    {
      /* use ringbuffer */
      fifo_item = RINGBUF_PTR_PUTBYADDR(data_sender->stream_sender_fifo);
      if(fifo_item)
	{
	  buf_main = fifo_item->buf;
	  fifo_item->len = 0;
	}
      else
	{	  
	  /* FIXME : gerer l'empilement d'un message d'erreur des qu'il y a de la place dans le ringbuf */
	  ret = FALSE;
	  STRACE_WARNING(("Stream sender ringbuf full"));
	}
    }
  else  /* not ringbuffer, use direct buffer */
    {
      buf_main = (char*)(data_sender->buf);
    }
  
  if(ret)
    {
      buf_int = (int*)(buf_main);
      *( buf_int++ ) = TSP_ENCODE_INT(time_stamp);
      *( buf_int++ ) = TSP_ENCODE_INT(group_index);
      buf_char = (char*)(buf_int);

  
      if( group->group_len > 0)
	{
	  for( i = 0 ; i < group->group_len ; i++)
	    {
	      /* FIXME : gerer tous les types */
	      /* avec la fonction d'encodage */
	      STRACE_DEBUG(("Gr=%d V=%f", group_index, *(double*)(group->items[i].data)));

	      /* Call encode function */
	      assert(group->items[i].data_encoder);
	      size = (group->items[i].data_encoder)(group->items[i].data,
						    buf_char,
						    TSP_DATA_STREAM_CREATE_BUFFER_SIZE - ( buf_char - buf_main) );
	      if ( 0 == size )
		{
		  STRACE_ERROR(("data_encoder failed"));	    
		  ret = FALSE;
		  break;
		}
	  
	      buf_char += size;
	      /**(double*)buf_char = *(double*)(group->items[i].data);
		 buf_char += sizeof(double);*/

	    } /*for*/


	  /* use ringbuffer or not ? */
	  if(! (data_sender->use_stream_sender_fifo) )
	    {
	      /* not ringbuffer. send data now */
	      if( ret && (FALSE == TSP_stream_sender_send(data_sender->stream_sender,
							  buf_main,
							  buf_char - buf_main)))
		{
		  STRACE_ERROR(("Function TSP_stream_sender_send failed "));
		  ret = FALSE;
		}
	    }
	  else
	    {
	      /* use ringbuffer */
	      fifo_item->len = buf_char - buf_main;
	      RINGBUF_PTR_PUTBYADDR_COMMIT(data_sender->stream_sender_fifo);
	    }


	}
    }
    
  STRACE_IO(("-->OUT"));

  return ret;
        
}

const char* TSP_data_sender_get_data_address_string(TSP_data_sender_t sender)
{
  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  return TSP_stream_sender_get_data_address_string(data_sender->stream_sender);
}

TSP_data_encoder_t TSP_data_sender_get_double_encoder(void)
{
  return TSP_data_sender_double_encoder;
}



int TSP_data_sender_is_consumer_connected(TSP_data_sender_t sender)
{
   TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;

   return TSP_stream_sender_is_client_connected(data_sender->stream_sender);
}
