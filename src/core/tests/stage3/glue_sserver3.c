/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/tests/stage3/Attic/glue_sserver3.c,v 1.2 2002-10-01 15:47:42 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>
#include <math.h>
#include "glue_sserver.h"
#include "tsp_ringbuf.h"
#include "tsp_time.h"


#define _LIBUTIL_REENTRANT 1
#include "libUTIL.h"



/*µS*/
#define TSP_USLEEP_PERIOD_US 10*1000



#define GLU_MAX_SYMBOLS 1000

/*RINGBUF_DEFINE_DYNAMIC(glu_ringbuf, glu_ring,  RINGBUF_SZ(GLU_RING_BUFSIZE));*/

long count_add;

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




glu_ringbuf* GLU_active_get_ringbuf(GLU_handle_t h_glu)
{
  return 0;
}

char* GLU_get_server_name(void)
{
  return "RES Server";
}

/*int  GLU_get_symbol_number(void)

{
  SFUNC_NAME(GLU_get_symbol_number);

	
  int i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = X_sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  STRACE_IO(("-->OUT : -Nombre symboles = %u", i));
  
  return i;
}*/




int GLU_pasive_get_next_item(GLU_handle_t h_glu, glu_item_t* item)
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
      
      /*if(item->value)
	{

	}*/

      return TRUE;
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
      return TRUE;

	}   
      else
	{
	  return FALSE;
	}
    }

  
}

int GLU_init(char* fallback_stream_init)
{
  SFUNC_NAME(GLU_init);

  int ret = TRUE;
  int use_dbl;

  STRACE_IO(("-->IN"));

  /* is there a fallback stream ? */
  if(fallback_stream_init)
    {
      /* Yes, we must test it */
      d_rhandle h_res = d_ropen_r(fallback_stream_init, &use_dbl);
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
	  STRACE_ERROR(("Provided fallback stream '%s' does not work...bye...bye...", fallback_stream_init));
	}            
    }

  STRACE_IO(("-->OUT"));

  return ret;
  
}

int  GLU_get_sample_symbol_info_list(GLU_handle_t h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(GLU_get_sample_symbol_info_list);
  GLU_state_t* obj = (GLU_state_t*)h_glu;

	
  gint32 i = 0;
  TSP_sample_symbol_info_t* p;
	
  STRACE_IO(("-->IN"));
	
  for(p = obj->sample_symbol_info_list_val;
      p->name != 0 ; 
      p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = obj->sample_symbol_info_list_val;
	    
  STRACE_IO(("-->OUT : -Nombre symboles = %u", symbol_list->TSP_sample_symbol_info_list_t_len));

  return TRUE;
}

int GLU_add_block(GLU_handle_t h_glu, int provider_global_index, xdr_and_sync_type_t type)
{

  SFUNC_NAME(GLU_add_block);
  STRACE_IO(("-->IN index=%d type=%X", provider_global_index, type));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  /* FIXME : gérer ca ! */
  STRACE_IO(("-->OUT"));
	
  return TRUE;
}

int GLU_commit_add_block(GLU_handle_t h_glu)
{
  SFUNC_NAME(GLU_commit_add_block);
  STRACE_IO(("-->IN"));
  STRACE_IO(("FIXME : JE NE FAIS RIEN POUR LE MOMENT !"));
  STRACE_IO(("-->OUT"));
	
  return TRUE;
	
}

GLU_server_type_t GLU_get_server_type(void)
{
  return GLU_SERVER_TYPE_PASIVE;
}



GLU_handle_t GLU_get_instance(char *stream_init, char** error_info)
{

  SFUNC_NAME(GLU_get_instance);

  int i;
  int nbrec;  
  char namev[RES_NAME_LEN];
  char descv[RES_DESC_LEN];
  GLU_state_t* obj;
  char* file;

  STRACE_IO(("-->IN"));
  

 if(error_info)
   *error_info = "";

 if(stream_init)
   {

     /* FIXME : quand la fonction renvera une code d'erreur, le verifier ! */
     obj = (GLU_state_t*)calloc(1, sizeof(GLU_state_t) );
     obj->time_stamp = -1;

     /* FIXME : faire le close dans le GLU_end */
     obj->h_res = d_ropen_r(stream_init, &(obj->use_dbl));
     STRACE_INFO(("stream_init = '%s'", stream_init));
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

       }
     else
       {
	 STRACE_ERROR(("Function d_rval_r failed for file : '%s'", stream_init));
	 if(error_info)
	   *error_info = "Unable to open file";

	 return 0;
       }
   }
 else
   {
     STRACE_INFO(("A Stream initialisation string must be provided"));
     if(error_info)
       *error_info = "A Stream initialisation string must be provided";

     return 0;
   }

  STRACE_IO(("-->OUT"));

  return obj;

}
