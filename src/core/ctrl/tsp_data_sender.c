/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_data_sender.c,v 1.3 2002-10-01 15:18:05 galles Exp $

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

TSP_data_sender_t TSP_data_sender_create(void)
{
  SFUNC_NAME(TSP_data_sender_create);

    
  TSP_struct_data_sender_t* sender;
    
  STRACE_IO(("-->IN"));

  sender = (TSP_struct_data_sender_t*)calloc(1, sizeof(TSP_struct_data_sender_t));
  TSP_CHECK_ALLOC(sender, 0);

  /* FIXME : a desallouer */
  sender->buf = (char*)calloc(TSP_DATA_STREAM_CREATE_BUFFER_SIZE, sizeof(char) );
  TSP_CHECK_ALLOC(sender->buf, 0);

  /* Create the sender stream */
  sender->stream_sender = (TSP_data_sender_t)TSP_stream_sender_create();
    
  STRACE_IO(("-->OUT"));

    
  return sender;

}

/**
 * Send groupe EOF.
 * @param sender sender used to send the data
 * The special group ID groupe EOF indicate the End of Stream for the consumer
 * @return TRUE = OK
 */
int TSP_data_sender_send_eof(TSP_data_sender_t sender)
{
  SFUNC_NAME(TSP_data_sender_send);

  TSP_struct_data_sender_t* data_sender = (TSP_struct_data_sender_t*)sender;
  int* buf_int = (int*)(data_sender->buf);
  int ret = TRUE;
  
  STRACE_IO(("-->IN"));
  
  /*Encode dummy time stamp */
  buf_int[0] = TSP_ENCODE_INT(-1);
  buf_int[1] = TSP_ENCODE_INT(TSP_RESERVED_GROUPE_EOF);
  
  if(!TSP_stream_sender_send(data_sender->stream_sender,
			     data_sender->buf,
			     sizeof(int)*2))
    {
      STRACE_ERROR(("Function TSP_stream_sender_send failed "));
      ret = FALSE;
      
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

  int* buf_int;

  char* buf_char;
    
  TSP_algo_group_t* group;
  int group_index;
  int i;
  int ret = TRUE;
  int size;


  STRACE_IO(("-->IN"));
  group_index = time_stamp % groups_table->table_len;
  group = &(groups_table->groups[group_index]);
    

  buf_int = (int*)(data_sender->buf);
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
						TSP_DATA_STREAM_CREATE_BUFFER_SIZE - ( buf_char - data_sender->buf) );
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


      if( ret && (FALSE == TSP_stream_sender_send(data_sender->stream_sender,
						  data_sender->buf,
						  buf_char - data_sender->buf)))
        {
	  STRACE_ERROR(("Function TSP_stream_sender_send failed "));
	  ret = FALSE;

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
