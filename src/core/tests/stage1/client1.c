#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_consumer.h"
#include "tsp_time.h"
#include <math.h>
/*#include "fortify.h"*/

/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (100*1000)

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 800000


int main(int argc, char *argv[]){

  SFUNC_NAME(main);



  const TSP_consumer_information_t*  information;
  TSP_consumer_symbol_requested_list_t symbols;

  int i, j, count=0;
  int nb_providers;
  int period=0;
  char* name;
  int count_samples = 0;
  char symbol_buf[50];
  int test_ok = TRUE;


  int test_mode;

  TSP_provider_t* providers;


  /*Fortify_EnterScope();*/
 
  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

   if(!TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR(("TSP init failed"));
      return -1;
    }

  if (argc>2)
    {   
      name = argv[1];
      period = atoi (argv[2]);
      /* Anything after name and perdio --> test mode */
      test_mode = (argc>3) ? TRUE : FALSE;

    }
  else
    {
      STRACE_ERROR(("USAGE %s : server period\n", argv[0]));
      return -1;
    }

 once_again:
  
/*-------------------------------------------------------------------------------------------------------*/ 
/* TEST : STAGE 001 | STEP 001 */
/*-------------------------------------------------------------------------------------------------------*/ 
  TSP_consumer_open_all(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const char* info = TSP_consumer_get_server_info(providers[i]) ;
	  STRACE_INFO(("Server Nb %d, info = '%s'", i, info));
	  
	  /* Check name */
	  if(strcmp(info, "ServeurPetitScarabe"))
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

  if(!TSP_consumer_request_open(providers[0], 0, 0))
  {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      STRACE_TEST(("STAGE 001 | STEP 002 : FAILED"));
      return -1;
  }
  
  STRACE_TEST(("STAGE 001 | STEP 002 : PASSED"));

/*-------------------------------------------------------------------------------------------------------*/ 
/* TEST : STAGE 001 | STEP 003 */
/*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }
  
  /* be wild, ask twice to try to trigger mem leak  */
  if(!TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;
    }


  information = TSP_consumer_get_information(providers[0]);

  /* Check total symbol number */
  if(1000 !=  information->symbols.len )
    {
      STRACE_ERROR(("The total number of symbols should be 1000"));
      STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
      return -1;      
    }

  /* Compare symbols names */
  for( i = 1 ; i<  information->symbols.len ; i++)
    {
      sprintf(symbol_buf, "Symbol%d",i);
      if(strcmp(symbol_buf,  information->symbols.val[i].name))
	{
	  STRACE_ERROR(("Symbol name corrupted"));
	  STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
	  return -1;
	}
      
    }
      
      STRACE_TEST(("STAGE 001 | STEP 003 : PASSED"));

      symbols.val = (TSP_consumer_symbol_requested_t*)calloc(information->symbols.len, sizeof(TSP_consumer_symbol_requested_t));
      TSP_CHECK_ALLOC(symbols.val, -1);
      symbols.len = information->symbols.len;
  /* Change period of sampling for each client */
  for(i = 0 ; i < information->symbols.len ; i++)
    {
      symbols.val[i].name = information->symbols.val[i].name;
      symbols.val[i].period = period;
      symbols.val[i].phase = 0;
    }
/*-------------------------------------------------------------------------------------------------------*/ 
/* TEST : STAGE 001 | STEP 004 */
/*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample(providers[0], &symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
      return -1;
    }
    /* be wild, ask twice to try to trigger mem leak  */
  if(!TSP_consumer_request_sample(providers[0], &symbols))
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
  if(!TSP_consumer_request_sample_init(providers[0], 0))
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
      int all_data_ok = TRUE;
      int t = -1000;
      
      
      new_sample = FALSE;
      do{
	if(TSP_consumer_read_sample(providers[0],&sample, &new_sample))
	  {
	    if(new_sample)
	      {
		    
		double calc;
		    
		/*printf("T=%d V=%f\n", sample.time, sample.user_value); */
		/*if((i == 20) && (!(sample.time%100)))*/
		    
		i = sample.provider_global_index;
		if(t == -1000) { t = sample.time - 1; }

		count_samples++;
		if(i == 1 )
		  {
		    STRACE_INFO(("T=%d Va=%f", sample.time, sample.user_value));
		  }
		calc = sin((double)(sample.time + i)/300);
		
		/* i = 0 is t */
		if(i != 0)
		  {
		    if( (ABS(sample.user_value - calc) > 1e-7) && (t == (sample.time - 1)) )
		      {
			STRACE_ERROR(("!!!!ERROR : T=%u, I=%d, V1=%f, V2=%f",
				      sample.time,
				      i,
				      sample.user_value,calc ));			
			all_data_ok = FALSE;
		      }
		  }
		    t = sample.time;
		/* Test */
		if(count_samples == TSP_TEST_COUNT_SAMPLES)
		  {
		    if(all_data_ok)
		      {

			STRACE_TEST(("STAGE 001 | STEP 006 : PASSED" ));			
			if( test_mode ) 
			  {
			    /* If test mode, the return code is the number of opened providers */
			


			    /* --------------- */
			    /* Close providers */
			    /* --------------- */
			    if(!TSP_consumer_request_sample_destroy(providers[0]))
			      {
				STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed" ));			    
			      }
			    
			    if(!TSP_consumer_request_close(providers[0]))
			      {
				STRACE_ERROR(("Function TSP_consumer_request_close failed" ));			    
			      }
			    
			    TSP_consumer_close_all(providers);


			    /* Fortify calls */
			    /*Fortify_LeaveScope();
			    Fortify_OutputStatistics();*/
			    
			   /* goto once_again; */
			    TSP_consumer_end();
			    
			    return nb_providers;
			  }
		      }
		    else
		      {
			if (test_mode) return -1;
		      }
		  }
	      }
	  }
	else
	  {
	    STRACE_ERROR(("TSP_read_sample failed"));
	  }
      }while(new_sample);
        	
      tsp_usleep(TSP_NANOSLEEP_PERIOD_US); 

      STRACE_INFO(("Top"));

    }

  /*STRACE_INFO(("Nb groups = %d",TSP_group_algo_get_nb_groups(symbols)));*/

  return 0;
}
