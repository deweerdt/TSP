/*!  \file 

$Id: glue_res.c,v 1.6 2004-09-16 09:39:20 tractobob Exp $

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

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"


#define _LIBUTIL_REENTRANT 1
#include "libUTIL.h"

#define GLU_STREAM_INIT_USAGE "TSP RES Reader usage : filename[.res]  [-eof N]\n\t\twait N seconds after EOF to check file increment, default = 0 (NO WAIT)\n"
#define GLU_RES_FILE_ARG_NUMBER 1
#define GLU_RES_WAIT_EOF       	3

static int _wait_eof=0;
static int _started = FALSE;

struct GLU_state_t
{
  void* res_values;
  int time_stamp;
  int nbvar;
  int use_dbl;
  double freq;
  d_rhandle h_res;
  TSP_sample_symbol_info_t* sample_symbol_info_list_val;
};

typedef struct GLU_state_t GLU_state_t;

GLU_state_t glu_handler;


void GLU_loop()
{
  SFUNC_NAME(GLU_thread);
  GLU_state_t* obj = &glu_handler;
  glu_item_t  item;
  int i;
  GLU_get_state_t state;
  int run = _wait_eof+1;

  while(!_started)
    sleep(1);

  while(run)
    {
      if(d_read_r(obj->h_res, obj->res_values) == EOF)
	{
	  STRACE_DEBUG (("EOF for time=%d, val[0]=%g", obj->time_stamp, item.value));
	  if(!_wait_eof)
	    {
	      state = GLU_GET_EOF;
	      run = FALSE;
	    }
	  else
	    {
	      state = GLU_GET_NO_ITEM;
	      sleep(1); /* wait 1 s for update of file */
	      run--;
	    }
	}
      else
	{
	  /* got a new RES line : reset time-out counter and increment time_stamp */
	  run = _wait_eof+1;
	  obj->time_stamp++;
	  state = GLU_GET_NEW_ITEM;
	 
	  /* push all data of line */
	  for(i=0; i<obj->nbvar; i++)
	    {
	      item.provider_global_index = i;
	      item.value = obj->use_dbl ?
		((double*)(obj->res_values)) [i] :
	        ((float*) (obj->res_values)) [i];
	      
	      TSP_datapool_push_next_item(&item);

	      if(i==0)
		STRACE_INFO (("New record : time=%d, val[0]=%g", obj->time_stamp, item.value));
	    }
      
	}

      /* commit ... */
      TSP_datapool_push_commit(obj->time_stamp, state);     
    }

  /* close .res file */
  d_rclos_r(obj->h_res);
}

int GLU_start(void)
{
  _started = TRUE;
  return TRUE;
}


int GLU_init(int fallback_argc, char* fallback_argv[])
{
  SFUNC_NAME(GLU_init);

  int ret = TRUE;
  GLU_state_t* obj = &glu_handler;
  int i;
  double t1, t2;
  int nbrec;  
  char namev[RES_NAME_LEN];
  char descv[RES_DESC_LEN];

  /* is there a fallback stream ? */
  if(fallback_argc>1 && fallback_argv)
    {
      /* Yes, we must test it. We are expectig one arg */
      char* res_file = fallback_argv[GLU_RES_FILE_ARG_NUMBER];
      /* FIXME : quand la fonction renvera un code d'erreur, le verifier ! */
      obj->time_stamp = -1;

      STRACE_INFO(("stream_init = '%s'", res_file));
      obj->h_res = d_ropen_r(res_file, &(obj->use_dbl));
	 
      if( obj->h_res )
	{
	  obj->nbvar = d_rval_r(obj->h_res, 'v');
	  nbrec = d_rval_r(obj->h_res, 'r');

	  STRACE_INFO(("Total number of records = %d", nbrec));
	  STRACE_INFO(("Total number of variables = %d", obj->nbvar));
	  STRACE_INFO(("Data type = %s", obj->use_dbl ? "DOUBLE" : "FLOAT" ));

	  obj->sample_symbol_info_list_val = calloc (obj->nbvar+1, sizeof (TSP_sample_symbol_info_t)) ;
	  assert(obj->sample_symbol_info_list_val);
	  for (i=0; i<obj->nbvar; i++)
	    {      
	      d_rnam_r(obj->h_res, namev, descv, i);

	      obj->sample_symbol_info_list_val[i].name = strdup(namev);
	      obj->sample_symbol_info_list_val[i].provider_global_index = i;
	      obj->sample_symbol_info_list_val[i].period = 1;
	    }
  
	  obj->res_values = obj->use_dbl ?
	    calloc((obj->nbvar+1),sizeof(double))
	    : calloc((obj->nbvar+1),sizeof(float));
	  TSP_CHECK_ALLOC(obj->res_values, FALSE);
	      
	  /* Try to compute res file frequency */
	  d_read_r(obj->h_res, obj->res_values);
	  t1 = obj->use_dbl ? ((double*)(obj->res_values)) [0] : ((float*)(obj->res_values)) [0];
	  d_read_r(obj->h_res, obj->res_values);
	  t2 = obj->use_dbl ? ((double*)(obj->res_values)) [0] : ((float*)(obj->res_values)) [0];
	  if (t2!=t1)
	    obj->freq = 1/(t2-t1);
	  else
	    obj->freq = 0;
	  d_restart_r(obj->h_res); /* ask to restart from begining */

	}
      else
	{
	  STRACE_ERROR(("Function d_rval_r failed for file : '%s'", res_file));
	}

       if(fallback_argc > GLU_RES_WAIT_EOF)
	{
	  _wait_eof = atoi(fallback_argv[GLU_RES_WAIT_EOF]);
	}
    }
  else
    {
      STRACE_ERROR((GLU_STREAM_INIT_USAGE));
      ret = FALSE;
    }

  return ret;
}



int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(GLU_get_sample_symbol_info_list);
  GLU_state_t* obj = &glu_handler;

  symbol_list->TSP_sample_symbol_info_list_t_len = obj->nbvar;
  symbol_list->TSP_sample_symbol_info_list_t_val = obj->sample_symbol_info_list_val;
	    
  return TRUE;
}


char* GLU_get_server_name(void)
{
  return "ResServer";
}

GLU_server_type_t GLU_get_server_type(void)
{
  return GLU_SERVER_TYPE_PASSIVE;
}

double GLU_get_base_frequency(void)
{
  GLU_state_t* obj = &glu_handler;
  /* Server is passive, frequency is computed in GLU_init */
  return obj->freq;
}

/* following functions are meaningless : do nothing */  

void GLU_forget_data(GLU_handle_t h_glu)
{
}

GLU_handle_t GLU_get_instance(int argc, char* argv[], char** error_info)
{
  return GLU_GLOBAL_HANDLE;
}

