/*!  \file 

$Id: main_stub.c,v 1.2 2005-10-23 13:15:23 erk Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Simple Black Board Stub, with S/C simulation & Real Time

-----------------------------------------------------------------------
*/


/* POSIX.1 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

/* POSIX.4 */
#include <signal.h>

#include <libgen.h>  /*basename */

#include <bb_core.h>
#include <bb_simple.h>

/* Projet */
#include "histogram.h"
#include "tsp_provider_init.h"
#include "tsp_datapool.h"
#include "tsp_sys_headers.h"
#include <tsp_glu.h>
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "calc_func.h"
#include "rtc_util.h"
#include "bb_vars.h"



/* TSP glue server defines */
#define TSP_STUB_FREQ 128 /*Hz, must be a 2 power for RTC*/
#define TSP_PERIOD_US (1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS 100
/* Glue server ringbuf size */



// My Globals : Bouh
int stopit=0;
int rt_mode=1; /* 0 = NRT, 1  RT mode */
int it_mode=0; /* 0 = Polling on date, 1=IT , 2=IT blocking IO */
unsigned long affinity_mask=0x1; /* Run on proc #0 */


void usage(char *name)
{
  printf ("USAGE: %s [-r rtMode(0|1) ]  [-i itMode(0|1|2) ] [-m affinityMask ]\n", name);
  exit (-1);
}


void signalled(int sig)
{
  stopit = 1;
}



int main(int argc, char *argv[])
{
  int my_time=0;
  unsigned long  last_us, now_us, t0_us, jitter, jitter_max=0;
  long delay;
  double retard, simtime, rtctime;

  S_BB_T	*mybb;
  mybbdata_t    *data;

  printf ("#============================================================#\n");
  printf ("# Launching <LinuxRTServer> for %d Symbols at %dHz #\n", GLU_MAX_SYMBOLS, TSP_STUB_FREQ);
  printf ("# RTmode=%d	Mask=0x%lX \n", rt_mode, affinity_mask);
  printf ("#============================================================#\n");

  /* Open the logical device */
  printf ("=====================================\n");
  rtc_init();
  tsp_histo_init();
  tsp_histo_set_delta (10); /* Jitter of 10us*/
  if (rt_mode) {
      rt_memory_lock(0);
    }
  
  /* Can use kill -2 for stopping prog */
  signal(SIGINT, signalled);
  
  /* Creation BB 1000 data of 1KO => 1Mo*/
  /***************/
  if (E_NOK==bb_create(&mybb,basename(argv[0]),1000,1000)) {
    bb_attach(&mybb,basename(argv[0]));
    /*     bb_destroy(&mybb); */
    /*     bb_create(&mybb,argv[0],n_data,data_size); */
  }
  data = bb_data_allocate();
  bb_data_publish(mybb,data);
  bb_data_propagate(mybb,data);

  printf (" GLU_thread :Last Time in NO RT mode !!!!\n");

  /* Must synchronise to first IT then RAZ  handler */
  rtc_wait_next_it();
  last_us = now_us = rtc_read_time();
  rtc_wait_next_it();
  t0_us = rtc_read_time();

  /* infinite loop for symbols generation */
  stopit = 0;
  while(!stopit)
    {
      delay = 0;

      /* Wait to a new Period of time */
      switch (it_mode) 
	{
	case 0: /* Polling like a beast on time */
	  while (delay < TSP_PERIOD_US)
	    {
	      now_us = rtc_read_time();
	      delay =  (now_us - last_us); /* in us  on 64 bits */
	    }
	  break;
	case 1: /* Blocking / Non blocking IT before read */
	  rtc_wait_next_it();
	  now_us = rtc_read_time();
	  delay =  (now_us - last_us); /* in us  on 64 bits */
	  if (delay<0 || delay >= 1.5* TSP_PERIOD_US) printf ("Warning : delay=%ld, now=%lu, last=%lu\n", delay, now_us, last_us);
	  break;
	default: break;
	}
	  

      /* We got a new period, must calculate the data */
      my_time++;    
      last_us = now_us;
      jitter =  delay-TSP_PERIOD_US; 
      simtime = (double)(my_time) / (double)(TSP_STUB_FREQ);
      rtctime =  ((now_us-t0_us)/1e6);
      retard = rtctime - simtime;
      if (jitter>jitter_max) jitter_max = jitter;
      tsp_histo_enter_with_date(jitter, rtctime);

      *data->bench.simtime	= simtime; /* s */
      *data->bench.rtime	= rtctime; /* s */
      *data->bench.delay	= (double)delay/1e3; /* ms */
      *data->bench.jitter	= jitter; /* s */
      *data->bench.jitter_max	= jitter_max; /* s */
      *data->bench.retard	= retard; /* ms */
 
    }

  /* Exit the program  */
  printf ("Exiting from GLU_Thread loop because stopit=%d\n", stopit);

	    
  tsp_histo_dump(stdout , "LOOP on RTC");
  /* Destruction BB */
  /******************/
  bb_destroy(&mybb);
 
  printf ("simtime\t= %gs\n", simtime);
  printf ("rtctime\t= %gs\n", rtctime);
  printf ("retard\t= %gs\n", retard);
  printf ("delay\t= %gms\n", (double)delay/1e3);
  printf ("max jit\t= %gms\n", jitter_max/1e3);

  return 0;
}
