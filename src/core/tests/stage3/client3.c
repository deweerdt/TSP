#include "tsp_sys_headers.h"
#include <math.h>

#include "tsp_prjcfg.h"
#include "tsp_consumer.h"
#include "tsp_time.h"
#include "libUTIL.h"


/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (200*1000)

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 200000

/* libUTIL */
extern _use_dbl;


int main(int argc, char *argv[]){

  SFUNC_NAME(main);

  TSP_sample_symbol_info_list_t*  symbols;
  TSP_answer_sample_t* ans_sample;
  TSP_request_sample_t req_sample;

  int i, j, count=0;
  int nb_providers;
  int period=0;
  char* name;
  int count_samples = 0;
  char symbol_buf[50];
  int test_ok = TRUE;
  int test_mode;
  int count_no_new_sample = 0;
  void* res_values;
  int new_sample;
  TSP_sample_t sample;
  char* out_file_res;
  char* in_file_res;
  int res_value_i;
  
  int all_data_ok = TRUE;

      


  TSP_provider_t* providers;

  int requested_nb;
  int group_nb;
 




  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

    /* TSP Init */

  if (argc>6)
    {   
      name = argv[1];
      period = atoi (argv[2]);
      in_file_res = argv[3];
      if(!strcmp("_", in_file_res))
	{
	  in_file_res = 0;
	 
	}
     
      out_file_res = argv[4];
      if(!strcmp("f", argv[5]))
	{
	   _use_dbl = 0;
	}
      else if(!strcmp("d", argv[5]))
	{
	   _use_dbl = 1;
	}
      else
	{
	  STRACE_ERROR(("param 5 must f or d for float of double"));
	  return -1;
	}
      test_mode = atoi(argv[6]);

    }
  else
    {
      STRACE_ERROR(("USAGE : %s server period   (in_file.res|_)  out_file.res (f|d) (1|2|3) ", argv[0]));
      STRACE_ERROR(("Last arg is mode test number :"));
      STRACE_ERROR(("- 1 : All variables"));
      STRACE_ERROR(("- 2 : 3 variables (first, middle, last)"));
      STRACE_ERROR(("- 3 : 10 first variables"));

      return -1;
    }

  if(!TSP_consumer_init(&argc, &argv))
    return -1;
  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_open_all_provider(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const TSP_otsp_server_info_t* info_struct = TSP_get_provider_simple_info(providers[i]) ;
	  STRACE_INFO(("Server Nb %d, info = '%s'", i, info_struct->info));
	  
	}
    }
  else
    {
      STRACE_ERROR(("Unable to find any provider for host"));
      return -1;
    }




  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 002 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Le 1er provider existe puisqu'il y en a au moins 1 */

  if(!TSP_request_provider_open(providers[0], in_file_res))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      return -1;
    }



  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_request_provider_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      return -1;
    }

  symbols = TSP_get_provider_information(providers[0]);


  for( i = 0 ; i< symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      STRACE_INFO(("Id=%d Sym='%s'",i, symbols->TSP_sample_symbol_info_list_t_val[i].name));
      
    }


  		       
  

  /* take only the first, midle and last variable in 'first_last' mode*/
  switch(test_mode)
    {
    case 1 : 
      /* all variables */
      break;
    case 2 :
      symbols->TSP_sample_symbol_info_list_t_val[1] = symbols->TSP_sample_symbol_info_list_t_val[ symbols->TSP_sample_symbol_info_list_t_len/2];
      symbols->TSP_sample_symbol_info_list_t_val[2] = symbols->TSP_sample_symbol_info_list_t_val[ symbols->TSP_sample_symbol_info_list_t_len-1];
      symbols->TSP_sample_symbol_info_list_t_len = 3;
    break;
    case 3 :     
      symbols->TSP_sample_symbol_info_list_t_len = 10;
      break;
    default:
      STRACE_ERROR(("Unknown test number"));
      return -1;
    }

 
  


  /* Change period of sampling for each client */
  for(i = 0 ; i < symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      symbols->TSP_sample_symbol_info_list_t_val[i].period = period;
    }

  req_sample.symbols = (*symbols);  
/*-------------------------------------------------------------------------------------------------------*/ 
/* TEST : STAGE 001 | STEP 004 */
/*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_request_provider_sample(&req_sample, providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      return -1;
    }


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 005 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_request_provider_sample_init(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_sample_init failed"));
      return -1;
    }
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 006 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Loop on data read */

  
  STRACE_INFO(("file=%s", out_file_res));
  d_wopen(out_file_res);
  d_wcom("");
  for (i = 0; i < symbols->TSP_sample_symbol_info_list_t_len; i++)
    {
      d_wnam(symbols->TSP_sample_symbol_info_list_t_val[i].name, "");
      
    }

  res_values = _use_dbl ? 
    calloc(( symbols->TSP_sample_symbol_info_list_t_len+1),sizeof(double)) :
      calloc(( symbols->TSP_sample_symbol_info_list_t_len+1),sizeof(float)) ;
  assert(res_values);
  
  res_value_i = 0;
  while(TSP_read_sample(providers[0],&sample, &new_sample))
    {

      if(new_sample)
	{

	  double calc;

	  if(_use_dbl)
	    {
	      double* d_res_values = (double*)res_values;
	      d_res_values[res_value_i++] = sample.user_value;
	    }
	  else
	    {
	      float* f_res_values = (float*)res_values;
	      f_res_values[res_value_i++] = sample.user_value;	      
	    }

	  if( res_value_i == symbols->TSP_sample_symbol_info_list_t_len )
	    {
	      d_writ(res_values);
	      res_value_i = 0;

	    }
	}
      else
	{
	  tsp_usleep(TSP_NANOSLEEP_PERIOD_US); 

	 
	}
    }

  d_clos();

  TSP_consumer_end();

  return 0;

}
