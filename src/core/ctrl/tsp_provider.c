/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_provider.c,v 1.7 2002-10-07 08:36:04 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Main implementation for the producer module

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_provider.h"

#include "tsp_session.h"
#include "tsp_server.h"
#include "tsp_datapool.h"
#include "glue_sserver.h"	

static char* X_default_stream_init = 0;

/** modified argc and argv, will be returned to user code after init */
static  char** X_argv = 0;
static  int X_argc = 0;

/** Default values for args to the GLU */
static  char** X_glu_argv = 0;
static  int X_glu_argc = 0;

/** default server number is 0, that's what we want
when there is one single server on a given host*/
static X_server_number = 0;


static X_tsp_provider_init_ok = FALSE;



static int TSP_cmd_line_parser(int* argc, char** argv[])
{
  int i;
  int final_argc = 1;
  int found_stream_start = FALSE;
  int found_stream_stop = FALSE;
  int found_server_number_flag = FALSE;
  char* p;
  int ret = TRUE;

  SFUNC_NAME((TSP_cmd_line_parser));
  STRACE_IO(("-->IN"));

  /* FIXME : FUITE */
  X_argv = (char**)calloc(*argc, sizeof(char*));
  X_glu_argv = (char**)calloc(*argc, sizeof(char*));
  X_glu_argc = 0;
  TSP_CHECK_ALLOC(X_argv, FALSE);
  TSP_CHECK_ALLOC(X_glu_argv, FALSE);
  /* Get program name anyway */
  X_argv[0] = (*argv)[0];
  for( i = 1 ; i < *argc && ret ; i++)
    {
      /* Is the arg a TSP arg ? */
      p = strstr( (*argv)[i], TSP_ARG_PREFIX );
      if(p && (p == (*argv)[i] ))
	{
	  /* TSP Arg */
	  STRACE_INFO(("Tsp ARG : '%s'", (*argv)[i]));

	  /* First we must not be looking for a argument */
	  if(found_server_number_flag)
	    {
	      /* Error we are expecting an arguement not a TSP option*/
	      STRACE_WARNING(("Unexpected %s", (*argv)[i]));
	      ret = FALSE;
	    }
	  /* Look for start flag */
	  else if(!strcmp(TSP_ARG_STREAM_INIT_START, (*argv)[i]))
	    {
	      if(!found_stream_stop && !found_stream_start)
		{
		  found_stream_start = TRUE;
		  /* Ok the user wants a default stream control, put
		     the first dummy element */
		  if ( 0 == X_glu_argc )
		    {
		      X_glu_argv[0] = TSP_ARG_DUMMY_PROG_NAME;
		      X_glu_argc = 1;
		    }
		}
	      else
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_STREAM_INIT_START));
		  ret = FALSE;
		}
	    }
	  else if (!strcmp(TSP_ARG_STREAM_INIT_STOP, (*argv)[i]))
	    {
	      if(found_stream_start && !found_stream_stop)		
		{
		  found_stream_stop = TRUE;
		}
	      else
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_STREAM_INIT_STOP));
		  ret = FALSE;
		}
	    }
	  else if (!strcmp(TSP_ARG_SERVER_NUMBER, (*argv)[i]))
	    {
	      found_server_number_flag = TRUE;
	      if(found_stream_start && !found_stream_stop)		
		{
		  STRACE_WARNING(("Unexpected "TSP_ARG_SERVER_NUMBER));
		  ret = FALSE;
		}
	    }
	  else
	    {
	      /* Unkown option */
	      STRACE_WARNING(("Unknown TSP option : '%s'",(*argv)[i] ))
	      ret = FALSE;
	    }
	}
      else /* Not a TSP arg */
	{
	  /* Are we in the TSP command line ? */
	  if ( found_stream_start && !found_stream_stop )
	    {
	      X_glu_argv[X_glu_argc++] = (*argv)[i];
	    }
	  else if(found_server_number_flag)
	    {
	      /* Found arg for server number option */
	      found_server_number_flag = FALSE;
	      X_server_number = atoi((*argv)[i]);
	      STRACE_INFO(("Server number = %d", X_server_number));
	    }
	  else
	    {	      
	      /* Nop, this arg is for the user */
	      X_argv[final_argc] = (*argv)[i];
	      final_argc++;
	    }
	}
    } /* for */
  
  /* Check is the stop was found */
  
  if( found_stream_start && !found_stream_stop )
    {
      STRACE_WARNING(("A " TSP_ARG_STREAM_INIT_STOP " flag was expected"));
      ret = FALSE;
    }

  if( found_server_number_flag )
    {
      STRACE_WARNING(("An argument was expected after " TSP_ARG_SERVER_NUMBER));
      ret = FALSE;
    }

  /* swap argc and argv values */
  *argc = final_argc;
  *argv = X_argv;
  
  /* Display usage */
  if(!ret)
    {
      STRACE_WARNING((TSP_ARG_PROVIDER_USAGE));
    }
  else
    {
      if (! X_glu_argc )
	{
	  STRACE_INFO(("No GLU stream init provided on command line"));
	}
    }

  STRACE_IO(("-->OUT"));
  
  return ret;


}


void TSP_provider_request_open(const TSP_request_open_t* req_open,
		      TSP_answer_open_t* ans_open)
{
  SFUNC_NAME(TSP_request_open);
  GLU_handle_t glu_h;
  char* error_info;
  int i;
	
  STRACE_IO(("-->IN"));


   ans_open->version_id = UNDEFINED_VERSION_ID;
   ans_open->channel_id = UNDEFINED_CHANNEL_ID;
   ans_open->status = TSP_STATUS_ERROR_UNKNOWN;
   ans_open->status_str = "";

   if(req_open->argv.TSP_argv_t_len)
     {
       for( i = 0; i< req_open->argv.TSP_argv_t_len ;i++)
	 {
	   STRACE_INFO(("arg %d is '%s'", i,  req_open->argv.TSP_argv_t_val[i]));
	 }
     }
   else
     {
          STRACE_INFO(("No custom args from consumer"));
     }


   /*  get GLU instance. If a stream init is provided, use it, else, use default */   
   /* Note : for an active server, the function GLU_get_instance wall called ins TSP_provider_init
      so it is useless here */
   if( 0 != req_open->argv.TSP_argv_t_len )
     {
       glu_h = GLU_get_instance(req_open->argv.TSP_argv_t_len, req_open->argv.TSP_argv_t_val, &error_info);
     }
   else
     {
       /* use fallback if provided */
       glu_h = GLU_get_instance(X_glu_argc, X_glu_argv, &error_info);
     }

   
   /* FIXME : condition  de sortie */
	
   if(glu_h)
     {
       if(TSP_add_session(&(ans_open->channel_id), glu_h))
	 {
	   if(req_open->version_id <= TSP_VERSION)
	     {
	       ans_open->version_id = TSP_VERSION;
	       ans_open->status = TSP_STATUS_OK;
	     }
	   else
	     {
	       STRACE_ERROR(("TSP version ERROR. Requested=%d Current=%d",req_open->version_id, TSP_VERSION ));
	       ans_open->status = TSP_STATUS_ERROR_VERSION;
	     }
	 }
       else
	 {
	   STRACE_ERROR(("TSP_add_session failed"));
	 }
     }
   else
     {
       STRACE_INFO(("Unable to get GLU instance"));
        ans_open->status = TSP_STATUS_ERROR_SEE_STRING;
	ans_open->status_str = error_info;
     }

  STRACE_IO(("-->OUT"));
	
}

void TSP_provider_request_close(const TSP_request_close_t* req_close)

{
  SFUNC_NAME(TSP_request_close);
		
  STRACE_IO(("-->IN"));
		
  /* On ferme la session */
  if(TSP_close_session_by_channel(req_close->channel_id))
    {	
      STRACE_INFO(("Session for channel_id=%u is closed", req_close->channel_id));
    }
  else
    {
      STRACE_ERROR(("Unable to close session for channel_id=%u", req_close->channel_id));
    }
		
  STRACE_IO(("-->OUT"));
	
}

void  TSP_provider_request_information(TSP_request_information_t* req_info, 
 			      TSP_answer_sample_t* ans_sample)
{

  SFUNC_NAME(tsp_request_information);
	
  STRACE_IO(("-->IN"));
	
  ans_sample->version_id = TSP_VERSION;
  ans_sample->channel_id = req_info->channel_id;
  /* FIXME : gerer le retour */  
  TSP_session_get_sample_symbol_info_list_by_channel(req_info->channel_id,
						     &(ans_sample->symbols));

  		
  STRACE_IO(("-->OUT"));
}

/** Provider initialisation function.
 * @param s The char string that may be used by the sample server
 */

int TSP_provider_init(int* argc, char** argv[])
{
  SFUNC_NAME(TSP_provider_init);

  int ret = TRUE;
  assert(argc);
  assert(argv);
  
  ret = TSP_cmd_line_parser(argc, argv);
  if(ret)
  {

    /* Faire le traitement du nom du serveur */
    TSP_session_init();

    /* We must start a global datapool when the sample server is active */
    if ( GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
      {
	/* We initialize the active server (0 is there coz a fall back stream does not make sense)*/
      
	if(ret) ret = GLU_init(0,0);

      /* We get the datapool instance. It MUST be GLOBAL because the server is active */      	 
	if(ret)
	  {	  
	    if ( GLU_GLOBAL_HANDLE != GLU_get_instance(X_glu_argc, X_glu_argv,0) )
	      {
		ret = FALSE;
		STRACE_ERROR(("function GLU_get_instance(stream_init) failed. I was expecting a GLU_GLOBAL_HANDLE"));
	      }
	  }
	if(ret) ret = TSP_global_datapool_init();	
      }
    else
      {
	if(ret) ret = GLU_init(X_glu_argc, X_glu_argv);
      }
    
    
      }

  if(!ret)
    {
      STRACE_ERROR(("TSP init error"));
    }

  X_tsp_provider_init_ok = ret;
  STRACE_IO(("-->OUT"));
  
  return ret;
}

int TSP_provider_run(void)
{
  assert(X_tsp_provider_init_ok);
  /* TSP_command_init must be the latest called init func, coz we will be blocked here forever */
  return TSP_command_init(X_server_number);
  
}

void  TSP_provider_request_sample(TSP_request_sample_t* req_info, 
			 TSP_answer_sample_t** ans_sample)
{
  SFUNC_NAME(TSP_request_sample);
    
  int ret = TRUE;
  int use_global_datapool;
	
  STRACE_IO(("-->IN"));
	
  /* FIXME : ce n'est pas le meilleur endroit pour faire ca,
     le faire plutot lors du sample_init avec les données
     conservée dans la session*/
  /*ret = TSP_global_datapool_add_symbols(&(req_info->symbols));*/
  if(ret)
    {
     
      /* Must we use a session or global data pool ? */
      if (  GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
	  use_global_datapool = TRUE;
      else
	  use_global_datapool = FALSE;

      
      ret = TSP_session_create_symbols_table_by_channel(req_info, ans_sample, use_global_datapool);
      if(!ret) 
        {
	  STRACE_ERROR(("Function TSP_session_create_symbols_table_by_channel failed"));
        }
        
    }
  else
    {
      STRACE_ERROR(("Function TSP_datapool_add_symbols failed"));
    }
	
  STRACE_IO(("-->OUT"));
}

void  TSP_provider_request_sample_init(TSP_request_sample_init_t* req_info, 
 			      TSP_answer_sample_init_t** ans_sample)
{

  SFUNC_NAME(TSP_request_sample_init);
  int ret = FALSE;

  int start_local_thread;

    
  STRACE_IO(("-->IN"));
    

  /*FIXME : oups, fuite de memoire*/
  *ans_sample = (TSP_answer_sample_init_t*)calloc(1, sizeof(TSP_answer_sample_init_t));
  TSP_CHECK_ALLOC(*ans_sample,);
  
  /* If the sample server is a lazy pasive server, we need a thread per session*/
  if (  GLU_SERVER_TYPE_ACTIVE == GLU_get_server_type() )
    start_local_thread = FALSE;
  else
    start_local_thread = TRUE;
  
  ret = TSP_session_create_data_sender_by_channel(req_info->channel_id, start_local_thread);
  if(!ret)
    {
      STRACE_ERROR(("TSP_data_sender_create failed"));
    }
    
  /* send data address to client */
  (*ans_sample)->data_address =
    (char*)TSP_session_get_data_address_string_by_channel(req_info->channel_id);
  
  STRACE_INFO(("DATA_ADDRESS = '%s'", (*ans_sample)->data_address));
  STRACE_IO(("-->OUT"));

}
