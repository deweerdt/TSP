/*!  \file 

$Id: glue_stub.c,v 1.2 2004-07-30 09:20:42 mia Exp $

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
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "calc_func.h"

RINGBUF_DECLARE_TYPE_DYNAMIC(glu_ringbuf,glu_item_t);

/* TSP glue server defines */
#define TSP_STUB_FREQ 100 /*Hz*/
#define TSP_USLEEP_PERIOD_US /*(long long)*/(1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS 1000

/* Glue server ringbuf size */
#define GLU_RING_BUFSIZE (TSP_STUB_FREQ * GLU_MAX_SYMBOLS)


/* Nasty static variables */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;
static glu_ringbuf* glu_ring = 0;
pthread_t thread_id = 0;
static char* X_server_name = "StubbedServer";
static time_stamp_t my_time = 0;

/* protypes declarations */
int  GLU_get_symbol_number(void) ;

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

int data_missed = FALSE;
void* GLU_thread(void* arg)
{
  
  SFUNC_NAME(GLU_thread);

  int i, symbols_nb;
  tsp_hrtime_t current_time, next_time;
  unsigned long tickNow;
  glu_item_t item;
  double memo_val[GLU_MAX_SYMBOLS]; /* for building calc_function % 10 */

  symbols_nb  = GLU_get_symbol_number();
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
 
      for(i = 0 ; i < symbols_nb ; i++)
	{
	  if(my_time%X_sample_symbol_info_list_val[i].period == 0)
	    {
	      item.time = my_time;
	      item.provider_global_index = i;
	      if (i!=0)
		item.value = calc_func(i, my_time);
	      else
		item.value = (double)(my_time) / (double)(TSP_STUB_FREQ);
	      memo_val[i]=item.value;

	      RINGBUF_PTR_PUT(glu_ring, item);
	    }
	}
      

      if(RINGBUF_PTR_MISSED(glu_ring) && !data_missed)
	{
	  data_missed = TRUE;
	  STRACE_ERROR(("TOP %d, data missed : %d ticks\n", my_time, tickGet() - tickNow));
	}
      
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
int GLU_init(int fallback_argc, char* fallback_argv[])
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
	

  RINGBUF_PTR_INIT(glu_ringbuf, glu_ring, glu_item_t,  0, RINGBUF_SZ(GLU_RING_BUFSIZE));
  RINGBUF_PTR_RESET_CONSUMER (glu_ring);
  
  return TRUE;
}

int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(GLU_get_sample_symbol_info_list);

  gint32 i = 0;
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

/* not used */
GLU_get_state_t GLU_get_next_item(GLU_handle_t h_glu,glu_item_t* item)
{
  SFUNC_NAME(GLU_get_next_item);

  GLU_get_state_t res = GLU_GET_NEW_ITEM;
  assert(h_glu == GLU_GLOBAL_HANDLE);
  
  if(!data_missed)
    {
      if (!RINGBUF_PTR_ISEMPTY(glu_ring))
	{
	  /* OK data found */
	  RINGBUF_PTR_NOCHECK_GET(glu_ring ,(*item));                  
	}
      else
	{
	  res = GLU_GET_NO_ITEM;
	}
    }
  else
    {
      /* There's a huge problem, we were unable to put data
	 in the ringbuffer */
      res = GLU_GET_DATA_LOST;
      data_missed = FALSE;
    }

  return res;
}

GLU_handle_t GLU_get_instance(int argc, char* argv[], char** error_info)
{
  SFUNC_NAME(GLU_get_instance);

  /* At first consumer connection : start thread */
  if(!thread_id)
    {
      TSP_CHECK_THREAD( (pthread_create(&thread_id, NULL, GLU_thread, NULL)), FALSE);
    }

  if(error_info)
    *error_info = "";

  return GLU_GLOBAL_HANDLE;
}

double GLU_get_base_frequency(void)
{
  /* Calculate base frequency */
  return TSP_STUB_FREQ;
}

void GLU_forget_data(GLU_handle_t h_glu)
{
  RINGBUF_PTR_RESET_CONSUMER(glu_ring);
}



