/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_consumer.c,v 1.2 2002-08-28 09:15:24 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Main implementation for the TSP consumer library

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>

#include "tsp_consumer.h"
#include "tsp_client.h"
#include "tsp_group.h"
#include "tsp_data_receiver.h"
#include "tsp_sample_ringbuf.h"

 

/**
 * Quick check for a session.
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
	

/**
 * TSP object.
 */
struct TSP_otsp_t 
{
  /* FIXME : avec un systeme de variables à enum indiquant chaque etat
     de la session (request fait, sample fait, sample_int fait, gerer les erreur
     si le client ne respecte pas les ordres d'appel*/
    
  /** Handle sur serveur RPC (ou autre chose Sockets...)*/
  TSP_server_t* server;
  
  /** Informations sur le serveur*/
  TSP_otsp_server_info_t server_info; 
  
  /** Numero de channel */
  channel_id_t channel_id;
	
  /** Liste des symboles disponibles */
  TSP_sample_symbol_info_list_t symbols;
  
  /** Groups table */
  TSP_groups_t groups;
  
  /** receiver */
  TSP_data_receiver_t receiver;
  
  /** Array of ringbuf for all symbols managed by server */
  TSP_sample_ringbuf_t** sample_ringbuf;
  
  /** Thread receving data socket*/
  pthread_t thread_receiver;

  /** If data_link_broken = TRUE, the server is unreachable */	
  int data_link_broken; 

  
};

typedef struct TSP_otsp_t TSP_otsp_t;

/*-------------------------------------------------------------------*/

static TSP_otsp_t* TSP_new_object_tsp(	TSP_server_t server,
		  		TSP_server_info_string_t server_info)
{
	SFUNC_NAME(TSP_new_object_tsp);

	
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
	
	obj->symbols.TSP_sample_symbol_info_list_t_len = 0;
	obj->symbols.TSP_sample_symbol_info_list_t_val = 0;
	obj->groups = 0;
	obj->receiver = 0;
	obj->sample_ringbuf = NULL;
	obj->data_link_broken=FALSE;
	
	STRACE_IO(("-->OUT"));

	
	return obj;
}

static void TSP_delete_object_tsp(TSP_otsp_t* o)
{

	SFUNC_NAME(TSP_delete_object_tsp);

	
	STRACE_IO(("-->IN"));

	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	/* A faire : desallocation de la liste des symboles */
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

	free(o);
	
	STRACE_IO(("-->OUT"));

}

static  void TSP_print_object_tsp(TSP_otsp_t* o)
{
	SFUNC_NAME(TSP_print_object_tsp);

	
	STRACE_IO(("-->IN"));

	STRACE_INFO(("----------------------------------------------"));

	STRACE_INFO(("SERVER_INFO->INFO='%s'\n", o->server_info.info));

	STRACE_INFO(("----------------------------------------------"));

	
	STRACE_IO(("-->OUT"));

}

/*-------------------------------------------------------------------*/

/**
* Open all providers.
* @param target_name the host where the providers must be found
* @param providers pointer on an array of providers
* @param nb_providers total number of providers found
*/
void TSP_open_all_provider(const char* target_name, TSP_provider_t** providers, int* nb_providers)
{	
	SFUNC_NAME(TSP_remote_open_all_provider);
	
	int i;
	int server_max_number = TSP_get_server_max_number();

	STRACE_IO(("-->IN"));
	
	*nb_providers = 0;
	
	if( server_max_number > 0 )
	{
		*providers = (TSP_provider_t*)calloc(server_max_number,sizeof(TSP_provider_t));
		TSP_CHECK_ALLOC(providers,)
		
		for(i = 0 ; i < server_max_number ; i++)
		{
			TSP_server_t server;
			TSP_server_info_string_t server_info;

			STRACE_DEBUG(("Trying to open server No %d", i));
			
			if(TSP_remote_open_server(  target_name,
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
				STRACE_DEBUG(("unable to open server No %d for target '%s'", i, target_name));
			}
				
		}
	}
	else
	{
		STRACE_ERROR(("Unable to get server max number"));
	}

	/* Realloc size of the found servers */
	*providers = (TSP_provider_t*)realloc(*providers,sizeof(TSP_provider_t)*(*nb_providers));
	if(*nb_providers > 0)
	{
	    TSP_CHECK_ALLOC((*providers),);
	}

	STRACE_INFO(("%d server opened", *nb_providers));
	STRACE_IO(("-->OUT"));

}

/**
* Close all providers.
* @param provider the provider that must be close.
*/
void TSP_close_all_provider(TSP_provider_t providers[])
{	
	SFUNC_NAME(TSP_remote_close_all_provider);

	int server_max_number;
	int i;
	
	STRACE_IO(("-->IN"));
	
	server_max_number = TSP_get_server_max_number();
	if( server_max_number > 0 )
	{
		
		for(i = 0 ; i < server_max_number ; i++)
		{
			TSP_close_provider(providers[i]);
			providers[i] = 0;
				
		}
	}
	else
	{
		STRACE_ERROR(("Unable to get server max number"));
	}
	
	free(providers);
	
	STRACE_IO(("-->OUT"));

}

/**
* Close a provider.
* @param provider the provider that must be close.
*/
void TSP_close_provider(TSP_provider_t provider)
{	
	SFUNC_NAME(TSP_close_provider);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
	STRACE_IO(("-->IN"));

	
	if(otsp)
	{
		TSP_remote_close_server(otsp->server);
		TSP_delete_object_tsp(otsp);
	}
	
	STRACE_IO(("-->OUT"));

}

/**
* Get the provider information string
* @param provider the provider from which the string must be read
* @return The information structure for the provider
*/
const TSP_otsp_server_info_t* TSP_get_provider_simple_info(TSP_provider_t provider)			  
{
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
	return(&(otsp->server_info));
}

void TSP_print_provider_info(TSP_provider_t provider)
{
	SFUNC_NAME(TSP_print_provider_info);

	
	if(provider) TSP_print_object_tsp((TSP_otsp_t*)provider);
	else STRACE_ERROR(("provider empty"));
}

/**
* Open the session for a remote_opened provider.
* @param provider the provider on which apply the action
* @return The action result (TRUE or FALSE)
*/
int TSP_request_provider_open(TSP_provider_t provider)
{
	
	SFUNC_NAME(TSP_request_provider_open);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	TSP_request_open_t req_open;
	TSP_answer_open_t* ans_open = 0;
	int ret = FALSE;
	
	STRACE_IO(("-->IN"));

	
	req_open.version_id = TSP_VERSION;

	
	if(0 != otsp)
	{
		ans_open = TSP_request_open(&req_open, otsp->server);
		if( NULL != ans_open)
		{
			otsp->channel_id = ans_open->channel_id;
			ret = TRUE;
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

/**
* Close the session for a provider after a TSP_request_provider_open.
* @param provider the provider on which apply the action
* @return The action result (TRUE or FALSE)
*/
int TSP_request_provider_close(TSP_provider_t provider)
{
	
	SFUNC_NAME(TSP_request_provider_close);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	TSP_request_close_t req_close;
	int ret = FALSE;
	
	STRACE_IO(("-->IN"));

	
	TSP_CHECK_SESSION(otsp, FALSE);
	
	req_close.version_id = TSP_VERSION;
	req_close.channel_id = otsp->channel_id;
	
	STRACE_DEBUG(("Trying to close channel_id=%"G_GUINT64_FORMAT, otsp->channel_id));

	
	ret = TSP_request_close(&req_close, otsp->server);
	if( FALSE == ret)
	{
		STRACE_DEBUG(("Unable to close channel_id=%"G_GUINT64_FORMAT, otsp->channel_id));

	}
	else
	{
		STRACE_DEBUG(("channel_id=%"G_GUINT64_FORMAT" is closed", otsp->channel_id));

	}
	
	STRACE_IO(("-->OUT"));

	
	return ret;
	
}

/**
* Request sample symbol information list.
* @param provider the provider on which apply the action
* @return The action result (TRUE or FALSE)
*/
int TSP_request_provider_information(TSP_provider_t provider)
{
	
	SFUNC_NAME(TSP_request_provider_information);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	TSP_request_information_t req_info;
	TSP_answer_sample_t* ans_sample = 0;
	int ret = FALSE;
	int i;
	
	STRACE_IO(("-->IN"));

	
	TSP_CHECK_SESSION(otsp, FALSE);
	
	req_info.version_id = TSP_VERSION;
	req_info.channel_id = otsp->channel_id;
	
	ans_sample = TSP_request_information(&req_info, otsp->server);
    
    /* Save all thoses sample data in memory */
	if( NULL != ans_sample)
	{
		unsigned int symbols_number =
			ans_sample->symbols.TSP_sample_symbol_info_list_t_len;
		unsigned int i;
	
		otsp->symbols.TSP_sample_symbol_info_list_t_len = symbols_number;
			
		STRACE_DEBUG(("Total number of symbols found = %d",symbols_number));

			
		otsp->symbols.TSP_sample_symbol_info_list_t_val = 
			(TSP_sample_symbol_info_t* )calloc(1,symbols_number*sizeof(TSP_sample_symbol_info_t));
		TSP_CHECK_ALLOC(otsp->symbols.TSP_sample_symbol_info_list_t_val, FALSE);

		
		for(i = 0 ; i< symbols_number ; i++)
		{
			otsp->symbols.TSP_sample_symbol_info_list_t_val[i] = 
				ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i];
				
			otsp->symbols.TSP_sample_symbol_info_list_t_val[i].name =
				strdup(ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name);
				
			TSP_CHECK_ALLOC(otsp->symbols.TSP_sample_symbol_info_list_t_val[i].name, FALSE);

			
		}
        
        /* Create the array of pointers for the samples ring buffer (one ring buf
        for each symbol)*/
        /* FIXME : desallouer */
        otsp->sample_ringbuf = (TSP_sample_ringbuf_t**)calloc(symbols_number, sizeof(TSP_sample_ringbuf_t*));
        TSP_CHECK_ALLOC(otsp->sample_ringbuf, FALSE);

        
			
		ret = TRUE;
	}
	else
	{
		STRACE_ERROR(("Unable to communicate with the provider"));

	}
		
	
	STRACE_IO(("-->OUT"));

	
	return ret;
	
}

/**
* Get sample symbol information list.
* The list must habe been requested first with TSP_request_provider_information
* @param provider the provider on which apply the action
* @return The symbol list.
*/
TSP_sample_symbol_info_list_t*  TSP_get_provider_information(TSP_provider_t provider)
{
	SFUNC_NAME(TSP_get_provider_information);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	
	STRACE_IO(("-->IN"));

	
	TSP_CHECK_SESSION(otsp, 0);
	
	STRACE_IO(("-->OUT"));

	
	return &(otsp->symbols);
	

}

/**
* Configure the list of symbols that will be sampled.
* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
* MISSING : consumer_timeout & feature_words
* @param provider the provider on which apply the action
* @return The action result (TRUE or FALSE)
*/
int TSP_request_provider_sample(TSP_request_sample_t* req_sample, TSP_provider_t provider)
{
	SFUNC_NAME(TSP_request_provider_sample);

	
	TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
	int ret = FALSE;
	TSP_answer_sample_t* ans_sample = 0;
    int i;
	
	STRACE_IO(("-->IN"));

	
	TSP_CHECK_SESSION(otsp, FALSE);
	
	req_sample->version_id = TSP_VERSION;
	req_sample->channel_id = otsp->channel_id;
	
	ans_sample = TSP_request_sample(req_sample, otsp->server);
    
    for( i = 0 ; i< ans_sample->symbols.TSP_sample_symbol_info_list_t_len ; i++)
    {
        STRACE_DEBUG(("N=%s Id=%d Gr=%d Rank=%d", 
        ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name,
        ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
        ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
        ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank))    
    }
    STRACE_DEBUG(("NOMBRE DE GROUPES = %d", ans_sample->provider_group_number));

    
	if( 0 != ans_sample)
	{
    
        /* Create group table */
        /* FIXME : faire la desallocation */
        otsp->groups = TSP_group_create_group_table(&(ans_sample->symbols), ans_sample->provider_group_number);
        if( 0 != otsp->groups)
        {
            ret = TRUE;
	    
            /* Create ring buffer arrays per asked symbol (n asked symbols => n ring buf )*/
            TSP_sample_ringbuf_create(&(ans_sample->symbols),otsp->sample_ringbuf);
	    
		    
        }
        else
        {
            STRACE_ERROR(("Function TSP_group_create_group_table failed"));

        }
	}
	else
	{
		STRACE_ERROR(("Unable to communicate with the provider"));

	}
    
	
	STRACE_IO(("-->OUT"));

	
	return ret;
}

static void* TSP_request_provider_thread_receiver(void* arg)
{
    
  SFUNC_NAME(TSP_request_provider_thread_receiver);
    
  TSP_otsp_t* otsp = (TSP_otsp_t*)arg;
    
                    
  STRACE_IO(("-->IN"));
  STRACE_INFO(("Receiver thread started. Id=%u", pthread_self())); 

    
  /* FIXME : faire l'arret */
  /* FIXME : faire le detach */
  while(TRUE)
    {
      if(!TSP_data_receiver_receive(otsp->receiver, otsp->groups, otsp->sample_ringbuf))
        {
	  STRACE_ERROR(("function TSP_data_receiver_receive failed"));
	  
	  /*FIXME : IL faudra faire autre chose sans doute */
	  break;
        }
    }
    
  STRACE_IO(("-->OUT"));
}

int TSP_request_provider_sample_init(TSP_provider_t provider)
{
  SFUNC_NAME(TSP_request_provider_sample_init);

	
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  int ret = FALSE;
  TSP_answer_sample_t* ans_sample = 0;
  TSP_request_sample_t req_sample;
  int i;
	
  STRACE_IO(("-->IN"));

	
  TSP_CHECK_SESSION(otsp, FALSE);
	
  /* FIXME : la fonction rpc correspondante doit etre simplifiee 
     en ne gardarnt que la data_adress en retour*/

  req_sample.version_id = TSP_VERSION;
  req_sample.channel_id = otsp->channel_id;
  req_sample.symbols.TSP_sample_symbol_info_list_t_len = 0;
	
  ans_sample = TSP_request_sample_init(&req_sample, otsp->server);
  STRACE_DEBUG(("data_address = '%s'", ans_sample->data_address));

  /* Create the data receiver */
  otsp->receiver = TSP_data_receiver_create(ans_sample->data_address);
    
  if(otsp->receiver)
    {
      int status;
    
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
    
  STRACE_IO(("-->OUT"));

	
  return ret;
}

int TSP_read_sample(TSP_provider_t provider, int provider_global_index, TSP_sample_t* sample, int* new_sample)
{
  SFUNC_NAME(TSP_receive);

    
  TSP_otsp_t* otsp = (TSP_otsp_t*)provider;
  TSP_sample_ringbuf_t* symbol_ringbuf = otsp->sample_ringbuf[provider_global_index];
  int ret = FALSE;
    
  STRACE_IO(("-->IN"));

  if(0 != symbol_ringbuf)
    {
      
      if ( *new_sample = !(RINGBUF_PTR_ISEMPTY(symbol_ringbuf)) )
	{
	  RINGBUF_PTR_NOCHECK_GET( symbol_ringbuf,(*sample));
	}
      
      ret = TRUE;
    }      
  else
    {
      STRACE_ERROR(("Sample ring buf does not exist for global_index %d", provider_global_index))
	}
    

  STRACE_IO(("-->OUT"));
    
  return ret;
    

}
