/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.c,v 1.7 2002-11-29 17:33:30 tntdev Exp $

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

static void TSP_data_receiver_process_receiver_error(TSP_sample_ringbuf_t* sample_fifo)
{

  SFUNC_NAME(TSP_data_receiver_process_receiver_error);
  int ret = TRUE;
  TSP_sample_t* sample;

  STRACE_IO(("-->IN"));
  
  sample = RINGBUF_PTR_PUTBYADDR(sample_fifo);
  assert(sample);
  sample->time = -1;
  sample->user_value = -1;	
  sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECEIVER_ERROR;
  RINGBUF_PTR_PUTBYADDR_COMMIT(sample_fifo);
    
  STRACE_IO(("-->OUT"));

}

static int TSP_data_receiver_process_reserved_group_id(int group_index, TSP_sample_ringbuf_t* sample_fifo)
{

  SFUNC_NAME(TSP_data_receiver_process_reserved_group_id);
  int ret = TRUE;
  TSP_sample_t* sample;

  STRACE_IO(("-->IN"));
  

  sample = RINGBUF_PTR_PUTBYADDR(sample_fifo);
  assert(sample);
  sample->time = -1;
  sample->user_value = -1;	
  switch(group_index)
    {
      /* received EOF */
    case TSP_RESERVED_GROUP_EOF: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF;
	break;

    case TSP_RESERVED_GROUP_RECONF: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECONF;
	break;
    
    default:	 
      STRACE_ERROR(("Group id % in not a reserved group", group_index));
      ret = FALSE;
    }
  
  if(ret)
    {
      RINGBUF_PTR_PUTBYADDR_COMMIT(sample_fifo);
    }
  
  STRACE_IO(("-->OUT"));

  return ret;

}

TSP_data_receiver_t TSP_data_receiver_create(const char* data_address)
{
  SFUNC_NAME(TSP_data_receiver_create);

    
  TSP_struct_data_receiver_t* receiver;
    
  STRACE_IO(("-->IN"));


  receiver = (TSP_struct_data_receiver_t*)calloc(1, sizeof(TSP_struct_data_receiver_t));
  TSP_CHECK_ALLOC(receiver, 0);

  /* Allocate buffer for reception*/
  receiver->buf = (char*)calloc(TSP_DATA_STREAM_CREATE_BUFFER_SIZE, sizeof(char));
  TSP_CHECK_ALLOC(receiver->buf, 0);
    
  receiver->stream_receiver = TSP_stream_receiver_create(data_address);
  if( 0 == receiver->stream_receiver)
    {
      /* FIXME : bof...*/
      STRACE_ERROR(("TSP_stream_receiver_create failed")); 
      free(receiver->buf);receiver->buf = 0;
      free(receiver);
      receiver = 0;
    
    }
    
        
  STRACE_IO(("-->OUT"));

    
  return receiver;

}

/**
* Receive data from provider and decode them.
* @param _receiver The receiver instance (network, incore ...)
* @param _groups The group decoder instance
* @param sample_fifo The ring buf instance where we are going to put the new data
* @param fifo_full If == TRUE, means that no data can be added be as sample_fifo is full
* @param if FALSE, function failed
*/
int TSP_data_receiver_receive(TSP_data_receiver_t _receiver,
                              TSP_groups_t _groups,
                              TSP_sample_ringbuf_t* sample_fifo,
			      int* fifo_full) 
{
    
  SFUNC_NAME(TSP_data_receiver_receive);

	
  TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;
  TSP_group_t* groups =  ((TSP_group_table_t*)_groups)->groups;
  int nb_groups = ((TSP_group_table_t*)_groups)->table_len;
  int max_group_len = ((TSP_group_table_t*)_groups)->max_group_len;
  int group_index = 0;
  time_stamp_t time_stamp = 0;
  int ret = FALSE;
  int* buf_int;  
  TSP_sample_t* sample;



  *fifo_full = FALSE;

  /* We read data if there enough room in ringbuf to store data, else, do nothing ; 
     we want the biggest group to have enough room, so we use max_group_len
     which the size of the biggest group and we compare with room left in ringbuf*/
  if( RINGBUF_PTR_ITEMS_LEFT(sample_fifo) > max_group_len )
    {
      
      /* OK, enough room. receive group size and time_stamp*/
      buf_int = (int*)(receiver->buf);
      ret = TSP_stream_receiver_receive(receiver->stream_receiver,
					(char*)buf_int,
					sizeof(int)*2);
      if(ret)
	{
	  int buf_len;
	  time_stamp = TSP_DECODE_INT(buf_int[0]);
	  group_index = TSP_DECODE_INT(buf_int[1]);
	
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
	      
		  /*printf("V=%f\n", *buf_double);*/
		  for( rank = 0 ; rank < groups[group_index].group_len ; rank++)
		    {
		      /*STRACE_DEBUG(("RECEIVED : T=%d Gr=%d V=%f",
			time_stamp,
			group_index,  
			buf_double[i]))*/
                     
		      if( (sample = RINGBUF_PTR_PUTBYADDR(sample_fifo))  )
			{
			  /* Call registered function to decode data */
			  assert(groups[group_index].items[rank].data_decoder);		      
			  ret = (groups[group_index].items[rank].data_decoder)(&(sample->user_value),in_buf);
			  if(!ret)
			    {
			      STRACE_ERROR(("decoder function failed"));
			      break;
			    }
		     
			  /* add time stamp */
			  sample->time = time_stamp;
			  sample->provider_global_index = groups[group_index].items[rank].provider_global_index;


			  RINGBUF_PTR_PUTBYADDR_COMMIT(sample_fifo);
			}
		      else
			{
			  STRACE_ERROR(("Receive RingBuffer full : %d missed data. Looks like a bug",
					RINGBUF_PTR_MISSED(sample_fifo)));
			  /* FIXME : This else  must be suppressed. If we are here. It is a bug 'coz' the
			   fifo can not be full, by design */
			  assert(0);
			  exit(-1);
			}

		      /* Goto next symbol */
		      in_buf += groups[group_index].items[rank].sizeof_encoded_item;
                     
		    }
		}
	      else
		{
		  if(!TSP_stream_receiver_is_stopped(receiver->stream_receiver))
		    {
		      STRACE_WARNING(("Unable to receive samples"));
		      /* Add in fifo a message to report the incident via the read_sample API*/
		      TSP_data_receiver_process_receiver_error(sample_fifo);
		    }
		}
	
	    }
	  else
	    {
	      /* Hum...Strange groupe index, may be it is a reserved on */
	      ret = TSP_data_receiver_process_reserved_group_id(group_index, sample_fifo);
	      if(!ret)
		{
		  STRACE_ERROR(("The received group id is corrupted. This should not happen..."));
		  assert(0);
		  TSP_data_receiver_process_receiver_error(sample_fifo);
		}
	    }
      
	}
      else
	{
	  if(!TSP_stream_receiver_is_stopped(receiver->stream_receiver))
	    {		  
	      STRACE_WARNING(("Unable to receive group size and time stamp"));
	      /* Add in fifo a message to report the incident via the read sample API*/
	      TSP_data_receiver_process_receiver_error(sample_fifo);
	    }
	}
    }
  else
    {
      if(!TSP_stream_receiver_is_stopped(receiver->stream_receiver))
	{	
	  *fifo_full = TRUE;
	  ret = TRUE;
	}
    }
    


  return ret;    
}

void TSP_data_receiver_stop(TSP_data_receiver_t _receiver)
{

   SFUNC_NAME(TSP_data_receiver_receive);
   
   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));

   TSP_stream_receiver_stop(receiver->stream_receiver);

   STRACE_IO(("-->OUT"));
}

void TSP_data_receiver_prepare_stop(TSP_data_receiver_t _receiver)
{

   SFUNC_NAME(TSP_data_receiver_receive);
   
   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));

   TSP_stream_receiver_prepare_stop(receiver->stream_receiver);

   STRACE_IO(("-->OUT"));
}


void TSP_data_receiver_destroy(TSP_data_receiver_t _receiver)
{

   SFUNC_NAME(TSP_data_receiver_receive);
   
   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));
   
   TSP_stream_receiver_destroy(receiver->stream_receiver);
   free(receiver->buf); receiver->buf = 0;
   free(receiver); 

   STRACE_IO(("-->OUT"));
}


TSP_data_decoder_t TSP_data_receiver_get_double_decoder(void)
{
  return TSP_data_receiver_double_decoder;
}

int TSP_data_receiver_get_double_encoded_size(void)
{
  return TSP_SIZEOF_ENCODED_DOUBLE;
}


 
            
	      
			      
            
