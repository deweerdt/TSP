/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.c,v 1.2 2002-09-19 08:36:53 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the object TSP_session_t that embody the
opened session from a client

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include <pthread.h>

#include "tsp_session.h"
#include "tsp_group_algo.h"
#include "tsp_data_sender.h"

#define TSP_GET_SESSION(session, channel_id, ret) \
{ \
	if( 0 == (session = TSP_get_session(channel_id) ) ) \
	{ \
		STRACE_ERROR(("Unable to get session for channel_id=%u",channel_id)) \
		return ret; \
	} \
}

struct TSP_session_data_t 
{
	
  version_id_t version_id;
    
  TSP_groups_t groups;
    
  TSP_data_sender_t sender;

  int data_link_broken; /**< If data_link_broken = TRUE, the client is unreachable */
	
};

typedef struct TSP_session_data_t TSP_session_data_t;

/* OBJET SESSION */
struct TSP_session_t 
{
	
  channel_id_t channel_id;
	
  TSP_session_data_t* session_data;
	
};

typedef struct TSP_session_t TSP_session_t;

static TSP_session_t X_session_t[TSP_MAX_CLIENT_NUMBER];

/** Memorize the highest session in X_session_t */
int X_session_nb = 0;

/** Used to create new channel_id for each client */
static channel_id_t X_count_channel_id = 0;

static int X_initialized = FALSE;

static pthread_mutex_t X_session_list_mutex = PTHREAD_MUTEX_INITIALIZER;

/*---------------------------------------------------------*/
/*                  FONCTIONS INTERNE  			   */
/*---------------------------------------------------------*/

static TSP_session_t* TSP_get_session(channel_id_t channel_id)
{

  SFUNC_NAME(TSP_get_session);

	
  TSP_session_t* session = 0;
  int i = 0;
	
  STRACE_IO(("-->IN channel_id=%u", channel_id));
  
  /* FIXME : pour que cela fonctionne, car une session est supprimee, il
     faut "combler les trous" dans le tableau */
  for( i = 0 ;  i < X_session_nb ; i++)
    {
      if( X_session_t[i].channel_id == channel_id )
	{
	  assert(X_session_t[i].session_data);
	  session = &(X_session_t[i]);
	  break;			
	}
    }
	
  if( 0 == session )
    { 
      STRACE_ERROR(("No session found for channel_id=%u", channel_id));

    }
	
  STRACE_IO(("-->OUT"));

  return session ;
}

static void TSP_delete_object_session_data(TSP_session_data_t* o)
{
  /*FIXME*/

  SFUNC_NAME(TSP_delete_object_session_data);

	
  STRACE_IO(("-->IN"));

  free(o);
	
  STRACE_IO(("-->OUT"));

}

static void TSP_print_object_session(TSP_session_t* o)
{
  SFUNC_NAME(TSP_print_object_tsp);

	
  STRACE_IO(("-->IN"));

  /*STRACE_INFO(("----------------------------------------------"));

    STRACE_INFO(("SERVER_INFO->INFO='%s'\n", o->server_info.info));

    STRACE_INFO(("----------------------------------------------"));

  */
  STRACE_IO(("-->OUT"));

}

/*---------------------------------------------------------*/
/*                  FONCTIONS EXTERNE  			   */
/*---------------------------------------------------------*/

void TSP_session_init(void)
{
  SFUNC_NAME(TSP_session_init);

		
  STRACE_IO(("-->IN"));

	
  if( FALSE == X_initialized )
    {
      /* Mise a 0 de la zone memoire */
      memset(X_session_t, 0, TSP_MAX_CLIENT_NUMBER*sizeof(TSP_session_t));
      X_initialized = TRUE;
    }
	
  STRACE_IO(("-->OUT"));

}

int TSP_close_session_by_channel(channel_id_t channel_id)
{

  /*FIXME*/
  SFUNC_NAME(TSP_close_session_by_channel);

  TSP_session_t* session;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

	
  TSP_GET_SESSION(session, channel_id, FALSE);

  TSP_delete_object_session_data(session->session_data);
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

    
  return TRUE;

}

int TSP_add_session(channel_id_t* new_channel_id)
{
  SFUNC_NAME(TSP_add_session);

  channel_id_t channel_id = (channel_id_t)(UNDEFINED_CHANNEL_ID);
  *new_channel_id = 0;
	
  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

  /* FIXME : Faire un realloc */
  /* Is there room left for the new session ? */
  if( X_session_nb == TSP_MAX_CLIENT_NUMBER)
    {
      STRACE_ERROR(("Max session number reached : %d", TSP_MAX_CLIENT_NUMBER));
      return FALSE;
    }

  
  /* Create a new channel Id*/
  channel_id = X_count_channel_id++;
  
  STRACE_DEBUG(("I've found room in X_session_t for the new session. Id in X_session_t is %d", X_session_nb));
  *new_channel_id = channel_id;
  
  X_session_t[X_session_nb].session_data = calloc(1, sizeof(TSP_session_data_t));
  X_session_t[X_session_nb].channel_id = *new_channel_id;
  TSP_CHECK_ALLOC(X_session_t[X_session_nb].session_data, FALSE);
  
  /* Intialize data_link_broken */
  X_session_t[X_session_nb].session_data->data_link_broken = FALSE;

  /* OK, there's a new session*/
  X_session_nb++;
  
  STRACE_INFO(("New channel_id=%u", *new_channel_id ));
  	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

	
  return TRUE;
}

int TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
						TSP_answer_sample_t** ans_sample)
{
  SFUNC_NAME(TSP_session_create_symbols_table);

	
  int ret = FALSE;
  char port[200];
    
  TSP_session_t* session = 0;
    
  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

    
  session =  TSP_get_session(req_sample->channel_id);
    
	
    
  (*ans_sample) = (TSP_answer_sample_t*)calloc(1, sizeof(TSP_answer_sample_t));
  TSP_CHECK_ALLOC((*ans_sample),FALSE);
    
   
    
  /* Creation table*/
  ret  = TSP_group_algo_create_symbols_table(&(req_sample->symbols),
					     &((*ans_sample)->symbols), 
					     &(session->session_data->groups));
  if(ret)
    {
      (*ans_sample)->provider_group_number = 
	TSP_group_algo_get_group_number(session->session_data->groups);
    }
  else
    {
      STRACE_ERROR(("Function TSP_group_algo_create_symbols_table failed"));

    }
    
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

  return ret;
    
}

void TSP_session_free_create_symbols_table_call(TSP_answer_sample_t** ans_sample)
{

  SFUNC_NAME(TSP_session_free_create_symbols_table_call);

    
  STRACE_IO(("-->IN"));

    
  /*
    TBD : Mettre la bonne desallocation
    free((*ans_sample)->symbols.TSP_sample_symbol_info_list_t_val);
    free(*ans_sample);
    (*ans_sample) = 0;
  */
    
  STRACE_IO(("-->OUT"));

}

/**
 * Send data to all clients.
 * This function is called by the datapool thread,
 * and for all opened session it send available data
 * to the clients.
 * @param t current time stamp sent with the data
 */
void TSP_session_all_session_send_data(time_stamp_t t)
{

  SFUNC_NAME(TSP_session_all_session_send_data);

    
  int i;
    
 /* STRACE_IO(("-->IN"));*/

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  for( i = 0 ; i< X_session_nb ; i++)
    {
    
      /* FIXME !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 session_data->sender sert à savoir si sample init
	 a ete fait, et session_data->groups si le sample tout
	 court a ete fait. Les remettre a 0 en cas de destruction
	 de la connection */
    
      if( X_session_t[i].session_data->groups /* The sample request was done */
	  && X_session_t[i].session_data->sender /* The sample request init was done */)
	{
                      
          assert(X_session_t[i].session_data);

	  if( X_session_t[i].session_data->data_link_broken == FALSE)
	    {
	      if(!TSP_data_sender_send(X_session_t[i].session_data->sender, 
				       X_session_t[i].session_data->groups, 
				       t))
		{
		  STRACE_ERROR(("Function TSP_data_sender_send failed"));
		  X_session_t[i].session_data->data_link_broken = TRUE;
		  
		}
	    }
            
	}
    }

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

  /*STRACE_IO(("-->OUT"));*/

}

int TSP_session_create_data_sender_by_channel(channel_id_t channel_id)
{

  SFUNC_NAME(TSP_session_create_data_sender_by_channel);

  TSP_session_t* session;
  int ret = TRUE;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

	
  TSP_GET_SESSION(session, channel_id, FALSE);

  session->session_data->sender = TSP_data_sender_create();
  if(0 == session->session_data->sender)
    {
      ret = FALSE;
      STRACE_ERROR(("function TSP_data_sender_create failed"));

    }
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

    
  return ret;

}

const char* TSP_session_get_data_address_string_by_channel(channel_id_t channel_id)
{

  SFUNC_NAME(TSP_session_get_data_address_string_by_channel);

  TSP_session_t* session;
  const char* data_address;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

	
  TSP_GET_SESSION(session, channel_id, FALSE);

  data_address = TSP_data_sender_get_data_address_string(session->session_data->sender);
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT data_address='%s'",data_address ));

    
  return data_address;

}
