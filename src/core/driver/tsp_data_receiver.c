/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.c,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implementation for the functions used to receive and decode the data
stream  for the asked symbols
-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <rpc/types.h>
#include <rpc/xdr.h>

#include "tsp_data_receiver.h"

#include "tsp_datastruct.h"

#include "tsp_group.h"
#include "tsp_group_data.h"

#include "tsp_stream_receiver.h"

#define TSP_SIZEOF_ENCODED_DOUBLE RNDUP(sizeof(double))

struct TSP_struct_data_receiver_t
{
  TSP_stream_receiver_t stream_receiver;
    
  char* buf;
};

typedef struct TSP_struct_data_receiver_t TSP_struct_data_receiver_t;

static int TSP_data_receiver_double_decoder(void* out_double,  char* in_buf)
{
#ifndef TSP_NO_XDR_ENCODE

  SFUNC_NAME(TSP_encode_double with XDR);

  XDR xhandle;
  xdrmem_create(&xhandle, in_buf, TSP_SIZEOF_ENCODED_DOUBLE, XDR_DECODE);
  /*FIXME ? Pas besoin de faire le memset ? */
  if( xdr_double(&xhandle, (double*)out_double) != TRUE)
    {
      STRACE_ERROR(("Function xdr_double failed"));
      return FALSE;
    }
  else
    {
      return TRUE;
    }
  /* FIXME ? on appel par xdr_free pour un double ? */

#else
  
  SFUNC_NAME(TSP_encode_double withOUT XDR);

  /* FIXME : dans ce cas la fonction reussit toujours pas besoin du return */

  *(gint64*)out_double = TSP_DECODE_DOUBLE_TO_GUINT64(in_buf);
  return TRUE;

#endif

}

TSP_data_receiver_t TSP_data_receiver_create(const char* data_address)
{
  SFUNC_NAME(TSP_data_receiver_create);

    
  TSP_struct_data_receiver_t* receiver;
    
  STRACE_IO(("-->IN"));

  /* FIXME : desallouer */
  receiver = (TSP_struct_data_receiver_t*)calloc(1, sizeof(TSP_struct_data_receiver_t));
  TSP_CHECK_ALLOC(receiver, 0);

  /* FIXME : desallouer */
  receiver->buf = (char*)calloc(TSP_DATA_STREAM_CREATE_BUFFER_SIZE, sizeof(char));
  TSP_CHECK_ALLOC(receiver->buf, 0);
    
  receiver->stream_receiver = TSP_stream_receiver_create(data_address);
  if( 0 == receiver->stream_receiver)
    {
      /* FIXME : bof...*/
      STRACE_ERROR(("TSP_stream_receiver_create failed")); 
      free(receiver);
      receiver = 0;
    
    }
    
        
  STRACE_IO(("-->OUT"));

    
  return receiver;

}

int TSP_data_receiver_receive(TSP_data_receiver_t _receiver,
                              TSP_groups_t _groups,
                              TSP_sample_ringbuf_t* sample_ringbuf[]) 
{
    
  SFUNC_NAME(TSP_data_receiver_receiver);

	
  TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;
  TSP_group_t* groups =  ((TSP_group_table_t*)_groups)->groups;
  int nb_groups = ((TSP_group_table_t*)_groups)->table_len;
  int group_index = 0;
  time_stamp_t time_stamp = 0;
  int ret = FALSE;
  int* buf_int;  
    
 

  STRACE_IO(("-->IN"));

    
  /* receive group size and time_stamp*/
  buf_int = (int*)(receiver->buf);
  ret = TSP_stream_receiver_receive(receiver->stream_receiver,
				    (char*)buf_int,
				    sizeof(int)*2);
  
                                       
  if(ret)
    {
      int buf_len;
      time_stamp = buf_int[0];
      group_index = buf_int[1];
	
    
      /* Check if the group_index looks real */
      if ((group_index < nb_groups) && (group_index >= 0) )
        {
            
	  /*receive all doubles*/
	  ret = TSP_stream_receiver_receive(receiver->stream_receiver,
					    receiver->buf,
					    groups[group_index].sizeof_encoded_group);    
                                           
	  if(ret)
            {
	      char* in_buf = receiver->buf;
	      int rank;
	      TSP_sample_ringbuf_t* symbol_ringbuf;
	      TSP_sample_t* sample;
	      /*printf("V=%f\n", *buf_double);*/
	      for( rank = 0 ; rank < groups[group_index].group_len ; rank++)
                {
		  /*STRACE_DEBUG(("RECEIVED : T=%d Gr=%d V=%f",
		    time_stamp,
		    group_index,  
		    buf_double[i]))*/
                     
		  
		  /* Add symbols to their ringbuf */                   
		  symbol_ringbuf = sample_ringbuf[groups[group_index].items[rank].provider_global_index];
                  
		  if( (sample = RINGBUF_PTR_PUTBYADDR(symbol_ringbuf))  )
		    {
		      /*Decode data*/
		      assert(groups[group_index].items[rank].data_decoder);
		      ret = (groups[group_index].items[rank].data_decoder)(&(sample->user_value),in_buf);
		      if(!ret)
			{
			  STRACE_ERROR(("decoder function failed"));
			  break;
			}
		     
		      /* add time stamp */
		      sample->time = time_stamp;
		     		      
		      RINGBUF_PTR_PUTBYADDR_COMMIT(symbol_ringbuf);
		    }
		  else
		    {
		      STRACE_ERROR(("RingBuffer full for global index %d : %d missed data",
				    groups[group_index].items[rank].provider_global_index,
				    RINGBUF_PTR_MISSED(symbol_ringbuf)));
		    }

		  /* Goto next symbol */
		  in_buf += groups[group_index].items[rank].sizeof_encoded_item;
                     
                }
                
            }
	  else    
            {
	      STRACE_ERROR(("Unable to receive samples"));

            }
                                           
        }
      else
        {
	  STRACE_ERROR(("The received group id is corrupted"));
        }
    }
  else
    {
      STRACE_ERROR(("Unable to receive group size and time stamp"));

    }
    
  STRACE_IO(("-->OUT"));

  return ret;
    
  /*FIXME : gerer les autres cas, et les broken pipe*/
        

}

TSP_data_decoder_t TSP_data_receiver_get_double_decoder(void)
{
  return TSP_data_receiver_double_decoder;
}

int TSP_data_receiver_get_double_encoded_size(void)
{
  return TSP_SIZEOF_ENCODED_DOUBLE;
}
