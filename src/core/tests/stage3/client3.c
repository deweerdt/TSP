#include "tsp_sys_headers.h"
#include <math.h>
#include <signal.h>

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

typedef void Sigfunc(int);

static old_sigfunc;

static stop = FALSE;
static stop_end = FALSE;

static int count = 0;

static Sigfunc* signal(int signo, Sigfunc* func)
{
  struct sigaction act, oact;
  
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if(signo == SIGALRM )
    {
#ifdef SA_INTERRUPT
      act.sa_flags |= SA_INTERRUPT; /*SunOS 4;x */
#endif
    }
  else
    {
#ifdef SA_RESTART
      act.sa_flags |= SA_RESTART; /*SVR4, 4.4BSD*/
#endif
    }
  if(sigaction(signo, &act, &oact) < 0)
    return(SIG_ERR);
  return (oact.sa_handler);
} 

void catch_ctrl_c(int i)
{
  SFUNC_NAME(main);
  stop = TRUE;

  STRACE_TEST(("Waiting eol and saving file..."));
}

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
  int count_no_new_sample = 0;
  void* res_values;
  int new_sample;
  TSP_sample_t sample;
  char* out_file_res;
  int res_value_i;
  char* custom_argv[10];
  
  int all_data_ok = TRUE;

      


  TSP_provider_t* providers;

  int requested_nb;
  int group_nb;
 

  /* catch ctrl-c */
  signal(SIGINT, catch_ctrl_c);


  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

    /* TSP Init */
  if(!TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR(("TSP init failed"));
      return -1;
    }
    

  if (argc == 6)
    {   
      name = argv[1];
      period = atoi (argv[2]);
     
      out_file_res = argv[3];
      if(!strcmp("f", argv[4]))
	{
	   _use_dbl = 0;
	}
      else if(!strcmp("d", argv[4]))
	{
	   _use_dbl = 1;
	}
      else
	{
	  STRACE_ERROR(("param 5 must f or d for float of double"));
	  return -1;
	}
      test_mode = atoi(argv[5]);

    }
  else
    {
      STRACE_ERROR(("USAGE : %s server period  out_file.res (f|d) (1|2|3) [ --tsp-stream-init-start file[.res] --tsp-stream-init-stop ]", argv[0]));
      STRACE_ERROR(("Last arg is mode test number :"));
      STRACE_ERROR(("- 1 : All variables"));
      STRACE_ERROR(("- 2 : 3 variables (first, middle, last)"));
      STRACE_ERROR(("- 3 : 10 first variables"));
      STRACE_ERROR(("Note : CTRL+C cleanly save res file and quit"));

      return -1;
    }

  
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_consumer_open_all(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const TSP_otsp_server_info_t* info_struct = TSP_consumer_get_server_info(providers[i]) ;
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


  if(!TSP_consumer_request_open(providers[0], 0, 0 ))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
	  return -1;
    }
  


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_information(providers[0]))
    {
      STRACE_ERROR(("TSP_request_provider_information failed"));
      return -1;
    }

  information = TSP_consumer_get_information(providers[0]);

  symbols.val = (TSP_consumer_symbol_requested_t*)calloc(information->symbols.len, sizeof(TSP_consumer_symbol_requested_t));
  TSP_CHECK_ALLOC(symbols.val, -1);
  symbols.len = information->symbols.len;  

  for( i = 0 ; i< information->symbols.len ; i++)
    {
      STRACE_INFO(("Id=%d Sym='%s'",i, information->symbols.val[i].name));
      symbols.val[i].index = information->symbols.val[i].index;;
      symbols.val[i].period = period;
      symbols.val[i].phase = 0;
    }
  
  
      

  /* take only the first, midle and last variable in 'first_last' mode*/
  switch(test_mode)
    {
    case 1 : 
      /* all variables */
      break;
    case 2 :
      symbols.val[1] = symbols.val[ symbols.len/2];
      symbols.val[2] = symbols.val[ symbols.len-1];
      symbols.len = 3;
    break;
    case 3 :     
      symbols.len = 10;
      break;
    default:
      STRACE_ERROR(("Unknown test number"));
      return -1;
    }

 
  
/*-------------------------------------------------------------------------------------------------------*/ 
/* TEST : STAGE 001 | STEP 004 */
/*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample(providers[0],&symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      return -1;
    }


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 005 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample_init(providers[0]))
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
  for (i = 0; i < symbols.len; i++)
    {
      d_wnam(information->symbols.val[symbols.val[i].index].name, "");
      
    }

  res_values = _use_dbl ? 
    calloc(( symbols.len+1),sizeof(double)) :
      calloc(( symbols.len+1),sizeof(float)) ;
  assert(res_values);
  
  res_value_i = 0;
  while(TSP_consumer_read_sample(providers[0],&sample, &new_sample) && !stop_end )
    {

      if(new_sample)
	{

	  double calc;

	  /* overide time */
	  /*if(  0 == res_value_i ) sample.user_value = ((double)count)/40.0;*/

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

	  if( res_value_i == symbols.len )
	    {
	      count++;
	      d_writ(res_values);
	      STRACE_INFO(("Write %f", *(float*)res_values));
	      res_value_i = 0;
	      if(stop)
		{
		  stop_end = TRUE;
		}


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
