/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/tests/stage1/Attic/glue_sserver.c,v 1.6 2002-12-24 14:14:33 tntdev Exp $

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
Maintainer : tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "calc_func.h"

RINGBUF_DECLARE_TYPE_DYNAMIC(glu_ringbuf,glu_item_t);

/* Glue server ringbuf size */
#define GLU_RING_BUFSIZE (1000 * 100 * 3)

/* TSP glue server defines */
#define TSP_USLEEP_PERIOD_US 10*1000 /*µS*/
#define GLU_MAX_SYMBOLS 1000


static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;

static tsp_hrtime_t X_lasttime;

static glu_ringbuf* glu_ring = 0;

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

long count_add = 0;
static int data_missed = FALSE;

static void* GLU_thread(void* arg)
{
  
  SFUNC_NAME(GLU_thread);

  static int last_missed = 0;
  int symbols_nb;
  int i;
  tsp_hrtime_t current_time;
  glu_item_t item;

  symbols_nb  = GLU_get_symbol_number();
  current_time = X_lasttime = tsp_gethrtime();  

  /* infinite loop for symbols generation */
  while(1)
    {
      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  if(my_time%X_sample_symbol_info_list_val[i].period == 0)
	    {

	      item.time = my_time;
	      item.provider_global_index = i;
	      item.value = calc_func(i, my_time);
	      RINGBUF_PTR_PUT(glu_ring, item);
	      count_add ++;
	    }
	}
      
      if( current_time <= X_lasttime )
	{ 
	  tsp_usleep(TSP_USLEEP_PERIOD_US);
	}

      X_lasttime += TSP_USLEEP_PERIOD_US*1000;
      current_time = tsp_gethrtime();
      
      if(last_missed!=RINGBUF_PTR_MISSED(glu_ring))
	{
	  last_missed = RINGBUF_PTR_MISSED(glu_ring);
	  data_missed = TRUE;
	}
      
      my_time++;    
      
      if (!(my_time%1000))  STRACE_INFO(("TOP %d!", my_time));
      
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

int GLU_add_block(GLU_handle_t h_glu,int provider_global_index, xdr_and_sync_type_t type)
{
  SFUNC_NAME(GLU_add_block);
  STRACE_IO(("-->IN index=%d type=%X", provider_global_index, type));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  /* FIXME : gérer ca ! */
  return TRUE;
}

int GLU_commit_add_block(GLU_handle_t h_glu)
{
  SFUNC_NAME(GLU_commit_add_block);
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
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
  return 1.0/( TSP_USLEEP_PERIOD_US * (1e-6));
}

void GLU_forget_data(GLU_handle_t h_glu)
{
  RINGBUF_PTR_RESET_CONSUMER(glu_ring);
}

