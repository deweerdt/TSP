/*!  \file 

$Id: glue_stub.c,v 1.9 2004-09-15 15:02:01 dufy Exp $

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

#include "tsp_sys_headers.h"
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "tsp_datapool.h"
#include "calc_func.h"

/*RINGBUF_DECLARE_TYPE_DYNAMIC(glu_ringbuf,glu_item_t);*/

/* Glue server ringbuf size */
#define GLU_RING_BUFSIZE (1000 * 100 * 10)

/* TSP glue server defines */
#define TSP_STUB_FREQ 100.0 /*Hz*/
#define TSP_USLEEP_PERIOD_US (1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS 1000

/* Nasty static variables */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;
static tsp_hrtime_t X_lasttime;
/*static glu_ringbuf* glu_ring = 0;*/
pthread_t thread_id = 0;	
static char* X_server_name = "StubbedServer";
static time_stamp_t my_time = 0;


char* GLU_get_server_name(void)
{
  return X_server_name;
}

int  GLU_get_symbol_number(void)

{
  int i = 0;
  TSP_sample_symbol_info_t* p  = X_sample_symbol_info_list_val;
	
  for( p=X_sample_symbol_info_list_val; p->name!=0 ; p++)
    {
      i++;
    }

  return i;
}

/*static int data_missed = FALSE;*/

static void* GLU_thread(void* arg)
{
  int i, symbols_nb, *ptr_index;
  tsp_hrtime_t current_time;
  glu_item_t item;
  double memo_val[GLU_MAX_SYMBOLS]; /*for debug informatin */

  current_time = X_lasttime = tsp_gethrtime();  

  /* infinite loop for symbols generation */
  while(1)
    {
      /* Must be call at each step in case of new samples wanted */
      TSP_datapool_get_reverse_list (&symbols_nb, &ptr_index); 
      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  int index=ptr_index[i];
	  if(my_time%X_sample_symbol_info_list_val[index].period == 0)
	    {
	      item.time = my_time;
	      item.provider_global_index = index;
	      if (index!=0)
		item.value = calc_func(index, my_time);
	      else
		item.value = (double)(my_time) / (double)(TSP_STUB_FREQ);
	      memo_val[index]=item.value;

	      TSP_datapool_push_next_item(&item);
	      /*RINGBUF_PTR_PUT(glu_ring, item);*/
	    }
	}
      /* Finalize the datapool state with new time : Ready to send */
      TSP_datapool_push_commit(my_time, GLU_GET_NEW_ITEM);

      if( current_time <= X_lasttime )
	{ 
	  tsp_usleep(TSP_USLEEP_PERIOD_US);
	}

      X_lasttime += TSP_USLEEP_PERIOD_US*1000;
      current_time = tsp_gethrtime();
      /*      
      if(last_missed!=RINGBUF_PTR_MISSED(glu_ring))
	{
	  last_missed = RINGBUF_PTR_MISSED(glu_ring);
	  data_missed = TRUE;
	  }*/
      
      my_time++;    
      
      if (!(my_time%1000))  STRACE_INFO(("TOP %d : %s=%g \t%s=%g \t%s=%g \t%s=%g", my_time,
					 X_sample_symbol_info_list_val[0].name, memo_val[0],
					 X_sample_symbol_info_list_val[1].name, memo_val[1],
					 X_sample_symbol_info_list_val[2].name, memo_val[2],
					 X_sample_symbol_info_list_val[3].name, memo_val[3]
					));
    }
    
  return arg;

}

int GLU_init(int fallback_argc, char* fallback_argv[])
{
  int i;
  char symbol_buf[50];
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS+1, sizeof (TSP_sample_symbol_info_t)) ;
  for (i=0; i<GLU_MAX_SYMBOLS; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1; 
    }  
  /*overide first name*/
  X_sample_symbol_info_list_val[0].name = strdup("t");

  /*  RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(GLU_RING_BUFSIZE));
      RINGBUF_PTR_RESET_CONSUMER (glu_ring);*/
  return TRUE;
}

int GLU_start(void)
{
  return pthread_create(&thread_id, NULL, GLU_thread, NULL);  
}


int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
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


GLU_server_type_t GLU_get_server_type(void)
{
  return GLU_SERVER_TYPE_ACTIVE;
}


GLU_handle_t GLU_get_instance(int argc, char* argv[], char** error_info)
{
  if(error_info)
    *error_info = "";

  return GLU_GLOBAL_HANDLE;
}

double GLU_get_base_frequency(void)
{
  /* Calculate base frequency */
  return TSP_STUB_FREQ;
}


