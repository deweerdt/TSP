/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.c,v 1.9 2002-12-05 10:54:12 tntdev Exp $

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
#include "tsp_datapool.h"


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

  TSP_datapool_t datapool;

  /** Handle on glu server instance (could be global or specific to session ) */
  GLU_handle_t glu_h;

  /** Total number of symbols in glu server */
  int symbols_number;
	
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


static TSP_session_t* TSP_get_session(channel_id_t channel_id)
{

  SFUNC_NAME(TSP_get_session);

	
  TSP_session_t* session = 0;
  int i = 0;
	
  STRACE_IO(("-->IN channel_id=%u", channel_id));
  
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

static void TSP_session_close_session(channel_id_t channel_id)
{
  SFUNC_NAME(TSP_session_close_session_by_channel);
  int i;
  int ret = FALSE;

  STRACE_IO(("-->IN"));


  for( i = 0 ;  i < X_session_nb ; i++)
    {
      if( X_session_t[i].channel_id == channel_id )
	{
	  assert(X_session_t[i].session_data);
	  free(X_session_t[i].session_data);
	  /* Fill the hole with latest element */
	  X_session_t[i] = X_session_t[X_session_nb - 1];
	  X_session_nb--;
	  ret = TRUE;
	  break;			
	}
    }	

  if(!ret)
    {
      STRACE_WARNING(("Unable to find session number %d", channel_id));
    }

  STRACE_IO(("-->OUT"));

}

static  void TSP_session_destroy_symbols_table(TSP_session_t* session)
{
  /* If there was a local datapool, we erase it */
  if(session->session_data->datapool)
    {
      TSP_local_datapool_destroy(session->session_data->datapool);
      session->session_data->datapool = 0;
    }
  
  /* erase the group table */
  TSP_group_algo_destroy_symbols_table(session->session_data->groups);
}


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


void TSP_session_close_session_by_channel(channel_id_t channel_id)
{


  SFUNC_NAME(TSP_session_close_session_by_channel);
  int i;
  int ret = FALSE;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  TSP_session_close_session(channel_id);

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

  STRACE_IO(("-->OUT"));
}

int TSP_session_get_nb_session(void)
{
  int client_number;

  SFUNC_NAME(TSP_session_get_nb_session);
  
  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);
  
  client_number = X_session_nb;
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);

  return client_number;
}

int TSP_add_session(channel_id_t* new_channel_id, GLU_handle_t glu_h)
{
  SFUNC_NAME(TSP_add_session);

  channel_id_t channel_id = (channel_id_t)(UNDEFINED_CHANNEL_ID);
  
  TSP_sample_symbol_info_list_t symbol_list;

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
  
  /* The position is always X_session_nb, 'coz' any holes in array are filled during session removal 
   see : TSP_close_session_by_channel */
  X_session_t[X_session_nb].session_data = calloc(1, sizeof(TSP_session_data_t));
  X_session_t[X_session_nb].channel_id = *new_channel_id;
  TSP_CHECK_ALLOC(X_session_t[X_session_nb].session_data, FALSE);
  
  /* Intialize members */
  X_session_t[X_session_nb].session_data->data_link_broken = FALSE;
  X_session_t[X_session_nb].session_data->datapool = 0; 
  X_session_t[X_session_nb].session_data->glu_h = glu_h; 

  /* Get symbols number */
  if(!GLU_get_sample_symbol_info_list(glu_h,&symbol_list))
    {
      STRACE_ERROR(("Function GLU_get_sample_symbol_info_list failed"));
      return FALSE;
    }
    X_session_t[X_session_nb].session_data->symbols_number = symbol_list.TSP_sample_symbol_info_list_t_len; 

  /* OK, there's a new session*/
  X_session_nb++;
  
  STRACE_INFO(("New consumer connected : channel_id=%u", *new_channel_id ));
  	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

	
  return TRUE;
}


void TSP_session_destroy_symbols_table_by_channel(channel_id_t channel_id)
{
  SFUNC_NAME(TSP_session_destroy_symbols_table_by_channel);

  TSP_session_t* session = 0;
   
  STRACE_IO(("-->IN"));
        
  TSP_LOCK_MUTEX(&X_session_list_mutex,);    
  TSP_GET_SESSION(session, channel_id,);

  TSP_session_destroy_symbols_table(session);

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

  STRACE_IO(("-->OUT"));
   
}


int TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
						TSP_answer_sample_t* ans_sample,
						int use_global_datapool)
{
  SFUNC_NAME(TSP_session_create_symbols_table_by_channel);
  	
  int ret = FALSE;
  char port[200];
  TSP_session_t* session = 0;
  TSP_datapool_t target_datapool = 0;    
    
  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

  TSP_GET_SESSION(session, req_sample->channel_id, FALSE)

  /* The functions could have been called several time, so clean up first */
  TSP_session_destroy_symbols_table(session);    
    
  /* Use global datapool, or local datapool ? */
  if(use_global_datapool)
    {
      target_datapool = TSP_global_datapool_get_instance();
    }
  else
    {
      /* Nop. Create a brand new data pool for each session 
	 we provide the channel_id of the session to the datapool */
      target_datapool = TSP_local_datapool_allocate(session->channel_id,
								    session->session_data->symbols_number,
								    session->session_data->glu_h);
      TSP_CHECK_ALLOC(target_datapool,FALSE);
      /* each session has its own datapool */
      session->session_data->datapool = target_datapool;
       
    }
    
  /* Creating group table*/
  ret  = TSP_group_algo_create_symbols_table(&(req_sample->symbols),
					     &(ans_sample->symbols), 
					     &(session->session_data->groups),
					     target_datapool);   

  
  /* Set total group number */
  if(ret)
    {
      ans_sample->provider_group_number = 
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

void TSP_session_create_symbols_table_by_channel_free_call(TSP_answer_sample_t* ans_sample)
{
    SFUNC_NAME(TSP_session_create_symbols_table_by_channel_free_call);
    
    STRACE_IO(("-->IN"));

    TSP_group_algo_create_symbols_table_free_call(&ans_sample->symbols);

    STRACE_IO(("-->OUT"));
}

int  TSP_session_get_sample_symbol_info_list_by_channel(channel_id_t channel_id,
							TSP_sample_symbol_info_list_t* symbol_list)
{

  SFUNC_NAME(TSP_session_get_sample_symbol_info_list_by_channel);

  TSP_session_t* session;
  int ret;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
	
  TSP_GET_SESSION(session, channel_id, FALSE);

  ret = GLU_get_sample_symbol_info_list(session->session_data->glu_h, symbol_list);
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);
  
  STRACE_IO(("-->OUT"));
    
  return ret;

}

int TSP_session_send_msg_ctrl_by_channel(channel_id_t channel_id, TSP_msg_ctrl_t msg_ctrl)
{
  SFUNC_NAME(TSP_session_send_data_msg_ctrl_by_channel);

  TSP_session_t* session;
  int ret;

  /* The mutex can not be kept 'coz' we are going to block other sessions */
  /*So, be carefull : the session must not be suppressed when the send is active */
  TSP_LOCK_MUTEX(&X_session_list_mutex,);
  TSP_GET_SESSION(session, channel_id,);
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

  if( (session->session_data->data_link_broken == FALSE) 
      &&  session->session_data->groups
      &&  session->session_data->sender)
    {
      if(!TSP_data_sender_send_msg_ctrl(session->session_data->sender, msg_ctrl))

	{
	  STRACE_WARNING(("Data link broken for session No %d",channel_id ));
	  session->session_data->data_link_broken = TRUE;
	}
    }

  ret = !(session->session_data->data_link_broken);
  return ret;
  
}

int TSP_session_send_data_by_channel(channel_id_t channel_id, time_stamp_t t)
{

  SFUNC_NAME(TSP_session_send_data_by_channel);

  TSP_session_t* session;
  int ret;

  /* The mutex can not be kept 'coz' we are going to block other sessions */
  /*So, warning : the session must not be suppressed when the send is active */
  TSP_LOCK_MUTEX(&X_session_list_mutex,);
  TSP_GET_SESSION(session, channel_id,);
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

  if( (session->session_data->data_link_broken == FALSE) 
      &&  session->session_data->groups
      &&  session->session_data->sender)
    {
      if(!TSP_data_sender_send(session->session_data->sender, 
			       session->session_data->groups, 
			       t))
	{
	  STRACE_WARNING(("Data link broken for session No %d",channel_id ));
	  session->session_data->data_link_broken = TRUE;
	  
	}
    }
  
  ret = !(session->session_data->data_link_broken);
  return ret;

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

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  for( i = 0 ; i< X_session_nb ; i++)
    {
    
      if( X_session_t[i].session_data->groups /* The sample request was done */
	  && X_session_t[i].session_data->sender /* The sample request init was done */
	  && (X_session_t[i].session_data->data_link_broken == FALSE))
	{
	  if(!TSP_data_sender_send(X_session_t[i].session_data->sender, 
				   X_session_t[i].session_data->groups, 
				   t))
	    {
	      STRACE_WARNING(("Data link broken for session No %d",X_session_t[i].channel_id ));
	      X_session_t[i].session_data->data_link_broken = TRUE;		  
	    }
	}
    }

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

}


/**
 * Send msg ctrl to all clients
 * This function is called by the datapool thread,
 * and for all opened session it send available data
 * to the clients.
 * @param msg_ctrl The message that must be sent
 */
void TSP_session_all_session_send_msg_ctrl(TSP_msg_ctrl_t msg_ctrl)
{

  SFUNC_NAME(TSP_session_all_session_send_msg_ctrl);
    
  int i;

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  for( i = 0 ; i< X_session_nb ; i++)
    {
      if( X_session_t[i].session_data->groups /* The sample request was done */
	  && X_session_t[i].session_data->sender /* The sample request init was done */
	  && (X_session_t[i].session_data->data_link_broken == FALSE))
	{

	  if(!TSP_data_sender_send_msg_ctrl(X_session_t[i].session_data->sender, msg_ctrl))
	    {
	      STRACE_WARNING(("Data link broken for session No %d",X_session_t[i].channel_id ));
	      X_session_t[i].session_data->data_link_broken = TRUE;	      
	    }	  
	}
    }

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
}


int TSP_session_create_data_sender_by_channel(channel_id_t channel_id, int no_fifo)
{

  SFUNC_NAME(TSP_session_create_data_sender_by_channel);

  TSP_session_t* session;
  int ret = TRUE;
  int ringbuf_size;


  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
	
  TSP_GET_SESSION(session, channel_id, FALSE);

  session->session_data->sender = 0;

  /* Calculate fifo depth */
  if(no_fifo)
    {
      ringbuf_size = 0;
    }
  else
    {
      /* There is one single data pool, ask for a ringbuf to the socket layer */
      /* We calculate it with the server frequency */
      double base_frequency = GLU_get_base_frequency();
       if( base_frequency > 0 )
	{
	  ringbuf_size = TSP_STREAM_SENDER_RINGBUF_SIZE * base_frequency;
	  
	  STRACE_DEBUG(("Stream sender ringbuf size will be : %d items (i.e. %d secondes)",
		       ringbuf_size,
		       TSP_STREAM_SENDER_RINGBUF_SIZE));
	}
      else
	{
	  STRACE_ERROR(("GLU return base frequency = %f", base_frequency));
	  ret = FALSE;
	}
    }

  /*FIXME : il ne faudrait pas lancer le worker avant le start feature : mettre le thread en etat d'attente*/

  /*--------------------*/
  /* Create data sender */
  /*--------------------*/
  if(ret)
    {
      session->session_data->sender = TSP_data_sender_create(ringbuf_size);      

      if(0 != session->session_data->sender)
	{
	  /* If there's no fifo, we must start a new thread per client, because there's one
	     datapool per client */
	  if (no_fifo)
	    {
	      ret = TSP_local_datapool_start_thread(session->session_data->datapool);
	      if(!ret)
		{
		  STRACE_ERROR(("Unable to launch local datapool worker thread"));
		}
	    }
	}
      else
	{
	  ret = FALSE;
	  STRACE_ERROR(("function TSP_data_sender_create failed"));
      
	}
    }
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);
  
  STRACE_IO(("-->OUT"));
    
  return ret;

}

int TSP_session_destroy_data_sender_by_channel(channel_id_t channel_id, int stop_local_thread)
{
    SFUNC_NAME(TSP_session_destroy_data_sender_by_channel);

  TSP_session_t* session;
  int ret = TRUE;

  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
	
  TSP_GET_SESSION(session, channel_id, FALSE);

  /* Stop the session */
  TSP_data_sender_stop(session->session_data->sender);

  /* For a pasive server we must wait for the session thread to end */
  if(stop_local_thread)
    {
      TSP_local_datapool_wait_for_end_thread(session->session_data->datapool);
    }
  
  TSP_data_sender_destroy(session->session_data->sender);
  session->session_data->sender = 0;

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


int TSP_session_is_consumer_connected_by_channel(channel_id_t channel_id)
{

  
  SFUNC_NAME(TSP_session_is_consumer_connected_by_channel);
					
  TSP_session_t* session;
  int consumer_is_connected;
    
  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
  TSP_GET_SESSION(session, channel_id, FALSE);

  consumer_is_connected = TSP_data_sender_is_consumer_connected(session->session_data->sender);

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return consumer_is_connected;

}


int TSP_session_get_symbols_global_index_by_channel(channel_id_t channel_id,
						   TSP_sample_symbol_info_list_t* symbol_list)
{
  SFUNC_NAME(TSP_session_check_requested_symbols_by_channel);
					
  TSP_session_t* session;
  TSP_sample_symbol_info_list_t origin_sym_list;
  int i,j;
  int ret = TRUE;
  STRACE_IO(("-->IN"));
    
  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
  TSP_GET_SESSION(session, channel_id, FALSE);

  /* Get the original GLU symbol list */
   if(!GLU_get_sample_symbol_info_list(session->session_data->glu_h, &origin_sym_list))
    {
      STRACE_ERROR(("Function GLU_get_sample_symbol_info_list failed"));
      return FALSE;
    }

   /* For each requested symbols, check by name, and find the provider global index */

   for( i = 0 ; i < symbol_list->TSP_sample_symbol_info_list_t_len ; i++)
     {
       int found = FALSE;
       TSP_sample_symbol_info_t* looked_for = &(symbol_list->TSP_sample_symbol_info_list_t_val[i]);
       
       for( j = 0 ; j < origin_sym_list.TSP_sample_symbol_info_list_t_len ; j++)
	 {
	   TSP_sample_symbol_info_t* compared = &(origin_sym_list.TSP_sample_symbol_info_list_t_val[j]);
	   if(!strcmp(looked_for->name, compared->name))
	     {
	       found = TRUE;
	       looked_for->provider_global_index = compared->provider_global_index;
	     }
	   if(found) break;
	 }
       if(!found)
	 {
	   ret = FALSE;
	   STRACE_INFO(("Unable to find symbol '%s'",  looked_for->name));
	   break;	   
	 }
     }
   
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));
  return ret;

}

/**
 * Returns a session that must be destroyed.
 * @param channel_id The session that must be destroyed
 * @return FALSE = there isn't any garbage session
 */
int TSP_session_get_garbage_session(channel_id_t* channel_id)
{

  SFUNC_NAME(TSP_session_get_garbage_session);
	
  int i;
  int found = FALSE;
	
  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
  
  for( i = 0 ;  i < X_session_nb ; i++)
    {
      /* FIXME : here the criteria for a dirty session is 'the data
	 link is broken', but there are plenty of criteria that should be
	 added (like 'timeout on idle consumer', and so on) */
      if( X_session_t[i].session_data->data_link_broken == TRUE )
	{
	  found = TRUE;
	  *channel_id = X_session_t[i].channel_id;
	  break;
	}
    }
	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT"));

  return found ;
}
