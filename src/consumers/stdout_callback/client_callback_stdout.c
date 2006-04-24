/*

$Id: client_callback_stdout.c,v 1.5 2006-04-24 22:17:47 erk Exp $

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

Purpose   : Simple consummer test that print samples received to stdout
The callback function is used to harvest data

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_consumer.h"

#include "calc_func.h"

/*#include "fortify.h"*/

static int test_mode = 0;
static TSP_provider_t* providers = 0;
static test_is_finished = FALSE;
static int all_data_ok = TRUE;

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 200*1000
#define TSP_NANOSLEEP_PERIOD_US (100*1000) /*µS*/

#define USER_DATA "Hi ! I'm the user data that'll be used for the callback user data test ! "

void test_callback(TSP_sample_t* sample, void* user_data){

  static int i;int j;
  static int t = -1000;  
  static int count_samples = 0;

  double calc;

  i = sample->provider_global_index;
  if(t == -1000) 
    {
      t = sample->time - 1; 
    }
  
  count_samples++;
  if(i == 1 )
    {
      printf ("TSP : Sample nb[%d] time=%d val=%f\n", count_samples, sample->time, sample->uvalue.double_value);
    }
  
  calc = calc_func(i,sample->time);
  
  /* i = 0 is t */
  if(i != 0)
    {
      if((ABS(sample->uvalue.double_value - calc ) > 1e-7) || (0 != strncmp((char*)user_data, USER_DATA, strlen(USER_DATA))))
        {
          STRACE_ERROR(("!!!!ERROR : T=%u, I=%d, V1=%f, V2=%f", sample->time,i,sample->uvalue.double_value,calc ));			
          all_data_ok = FALSE;
        }
    }
  
  t = sample->time;
  
  /* Test for end */
  if(count_samples >= TSP_TEST_COUNT_SAMPLES && test_mode!=1)
    {
      test_is_finished = TRUE;
    }
  
}


int main(int argc, char *argv[]){

  const TSP_answer_sample_t*  information;
  TSP_sample_symbol_info_list_t symbols;

  int i, j, count=0;
  int nb_providers;
  int period=0;
  char* name;

  char symbol_buf[50];
  int test_ok = TRUE;


  /*Fortify_EnterScope();*/
  printf ("#===========================================================================#\n");
  printf ("# Launching <stdout_client> for printing symbols received (callback active) #\n");
  printf ("#===========================================================================#\n");
 
  if (TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv)) {
    STRACE_ERROR(("TSP init failed"));
    return -1;
  }

  if (argc>2)
    {   
      name = argv[1];
      period = atoi (argv[2]);
      /* Anything after name and perdio --> test mode */
      if (argc>3)
	test_mode = atoi(argv[3]);  /* 0 = no, 1=infinite loop */

    }
  else
    {
      STRACE_ERROR(("USAGE %s : server period <test_mode>\n", argv[0]));
      return -1;
    }

 once_again:
  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_consumer_connect_all(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const char* info = TSP_consumer_get_connected_name(providers[i]) ;
	  STRACE_INFO(("Server Nb %d, info = '%s'", i, info));
	  
	  /* Check name */
	  if(strcmp(info, "StubbedServer"))
	    {
	      STRACE_ERROR(("Serveur name corrupted"));
	      STRACE_TEST(("STAGE 001 | STEP 001 : FAILED"));
	      return -1;
	    }
	  
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

  if(TSP_STATUS_OK!=TSP_consumer_request_open(providers[0], 0, 0))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      STRACE_TEST(("STAGE 001 | STEP 002 : FAILED"));
      return -1;
    }
  
  STRACE_TEST(("STAGE 001 | STEP 002 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }
  
  /* be wild, ask twice to try to trigger mem leak  */
  if(TSP_STATUS_OK!=TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }


  information = TSP_consumer_get_information(providers[0]);

  /* Check total symbol number */
  if(1000 !=  information->symbols.TSP_sample_symbol_info_list_t_len )
    {
      STRACE_ERROR(("The total number of symbols should be 1000"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;      
    }

  /* Compare symbols names */
  for( i = 1 ; i<  information->symbols.TSP_sample_symbol_info_list_t_len ; i++)
    {
      sprintf(symbol_buf, "Symbol%d",i);
      if(strcmp(symbol_buf,  information->symbols.TSP_sample_symbol_info_list_t_val[i].name))
	{
	  STRACE_ERROR(("Symbol name corrupted"));
	  STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
	  return -1;
	}
      
    }
      
  STRACE_TEST(("STAGE 001 | STEP 003 : PASSED"));

  symbols.TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)calloc(information->symbols.TSP_sample_symbol_info_list_t_len, sizeof(TSP_sample_symbol_info_t));
  TSP_CHECK_ALLOC(symbols.TSP_sample_symbol_info_list_t_val, -1);
  symbols.TSP_sample_symbol_info_list_t_len = information->symbols.TSP_sample_symbol_info_list_t_len;
  /* Change period of sampling for each client */
  for(i = 0 ; i < information->symbols.TSP_sample_symbol_info_list_t_len ; i++)
    {
      symbols.TSP_sample_symbol_info_list_t_val[i].name = information->symbols.TSP_sample_symbol_info_list_t_val[i].name;
      symbols.TSP_sample_symbol_info_list_t_val[i].period = period;
      symbols.TSP_sample_symbol_info_list_t_val[i].phase = 0;
    }
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 004 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(providers[0], &symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
      return -1;
    }
  /* be wild, ask twice to try to trigger mem leak  */
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(providers[0], &symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
      return -1;
    }

  free(symbols.TSP_sample_symbol_info_list_t_val);

  STRACE_TEST(("STAGE 001 | STEP 004 : PASSED"));
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 005 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_sample_init(providers[0], test_callback, (void*)(USER_DATA) ))
    {
      STRACE_ERROR(("TSP_request_provider_sample_init failed"));
      STRACE_TEST(("STAGE 001 | STEP 005 : FAILED"));
      return -1;
    }
  STRACE_TEST(("STAGE 001 | STEP 005 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 006 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  while(1)
    {
      /* A 'condition' would have been cleaner to leave the test, but
         it is somewhat overkill for such test code */
      if(test_is_finished)
        {          
          if(all_data_ok)
            {
              
              STRACE_TEST(("STAGE 001 | STEP 006 : PASSED" ));			
              
              /* If test mode, the return code is the number of opened providers */
              /* --------------- */
              /* Close providers */
              /* --------------- */
              if(TSP_STATUS_OK!=TSP_consumer_request_sample_destroy(providers[0]))
                {
                  STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed" ));	 
                }
              
              if(TSP_STATUS_OK!=TSP_consumer_request_close(providers[0]))
                {
                  STRACE_ERROR(("Function TSP_consumer_request_close failed" ));			    
                }
              
              TSP_consumer_disconnect_all(providers);
              
              TSP_consumer_end();
              
              printf ("#=========================================================#\n");
              printf ("# End of Test OK \n");
              printf ("#=========================================================#\n");
              return nb_providers;
            }
          else
            {
              printf ("#=========================================================#\n");
              printf ("# End of Test KO \n");
              printf ("#=========================================================#\n");
              return -1;
            }

        }
      tsp_usleep(TSP_NANOSLEEP_PERIOD_US); 

    }

  return 0;
}
