/*

$Id: client_res.c,v 1.15 2006-04-07 10:37:17 morvan Exp $

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

Purpose   : Simple consummer for testing groups configuration

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"
#include <signal.h>
#include <unistd.h>
#include "tsp_prjcfg.h" 
#include "tsp_time.h" 
#include "tsp_consumer.h"
#include "libUTIL.h"


/*µS*/
#define TSP_NANOSLEEP_PERIOD_US (200*1000)


/* libUTIL */
extern int _use_dbl;

typedef void Sigfunc(int);

static int stop = FALSE;
static int stop_end = FALSE;

static Sigfunc* _signal(int signo, Sigfunc* func)
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
  stop = TRUE;

  STRACE_TEST(("Waiting eol and saving file..."));
}

void usage (char *txt)
{
  STRACE_ERROR(("USAGE : %s -f: [-u:] [-tmdh]", txt));
  printf("\t -f filename   : output RES format filename\n");
  printf("\t[-u serverURL] : TSP Universal Resource Locator\n\n");
  printf(TSP_URL_FORMAT_USAGE);
  printf("\n\t[-t period]    : expressed in provider's cycles (1-N), default 1\n");
  printf("\t[-m mode]      : recording mode (1-3), default 1\n");
  printf("\t                 1 = All variables, retry connection forever\n");
  printf("\t                 2 = 3 variables (first, middle, last)\n");
  printf("\t                 3 = 10 first variables\n");
  printf("\t[-d]           : use IEEE-754 double format for RES file\n");
  printf("\t[-h]           : this help\n");
  printf("\t[--tsp-stream-init-start file[.res] --tsp-stream-init-stop]\n");
  printf("\t               : stream sent to TSP provider\n");
  
  printf("Note : CTRL+C cleanly save RES file and quit\n");
  exit(-1);
}

int main(int argc, char *argv[]){

  const TSP_answer_sample_t*  information;
  TSP_sample_symbol_info_list_t symbols[TSP_MAX_SERVER_NUMBER];

  int i, count=0;
  int nb_providers = 0;
  void* res_values;
  int new_sample;
  int provider;
  int base_frequency = 1e6;
  TSP_sample_t sample;
  int res_value_i, res_values_nb;
  TSP_provider_t providers[TSP_MAX_SERVER_NUMBER];
  int buffersBeforeStop = 0;

  char myopt; /* Options */
  char* out_file_res = NULL;
  int period=1;
  int test_mode = 1;

  extern char* optarg;

  _use_dbl = 0;


  /* catch ctrl-c */
  _signal(SIGINT, catch_ctrl_c);


  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

  /* TSP Init */
  if(!TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR(("TSP init failed"));
      return -1;
    }
    
  
  while ((myopt = getopt(argc, argv, "u:f:t:m:dh")) != -1)
    {
      switch(myopt)
	{
	case 'u':
	  /*-------------------------------------*/ 
	  /* Connection to providers, URL based */
	  /*-------------------------------------*/ 
	  if(nb_providers < TSP_MAX_SERVER_NUMBER &&
	     (providers[nb_providers] = TSP_consumer_connect_url(optarg)))
	    nb_providers++;
	  else
	    {
	      STRACE_ERROR(("Cannot connect to %s", optarg));
	    }
	  break;
	case 'f':   out_file_res = optarg;      break;
	case 't':   period = atoi(optarg);      break;
	case 'm':   test_mode = atoi(optarg);   break;
	case 'd':   _use_dbl = 1;               break;
	case 'h':   /* no break please, do as default */
	default :   usage(argv[0]);             break;
	}
    }

  if(!out_file_res)
    usage(argv[0]);

  if(nb_providers == 0)
    {
      providers[nb_providers++] = TSP_consumer_connect_url(NULL);
      if(!providers[0])
        {
          STRACE_ERROR(("Cannot connect to a TSP provider on local host"));
          usage(argv[0]);
	 }
    }

  for(provider=0; provider<nb_providers; provider++)
    {
      /*-------------------------*/ 
      /* Open requested provider */
      /*-------------------------*/ 
      if(!TSP_consumer_request_open(providers[provider], 0, 0 ))
	{
	  STRACE_ERROR(("TSP_request_provider_open failed"));
	  return -1;
	}
 

      /*-----------------------------*/ 
      /* TEST : STAGE 002 | STEP 003 */
      /*-----------------------------*/
      do
	{
	  if(!TSP_consumer_request_information(providers[provider]))
	    {
	      STRACE_ERROR(("TSP_request_provider_information failed"));
	      return -1;
	    }
	  
	  information = TSP_consumer_get_information(providers[provider]);
	  symbols[provider].TSP_sample_symbol_info_list_t_len = information->symbols.TSP_sample_symbol_info_list_t_len;
	  if(!symbols[provider].TSP_sample_symbol_info_list_t_len)
	    tsp_usleep(TSP_NANOSLEEP_PERIOD_US);
	}
      while(!symbols[provider].TSP_sample_symbol_info_list_t_len);
      
      symbols[provider].TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)calloc(symbols[provider].TSP_sample_symbol_info_list_t_len, sizeof(TSP_sample_symbol_info_t));
      TSP_CHECK_ALLOC(symbols[provider].TSP_sample_symbol_info_list_t_val, -1);
      
      for( i = 0 ; i< symbols[provider].TSP_sample_symbol_info_list_t_len ; i++)
	{
	  STRACE_INFO(("Id=%d Sym='%s'",i, information->symbols.TSP_sample_symbol_info_list_t_val[i].name));
	  symbols[provider].TSP_sample_symbol_info_list_t_val[i].name = information->symbols.TSP_sample_symbol_info_list_t_val[i].name;
	  symbols[provider].TSP_sample_symbol_info_list_t_val[i].period = period;
	  symbols[provider].TSP_sample_symbol_info_list_t_val[i].phase = 0;
	}
      
      /* chose smallest frequency */
      if(information->base_frequency < base_frequency)
	base_frequency = information->base_frequency;

      
      /* take only the first, midle and last variable in 'first_last' mode*/
      switch(test_mode)
	{
	case 1 : 
	  /* all variables */
	  break;
	case 2 :
	  symbols[provider].TSP_sample_symbol_info_list_t_val[1] = symbols[provider].TSP_sample_symbol_info_list_t_val[ symbols[provider].TSP_sample_symbol_info_list_t_len/2];
	  symbols[provider].TSP_sample_symbol_info_list_t_val[2] = symbols[provider].TSP_sample_symbol_info_list_t_val[ symbols[provider].TSP_sample_symbol_info_list_t_len-1];
	  symbols[provider].TSP_sample_symbol_info_list_t_len = 3;
	  break;
	case 3 :     
	  symbols[provider].TSP_sample_symbol_info_list_t_len = 10;
	  break;
	default:
	  STRACE_ERROR(("Unknown test number"));
	  return -1;
	}

     
      /*-----------------------------------------------*/ 
      /* Adjust period according to smallest frequency */
      /*-----------------------------------------------*/ 
      /* TODO */
       
  
      /*---------------------*/ 
      /* Ask for sample list */
      /*---------------------*/ 
      if(!TSP_consumer_request_sample(providers[provider],&symbols[provider]))
	{
	  STRACE_ERROR(("TSP_request_provider_sample failed"));
	  return -1;
	}
    }

  /* in case of stop request, flush N buffers (if any) before stopping */
  buffersBeforeStop = base_frequency * 1 /* seconds */;


  for(provider=0; provider<nb_providers; provider++)
    {
      /*----------------*/ 
      /* Start sampling */
      /*----------------*/ 
      if(!TSP_consumer_request_sample_init(providers[provider],0,0))
	{
	  STRACE_ERROR(("TSP_request_provider_sample_init failed"));
	  return -1;
	}
    }

  
  /*-------------------*/ 
  /* Loop on data read */
  /*-------------------*/ 
  
  STRACE_INFO(("file=%s", out_file_res));
  d_wopen(out_file_res);
  d_wcom(""); /* No comment */

  res_values_nb = 0;
  for(provider=0; provider<nb_providers; provider++)
    {
      for (i = 0; i < symbols[provider].TSP_sample_symbol_info_list_t_len; i++)
	{
	  d_wnam(symbols[provider].TSP_sample_symbol_info_list_t_val[i].name, "?"); /* write header with no unit */
	}
      res_values_nb += symbols[provider].TSP_sample_symbol_info_list_t_len;
    }
  
  res_values = _use_dbl ? 
    calloc(( res_values_nb+1),sizeof(double)) :
      calloc(( res_values_nb+1),sizeof(float)) ;
  assert(res_values);
  
  res_value_i = 0;
  while(!stop_end)
    {
      for(provider=0; provider<nb_providers; provider++)
	{
	  if(!TSP_consumer_read_sample(providers[provider], &sample, &new_sample))
	    {
	      stop_end = TRUE;
	      break;
	    }
	  if(new_sample)
	    {
	      if(_use_dbl)
		{
		  double* d_res_values = (double*)res_values;
		  d_res_values[res_value_i++] = sample.uvalue.double_value;
		}
	      else
		{
		  float* f_res_values = (float*)res_values;
		  f_res_values[res_value_i++] = sample.uvalue.double_value;	      
		}

	      /* Received complete buffer, need to write */
	      if( res_value_i == res_values_nb )
		{
		  count++;
		  d_writ(res_values);
		  res_value_i = 0;

		  /* wait a little before stopping to flush buffers */
		  if(stop) stop++;
		  if(stop > buffersBeforeStop) stop_end = TRUE;
		}
	    }
	  else
	    {
	      tsp_usleep(TSP_NANOSLEEP_PERIOD_US);
	      /* no more buffers, stop immediately */
	      if(stop) stop_end = TRUE;
	    }
	}
    }

  d_clos();

  TSP_consumer_end();

  return 0;
}
