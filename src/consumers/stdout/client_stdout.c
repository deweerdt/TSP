/*!  \file 

$Id: client_stdout.c,v 1.8 2004-09-27 13:47:01 tractobob Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Simple consummer test that print samples received to stdout

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_consumer.h"

/*#include "fortify.h"*/


/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 100  
#define TSP_NANOSLEEP_PERIOD_US (100*1000) /* 10Hz */

int main(int argc, char *argv[]){

  const TSP_consumer_information_t*  information;
  TSP_consumer_symbol_requested_list_t symbols;

  int i, j;
  int period=0;
  char* name;
  int count_samples = 0;
  int nb_samples = -1;
  char symbol_buf[50];
  int test_ok = TRUE;
  int test_mode = 0;
  int all_data_ok = TRUE;
  TSP_provider_t provider;


  /*Fortify_EnterScope();*/
  printf ("#=========================================================#\n");
  printf ("# Launching <stdout_client> for printing symbols received #\n");
  printf ("#=========================================================#\n");
 
  if(!TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR(("TSP init failed"));
      return -1;
    }

  switch (argc-1) { /* No Breaks please */
  case 5: 
    nb_samples = atoi(argv[5]);  /* nb-sample */
  case 4:
    test_mode = atoi(argv[4]);  /* 0 = no, 1=infinite loop */
  case 3:       /* Anything after name and perdio --> test mode */
  case 2:
    period = atoi (argv[2]);
  case 1:      
    name = argv[1];
    break;
  default : 
      STRACE_ERROR(("USAGE %s : serverURL period <test mode nb_sample> \n", argv[0]));
      return -1;
  }

 once_again:
  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  provider = TSP_consumer_connect_url(name);
  if(provider)
    {
      const char* info = TSP_consumer_get_connected_name(provider) ;
      STRACE_INFO(("Server %s, info = '%s'", name, info));
	  
      /* Check name */
      if(strcmp(info, "StubbedServer"))
	{
	  STRACE_ERROR(("Serveur name corrupted"));
	  STRACE_TEST(("STAGE 001 | STEP 001 : FAILED"));
	  return -1;
	}
	  
    }
  else
    {
      STRACE_ERROR(("Unable to find any provider for host"));
      return -1;
    }

  STRACE_TEST(("STAGE 001 | STEP 001 : PASSED"));


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 002 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Le 1er provider existe puisqu'il y en a au moins 1 */

  if(!TSP_consumer_request_open(provider, 0, 0))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      STRACE_TEST(("STAGE 001 | STEP 002 : FAILED"));
      return -1;
    }
  
  STRACE_TEST(("STAGE 001 | STEP 002 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_information(provider))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }
  
  /* be wild, ask twice to try to trigger mem leak  */
  if(!TSP_consumer_request_information(provider))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }


  information = TSP_consumer_get_information(provider);

  /* Check total symbol number */
  if(information->symbols.len < 1 || information->symbols.len > 999999)
    {
      STRACE_ERROR(("The total number of symbols should be fair, and not %d",information->symbols.len));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;      
    }

  /* Compare symbols names */
  for( i = 1 ; i<  information->symbols.len ; i++)
    {
      sprintf(symbol_buf, "Symbol%d",i);
      if(strcmp(symbol_buf,  information->symbols.val[i].name))
	{
printf("%s != %s\n", symbol_buf,  information->symbols.val[i].name);
	  STRACE_ERROR(("Symbol name corrupted"));
	  STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
	  return -1;
	}
      
    }
      
  STRACE_TEST(("STAGE 001 | STEP 003 : PASSED"));

  if (nb_samples>0)
    symbols.len = nb_samples;
  else
    symbols.len = information->symbols.len;
  symbols.val = (TSP_consumer_symbol_requested_t*)calloc(symbols.len, sizeof(TSP_consumer_symbol_requested_t));
  TSP_CHECK_ALLOC(symbols.val, -1);

  /* Change period of sampling for each client */
  for(i = 0 ; i < symbols.len ; i++)
    {
      symbols.val[i].name = information->symbols.val[i].name;
      symbols.val[i].period = period;
      symbols.val[i].phase = 0;
    }

  STRACE_INFO(("Asking for %d symboles", symbols.len));
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 004 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample(provider, &symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
      return -1;
    }
  /* be wild, ask twice to try to trigger mem leak  */
  if(!TSP_consumer_request_sample(provider, &symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
      return -1;
    }

  free(symbols.val);

  STRACE_TEST(("STAGE 001 | STEP 004 : PASSED"));
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 005 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample_init(provider, 0, 0))
    {
      STRACE_ERROR(("TSP_request_provider_sample_init failed"));
      STRACE_TEST(("STAGE 001 | STEP 005 : FAILED"));
      return -1;
    }
  STRACE_TEST(("STAGE 001 | STEP 005 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 006 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Loop on data read */
  count_samples = 0;
  while(1)
    {
      int new_sample;
      TSP_sample_t sample;
      int i;int j;
      int t = -1000;
      
      new_sample = FALSE;
      do{
	if(TSP_consumer_read_sample(provider,&sample, &new_sample))
	  {
	    if(new_sample)
	      {
		    
		double calc;
		    
		i = sample.provider_global_index;
		if(t == -1000) 
		  {
		    t = sample.time - 1; 
		  }

		if(i == 1 )
		  {
		    count_samples++;
		    printf ("TSP : Sample nb[%d] time=%d val=%f\n", count_samples, sample.time, sample.user_value);
		  }

		calc = calc_func(i,sample.time);
		
		/* i = 0 is t */
		if(i != 0)
		  {
		    if( (ABS(sample.user_value - calc) > 1e-7) && (t == (sample.time - 1)) )
		      {
			STRACE_ERROR(("!!!!ERROR : T=%u, I=%d, V1=%f, V2=%f", sample.time,i,sample.user_value,calc ));			
			all_data_ok = FALSE;
		      }
		  }

		t = sample.time;

		/* Test */
		if(count_samples >= TSP_TEST_COUNT_SAMPLES && test_mode!=1)
		  {
		    if(all_data_ok)
		      {

			STRACE_TEST(("STAGE 001 | STEP 006 : PASSED" ));			
			
			/* --------------- */
			/* Close providers */
			/* --------------- */
			if(!TSP_consumer_request_sample_destroy(provider))
			  {
			    STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed" ));	 
			  }
			
			if(!TSP_consumer_request_close(provider))
			  {
			    STRACE_ERROR(("Function TSP_consumer_request_close failed" ));			    
			  }
			
			TSP_consumer_disconnect_one(provider);
			

			/* Fortify calls */
			/*Fortify_LeaveScope();
			  Fortify_OutputStatistics();*/
			
			/* goto once_again; */
			TSP_consumer_end();

			printf ("#=========================================================#\n");
			printf ("# End of Test OK \n");
			printf ("#=========================================================#\n");
			return 1;
		      }
		    else
		      {
			printf ("#=========================================================#\n");
			printf ("# End of Test KO \n");
			printf ("#=========================================================#\n");
			return -1;
		      }
		  }
	      }
	  }
	else
	  {
	    STRACE_ERROR(("TSP_read_sample failed"));
	  }
      } while(new_sample);

      /* Used to give time to other thread for filling fifo of received samples */
      tsp_usleep(TSP_NANOSLEEP_PERIOD_US); 

    }

  return 0;
}
