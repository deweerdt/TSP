/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/Attic/tsp_sample_ringbuf.c,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implementation for the creation of the n ring_buf used by the user of
the consumer to retreive the n asked symbols

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"

#include "tsp_sample_ringbuf.h"

/*---------------------------------------------------------*/
/*                  FONCTIONS INTERNES  			       */
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
/*                  FONCTIONS EXTERNE  			            */
/*---------------------------------------------------------*/

void TSP_sample_ringbuf_create(
			       const TSP_sample_symbol_info_list_t* symbols, 
			       TSP_sample_ringbuf_t** sample_ringbuf)
{
  SFUNC_NAME(TSP_request_provider_sample);

  int i;
  int global_index;
    
  STRACE_IO(("-->IN"));
    
  for( i = 0 ; i< symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      global_index = symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index;
      /* FIXME : impossible de verifier l'allocation avec cette implementation
	 du ringbuf*/
      RINGBUF_PTR_INIT(TSP_sample_ringbuf_t,
		       sample_ringbuf[global_index],
		       TSP_sample_t, 
		       RINGBUF_SZ(TSP_CONSUMER_RINGBUF_SIZE) )
                        
	}

  STRACE_IO(("-->OUT"));
    
}
