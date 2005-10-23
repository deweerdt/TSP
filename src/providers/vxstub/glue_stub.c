/*!  \file 

$Id: glue_stub.c,v 1.5 2005-10-23 13:15:24 erk Exp $

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

Purpose   : Implementation for the glue_server, for stub test
	    Allow the output of a datapool of 1000 symbols, 
	    cyclically generated at 100Hz

-----------------------------------------------------------------------
*/

#include <string.h>
#include <stdio.h>
#include <assert.h>

#include "tsp_sys_headers.h"
#include <tsp_glu.h>
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "tsp_datapool.h"
#include "calc_func.h"

/* TSP glue server defines */
#define TSP_STUB_FREQ 100 /*Hz*/
#define TSP_USLEEP_PERIOD_US /*(long long)*/(1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS 1000


/* Nasty static variables */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;
static time_stamp_t my_time = 0;

/* protypes declarations */
int  STUB_GLU_get_symbol_number(void) ;

int  STUB_GLU_get_symbol_number(void)

{
  int i = 0;
  TSP_sample_symbol_info_t* p  = X_sample_symbol_info_list_val;
	
  for( p=X_sample_symbol_info_list_val; p->name!=0 ; p++)
    {
      i++;
    }

  return i;
}

void* STUB_GLU_thread(GLU_handle_t* arg)
{
  
  int i, symbols_nb, *ptr_index;
  tsp_hrtime_t current_time, next_time;
  unsigned long tickNow;
  glu_item_t item;
  double memo_val[GLU_MAX_SYMBOLS]; /* for building calc_function % 10 */

  symbols_nb  = STUB_GLU_get_symbol_number();
  current_time = tsp_gethrtime();

  /* infinite loop for symbols generation */
  while(1)
    {
     tickNow = tickGet();
      next_time = current_time + (TSP_USLEEP_PERIOD_US)*1000;
      while((current_time = tsp_gethrtime()) < next_time)
	{ 
	  taskDelay(1);
	}
 
      /* Must be call at each step in case of new samples wanted */
      TSP_datapool_get_reverse_list (&symbols_nb, &ptr_index); 
      for(i = 0 ; i < symbols_nb ; i++)
	{
          int index=ptr_index[i];
	  if(my_time%X_sample_symbol_info_list_val[i].period == 0)
	    {
	      item.time = my_time;
	      item.provider_global_index = index;
	      if (index!=0)
		item.value = calc_func(index, my_time);
	      else
		item.value = (double)(my_time) / (double)(TSP_STUB_FREQ);
	      memo_val[index]=item.value;

              TSP_datapool_push_next_item(&item);
	    }
	}
      /* Finalize the datapool state with new time : Ready to send */
      TSP_datapool_push_commit(my_time, GLU_GET_NEW_ITEM);
      

      if (!(my_time%1000))  STRACE_INFO(("TOP %d : %s=%g \t%s=%g \t%s=%g \t%s=%g", my_time,
					 X_sample_symbol_info_list_val[0].name, memo_val[0],
					 X_sample_symbol_info_list_val[1].name, memo_val[1],
					 X_sample_symbol_info_list_val[2].name, memo_val[2],
					 X_sample_symbol_info_list_val[3].name, memo_val[3]
					));
      my_time++;    
    }
    
  return arg;

}

int STUB_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[])
{
  int i;
  char symbol_buf[50];
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS+1, sizeof (TSP_sample_symbol_info_t)) ;
  assert(X_sample_symbol_info_list_val);

  for (i=0; i<GLU_MAX_SYMBOLS; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
    }  
  /*overide first name*/
  X_sample_symbol_info_list_val[0].name = strdup("t");
	
  return TRUE;
}

int  STUB_GLU_get_sample_symbol_info_list(GLU_handle_t* h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{

  int i = 0;
  TSP_sample_symbol_info_t* p; 
	
  for (p=X_sample_symbol_info_list_val; p->name!=0 ; p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  return TRUE;
}

int STUB_GLU_start(GLU_handle_t* this)
{  /* At first consumer connection : start thread */
  if(!this->tid)
    {
      TSP_CHECK_THREAD( (pthread_create(&(this->tid), NULL, GLU_thread, NULL)), FALSE);
    }
  return ((this->tid) != 0);
}

/* create the GLU handle instance for STUB */
GLU_handle_t* GLU_stub_create() {
  
  /* create a default GLU */
  GLU_handle_create(&stub_GLU,"StubbedServer",GLU_SERVER_TYPE_ACTIVE,TSP_STUB_FREQ);
  
  stub_GLU->initialize     = &STUB_GLU_init;
  stub_GLU->run            = &STUB_GLU_thread;
  stub_GLU->get_ssi_list   = &STUB_GLU_get_sample_symbol_info_list;
  /* ovveride default method */
  stub_GLU->start          = &STUB_GLU_start;

  return stub_GLU;
}

