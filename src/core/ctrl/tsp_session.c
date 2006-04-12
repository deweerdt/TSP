/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.c,v 1.23 2006-04-12 06:56:03 erk Exp $

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
Maintainer : tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the object TSP_session_t that embody the
opened session from a client

-----------------------------------------------------------------------
 */

#ifdef __OpenBSD__
#include <machine/types.h> 
#include <machine/endian.h>
#endif /* __OpenBSD__ */

#include "tsp_sys_headers.h"

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
  GLU_handle_t* glu_h;

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
	
  TSP_session_t* session = 0;
  int i = 0;
	
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
	
  return session ;
}

static void TSP_session_close_session(channel_id_t channel_id)
{
  int i;
  int ret = FALSE;

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
  if( FALSE == X_initialized )
    {
      /* Mise a 0 de la zone memoire */
      memset(X_session_t, 0, TSP_MAX_CLIENT_NUMBER*sizeof(TSP_session_t));
      X_initialized = TRUE;
    }
}


void TSP_session_close_session_by_channel(channel_id_t channel_id)
{
  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  TSP_session_close_session(channel_id);

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
}

int TSP_session_get_nb_session(void)
{
  int client_number;

  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);
  
  client_number = X_session_nb;
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);

  return client_number;
}

int TSP_add_session(channel_id_t* new_channel_id, GLU_handle_t* glu_h)
{
  channel_id_t channel_id = (channel_id_t)(UNDEFINED_CHANNEL_ID);
  
  TSP_sample_symbol_info_list_t symbol_list;

  *new_channel_id = 0;
	
  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

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
  if(!glu_h->get_ssi_list(glu_h,&symbol_list))
    {
      STRACE_ERROR(("Function GLU_handle_t::get_ssi_list failed"));
      return FALSE;
    }
    X_session_t[X_session_nb].session_data->symbols_number = symbol_list.TSP_sample_symbol_info_list_t_len; 

  /* OK, there's a new session*/
  X_session_nb++;
  
  STRACE_INFO(("New consumer connected : channel_id=%u", *new_channel_id ));
  	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return TRUE;
}


void TSP_session_destroy_symbols_table_by_channel(channel_id_t channel_id)
{
  TSP_session_t* session = 0;
   
  TSP_LOCK_MUTEX(&X_session_list_mutex,);    
  TSP_GET_SESSION(session, channel_id,); 

  TSP_session_destroy_symbols_table(session);  

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

}


int TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
						TSP_answer_sample_t* ans_sample)

{
  int ret = FALSE;
  TSP_session_t* session = 0;
  TSP_datapool_t target_datapool = 0;    

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

  TSP_GET_SESSION(session, req_sample->channel_id, FALSE)

  /* The functions could have been called several time, so clean up first */
  TSP_session_destroy_symbols_table(session);    
    
  /* Use global datapool */
  target_datapool = TSP_global_datapool_get_instance(session->session_data->glu_h);

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

  return ret;
}

void TSP_session_create_symbols_table_by_channel_free_call(TSP_answer_sample_t* ans_sample)
{
    TSP_group_algo_create_symbols_table_free_call(&ans_sample->symbols);
}

int  TSP_session_get_sample_symbol_info_list_by_channel(channel_id_t channel_id,
							TSP_sample_symbol_info_list_t* symbol_list)
{

  TSP_session_t* session;
  int ret;

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
	
  TSP_GET_SESSION(session, channel_id, FALSE);

  ret = (session->session_data->glu_h)->get_ssi_list(session->session_data->glu_h, symbol_list);
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);
  
  return ret;

}

int TSP_session_send_msg_ctrl_by_channel(channel_id_t channel_id, TSP_msg_ctrl_t msg_ctrl)
{
  TSP_session_t* session;
  int ret;

  /* The mutex can not be kept 'coz' we are going to block other sessions */
  /* So, be carefull : the session must not be suppressed when the send is active */
  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);
  TSP_GET_SESSION(session, channel_id,-1);
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);

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

  TSP_session_t* session;
  int ret;

  /* The mutex can not be kept 'coz' we are going to block other sessions */
  /*So, warning : the session must not be suppressed when the send is active */
  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);
  TSP_GET_SESSION(session, channel_id,-1);
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);

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

void TSP_session_all_session_send_data(time_stamp_t t)
{
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


void TSP_session_all_session_send_msg_ctrl(TSP_msg_ctrl_t msg_ctrl)
{
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

  TSP_session_t* session;
  int ret = TRUE;
  int ringbuf_size;

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
      double base_frequency = (session->session_data->glu_h)->get_base_frequency(session->session_data->glu_h);
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

  /*--------------------*/
  /* Create data sender */
  /*--------------------*/
  if(ret)
    {
      int max_group_size = TSP_group_algo_get_biggest_group_size(session->session_data->groups);
      session->session_data->sender = TSP_data_sender_create(ringbuf_size, max_group_size);      

      if(0 != session->session_data->sender)
	{
	  /* If there's no fifo, we must start a new thread per client, because there's one
	     datapool per client */
	  if (no_fifo)
	    {
	      ret = TRUE; /*No more ... TSP_local_datapool_start_thread(session->session_data->datapool);*/
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
  
  return ret;

}

int TSP_session_destroy_data_sender_by_channel(channel_id_t channel_id, int stop_local_thread)
{
  TSP_session_t* session;
  int ret = TRUE;

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
	
  TSP_GET_SESSION(session, channel_id, FALSE);

  /* Stop the session */
  TSP_data_sender_stop(session->session_data->sender);

  /* For a pasive server we must wait for the session thread to end */
  if(stop_local_thread)
    {
      /*TSP_local_datapool_wait_for_end_thread(session->session_data->datapool);*/
    }
  
  TSP_data_sender_destroy(session->session_data->sender);
  session->session_data->sender = 0;

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return ret;


}

const char* TSP_session_get_data_address_string_by_channel(channel_id_t channel_id)
{

  TSP_session_t* session;
  const char* data_address;

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

	
  TSP_GET_SESSION(session, channel_id, FALSE);

  data_address = TSP_data_sender_get_data_address_string(session->session_data->sender);
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_IO(("-->OUT data_address='%s'",data_address ));

    
  return data_address;

}


int TSP_session_is_consumer_connected_by_channel(channel_id_t channel_id)
{
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
    TSP_session_t* session;
    int i;
    int ret=TRUE;
    TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
    TSP_GET_SESSION(session, channel_id, FALSE);
    GLU_handle_t* myGLU;
    
    int* pg_indexes = calloc( symbol_list->TSP_sample_symbol_info_list_t_len, sizeof(int) );
    
    if (pg_indexes == NULL) {
      STRACE_ERROR(("Unable to allocate memory for global provider index"));
      return FALSE;
    }
  
    /* Get global provider indexes */
    myGLU = session->session_data->glu_h;
    if (myGLU->get_pgi(myGLU,symbol_list,pg_indexes) == FALSE) {
      STRACE_ERROR(("Some symbols have not been found"));
      ret=FALSE;
    }
     
    TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return ret;

}

int TSP_session_get_garbage_session(channel_id_t* channel_id)
{
  int i;
  int found = FALSE;
	
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

  return found ;
}
