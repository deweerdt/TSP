/*

$Id: simple_client.c,v 1.8 2006-04-28 09:40:16 erk Exp $

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
Maintainer: tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Simple consummer tutorial

-----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
/* All what you need for creating a TSP consumer */
#include <tsp_consumer.h>
#include <tsp_time.h>


/* Just for fast exit */
void perror_and_exit(char *txt)
{
  perror (txt);
  exit (-1);
}


/* Everthing must begin somewhere */
int main(int argc, char *argv[])
{
  const TSP_answer_sample_t*  information;
  TSP_sample_symbol_info_list_t symbols;
  int i, count_frame, wanted_sym=10, t = -1;
  TSP_provider_t provider;
  char* url;

  printf ("#=========================================================#\n");
  printf ("# Launching tutorial client\n");
  printf ("#=========================================================#\n");

  /* Initialisation for TSP library. */
  if(TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv))
      perror_and_exit("TSP init failed");
 
  if (argc==2)
    url = argv[1];
  else
    {
      printf("USAGE %s : TSP_url \n", argv[0]);
      return -1;
    }

  /*-------------------------------------------------------------------------------------------------------*/   
  /* Connects to all found providers on the given host. */
  provider = TSP_consumer_connect_url(url);
  if(0==provider)
    perror_and_exit("TSP_consumer_connect_url failed ");

  /* Ask the provider for a new consumer session.*/
  if(TSP_STATUS_OK!=TSP_consumer_request_open(provider, 0, 0))
    perror_and_exit("TSP_request_provider_open failed");

  /* Ask the provider informations about several parameters, including
   * the available symbol list that can be asked. */
  if(TSP_STATUS_OK!=TSP_consumer_request_information(provider))
    perror_and_exit("TSP_request_provider_information failed");

  /* Get the provider information asked by TSP_consumer_request_information */
  information = TSP_consumer_get_information(provider);
  if (wanted_sym > information->symbols.TSP_sample_symbol_info_list_t_len)
	wanted_sym = information->symbols.TSP_sample_symbol_info_list_t_len;

  /* Will use only the "wanted_sym" first symbols of provider */
  symbols.TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)calloc(wanted_sym, sizeof(TSP_sample_symbol_info_t));

  symbols.TSP_sample_symbol_info_list_t_len = wanted_sym;
  for(i = 0 ; i < wanted_sym ; i++)
    {
      symbols.TSP_sample_symbol_info_list_t_val[i].name = information->symbols.TSP_sample_symbol_info_list_t_val[i].name;
      symbols.TSP_sample_symbol_info_list_t_val[i].period = 1;	/* at max frequency */
      symbols.TSP_sample_symbol_info_list_t_val[i].phase = 0;		/* with no offset */
      printf ("Asking for symbol = %s\n", symbols.TSP_sample_symbol_info_list_t_val[i].name);
    }

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Ask the provider for sampling this  list of symbols. Should check if all symbols are OK*/
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(provider, &symbols))
    perror_and_exit("TSP_request_provider_sample failed");

  /* Start the sampling sequence. */
  if(TSP_STATUS_OK!=TSP_consumer_request_sample_init(provider, 0, 0))
    perror_and_exit("TSP_request_provider_sample_init failed");
  
  /* Loop on data read */
  for (count_frame = 0; count_frame<100; )
    {
      int new_sample=FALSE;
      TSP_sample_t sample;
      
      /* Read a sample symbol.*/
      if (TSP_STATUS_OK==TSP_consumer_read_sample(provider, &sample, &new_sample))
	{
	  if(new_sample)
	    {
	      if (t != sample.time)
		{
		  count_frame++;
		  t = sample.time;
		  printf ("======== Frame[%d] ======== Time : %d ========\n", count_frame, t);
		}		  
	      i = sample.provider_global_index;
	      printf ("# Sample nb[%d] \t%s \tval=%f\n", i, symbols.TSP_sample_symbol_info_list_t_val[i].name, sample.uvalue.double_value);
	    }
	  else
	    {
	      /* Used to give time to other thread for filling fifo of received samples */
	      tsp_usleep(100*1000); /* in µS, so about 100msec */
	    }
	}
      else
	{
	  perror_and_exit ("Function TSP_consumer_read_sample failed !!\n");
	}
    }

  free (symbols.TSP_sample_symbol_info_list_t_val);

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Stop and destroy the sampling sequence*/
  if(TSP_STATUS_OK!=TSP_consumer_request_sample_destroy(provider))
    perror_and_exit("Function TSP_consumer_request_sample_destroy failed" );	 

  /* Close the session.*/
  if(TSP_STATUS_OK!=TSP_consumer_request_close(provider))
    perror_and_exit("Function TSP_consumer_request_close failed" );			    

  /* call this function when you are done with the librairy.*/
  TSP_consumer_end();			

  return 0;
}
