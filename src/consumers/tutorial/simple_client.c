/*!  \file 

$Id: simple_client.c,v 1.2 2003-03-13 18:08:50 yduf Exp $

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

/* All what you need for creating a TSP consumer */
#include "tsp_consumer.h"


/* Just for fast exit */
void perror_and_exit(char *txt)
{
  perror (txt);
  exit (-1);
}


/* Everthing must begin somewhere */
int main(int argc, char *argv[])
{
  const TSP_consumer_information_t*  information;
  TSP_consumer_symbol_requested_list_t symbols;
  int i, count_frame, nb_providers, wanted_sym=10, t = -1;
  TSP_provider_t* providers;
  char* name;

  printf ("#=========================================================#\n");
  printf ("# Launching tutorial client\n");
  printf ("#=========================================================#\n");

  /* Initialisation for TSP library. */
  if(!TSP_consumer_init(&argc, &argv))
      perror_and_exit("TSP init failed");
 
  if (argc==2)
    name = argv[1];
  else
    {
      printf("USAGE %s : server_name \n", argv[0]);
      return -1;
    }

  /*-------------------------------------------------------------------------------------------------------*/   
  /* Connects to all found providers on the given host. */
  TSP_consumer_connect_all(name, &providers, &nb_providers);
  if(!nb_providers)
      perror_and_exit("TSP_consumer_connect_all found no provider on host");
  
  /* Ask the provider for a new consumer session.*/
  if(!TSP_consumer_request_open(providers[0], 0, 0))
    perror_and_exit("TSP_request_provider_open failed");

  /* Ask the provider informations about several parameters, including
   * the available symbol list that can be asked. */
  if(!TSP_consumer_request_information(providers[0]))
    perror_and_exit("TSP_request_provider_information failed");

  /* Get the provider information asked by TSP_consumer_request_information */
  information = TSP_consumer_get_information(providers[0]);
  if (wanted_sym > information->symbols.len)
	wanted_sym = information->symbols.len;

  /* Will use only the "wanted_sym" first symbols of provider */
  symbols.val = (TSP_consumer_symbol_requested_t*)calloc(wanted_sym, sizeof(TSP_consumer_symbol_requested_t));

  symbols.len = wanted_sym;
  for(i = 0 ; i < wanted_sym ; i++)
    {
      symbols.val[i].name = information->symbols.val[i].name;
      symbols.val[i].period = 1;	/* at max frequency */
      symbols.val[i].phase = 0;		/* with no offset */
      printf ("Asking for symbol = %s\n", symbols.val[i].name);
    }

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Ask the provider for sampling this  list of symbols. Should check if all symbols are OK*/
  if(!TSP_consumer_request_sample(providers[0], &symbols))
    perror_and_exit("TSP_request_provider_sample failed");

  /* Start the sampling sequence. */
  if(!TSP_consumer_request_sample_init(providers[0], 0))
    perror_and_exit("TSP_request_provider_sample_init failed");
  
  /* Loop on data read */
  for (count_frame = 0; count_frame<100; )
    {
      int new_sample=FALSE;
      TSP_sample_t sample;
      
      /* Read a sample symbol.*/
      if (TSP_consumer_read_sample(providers[0], &sample, &new_sample))
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
	      printf ("# Sample nb[%d] \t%s \tval=%f\n", i, symbols.val[i].name, sample.user_value);
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

  free (symbols.val);

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Stop and destroy the sampling sequence*/
  if(!TSP_consumer_request_sample_destroy(providers[0]))
    perror_and_exit("Function TSP_consumer_request_sample_destroy failed" );	 

  /* Close the session.*/
  if(!TSP_consumer_request_close(providers[0]))
    perror_and_exit("Function TSP_consumer_request_close failed" );			    

  /* Disconnected all found providers.*/
  TSP_consumer_disconnect_all(providers);
  
  /* call this function when you are done with the librairy.*/
  TSP_consumer_end();			

  return 0;
}
