/*

$Id: client_stdout.c,v 1.14 2006-10-18 21:22:34 erk Exp $

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

Purpose   : Simple consumer test that print samples received to stdout

-----------------------------------------------------------------------
*/

#ifdef WIN32
    #define assert(exp)     ((void)0)
    #include  "getopt.h"
#else
    #include <unistd.h>
    #include <assert.h>
#endif

#include <tsp_sys_headers.h>
#include <tsp_prjcfg.h>
#include <tsp_consumer.h>
#include <tsp_time.h>

/**
 * @defgroup TSP_Stdout Stdout Client Consumer
 * A Simple TSP consumer printing on stdout.
 * a TSP consumer which is able to output symbols values on standard output.
 * You select the first 's' symbol(s) of the provider, the period of
 * the requested samples and the number of sample to receive.
 *
 * \par tsp_stdout_client [-h] [-u TSP_URL] [-p period] [-n number_sample] [-s number_symbols] [-t]
 * \par 
 * <ul>
 *   <li> \b -h  (optional) print command line help.</li>
 *   <li> \b -u  (optional) the TSP provider URL. If not specified default is localhost.</li>
 *   <li> \b -p  (optional) the period used for the request sample, default=1.</li>
 *   <li> \b -n  (optional) the number of sample to receive, default=5, 0=infinite loop</li>
 *   <li> \b -s  (optional) the number of sample symbols to request for, default=1</li>
 *   <li> \b -t  (optional) test mode to be used with StubbedServer (TSP developer only)</li>
 * </ul>
 * @ingroup TSP_Consumers
 */

/* Number of samples  that will be counted before the data check test pass */
#define TSP_TEST_COUNT_SAMPLES 10  
#define TSP_NANOSLEEP_PERIOD_US (100*1000) /* 10Hz */


void 
stdout_usage(const char* me) {
  printf("%s is a (very) simple TSP consumer which may send simple TSP Request Sample.\n",me);
  printf("With no args %s will connect to the first provider on 'localhost'",me); 
  printf("and print out the first 5 values of the first symbols.\n");
 
  printf("Usage: %s [-u TSP_URL] [-p period] [-n number_sample] [-s number_symbols] [-t]\n",me);
  printf("   -u TSP_URL    (optional) the TSP provider URL, default=localhost\n");
  printf("   -p period     (optional) the period used for the request sample, default=1\n");
  printf("   -n nb_sample  (optional) the number of sample to receive, default=5, 0=infinite loop\n");
  printf("   -s nb_symbols (optional) the number of sample symbols to request for, default=1\n");
  printf("   -t            (optional) test mode to be used with StubbedServer (TSP developer only)\n");
  printf("   -h            (optional) print this help\n");
  printf("%s",TSP_URL_FORMAT_USAGE);  
}


int 
main(int argc, char *argv[]){

  const TSP_answer_sample_t*  information;
  TSP_sample_symbol_info_list_t symbols;

  int   i, j;
  int   retcode=0;
  int   period=1;
  char* provider_url;
  int   count_samples;
  int   nb_samples;
  int   nb_symbols;
  char  symbol_buf[50];
  int   test_ok = TRUE;
  int   test_mode = 0;
  int   all_data_ok = TRUE;
  TSP_provider_t provider;
  int           opt_ok=1;
  char          c_opt;

  /* Setup default values for non-existent option */
  provider_url = strdup("localhost");
  period       = 1;
  nb_samples   = 5;
  nb_symbols   = 1;
  test_mode    = 0;

  printf ("#=========================================================#\n");
  printf ("# Launching <stdout_client> for printing symbols received #\n");
  printf ("#=========================================================#\n");

  /* Analyse command line parameters */
  c_opt = getopt(argc,argv,"u:p:n:s:ht");

  if(opt_ok && EOF != c_opt) {
    opt_ok  = 1;
    do { 
      switch (c_opt) {
      case 'u':
	free(provider_url);
	provider_url = strdup(optarg);
	break;
      case 'p':
	period = atoi(optarg);
	/* period should be > 0 */
	if (period <=0) {
	  period = 1;
	}
	break;
      case 'n':
	nb_samples = atoi(optarg);
	/* nb_samples should be >= 0, 0 meaning infinite loop */
	if (nb_samples <0) {
	  nb_samples = 5;
	}
	break;
      case 's':	
	nb_symbols = atoi(optarg);
	/* nb_symbols should be >= 0, 0 meaning infinite loop */
	if (nb_symbols <=0) {
	  nb_symbols = 1;
	}
	break;
      case 't':
	test_mode = 1;
	break;
      case 'h':
	opt_ok = 0;
	break;
      case '?':
	fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
	opt_ok = 0;
	break;
      default:
	opt_ok = 0;
	break;
      } /* end of switch */  
      c_opt = getopt(argc,argv,"u:p:n:s:ht");  
    }
    while (opt_ok && (EOF != c_opt));
  }

  if (!opt_ok) {
    retcode=-1;
    stdout_usage(argv[0]);
    exit(retcode);
  } else {
    fprintf(stdout,"%s: Using provider URL <%s>\n",argv[0],provider_url);
    fprintf(stdout,"%s: Asking for:\n",argv[0]);
    fprintf(stdout,"         <%d> samples (0 => INFINITE loop)\n",nb_samples);
    fprintf(stdout,"      of <%d> TSP symbols\n",nb_symbols);
    fprintf(stdout,"      at period <%d>.\n",period);
    if (0==nb_samples) {
      fprintf(stdout,"%s: INFINITE LOOP mode (Ctrl-C for stopping sample)\n",argv[0]);
    }
    if (test_mode) {
      fprintf(stdout,"%s: TEST MODE (should only be used with Stubbed Server)\n",argv[0]);
    }
  }
  
  if(TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv)) {
    retcode=1;
    STRACE_ERROR(("TSP init failed"));
    return retcode;
  }

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 001 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  provider = TSP_consumer_connect_url(provider_url);
  if (provider) {
    const char* info = TSP_consumer_get_connected_name(provider) ;
    STRACE_INFO(("Server %s, info = '%s'", provider_url, info));
    
    /* Check name */
    if(test_mode && !strstr(info, "StubbedServer")) {
      STRACE_ERROR(("Serveur name corrupted"));
      retcode=2;
      STRACE_TEST(("STAGE 001 | STEP 001 : FAILED"));
      return retcode;
    }	  
  }
  else {
    retcode = 3;
    STRACE_ERROR(("Unable to find any provider for host"));
    return retcode;
  }
  
  STRACE_TEST(("STAGE 001 | STEP 001 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 002 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* Request Open on the [found] provider */

  if(TSP_STATUS_OK!=TSP_consumer_request_open(provider, 0, NULL)) {
    STRACE_ERROR(("TSP_request_provider_open failed"));
    STRACE_TEST(("STAGE 001 | STEP 002 : FAILED"));
    return -1;
  }
  
  STRACE_TEST(("STAGE 001 | STEP 002 : PASSED"));

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 003 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_information(provider)) {
    STRACE_ERROR(("TSP_request_provider_information failed"));
    STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
    return -1;
  }
  
  /* be wild, ask twice to try to trigger mem leak  */
  if(TSP_STATUS_OK!=TSP_consumer_request_information(provider)) {
    STRACE_ERROR(("TSP_request_provider_information failed"));
    STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
    return -1;
  }

  information = TSP_consumer_get_information(provider);
  /* Check total symbol number */
  if ((TSP_SSIList_getSize(information->symbols) < 1 ) ||
      (TSP_SSIList_getSize(information->symbols) > 999999)
      ) {
    STRACE_ERROR(("The total number of symbols should be fair, and not %d",TSP_SSIList_getSize(information->symbols)));
    STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
    return -1;      
  }

  if (test_mode) {
    /* Compare (first 1000) symbols names */
    for (i = 1 ; i <1000 ; ++i) {
      sprintf(symbol_buf, "Symbol%d",i);
      if(strcmp(symbol_buf, TSP_SSIList_getSSI(information->symbols,i)->name)) {
	printf("%s != %s\n", symbol_buf, TSP_SSIList_getSSI(information->symbols,i)->name);
	STRACE_ERROR(("Symbol name corrupted"));
	STRACE_TEST(("STAGE 001 | STEP 003 : FAILED"));
	return -1;
      }
    }
  }
      
  STRACE_TEST(("STAGE 001 | STEP 003 : PASSED"));
  
  /* Initialize symbols list */
  TSP_SSIList_initialize(&symbols,nb_symbols);

  /* Update requested sample period */
  printf("%s: Asking for %d symbols\n", argv[0],nb_symbols);
  for (i = 0 ; i < TSP_SSIList_getSize(symbols) ; ++i) {
    TSP_SSI_initialize_request_minimal(TSP_SSIList_getSSI(symbols,i),
				       TSP_SSIList_getSSI(information->symbols,i)->name,
				       period);
    printf("   symbol <%d> is <%s>\n",i,TSP_SSIList_getSSI(symbols,i)->name);
  }

  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 004 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(provider, &symbols)) {
    STRACE_ERROR(("TSP_request_provider_sample failed"));
    STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
    return -1;
  }
  
  /* be wild, ask twice to try to trigger mem leak  */
  if(TSP_STATUS_OK!=TSP_consumer_request_sample(provider, &symbols)) {
    STRACE_ERROR(("TSP_request_provider_sample failed"));
    STRACE_TEST(("STAGE 001 | STEP 004 : FAILED"));
    return -1;
  }

  STRACE_TEST(("STAGE 001 | STEP 004 : PASSED"));
  /*-------------------------------------------------------------------------------------------------------*/ 
  /* TEST : STAGE 001 | STEP 005 */
  /*-------------------------------------------------------------------------------------------------------*/ 
  if(TSP_STATUS_OK!=TSP_consumer_request_sample_init(provider, 0, 0)) {
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
  while(1) {
    int new_sample;
    TSP_sample_t sample;
    int pgi;
    int j;
    int t;
      
    new_sample = FALSE;
    /* Loop on read sample */
    do {
      if (TSP_STATUS_OK==TSP_consumer_read_sample(provider,&sample, &new_sample)) {
	
	if(new_sample) {	    

	  /* If we get first PGI then it must be a new sample set (provided first sample is not an array) */
	  if ((TSP_SSIList_getSSI(symbols,0)->provider_global_index==sample.provider_global_index) && 
	      (0==sample.array_index)
	      ) {
	    count_samples++;
	    if ((count_samples <= nb_samples) || (0==nb_samples)) {
	      printf("\nNew Sample Set nb[%d] time=%d %s=%f", count_samples, sample.time, 
		     TSP_SSIList_getSSI(symbols,sample.provider_global_index)->name,
		     sample.uvalue.double_value);
	    }
	  } else {
	    printf(" %s=%f",TSP_SSIList_getSSI(symbols,sample.provider_global_index)->name,sample.uvalue.double_value);
	  }
	  	    
	  /* We know how stubbed server symbols values are computed so verify the values */
	  if (test_mode) {
	    double calc = calc_func(sample.provider_global_index,sample.time);
	    
	    /* pgi == 0 is t so don't check for t*/
	    if(sample.provider_global_index != 0) {
	      if( (ABS(sample.uvalue.double_value - calc) > 1e-7) && (t == (sample.time - 1)) ) {
		STRACE_ERROR(("!!!!ERROR : T=%u, I=%d, V1=%f, V2=%f", sample.time,i,sample.uvalue.double_value,calc ));
		all_data_ok = FALSE;
	      }
	    }	  
	    t = sample.time;
	  } /* end if (test_mode) */
	  
	  /* End up sampling properly */
	  if( (0!=nb_samples) && 
	      (count_samples > nb_samples)
	      ) {
	    if(all_data_ok) {
	      
	      printf("\n");
	      STRACE_TEST(("STAGE 001 | STEP 006 : PASSED" ));			
	      
	      /* --------------- */
	      /* Close providers */
	      /* --------------- */
	      if(TSP_STATUS_OK!=TSP_consumer_request_sample_destroy(provider)) {
		STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed" ));	 
	      }
	      TSP_SSIList_finalize(&symbols);
		  
	      if(TSP_STATUS_OK!=TSP_consumer_request_close(provider)) {
		STRACE_ERROR(("Function TSP_consumer_request_close failed" ));			    
	      }
		  
	      TSP_consumer_disconnect_one(provider);	      
	      TSP_consumer_end();
	      
	      printf ("#=========================================================#\n");
	      printf ("# End of Test OK \n");
	      printf ("#=========================================================#\n");
	      return 0;
	    }
	    else {
	      printf ("#=========================================================#\n");
	      printf ("# End of Test KO \n");
	      printf ("#=========================================================#\n");
	      return -1;
	    }
	  }
	}
      }
      else {
	STRACE_ERROR(("TSP_read_sample failed"));
      }
    } while(new_sample);
    
    /* Used to give time to other thread for filling fifo of received samples */
    tsp_usleep(TSP_NANOSLEEP_PERIOD_US);     
  }
  
  return 0;
}
