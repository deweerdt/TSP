/*!  \file 

$Id: client_res.c,v 1.5 2003-12-27 13:30:59 uid67973 Exp $

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

Purpose   : Simple consummer for testing groups configuration

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"
#include <signal.h>
#include "tsp_prjcfg.h" 
#include "tsp_consumer.h"
#include "libUTIL.h"


/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (200*1000)


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
  /* Connection to providers
  /*-------------------------------------------------------------------------------------------------------*/ 
  TSP_consumer_connect_all(name,&providers, &nb_providers);
  if(nb_providers > 0)
    {
      for( i = 0 ; i<nb_providers ; i++)
	{
	  const char* info = TSP_consumer_get_connected_name(providers[i]) ;
	  STRACE_INFO(("Server Nb %d, info = '%s'", i,info));
	}
    }
  else
    {
      STRACE_ERROR(("Unable to find any provider for host"));
      return -1;
    }




  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Open first provider
  /*-------------------------------------------------------------------------------------------------------*/ 
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
      symbols.val[i].name = information->symbols.val[i].name;;
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
  /* Ask for sample list
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample(providers[0],&symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      return -1;
    }


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Start sampling
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample_init(providers[0],0,0))
    {
      STRACE_ERROR(("TSP_request_provider_sample_init failed"));
      return -1;
    }

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Loop on data read */
  /*-------------------------------------------------------------------------------------------------------*/ 
  
  STRACE_INFO(("file=%s", out_file_res));
  d_wopen(out_file_res);
  d_wcom(""); /* No comment */

  for (i = 0; i < symbols.len; i++)
    {
      d_wnam(symbols.val[i].name, "?"); /* write header with no unit */
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

	  /* Received complete buffer, need to write */
	  if( res_value_i == symbols.len )
	    {
	      count++;
	      d_writ(res_values);
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
