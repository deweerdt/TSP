/*!  \file 

$Id: glue_res.c,v 1.2 2003-02-27 15:37:33 tsp_admin Exp $

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

#define GLU_STREAM_INIT_USAGE "GLU stream init usage : filename[.res]"
#define GLU_RES_FILE_ARG_NUMBER 1

static double res_freq=0;

struct GLU_state_t
{
  void* res_values;
  int current_var;
  int time_stamp;
  int nbvar;
  int use_dbl;
  d_rhandle h_res;
  TSP_sample_symbol_info_t* sample_symbol_info_list_val;
};

typedef struct GLU_state_t GLU_state_t;



char* GLU_get_server_name(void)
{
  return "ResServer";
}


GLU_get_state_t GLU_get_next_item(GLU_handle_t h_glu, glu_item_t* item)
{
  SFUNC_NAME(GLU_pasive_get_next_item);
  GLU_state_t* obj = (GLU_state_t*)h_glu;

  if ( obj->current_var != obj->nbvar )
    {
      item->provider_global_index = obj->current_var;
      item->value = obj->use_dbl ?
	((double*)(obj->res_values)) [obj->current_var]
	: ((float*) (obj->res_values)) [obj->current_var];
      obj->current_var++;
      item->time = obj->time_stamp;
      
      return GLU_GET_NEW_ITEM;
    }
  else
    {
      obj->current_var = 0;      
      obj->time_stamp++;
      if ( EOF != d_read_r(obj->h_res, obj->res_values) )
	{
	  item->value = obj->use_dbl ?
	    ((double*)(obj->res_values)) [obj->current_var]
	    : ((float*) (obj->res_values)) [obj->current_var];
	  item->provider_global_index = obj->current_var;
	  obj->current_var++;
	  item->time = obj->time_stamp;
	  return GLU_GET_NEW_ITEM;

	}   
      else
	{
	  return GLU_GET_EOF;
	}
    }

  
}

int GLU_init(int fallback_argc, char* fallback_argv[])
{
  SFUNC_NAME(GLU_init);

  int ret = TRUE;
  int use_dbl;

  /* is there a fallback stream ? */
  if(fallback_argc && fallback_argv)
    {
      /* Yes, we must test it. We are expectig one arg */
      if(fallback_argc > 1)
	{
	  d_rhandle h_res = d_ropen_r(fallback_argv[GLU_RES_FILE_ARG_NUMBER], &use_dbl);
	  if(h_res)
	    {
	      int nbrec;
	      int nbvar;
	      nbvar = d_rval_r(h_res, 'v');
	      nbrec = d_rval_r(h_res, 'r');

	      /* ok */
	      STRACE_INFO(("Total number of records for default RES = %d", nbrec));
	      STRACE_INFO(("Total number of variables for default RES = %d", nbvar));	  
	      STRACE_INFO(("Data type for default RES = %s", use_dbl ? "DOUBLE" : "FLOAT" ));	  
	      d_rclos_r(h_res);
	    }
	  else
	    {
	      /* yeak ! */
	      ret = FALSE;	      
	      STRACE_WARNING(("Provided fallback stream '%s' does not work...bye...bye...", fallback_argv[1]));
	    }
	}
      else
	{
	  ret = FALSE;
	  STRACE_WARNING((GLU_STREAM_INIT_USAGE));
	}
    }

  return ret;
}

int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(GLU_get_sample_symbol_info_list);
  GLU_state_t* obj = (GLU_state_t*)h_glu;
  TSP_sample_symbol_info_t* p;
  int i = 0;
	
  for(p = obj->sample_symbol_info_list_val; p->name != 0 ; p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = obj->sample_symbol_info_list_val;
	    
  return TRUE;
}


GLU_server_type_t GLU_get_server_type(void)
{
  return GLU_SERVER_TYPE_PASSIVE;
}

void GLU_forget_data(GLU_handle_t h_glu)
{
  /* meaningless : i do nothing */  
}



GLU_handle_t GLU_get_instance(int argc, char* argv[], char** error_info)
{

  SFUNC_NAME(GLU_get_instance);

  int i;
  double t1, t2, freq;
  int nbrec;  
  char namev[RES_NAME_LEN];
  char descv[RES_DESC_LEN];
  GLU_state_t* obj = 0;

  if(error_info)
    *error_info = "";

  if(argc && argv)
    {
      /* At least 1 parameter (the res file ) */
      if( argc > 1 )
	{
	  char* res_file = argv[GLU_RES_FILE_ARG_NUMBER];
	  /* FIXME : quand la fonction renvera une code d'erreur, le verifier ! */
	  obj = (GLU_state_t*)calloc(1, sizeof(GLU_state_t) );
	  TSP_CHECK_ALLOC(obj, 0);
	  obj->time_stamp = -1;

	  /* FIXME : faire le close dans le GLU_end */
	  STRACE_INFO(("stream_init = '%s'", res_file));
	  obj->h_res = d_ropen_r(res_file, &(obj->use_dbl));
	 
	  if( obj->h_res )
	    {
	      obj->nbvar = d_rval_r(obj->h_res, 'v');
	      nbrec = d_rval_r(obj->h_res, 'r');
	      obj->current_var = obj->nbvar;

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
		res_freq = 1/(t2-t1);
	      else
		res_freq = 0;
	      d_restart_r(obj); /* ask to restart from begining */

	    }
	  else
	    {
	      STRACE_ERROR(("Function d_rval_r failed for file : '%s'", res_file));
	      if(error_info)
		*error_info = "Unable to open file";
	      free(obj);
	      obj = 0;
	    }

	}
      else
	{
	  STRACE_WARNING(("Wrong number of parameters"));
	  if(error_info)
	    *error_info = "Wrong number of parameters ; " GLU_STREAM_INIT_USAGE ;
	  free(obj);
	  obj = 0;
	}

    }
  else
    {
      STRACE_WARNING(("A Stream initialisation string must be provided"));
      if(error_info)
	*error_info = "A Stream initialisation string must be provided\n" GLU_STREAM_INIT_USAGE ;
      free(obj);
      obj = 0;
    }

  return obj;
}

double GLU_get_base_frequency(void)
{
/* Server is passive, frequency is computed in get instance */
return res_freq;
}
