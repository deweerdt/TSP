/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.c,v 1.30 2004-10-06 09:44:38 tractobob Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Consumer

-----------------------------------------------------------------------

Purpose   : Main implementation for the TSP consumer library

-----------------------------------------------------------------------
*/

#include "tsp_sys_headers.h"

#include "tsp_consumer.h"
#include "tsp_client.h"
#include "tsp_group.h"
#include "tsp_data_receiver.h"
#include "tsp_sample_ringbuf.h"
#include "tsp_datastruct.h"


/* Pool time for network data read (µs) */
#define TSP_RECEIVER_THREAD_WAIT_FIFO_FULL (2e5)

/**
 * Quick check for a session validity.
 */
#define TSP_CHECK_SESSION(session, ret) \
	{ \
		if (0 == session) \
		{  \
			STRACE_ERROR(("The session object is NULL !")) \
			return (ret); \
		} \
		if( UNDEFINED_CHANNEL_ID == session->channel_id) \
		{  \
			STRACE_ERROR(("No Channel Id available, the session need to be opened first !")) \
			return (ret); \
		} \
	}


/*-----------------------------------------------------------------*/
/** modified argc and argv, will be returned to user code after init */
static  char** X_argv = 0;
static  int X_argc = 0;

/** Default values for args bound to the provider */
static TSP_argv_t X_tsp_argv;

/** Tell if the initialisation was done and was OK */
static int X_tsp_init_ok = FALSE;
/*-----------------------------------------------------------------*/

/**
 * TSP object.
 * A TSP_otsp_t is created for each consumer connection
 */
struct TSP_otsp_t 
{
    
  /** Handle for the command canal connection.
   * (i.e. : handle on RPC connection for the current implementation).
   */
  TSP_server_t* server;
  
  /** 
   * Server information.
   * Used by the client to provide information
   * about the producer. May be used to choose the providers that will be
   * kept opened.
   */
  TSP_otsp_server_info_t server_info; 
  
  /**
   * Channel identificator. Attributed by the provider to distinguish
   * several consumers
   */
  channel_id_t channel_id;
	
  /** 
   * List of symbols avaible in the producer.
   * The consumer must chose its symbols in this list
   */
  TSP_consumer_information_t information;
  
  /** 
   * List of symbols that were requested
   */
  TSP_consumer_symbol_requested_list_t requested_sym;

  /**
   * Groups table.
   * This group table is calculated by the provider when the symbols
   * are choosen by the consumer. The table is sent back from the provider
   * to the consumer and stored in this object
   */
  TSP_groups_t groups;
  
  /**
   * Data receiver.
   * This object is used to receive and decode the data stream from
   * the provider
   */
  TSP_data_receiver_t receiver;
  
  /**
   * Array of ringbuf for all symbols managed by server
   */
  TSP_sample_ringbuf_t* sample_fifo;
  
  /**
   * Receiver thread.
   * This thread calls the data_receiver functions to receive the symbols data
   * from the provider.
   */
  pthread_t thread_receiver;


  /** If data_link_broken = TRUE, the server is unreachable.*/	
  int data_link_broken; 

  
  
};

typedef struct TSP_otsp_t TSP_otsp_t;

/*-------------------------------------------------------------------*/

static void TSP_consumer_delete_information(TSP_otsp_t* otsp)
{
  int i;

  STRACE_IO(("-->IN"));

  for(i = 0 ; i< otsp->information.symbols.len ; i++)
    {
      free(otsp->information.symbols.val[i].name);
      otsp->information.symbols.val[i].name = 0;
    }
  free(otsp->information.symbols.val);
  otsp->information.symbols.val = 0;
  
  STRACE_IO(("-->OUT"));

}

static void TSP_consumer_delete_requested_symbol(TSP_otsp_t* otsp)
{
  int i;

  STRACE_IO(("-->IN"));

    if(otsp->requested_sym.val)
    {
      for (i = 0 ;  i < otsp->requested_sym.len ; i++)
	{
	  /* free strdup */
	  free(otsp->requested_sym.val[i].name);
	  otsp->requested_sym.val[i].name = 0;
      
	}
      free(otsp->requested_sym.val);
      otsp->requested_sym.val = 0;
    }

  STRACE_IO(("-->OUT"));

}


/**
 * Allocate a consumer object.
 * @param server handle for the command canal (RPC) for the consumer
 * @param server_info information sent by the provider about itself
 * @return the allocated consumer object
 */ 
static TSP_otsp_t* TSP_new_object_tsp(	TSP_server_t server,
					TSP_server_info_string_t server_info)
{
  TSP_otsp_t* obj;
	
  STRACE_IO(("-->IN"));

	
  obj = (TSP_otsp_t*)calloc(1, sizeof(TSP_otsp_t) );
	
  if(0 == obj)
    { 
      printf("ERROR : calloc error\n");
      return 0;
    }
	
  obj->server = server;
  strncpy(obj->server_info.info, server_info, STRING_SIZE_SERVER_INFO);
	
  /* Init */
  obj->channel_id = UNDEFINED_CHANNEL_ID;
	
  obj->information.symbols.len = 0;
  obj->information.symbols.val = 0;
  obj->requested_sym.len = 0;
  obj->requested_sym.val = 0;
  obj->groups = 0;
  obj->receiver = 0;
  obj->sample_fifo = NULL;
  obj->data_link_broken=FALSE;
	
  STRACE_IO(("-->OUT"));

	
  return obj;
}


static void TSP_delete_object_tsp(TSP_otsp_t* o)
{
  STRACE_IO(("-->IN"));

  TSP_consumer_delete_information(o);
  TSP_consumer_delete_requested_symbol(o);
  TSP_group_delete_group_table(o->groups); o->groups = 0;
  free(o);
	
  STRACE_IO(("-->OUT"));

}

static  void TSP_print_object_tsp(TSP_otsp_t* o)
{
	
  STRACE_IO(("-->IN"));
  STRACE_INFO(("----------------------------------------------"));
  STRACE_INFO(("SERVER_INFO->INFO='%s'\n", o->server_info.info));
  STRACE_INFO(("----------------------------------------------"));
  STRACE_IO(("-->OUT"));

}

/*-------------------------------------------------------------------*/

int TSP_consumer_init(int* argc, char** argv[])
{
  int i;
  int final_argc = 1;
  int found_stream_start = FALSE;
  int found_stream_stop = FALSE;
  char* p;
  int ret = TRUE;

  STRACE_IO(("-->IN"));

  X_argv = (char**)calloc(*argc, sizeof(char*));
  X_tsp_argv.TSP_argv_t_val = (char**)calloc(*argc, sizeof(char*));
  X_tsp_argv.TSP_argv_t_len = 0;
  TSP_CHECK_ALLOC(X_argv, FALSE);
  TSP_CHECK_ALLOC(X_tsp_argv.TSP_argv_t_val, FALSE);
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
	  

	  /* Look for start flag */
	  if(!strcmp(TSP_ARG_STREAM_INIT_START, (*argv)[i]))
	    {
	      if(!found_stream_stop && !found_stream_start)
		{
		  found_stream_start = TRUE;
		  /* Ok the user wants a default stream control, put
		     the first dummy element */
		  if ( 0 == X_tsp_argv.TSP_argv_t_len )
		    {
		      X_tsp_argv.TSP_argv_t_val[0] = TSP_ARG_DUMMY_PROG_NAME;
		      X_tsp_argv.TSP_argv_t_len = 1;
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
	      X_tsp_argv.TSP_argv_t_val[X_tsp_argv.TSP_argv_t_len++] = (*argv)[i];
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

  /* swap argc and argv values */
  *argc = final_argc;
  *argv = X_argv;

  STRACE_IO(("-->OUT"));

  X_tsp_init_ok = ret;
  
  /* Display usage */
  if(!ret)
    {
      STRACE_WARNING((TSP_ARG_CONSUMER_USAGE));
    }

  return ret;
  
}



void TSP_consumer_end(void)
{	

  STRACE_IO(("-->IN"));
  
  /* This is the end my friend ... the end ...*/

  /* Some day, we will find stuff to do here ;) */

   /* By the way. do ->NOT<- free X_tsp_argv and X_argv,
     the main code may be using them... */

  STRACE_INFO(("End..."));

  STRACE_IO(("-->OUT"));
}

TSP_provider_t* TSP_consumer_connect_url(const char*  url)
{	
  TSP_provider_t *provider = NULL;
  TSP_server_t server;
  TSP_server_info_string_t server_info;

  int i, servernumber;
  char url_tok[2*MAXHOSTNAMELEN], url_lkup[2*MAXHOSTNAMELEN];
  char *protocol, *hostname, *servername, *p;

  /** Parse (simply ...) URL **/ 
  protocol = NULL;
  hostname = NULL;
  servername = NULL;
  servernumber = -1;

  if(!url)
    url = "";

  bzero(url_tok, sizeof(url_tok));
  strcpy(url_tok, url);
    
  protocol = url_tok;
  p = strstr(url_tok, "://");
  if(!p)
    {
      /* set p to hostname field, if any */
      p = strstr(url_tok, "//");
      if(p) p += 2;
      else p = url_tok; /* may start of string be the hostname ?! */

      /* no protocol specified, use default */
      protocol = strdup(TSP_RPC_PROTOCOL);
   }
  else
    {
      /* protocol should be OK (start of URL), set p to hostname field */
      if(p == url_tok) protocol = strdup(TSP_RPC_PROTOCOL);
      *p = '\0';
      p += 3;
    }

  hostname = p;
  p = strstr(hostname, "/");
  if(p == hostname)
    {
      /* no hostname provided, use default & set p to server name field */
      hostname = strdup("localhost");
      p += 1;
    }
  else if(!p)
    {
      /* end of string ... hostname should be OK, set p to the end */
      p = hostname + strlen(hostname);
    }
  else
    {
      /* hostname is OK, set p to server name field */
      *p = '\0';
      p += 1;
    }

  servername = p;
  p = strstr(servername, ":");
  if(!p)
    {
      /* servername should be OK (or 0 length), set p to number field */
      p = servername + strlen(servername);
    }
  else
    {
      /* servername is OK, set p to number field */
      *p = '\0';
      p += 1;
    }

  if(*p)
    {
      servernumber = atoi(p);
      if(errno == EINVAL)
	servernumber = -1;
    }


  /** Full URL, or without server name : try to connect to server number **/
  if( servernumber >= 0 )
    {
      sprintf(url_lkup, TSP_URL_FORMAT, protocol, hostname, servername, servernumber);
      STRACE_INFO(("Trying to connect to <%s>", url_lkup ));

      /* Is server name/number alive on given host on that protocol ?*/ 
      if(TSP_remote_open_server(  protocol,
				  hostname,
				  servername,
				  servernumber, 
				  &server,
				  server_info))
	{
	  /* yes, got it !!! */
	  sprintf(url_lkup, TSP_URL_FORMAT, protocol, hostname, server_info, servernumber);
	  return (TSP_provider_t*)TSP_new_object_tsp(server, url_lkup);
	}
      
      STRACE_ERROR(("No TSP provider on URL <%s>", url_lkup));
      return NULL;
    }
else
    {
      /** Partial URL, without server number : try to find one, by recursion **/
      int server_max_number = TSP_get_server_max_number();
       
      for(i = 0; i < server_max_number; i++)
	{
	  sprintf(url_lkup, TSP_URL_FORMAT, protocol, hostname, servername, i);
	  provider = TSP_consumer_connect_url(url_lkup);
	  if(provider)
	    return provider;
	}
      STRACE_ERROR(("No TSP provider based on URL <%s>", url));
      return NULL;
      
    }

  STRACE_ERROR(("Cannot parse such URL %s", url));
  return NULL;
}

void TSP_consumer_disconnect_one(TSP_provider_t provider)
{	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;

  STRACE_IO(("-->IN"));
  
  TSP_remote_close_server(otsp->server);
  TSP_delete_object_tsp(otsp);
  	
  STRACE_IO(("-->OUT"));

}



/*--- Connect/Disconnect All & Get Connected name deprecated ... ---*/

void TSP_consumer_connect_all(const char*  host_name, TSP_provider_t** providers, int* nb_providers)
{	
	
  int i;

  /* Get max number of provider allowed on any host */
  int server_max_number = TSP_get_server_max_number();

  fprintf(stderr, "\n\007This function is now deprecated, use TSP_consumer_connect_url instead\007\n");

  STRACE_IO(("-->IN"));
	
  *nb_providers = 0;
	
  if( server_max_number > 0 )
    {
      *providers = (TSP_provider_t*)calloc(server_max_number,sizeof(TSP_provider_t));
      TSP_CHECK_ALLOC(providers,);
		
      /* Iterate on all providers, and try to contact them */
      for(i = 0 ; i < server_max_number ; i++)
	{
	  TSP_server_t server;
	  TSP_server_info_string_t server_info;

	  STRACE_DEBUG(("Trying to open server No %d", i));

	  /* Is server number 'i' alive ?*/ 
	  if(TSP_remote_open_server(  TSP_RPC_PROTOCOL,
				      host_name,
				      "",
				      i, 
				      &server,
				      server_info))
	    {
			  
	      (*providers)[*nb_providers] = TSP_new_object_tsp(server, server_info);
	      if( 0 == (*providers)[*nb_providers])
		{
		  STRACE_ERROR(("TSP_new_object_tsp failedfor No=%d", i));
		  (*nb_providers) = 0;
		  return;

		}
	      (*nb_providers)++;
				
	    }
	  else
	    {
	      STRACE_DEBUG(("unable to open server No %d for target '%s'", i, host_name));
	    }
				
	}
    }
  else
    {
      STRACE_ERROR(("Unable to get server max number"));
    }

  STRACE_INFO(("%d server opened", *nb_providers));
  STRACE_IO(("-->OUT"));

}


void TSP_consumer_disconnect_all(TSP_provider_t providers[])
{	
  int server_max_number;
  int i;
	
  STRACE_IO(("-->IN"));

  fprintf(stderr, "\n\007This function is now deprecated, use TSP_consumer_disconnect_one instead\007\n");

  server_max_number = TSP_get_server_max_number();
  if( server_max_number > 0 )
    {
		
      for(i = 0 ; i < server_max_number ; i++)
	{
	  if(providers[i])
	    {
	      TSP_consumer_disconnect_one(providers[i]);
	      providers[i] = 0;
	    }
				
	}
    }
  else
    {
      STRACE_ERROR(("Unable to get server max number"));
    }
  
  free(providers);
	
  STRACE_IO(("-->OUT"));

}

/*--- End of deprecated functions ---*/

const char* TSP_consumer_get_connected_name(TSP_provider_t provider)			  
{
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
  return(otsp->server_info.info);
}


int TSP_consumer_request_open(TSP_provider_t provider, int custom_argc, char* custom_argv[])
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  TSP_request_open_t req_open;
  TSP_answer_open_t* ans_open = 0;
  int ret = FALSE;
	
  STRACE_IO(("-->IN"));
  assert(X_tsp_init_ok);
	
  req_open.version_id = TSP_VERSION;

  /* Default argv to command line (may be empty) */
  req_open.argv = X_tsp_argv;

  /* Does the user want a specific argv  value ? */
  if( ( 0 != custom_argc)  && custom_argv)
    {
      /* Check if a command line exists and trace a warning */
      if( 0 != X_tsp_argv.TSP_argv_t_len )
	{
	  STRACE_WARNING(("Overiding command line stream initialisation by custom stream initialisation")); 
	}
      req_open.argv.TSP_argv_t_val = custom_argv;
      req_open.argv.TSP_argv_t_len = custom_argc;
    }

  	
  if(0 != otsp)
    {
      ans_open = TSP_request_open(&req_open, otsp->server);
      if( NULL != ans_open)
	{

	  switch (ans_open->status)
	    {
	    case TSP_STATUS_OK :
	      otsp->channel_id = ans_open->channel_id;
	      ret = TRUE;
	      break;
	    case TSP_STATUS_ERROR_SEE_STRING :
	      STRACE_WARNING(("Provider error : %s", ans_open->status_str));
	      break;
	    case TSP_STATUS_ERROR_UNKNOWN :
	      STRACE_WARNING(("Provider unknown error"));
	      break;
	    case TSP_STATUS_ERROR_VERSION :
	      STRACE_WARNING(("Provider version error"));
	      break;
	    default:
	      STRACE_ERROR(("The provider sent an unreferenced error. It looks like a bug."));
	      break;
	    }
	}
      else
	{
	  STRACE_ERROR(("Unable to communicate with the provider"));

	}
		
    }
  else
    {
      STRACE_ERROR(("This provider need to be remote_opened first"));

    }
	
  STRACE_IO(("-->OUT"));

	
  return ret;
	
}

int TSP_consumer_request_close(TSP_provider_t provider)
{
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  TSP_request_close_t req_close;
  int ret = FALSE;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_SESSION(otsp, FALSE);
	
  req_close.version_id = TSP_VERSION;
  req_close.channel_id = otsp->channel_id;
	
  STRACE_DEBUG(("Trying to close channel_id=%u", otsp->channel_id));

	
  ret = TSP_request_close(&req_close, otsp->server);
  if( FALSE == ret)
    {
      STRACE_DEBUG(("Unable to close channel_id=%u", otsp->channel_id));

    }
  else
    {
      STRACE_DEBUG(("channel_id=%u is closed", otsp->channel_id));

    }

  STRACE_IO(("-->OUT"));

	
  return ret;
	
}



int TSP_consumer_request_information(TSP_provider_t provider)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  TSP_request_information_t req_info;
  TSP_answer_sample_t* ans_sample = 0;
  int ret = FALSE;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_SESSION(otsp, FALSE);

  /* Delete allocation of any previous call */
  TSP_consumer_delete_information(otsp);
	
  req_info.version_id = TSP_VERSION;
  req_info.channel_id = otsp->channel_id;
	
  /* Ask the provider for informations */
  ans_sample = TSP_request_information(&req_info, otsp->server);
    
  if( NULL != ans_sample)
    {
      
      switch (ans_sample->status)
	{
	case TSP_STATUS_OK :
	  ret = TRUE;
	  break;
	case TSP_STATUS_ERROR_UNKNOWN :
	  STRACE_WARNING(("Provider unknown error"));
	  break;
	case TSP_STATUS_ERROR_VERSION :
	  STRACE_WARNING(("Provider version error"));
	  break;
	default:
	  STRACE_ERROR(("The provider sent an unreferenced error. It looks like a bug."));
	  break;
	}
    }

  /* Save all thoses sample data in memory */
  if( TRUE == ret )
    {
      unsigned int symbols_number =
	ans_sample->symbols.TSP_sample_symbol_info_list_t_len;
      unsigned int i;
	

      otsp->information.base_frequency = ans_sample->base_frequency;
      otsp->information.max_period = ans_sample->max_period;
      otsp->information.max_client_number = ans_sample->max_client_number;
      otsp->information.current_client_number = ans_sample->current_client_number;
			
      STRACE_DEBUG(("Total number of symbols found = %d",symbols_number));
      STRACE_INFO(("Provider base frequency = %f Hz", ans_sample->base_frequency));

      /* allocate memory to store those symbols */
      otsp->information.symbols.len = symbols_number;
      if(symbols_number > 0)
	{
	  otsp->information.symbols.val = 
	    (TSP_consumer_symbol_info_t* )calloc(symbols_number,sizeof(TSP_consumer_symbol_info_t));
	  TSP_CHECK_ALLOC(otsp->information.symbols.val, FALSE);
		
	  for(i = 0 ; i< symbols_number ; i++)
	    {		
	      otsp->information.symbols.val[i].index =
		ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index;
	      otsp->information.symbols.val[i].name =
		strdup(ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name);				
	      TSP_CHECK_ALLOC(otsp->information.symbols.val[i].name, FALSE);			
	    }
        }
    }
  else
    {
      STRACE_ERROR(("Unable to communicate with the provider"));

    }
		
	
  STRACE_IO(("-->OUT"));

	
  return ret;
	
}


const TSP_consumer_information_t*  TSP_consumer_get_information(TSP_provider_t provider)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_SESSION(otsp, 0);
	
  STRACE_IO(("-->OUT"));

	
  return &(otsp->information);
	

}

static int TSP_consumer_store_requested_symbols(TSP_consumer_symbol_requested_list_t* stored_sym,
						TSP_sample_symbol_info_list_t* new_sym)
{

  int i;
  if(stored_sym->val)
    {
      for (i = 0 ;  i < stored_sym->len ; i++)
	{
	  /* free strdup */
	  free(stored_sym->val[i].name);
	}
      free(stored_sym->val);
    }
  stored_sym->len = new_sym->TSP_sample_symbol_info_list_t_len;	
  stored_sym->val = 
    (TSP_consumer_symbol_requested_t* )calloc(stored_sym->len,
					      sizeof(TSP_consumer_symbol_requested_t));
  TSP_CHECK_ALLOC(stored_sym->val, FALSE);
		
  /* For each requested symbol, we store it */
  for(i = 0 ; i< stored_sym->len ; i++)
    {		
      /* FIXME : ajouter les autres valeurs */
      stored_sym->val[i].name = strdup(new_sym->TSP_sample_symbol_info_list_t_val[i].name);
      TSP_CHECK_ALLOC(stored_sym->val[i].name, FALSE);
      stored_sym->val[i].index = new_sym->TSP_sample_symbol_info_list_t_val[i].provider_global_index;
      stored_sym->val[i].phase = new_sym->TSP_sample_symbol_info_list_t_val[i].phase;
      stored_sym->val[i].period = new_sym->TSP_sample_symbol_info_list_t_val[i].period;
    }
  return TRUE;
}


int TSP_consumer_request_sample(TSP_provider_t provider, TSP_consumer_symbol_requested_list_t* symbols)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  int ret = FALSE;
  TSP_answer_sample_t* ans_sample = 0;
  TSP_request_sample_t req_sample;
  int i;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_SESSION(otsp, FALSE);
	
  req_sample.version_id = TSP_VERSION;
  req_sample.channel_id = otsp->channel_id;
  req_sample.symbols.TSP_sample_symbol_info_list_t_len = symbols->len;
  req_sample.symbols.TSP_sample_symbol_info_list_t_val = 
    (TSP_sample_symbol_info_t*)calloc(symbols->len, sizeof(TSP_sample_symbol_info_t));
  TSP_CHECK_ALLOC(req_sample.symbols.TSP_sample_symbol_info_list_t_val, FALSE);

  for(i = 0 ; i <  symbols->len ; i++)
    {

      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index = -1;
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].period = symbols->val[i].period;
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].phase = symbols->val[i].phase;
      req_sample.symbols.TSP_sample_symbol_info_list_t_val[i].name = symbols->val[i].name;
    }
	
  /* Get the computed ans_sample from the provider */
  ans_sample = TSP_request_sample(&req_sample, otsp->server);

  /*free allocated request sample symbol list */
  free(req_sample.symbols.TSP_sample_symbol_info_list_t_val);  
       
  if( 0 != ans_sample)
    {      
      
      switch (ans_sample->status)
	{
	case TSP_STATUS_OK :
	  ret = TRUE;
	  break;
	case TSP_STATUS_ERROR_UNKNOWN :
	  STRACE_WARNING(("Provider unknown error"));
	  break;
	case TSP_STATUS_ERROR_VERSION :
	  STRACE_WARNING(("Provider version error"));
	  break;
	case TSP_STATUS_ERROR_SYMBOLS :
	  STRACE_WARNING(("Provider symbols error"));
	  break;
	default:
	  STRACE_ERROR(("The provider sent an unreferenced error. It looks like a bug."));
	  break;
	}


      /*-------------------------------------------------*/
      /* Create group table and store requested symbols */
      /*-------------------------------------------------*/
      if(ret)
	{
	  STRACE_INFO(("Total groupe number = %d", ans_sample->provider_group_number));
	  /* Create group table but delete any previous allocation*/
	  TSP_group_delete_group_table(otsp->groups);
	  otsp->groups = TSP_group_create_group_table(&(ans_sample->symbols), ans_sample->provider_group_number);
	  if( 0 != otsp->groups)
	    {
	      ret = TSP_consumer_store_requested_symbols(&otsp->requested_sym,&ans_sample->symbols);
	    }
	  else
	    {
	      STRACE_ERROR(("Function TSP_group_create_group_table failed"));

	    }
	}
    }
  else
    {
      STRACE_ERROR(("Unable to communicate with the provider"));

    }
    
	
  STRACE_IO(("-->OUT"));

	
  return ret;
}

const TSP_consumer_symbol_requested_list_t* TSP_consumer_get_requested_sample(TSP_provider_t provider)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
  STRACE_IO(("-->IN"));
	
  TSP_CHECK_SESSION(otsp, 0);
	
  STRACE_IO(("-->OUT"));
  
  if(otsp->requested_sym.val)
    return &(otsp->requested_sym);
  else
    {
      STRACE_ERROR(("TSP_consumer_request_sample must be called first"));
      return 0;
    }
}

static void* TSP_request_provider_thread_receiver(void* arg)
{
    
  TSP_otsp_t* otsp = (TSP_otsp_t*)arg;
  int is_fifo_full;  
                    
  STRACE_IO(("-->IN"));
  STRACE_INFO(("Receiver thread started. Id=%u", pthread_self())); 

  while(TRUE)
    {
      if(TSP_data_receiver_receive(otsp->receiver, otsp->groups, otsp->sample_fifo, &is_fifo_full))
	{
	  /* Fifo is full, wait for user thread to free room for new data */
	  if(is_fifo_full)
	    {
	      tsp_usleep(TSP_RECEIVER_THREAD_WAIT_FIFO_FULL);      
	    }
	}
      else
        {
	  STRACE_INFO(("function TSP_data_receiver_receive returned FALSE. End of Thread"));
	  break;
        }
 
    }
  
  STRACE_IO(("-->OUT"));
  return (void*)NULL;
}

int TSP_consumer_request_sample_init(TSP_provider_t provider, TSP_sample_callback_t callback, void* user_data)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  int ret = FALSE;
  TSP_answer_sample_init_t* ans_sample = 0;
  TSP_request_sample_init_t req_sample;
	
  STRACE_IO(("-->IN"));
  
  
  TSP_CHECK_SESSION(otsp, FALSE);
  
  req_sample.version_id = TSP_VERSION;
  req_sample.channel_id = otsp->channel_id;
  
  ans_sample = TSP_request_sample_init(&req_sample, otsp->server);
  
  if( ans_sample)
    {
      STRACE_DEBUG(("data_address = '%s'", ans_sample->data_address));
      
      /* Create the data receiver */
      otsp->receiver = TSP_data_receiver_create(ans_sample->data_address, callback, user_data);
      
      if(otsp->receiver)
	{
	  int status;

	  /* Create receiver fifo */
	  RINGBUF_PTR_INIT(TSP_sample_ringbuf_t,
			   otsp->sample_fifo,
			   TSP_sample_t,
			   0,
			   RINGBUF_SZ(TSP_CONSUMER_RINGBUF_SIZE) )
	  	    
	    
	  /* Begin to receive */
	  status = pthread_create(&(otsp->thread_receiver), 
				  NULL,
				  TSP_request_provider_thread_receiver,
				  otsp);
	  
	  TSP_CHECK_THREAD(status, FALSE);
	  ret = TRUE;
	  
	}
      else
	{
	  STRACE_ERROR(("Unable to create data receiver"));
	  
	}
    }
  else
    {
      STRACE_ERROR(("Unable to communicate with the provider"));
    }
  
  STRACE_IO(("-->OUT"));

	
  return ret;
}

int TSP_consumer_request_sample_destroy(TSP_provider_t provider)
{
	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  int ret = FALSE;
  TSP_answer_sample_destroy_t* ans_sample = 0;
  TSP_request_sample_destroy_t req_sample;
	
  STRACE_IO(("-->IN"));
  
  
  TSP_CHECK_SESSION(otsp, FALSE);
  
  req_sample.version_id = TSP_VERSION;
  req_sample.channel_id = otsp->channel_id;  
  
  /* turn all alarms off, coz the provider is going to break its socket */
  TSP_data_receiver_prepare_stop(otsp->receiver);

  /* break the provider socket !  */
  ans_sample = TSP_request_sample_destroy(&req_sample, otsp->server);  
  if(ans_sample)    
    {
      switch (ans_sample->status)
	{
	case TSP_STATUS_OK :
	  ret = TRUE;
	  break;
	case TSP_STATUS_ERROR_UNKNOWN :
	  STRACE_WARNING(("Provider unknown error"));
	  break;
	case TSP_STATUS_ERROR_VERSION :
	  STRACE_WARNING(("Provider version error"));
	  break;
	default:
	  STRACE_ERROR(("The provider sent an unreferenced error. It looks like a bug."));
	  break;
	}
    }
  else
    {
      ret = FALSE;
      STRACE_WARNING(("Unable to communicate with the provider"));            
    }
  
    /* Destroy our socket */
  TSP_data_receiver_stop(otsp->receiver);
  
  /* Wait for receiver thread to end */
  pthread_join(otsp->thread_receiver, NULL);

  /* OK, the thread is stopped. We can desallocate */
  TSP_data_receiver_destroy(otsp->receiver);

  /*Destroy ringbuf*/
  RINGBUF_PTR_DESTROY(otsp->sample_fifo);
  
  STRACE_IO(("-->OUT"));
	
  return ret;
}


int TSP_consumer_read_sample(TSP_provider_t provider, TSP_sample_t* sample, int* new_sample)
{

  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  int ret = TRUE;
    
  assert(otsp->sample_fifo!=0);

  *new_sample =  !(RINGBUF_PTR_ISEMPTY(otsp->sample_fifo)) ;
  if (*new_sample)
    {
      RINGBUF_PTR_NOCHECK_GET(otsp->sample_fifo ,(*sample));

      /* end of stream ? */
      if ( sample->provider_global_index < 0 )
	{
	  ret = FALSE;
	  
	  /* GIGAFIXME : We need some kind of get_last_error to read
	     these values thrue the consumer API */
	  STRACE_INFO(("Received status message %X",  sample->provider_global_index));
	  switch(sample->provider_global_index)
	    {
	    case TSP_DUMMY_PROVIDER_GLOBAL_INDEX_EOF :
	      STRACE_INFO (("status message EOF"));
	      /* FIXME : get last error à gerer ? */
	      break;
	    case TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECONF :
	      STRACE_INFO (("status message RECONF"));
	      /* FIXME : get last error à gerer ? */
	      break;
	    case TSP_DUMMY_PROVIDER_GLOBAL_INDEX_RECEIVER_ERROR :
	      STRACE_WARNING (("status message RECEIVER ERROR"));
	      /* FIXME : get last error à gerer ? */
	      break;
	    case TSP_DUMMY_PROVIDER_GLOBAL_INDEX_GLU_DATA_LOST :
	      STRACE_WARNING (("status message GLU DATA LOST. Some data were lost by the GLU on the provider side. "
			       "is the provider too slow ?"));
	      /* FIXME : get last error à gerer ? */
	      break;
	    case TSP_DUMMY_PROVIDER_GLOBAL_INDEX_CONSUMER_DATA_LOST :
	      STRACE_WARNING (("status message CONSUMER DATA LOST. Some data were lost for this consumer"
			       " on the provider side. Is the consumer too slow, or"
			       " the network overloaded ?"));
	      /* FIXME : get last error à gerer ? */
	      break;
	    default:
	      STRACE_ERROR (("Unknown status message"));
	      /* FIXME : get last error à gerer ? */
	    }
	}
    }
      
  return ret;
}

TSP_groups_t TSP_test_get_groups(TSP_provider_t provider)
{
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  
  return otsp->groups;
}

