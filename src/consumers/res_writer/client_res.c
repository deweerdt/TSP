/*!  \file 

$Id: client_res.c,v 1.7 2004-09-22 14:25:58 tractobob Exp $

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
  stop = TRUE;

  STRACE_TEST(("Waiting eol and saving file..."));
}

void usage (char *txt)
{
  STRACE_ERROR(("USAGE : %s [s:f:p:t:m:dh]", txt));
  printf("\t -s server    : TSP provider server name\n");
  printf("\t -f filename  : output RES format filename\n");
  printf("\t[-p prov_num] : TSP provider number (0-N), default 0\n");
  printf("\t[-t period]   : expressed in provider's cycles (1-N), default 1\n");
  printf("\t[-m mode]     : recording mode (1-3), default 1\n");
  printf("\t                1 = All variables, retry connection forever\n");
  printf("\t                2 = 3 variables (first, middle, last)\n");
  printf("\t                3 = 10 first variables\n");
  printf("\t[-d]          : use IEEE-754 double format for RES file\n");
  printf("\t[-h]          : this help\n");
  printf("\t[--tsp-stream-init-start file[.res] --tsp-stream-init-stop]\n");
  printf("\t              : stream sent to TSP provider\n");
  
  printf("Note : CTRL+C cleanly save RES file and quit\n");
  exit(-1);
}

int main(int argc, char *argv[]){

  const TSP_consumer_information_t*  information;
  TSP_consumer_symbol_requested_list_t symbols;

  int i, j, count=0;
  int nb_providers;
  int count_samples = 0;
  char symbol_buf[50];
  int test_ok = TRUE;
  int count_no_new_sample = 0;
  void* res_values;
  int new_sample;
  TSP_sample_t sample;
  int res_value_i;
  char* custom_argv[10];
  int all_data_ok = TRUE;
  TSP_provider_t* providers;
  int requested_nb;
  int group_nb;
  int buffersBeforeStop;

  char myopt; /* Options */
  char* name = NULL;
  char* out_file_res = NULL;
  int provider=0;
  int period=1;
  int test_mode = 1;

  _use_dbl = 0;


  /* catch ctrl-c */
  signal(SIGINT, catch_ctrl_c);


  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

  /* TSP Init */
  if(!TSP_consumer_init(&argc, &argv))
    {
      STRACE_ERROR(("TSP init failed"));
      return -1;
    }
    
  
  while ((myopt = getopt(argc, argv, "s:f:p:t:m:dh")) != -1)
    {
      switch(myopt)
	{
	case 's':   name = optarg;              break;
	case 'f':   out_file_res = optarg;      break;
	case 'p':   provider = atoi(optarg);    break;
	case 't':   period = atoi(optarg);      break;
	case 'm':   test_mode = atoi(optarg);   break;
	case 'd':   _use_dbl = 1;               break;
	case 'h':   /* no break please, do as default */
	default :   usage(argv[0]);             break;
	}
    }

  if(!name || !out_file_res)
    usage(argv[0]);

  
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

      if(provider < 0 || provider >= nb_providers)
	provider = 0;
    }
  else
    {
      STRACE_ERROR(("Unable to find any provider for host"));
      return -1;
    }




  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Open requested provider
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_open(providers[provider], 0, 0 ))
    {
      STRACE_ERROR(("TSP_request_provider_open failed"));
      return -1;
    }
  


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 002 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/
  do
    {
      if(!TSP_consumer_request_information(providers[provider]))
	{
	  STRACE_ERROR(("TSP_request_provider_information failed"));
	  return -1;
	}
      
      information = TSP_consumer_get_information(providers[provider]);
      symbols.len = information->symbols.len;
      if(!symbols.len)
	tsp_usleep(TSP_NANOSLEEP_PERIOD_US);
    }
  while(!symbols.len);

  symbols.val = (TSP_consumer_symbol_requested_t*)calloc(information->symbols.len, sizeof(TSP_consumer_symbol_requested_t));
  TSP_CHECK_ALLOC(symbols.val, -1);

  for( i = 0 ; i< information->symbols.len ; i++)
    {
      STRACE_INFO(("Id=%d Sym='%s'",i, information->symbols.val[i].name));
      symbols.val[i].name = information->symbols.val[i].name;;
      symbols.val[i].period = period;
      symbols.val[i].phase = 0;
    }

  /* in case of stop request, flush N buffers (if any) before stopping */
  buffersBeforeStop = information->base_frequency * 1 /* seconds */;

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
  if(!TSP_consumer_request_sample(providers[provider],&symbols))
    {
      STRACE_ERROR(("TSP_request_provider_sample failed"));
      return -1;
    }


  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Start sampling
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(!TSP_consumer_request_sample_init(providers[provider],0,0))
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
  while(TSP_consumer_read_sample(providers[provider],&sample, &new_sample) && !stop_end )
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

  d_clos();

  TSP_consumer_end();

  return 0;
}
