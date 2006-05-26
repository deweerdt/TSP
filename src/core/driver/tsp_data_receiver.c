/*

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_data_receiver.c,v 1.26 2006-05-26 14:12:06 erk Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implementation for the functions used to receive and decode the data
stream  for the requested symbols
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


struct TSP_struct_data_receiver_t
{
  TSP_stream_receiver_t stream_receiver;
    
  char* buf;

  TSP_sample_callback_t read_callback;

  void* user_data;
};

typedef struct TSP_struct_data_receiver_t TSP_struct_data_receiver_t;


static void TSP_data_receiver_process_receiver_error(TSP_sample_ringbuf_t* sample_fifo)
{

  TSP_sample_t* sample;

  STRACE_IO(("-->IN"));
  
  sample = RINGBUF_PTR_PUTBYADDR(sample_fifo);
  assert(sample);
  sample->time = -1;
  sample->uvalue.double_value = -1;	
  sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECEIVER_ERROR;
  RINGBUF_PTR_PUTBYADDR_COMMIT(sample_fifo);
    
  STRACE_IO(("-->OUT"));

}

static int TSP_data_receiver_process_reserved_group_id(int group_index, TSP_sample_ringbuf_t* sample_fifo)
{

  int ret = TRUE;
  TSP_sample_t* sample;

  STRACE_IO(("-->IN"));
  

  sample = RINGBUF_PTR_PUTBYADDR(sample_fifo);
  assert(sample);
  sample->time = -1;
  sample->uvalue.double_value = -1;	
  switch(group_index)
    {
      /* received EOF */
    case TSP_RESERVED_GROUP_EOF: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF;
	break;

    case TSP_RESERVED_GROUP_RECONF: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECONF;
	break;

    case TSP_RESERVED_GROUP_GLU_DATA_LOST: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_GLU_DATA_LOST;
	break;

    case TSP_RESERVED_GROUP_CONSUMER_DATA_LOST: 
	sample->provider_global_index = TSP_DUMMY_PROVIDER_GLOBAL_INDEX_CONSUMER_DATA_LOST;
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

TSP_data_receiver_t TSP_data_receiver_create(const char* data_address, TSP_sample_callback_t callback, void* user_data)
{
    
  TSP_struct_data_receiver_t* receiver;
    
  STRACE_IO(("-->IN"));


  receiver = (TSP_struct_data_receiver_t*)calloc(1, sizeof(TSP_struct_data_receiver_t));
  TSP_CHECK_ALLOC(receiver, 0);

  /* Allocate buffer for reception*/
  receiver->buf = (char*)calloc(TSP_DATA_RECEIVER_BUFFER_SIZE, sizeof(char));
  TSP_CHECK_ALLOC(receiver->buf, 0);
    
  receiver->read_callback = callback;
  receiver->user_data = user_data; 
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

int TSP_data_receiver_receive(TSP_data_receiver_t _receiver,
                              TSP_groups_t _groups,
                              TSP_sample_ringbuf_t* sample_fifo,
			      int* fifo_full) 
{
    
  TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;
  TSP_group_t* groups =  ((TSP_group_table_t*)_groups)->groups;
  int nb_groups = ((TSP_group_table_t*)_groups)->table_len;
  int max_group_len = ((TSP_group_table_t*)_groups)->max_group_len;
  int group_index = 0;
  time_stamp_t time_stamp = 0;
  int ret = FALSE;
  int* buf_int;  
  int32_t i;
  TSP_sample_t* sample;
  TSP_sample_t sample_buf;
  int receiver_stopped;

  *fifo_full = FALSE;
  sample = &sample_buf;


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
      /* Check if the receiver was not stopped */
      receiver_stopped = TSP_stream_receiver_is_stopped(receiver->stream_receiver);

      if(ret && !receiver_stopped)
	{
	 /*  int buf_len; */
	  time_stamp = TSP_DECODE_INT(buf_int[0]);
	  group_index = TSP_DECODE_INT(buf_int[1]);

	  /* Check if the group_index looks real */
	  if ((group_index < nb_groups) && (group_index >= 0) )
	    {
            
	      /* receive all (RAW) data for the current group */
	      ret = TSP_stream_receiver_receive(receiver->stream_receiver,
						receiver->buf,
						groups[group_index].sizeof_encoded_group);    

	      /* Check if the receiver was not stopped */
	      receiver_stopped = TSP_stream_receiver_is_stopped(receiver->stream_receiver);
                                           
	      if(ret && !receiver_stopped)
		{
		  char* in_buf = receiver->buf;
		  int rank;	
	      
		  /*printf("V=%f\n", *buf_double);*/
		  for( rank = 0 ; rank < groups[group_index].group_len ; rank++)
		    {

		      /* Call registered function to decode data */
		      
		     
		       /* decode received data */
		      ret = (groups[group_index].items[rank].data_decoder)(groups[group_index].decode_buffer,
									   groups[group_index].items[rank].symbol_info->nelem,
									   in_buf);
		      if(!ret)
			{
			  STRACE_ERROR(("decoder function failed"));
			  break;
			}

		      /* FIXME boucle à partir d'offset pour nelem element () sur sample->dimension pour affectation de groups[group_index].decode_buffer
		       * dans sample->uvalue.TYPE_value = decode_buffer[i]
		       * for scalar offset should be 0 and nelem = dimension = 1
		       * 
		       */
		      for(i=groups[group_index].items[rank].symbol_info->offset;
			  i < groups[group_index].items[rank].symbol_info->offset+groups[group_index].items[rank].symbol_info->nelem;
			  ++i)
		      {

			/*--------------*/
			if(! (receiver->read_callback) )
			{
			  sample = RINGBUF_PTR_PUTBYADDR(sample_fifo);
			  assert(sample); /* can not be full, by design */
			}
			/*---------------*/


		        sample->type=groups[group_index].items[rank].symbol_info->type;
		        switch( groups[group_index].items[rank].symbol_info->type) {
			case TSP_TYPE_DOUBLE :
			  sample->uvalue.double_value= ((double *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_FLOAT :
			  sample->uvalue.float_value= ((float *)groups[group_index].decode_buffer)[i];
			  break;
			case TSP_TYPE_INT8 :
			  sample->uvalue.int8_value= ((int8_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_INT16:
			  sample->uvalue.int16_value= ((int16_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_INT32 :
			  sample->uvalue.int32_value= ((int32_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_INT64 :
			  sample->uvalue.int64_value= ((int64_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_UINT8:
			  sample->uvalue.uint8_value= ((uint8_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_UINT16:
			  sample->uvalue.uint16_value= ((uint16_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_UINT32:
			  sample->uvalue.uint32_value= ((uint32_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_UINT64:
			  sample->uvalue.uint64_value= ((uint64_t *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_CHAR:
			  sample->uvalue.char_value= ((char *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_UCHAR:
			  sample->uvalue.uchar_value= ((unsigned char *)groups[group_index].decode_buffer)[i];
			  break;
			  
			case TSP_TYPE_RAW:
			  sample->uvalue.uint8_value= ((uint8_t *)groups[group_index].decode_buffer)[i];
			  break;
			  /* FIXME should not reach this */
			case TSP_TYPE_UNKNOWN:
			  
			default:
			  STRACE_WARNING(("Impossible TSP TYPE in decode??"));
			  sample->uvalue.raw_value= ((uint8_t*)groups[group_index].decode_buffer)[i];
                               break;
			       
			}

			/* add time stamp */
			sample->time = time_stamp;
			sample->provider_global_index = groups[group_index].items[rank].provider_global_index;
			sample->array_index = i;

			/* FIXME : Implement error code returned by callback */
			if(! (receiver->read_callback) )
			{
			  /*(sample_fifo)->put = ((sample_fifo)->put + 1) % (sample_fifo)->size ;*/
			  RINGBUF_PTR_PUTBYADDR_COMMIT(sample_fifo);
			}
			else
			{
			  (receiver->read_callback)(sample, receiver->user_data);
			}
		      }

		      /* Goto next symbol */
		      in_buf += groups[group_index].items[rank].sizeof_encoded_item;

		    }
		}
	      else
		{
		  if(!receiver_stopped)
		    {
		      STRACE_WARNING(("Unable to receive samples"));
		      /* Add in fifo a message to report the incident via the read_sample API*/
		      TSP_data_receiver_process_receiver_error(sample_fifo);
		    }
		}
	
	    }
	  else
	    {
	      /* Hum...Strange group index, may be it is a reserved on */
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
	  if(!receiver_stopped)
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

   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));

   TSP_stream_receiver_stop(receiver->stream_receiver);

   STRACE_IO(("-->OUT"));
}

void TSP_data_receiver_prepare_stop(TSP_data_receiver_t _receiver)
{
   
   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));

   TSP_stream_receiver_prepare_stop(receiver->stream_receiver);

   STRACE_IO(("-->OUT"));
}


void TSP_data_receiver_destroy(TSP_data_receiver_t _receiver)
{
   
   TSP_struct_data_receiver_t*  receiver = ( TSP_struct_data_receiver_t*) _receiver;

   STRACE_IO(("-->IN"));
   
   TSP_stream_receiver_destroy(receiver->stream_receiver);
   free(receiver->buf); receiver->buf = 0;
   free(receiver); 

   STRACE_IO(("-->OUT"));
}



            
	      
			      
            
