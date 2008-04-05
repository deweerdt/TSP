/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_session.c,v 1.41 2008-04-05 20:05:22 erk Exp $

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

#ifdef _WIN32
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif

#include <tsp_sys_headers.h>
#include <tsp_time.h>
#include <tsp_session.h>
#include <tsp_group_algo.h>
#include <tsp_data_sender.h>
#include <tsp_datapool.h>

#define TSP_GET_SESSION(session, channel_id, ret)			\
  {									\
    if( NULL == (session = TSP_get_session(channel_id) ) )		\
      {									\
	STRACE_ERROR("Unable to get session for channel_id=%u",channel_id); \
	TSP_UNLOCK_MUTEX(&X_session_list_mutex,ret);			\
      }									\
  }

typedef struct TSP_session_data {
	
  version_id_t version_id;
    
  TSP_groups_t groups;
    
  TSP_data_sender_t sender;

  TSP_datapool_t* datapool;

  /** Handle on glu server instance (could be global or specific to session ) */
  GLU_handle_t* glu_h;

  /** Total number of symbols in glu server */
  int symbols_number;
	
} TSP_session_data_t;


/** Session Object  */
typedef struct TSP_session {
	
  channel_id_t channel_id;
  /** the session state */
  TSP_session_state_t state;
  /** the session data */
  TSP_session_data_t* session_data;
	
} TSP_session_t ;

static TSP_session_t X_session_t[TSP_MAX_CLIENT_NUMBER];

/** Memorize the highest session in X_session_t */
int X_session_nb = 0;

/** Used to create new channel_id for each client */
static channel_id_t X_count_channel_id = 0;

static int X_initialized = FALSE;

static pthread_mutex_t X_session_list_mutex = PTHREAD_MUTEX_INITIALIZER;


static TSP_session_t* 
TSP_get_session(channel_id_t channel_id) {
	
  TSP_session_t* session = NULL;
  int i = 0;
	
  for( i = 0 ;  i < X_session_nb ; i++) {
    if( X_session_t[i].channel_id == channel_id ) {
      STRACE_DEBUG("Channel_id = <%d> is session Idx <%d>", channel_id,i);
      assert(X_session_t[i].session_data);
      session = &(X_session_t[i]);
      break;			
    }
  }
  
  if( NULL == session ) { 
    STRACE_ERROR("No session found for channel_id=%u", channel_id);
  }
	
  return session ;
} /* end of TSP_get_session */

static int32_t 
TSP_session_change_stateTo(TSP_session_t* session, TSP_session_state_t newState) {
  int32_t retcode = TSP_STATUS_OK; 
  assert(session);

   /* FIXME for now we do not check invalid transition */
  switch(newState) {
  case TSP_SESSION_STATE_UNKNOWN:
  case TSP_SESSION_STATE_OPENED:
  case TSP_SESSION_STATE_CLOSED:
  case TSP_SESSION_STATE_REQUEST_SAMPLE_OK:
  case TSP_SESSION_STATE_REQUEST_SAMPLE_INIT_OK:
  case TSP_SESSION_STATE_SAMPLING:
  case TSP_SESSION_STATE_REQUEST_SAMPLE_DESTROY_OK:
  case TSP_SESSION_STATE_BROKEN_LINK:
  case TSP_SESSION_STATE_CLOSE_ON_EOF:
    STRACE_INFO("Session was in state <%d> goes to <%d>",session->state,newState);
    session->state = newState;
    break;
  default:
    STRACE_ERROR("Unknown transition from state <%d> to <%d> IGNORED.",session->state,newState);
    retcode = TSP_STATUS_ERROR_UNKNOWN; 
    break;
  }
  
  return retcode;
} /* end of TSP_session_change_stateTo */


static void 
TSP_session_close_session(channel_id_t channel_id) {
  int ret = FALSE;
  TSP_session_t* session = NULL;
  STRACE_DEBUG("Request Closing session channel_id = %d", channel_id);
  session = TSP_get_session(channel_id);
  if (NULL != session) {
    assert(session->session_data);
    free(session->session_data);
    TSP_session_change_stateTo(session,TSP_SESSION_STATE_CLOSED);
    /* Fill the hole with latest element */
    *session = X_session_t[X_session_nb - 1];
    X_session_nb--;
    STRACE_DEBUG("X_session_nb now = %d.", X_session_nb);
    ret = TRUE;
  }

  if(!ret) {
    STRACE_WARNING("Unable to find session number %d", channel_id);
  }

} /* end of TSP_session_close_session */


static void 
TSP_session_destroy_symbols_table(TSP_session_t* session) {
  /* If there was a local datapool, we erase it */
  if(NULL != session->session_data->datapool) {
    TSP_datapool_delete(&(session->session_data->datapool));
  }
  
  /* Erase the group table */
  TSP_group_algo_destroy_symbols_table(session->session_data->groups);
} /* end of TSP_session_destroy_symbols_table */

void 
TSP_session_init() {
  if( FALSE == X_initialized )
    {
      /* Mise a 0 de la zone memoire */
      memset(X_session_t, 0, TSP_MAX_CLIENT_NUMBER*sizeof(TSP_session_t));
      X_initialized = TRUE;
    }
} /* end of TSP_session_init */

GLU_handle_t* 
TSP_session_get_GLU_by_channel(channel_id_t channel_id) {
  TSP_session_t* session = NULL;
  TSP_LOCK_MUTEX(&X_session_list_mutex,NULL);
  TSP_GET_SESSION(session,channel_id,NULL);  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,NULL);
  return session->session_data->glu_h;
} /* end of TSP_session_get_GLU_by_channel */

void 
TSP_session_close_session_by_channel(channel_id_t channel_id) {
  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  TSP_session_close_session(channel_id);

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
} /* end of TSP_session_close_session_by_channel */

TSP_session_state_t 
TSP_session_get_state(channel_id_t channel_id) {

  TSP_session_t*      session;
  TSP_session_state_t retval=TSP_SESSION_STATE_UNKNOWN;

  TSP_LOCK_MUTEX(&X_session_list_mutex,retval);

  session = TSP_get_session(channel_id);
  if (NULL!=session) {
    retval= session->state;
  } 
  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,retval);
  return retval;
} /* end of TSP_session_get_state */


int 
TSP_session_get_nb_session() {
  int client_number;

  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);  
  client_number = X_session_nb;  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);
  return client_number;
} /* end of TSP_session_get_nb_session */

int32_t
TSP_add_session(channel_id_t* new_channel_id, GLU_handle_t* glu_h) {
  
  *new_channel_id = (channel_id_t)(TSP_UNDEFINED_CHANNEL_ID);
	
  TSP_LOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);

  /* Is there room left for the new session ? */
  if( X_session_nb == TSP_MAX_CLIENT_NUMBER) {
    STRACE_ERROR("Max session number reached : %d", TSP_MAX_CLIENT_NUMBER);
    /* do not forget to unlock!! */
    TSP_UNLOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);
    return TSP_STATUS_ERROR_NO_MORE_SESSION;
  }
  
  /* Create a new channel Id*/
  *new_channel_id = X_count_channel_id++;
  
  STRACE_DEBUG("I've found room in X_session_t for the new session. Id in X_session_t is %d", X_session_nb);
  
  /* The position is always X_session_nb, 
   * 'coz' any holes in array are filled during session removal 
   * see : TSP_close_session_by_channel */
  X_session_t[X_session_nb].state        = TSP_SESSION_STATE_UNKNOWN;
  X_session_t[X_session_nb].session_data = calloc(1, sizeof(TSP_session_data_t));
  X_session_t[X_session_nb].channel_id   = *new_channel_id;
  TSP_CHECK_ALLOC(X_session_t[X_session_nb].session_data, TSP_STATUS_ERROR_MEMORY_ALLOCATION);
  
  /* Initialize members */
  
  X_session_t[X_session_nb].session_data->datapool    = NULL; 
  X_session_t[X_session_nb].session_data->glu_h       = glu_h; 

  /* Get symbols number */
  X_session_t[X_session_nb].session_data->symbols_number =
    X_session_t[X_session_nb].session_data->glu_h->get_nb_symbols(X_session_t[X_session_nb].session_data->glu_h);
  
  /* OK, there's a new session*/
  X_session_nb++;
  
  STRACE_INFO("New consumer registered with TSP channel_id=%u", *new_channel_id);
  	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return TSP_STATUS_OK;
} /* end of TSP_add_session */

int32_t 
TSP_session_change_stateTo_byChannel(channel_id_t channel_id, TSP_session_state_t newState) {

  TSP_session_t * session;

  TSP_LOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);
  TSP_GET_SESSION(session,channel_id,TSP_STATUS_ERROR_INVALID_CHANNEL_ID);  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN) ;   
  return TSP_session_change_stateTo(session, newState);

} /* TSP_session_change_stateTo_byChannel */


void 
TSP_session_destroy_symbols_table_by_channel(channel_id_t channel_id) {
  TSP_session_t* session = 0;
   
  TSP_LOCK_MUTEX(&X_session_list_mutex,);    
  TSP_GET_SESSION(session, channel_id,); 
  TSP_session_destroy_symbols_table(session);  
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);

} /* TSP_session_destroy_symbols_table_by_channel */


int32_t 
TSP_session_create_symbols_table_by_channel(const TSP_request_sample_t* req_sample,
					    TSP_answer_sample_t* ans_sample)

{
  int             retcode         = TSP_STATUS_ERROR_NO_MORE_SESSION;
  TSP_session_t*  session         = 0;
  TSP_datapool_t* target_datapool = NULL;    

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

  TSP_GET_SESSION(session, req_sample->channel_id, FALSE)

  /* The functions could have been called several time, so clean up first */
  TSP_session_destroy_symbols_table(session);    
      
  /* 
   * Get datapool instance for this GLU 
   * ACTIVE   GLU will get global datapool singleton instance
   * PASSIVE  GLU will get their own local datapool instance
   */
  target_datapool = TSP_datapool_instantiate(session->session_data->glu_h);

  /* Creating group table*/
  retcode  = TSP_group_algo_create_symbols_table(&(req_sample->symbols),
						 &(ans_sample->symbols), 
						 &(session->session_data->groups),
						 target_datapool);   
  
  /* Set total group number */
  if(TSP_STATUS_OK==retcode) {
    ans_sample->provider_group_number = 
      TSP_group_algo_get_group_number(session->session_data->groups);
  }
  else {
    STRACE_ERROR("Function TSP_group_algo_create_symbols_table failed");
  }
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return retcode;
} /* end of TSP_session_create_symbols_table_by_channel */

int  
TSP_session_get_sample_symbol_info_list_by_channel(channel_id_t channel_id,
						   TSP_sample_symbol_info_list_t* symbol_list) {

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
  int ret = TRUE;

  /* The mutex can not be kept 'coz' we are going to block other sessions */
  /* So, be carefull : the session must not be suppressed when the send is active */
  TSP_LOCK_MUTEX(&X_session_list_mutex,-1);
  TSP_GET_SESSION(session, channel_id,-1);
  if (session->state == TSP_SESSION_STATE_REQUEST_SAMPLE_INIT_OK) {
    if (TRUE==TSP_data_sender_is_consumer_connected(session->session_data->sender)) {
      TSP_session_change_stateTo(session,TSP_SESSION_STATE_SAMPLING);
    }
  }
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,-1);

  if( (session->state == TSP_SESSION_STATE_SAMPLING)  &&
      (NULL!=session->session_data->groups) &&
      (NULL!=session->session_data->sender)
      ) {

    if(!TSP_data_sender_send_msg_ctrl(session->session_data->sender, msg_ctrl)) {
      STRACE_WARNING("Data link broken for session No %d",channel_id);
      session->state = TSP_SESSION_STATE_BROKEN_LINK;
    }
  } else {
    ret = FALSE;
  }

  return ret;  
}

void 
TSP_session_all_session_send_data(time_stamp_t t) {
  int i;
  TSP_session_t* currentSession;

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  /* Loop over all ACTIVE sessions */
  for (i = 0 ; i< X_session_nb ; ++i) {

    /* promote session to SAMPLING if consumer is connected */
    if (TSP_SESSION_STATE_REQUEST_SAMPLE_INIT_OK == X_session_t[i].state) {
      if (TRUE==TSP_data_sender_is_consumer_connected(X_session_t[i].session_data->sender)) {
	TSP_session_change_stateTo(&(X_session_t[i]),TSP_SESSION_STATE_SAMPLING);
      }
    }
    /* 
     * If GLU is PASSIVE this should block until 
     * socket is ready or session is garbage collected.
     */
    if ( GLU_SERVER_TYPE_PASSIVE == X_session_t[i].session_data->glu_h->type) {
      int32_t waitconsumer = 1;
      currentSession = &X_session_t[i];
      while (1==waitconsumer) {

	if (TSP_SESSION_STATE_BROKEN_LINK==currentSession->state) {
	  STRACE_DEBUG("Data link broken (during wait loop) for session = %d / idx = %d.",currentSession->channel_id,i);
	  waitconsumer = 0;
	  continue;
	}
	
	if (TSP_SESSION_STATE_CLOSED==currentSession->state) {
	  STRACE_DEBUG("Session closed (during wait loop) for session = %d / idx = %d.",currentSession->channel_id,i);
	  waitconsumer = 0;
	  continue;
	}

	if (TSP_SESSION_STATE_SAMPLING == currentSession->state) {
	  /* OK sampling running go out of wait loop */
	  waitconsumer = 0;	  
	  continue;
	}		

	/* Dangerous but should be OK for passive GLU though */
	TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
	tsp_usleep(500000);	
	TSP_LOCK_MUTEX(&X_session_list_mutex,);

	STRACE_DEBUG("Waiting session (session = %d / idx = %d) to reach SAMPLING state now %d...",currentSession->channel_id,i,currentSession->state);

	if (TSP_SESSION_STATE_REQUEST_SAMPLE_INIT_OK == currentSession->state) {
	  if (TRUE==TSP_data_sender_is_consumer_connected(currentSession->session_data->sender)) {
	    TSP_session_change_stateTo(currentSession,TSP_SESSION_STATE_SAMPLING);
	  }
	}
      }
    }
    
    /* We go for real send */
    if ((NULL != X_session_t[i].session_data->groups) && /* The request sample was done */
	(NULL != X_session_t[i].session_data->sender) && /* The request sample init was done */
	(X_session_t[i].state == TSP_SESSION_STATE_SAMPLING) /* Session is valid */
	) {
	if(!TSP_data_sender_send(X_session_t[i].session_data->sender, 
				 X_session_t[i].session_data->groups, 
				 t))
	  {
	    STRACE_WARNING("Data link broken for session No %d",X_session_t[i].channel_id);
	    X_session_t[i].state = TSP_SESSION_STATE_BROKEN_LINK;		  
	  }
      }
  }

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
}


void 
TSP_session_all_session_send_msg_ctrl(TSP_msg_ctrl_t msg_ctrl) {
  int i;

  TSP_LOCK_MUTEX(&X_session_list_mutex,);

  for( i = 0 ; i< X_session_nb ; i++) {
    if( X_session_t[i].session_data->groups /* The sample request was done */
	&& X_session_t[i].session_data->sender /* The sample request init was done */
	&& (X_session_t[i].state == TSP_SESSION_STATE_SAMPLING))
      {
	
	if(!TSP_data_sender_send_msg_ctrl(X_session_t[i].session_data->sender, msg_ctrl)) {
	  STRACE_WARNING("Data link broken for session No %d",X_session_t[i].channel_id);
	  X_session_t[i].state = TSP_SESSION_STATE_BROKEN_LINK;	      
	} else if (TSP_MSG_CTRL_EOF==msg_ctrl) {
	  /* terminate session on EOF (passive GLU case) please */
	  X_session_t[i].state = TSP_SESSION_STATE_CLOSE_ON_EOF;
	}
      }
  }

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,);
} /* end of TSP_session_all_session_send_msg_ctrl */


int32_t
TSP_session_create_data_sender_by_channel(channel_id_t channel_id) {

  TSP_session_t* session;
  int retcode = TSP_STATUS_OK;
  int ringbuf_size;

  TSP_LOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);
	
  TSP_GET_SESSION(session, channel_id, TSP_STATUS_ERROR_INVALID_CHANNEL_ID);

  /* 
   * Quick return since No Request Sample has been done on this session 
   * FIX bug #16430 
   */
  if (NULL==session->session_data->groups) {
    retcode = TSP_STATUS_ERROR_BAD_REQUEST_ORDER;
    goto out;
  }

  session->session_data->sender = NULL;

  /* Calculate fifo depth */
  if (GLU_SERVER_TYPE_PASSIVE==session->session_data->glu_h->type) {
    /* 
     * PASSIVE GLU do not need FIFO since
     * there is no RINGBUF between glu and datapool
     * and the datapool is local (i.e. private) for
     * each GLU instance.
     */
    ringbuf_size = 0;
  }
  else {
    /* There is one single data pool, ask for a ringbuf to the socket layer */
    /* We calculate it with the server frequency */
    double base_frequency = (session->session_data->glu_h)->get_base_frequency(session->session_data->glu_h);
    if ( base_frequency > 0.0 ) {
      ringbuf_size = TSP_STREAM_SENDER_RINGBUF_SIZE * base_frequency;
      
      STRACE_DEBUG("Stream sender ringbuf size will be : %d items (i.e. %d seconds)",
		   ringbuf_size,
		   TSP_STREAM_SENDER_RINGBUF_SIZE);
    }
    else {
      STRACE_ERROR("GLU return base frequency = %f", base_frequency);
      retcode = TSP_STATUS_ERROR_UNKNOWN;
    }
  }
  
  /*--------------------*/
  /* Create data sender */
  /*--------------------*/
  if (TSP_STATUS_OK==retcode) {
    uint32_t max_group_size = TSP_group_algo_get_biggest_group_size(session->session_data->groups);
    STRACE_DEBUG("Channel Id <%d> has Max TSP Group Size <%d> byte(s)",
		  channel_id,max_group_size);
    STRACE_DEBUG("Using RINGBUF size of <%d> byte(s)",
		  ringbuf_size);
    session->session_data->sender = TSP_data_sender_create(ringbuf_size, max_group_size);      
    if(NULL != session->session_data->sender) {
      /* If there's no fifo, 
       * we must start a new thread per client, 
       * because there's one datapool per client 
       */
      if (GLU_SERVER_TYPE_PASSIVE==session->session_data->glu_h->type) {
	/* 
	 * Should start GLU now 
	 * Note that multi-consumer PASSIVE GLU should protect themselves
	 * against multiple start (this is the case of the default GLU_start_default)
	 */
	if (0!=(session->session_data->glu_h->start(session->session_data->glu_h))) {
	  retcode = TSP_STATUS_ERROR_GLU_START;
	  STRACE_ERROR("Unable to start GLU (PASSIVE case)");
	} else {
	  STRACE_DEBUG("PASSIVE GLU started.");
	  retcode = TSP_STATUS_OK;
	}
      }
    }
    else {
      retcode = TSP_STATUS_ERROR_UNKNOWN;
      STRACE_ERROR("function TSP_data_sender_create failed");
    }
  }
  
out:
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);  
  return retcode;
}

int32_t 
TSP_session_destroy_data_sender_by_channel(channel_id_t channel_id) {
  TSP_session_t* session;
  int retcode = TSP_STATUS_OK;

  TSP_LOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);
	
  TSP_GET_SESSION(session, channel_id, TSP_STATUS_ERROR_INVALID_CHANNEL_ID);

  /* Stop the session */
  TSP_data_sender_stop(session->session_data->sender);

  /* For a passive server we must wait for the session thread to end */
  if (GLU_SERVER_TYPE_PASSIVE==session->session_data->glu_h->type) {
    /*TSP_local_datapool_wait_for_end_thread(session->session_data->datapool);*/
  }
  
  TSP_data_sender_destroy(session->session_data->sender);
  session->session_data->sender = NULL;

  TSP_UNLOCK_MUTEX(&X_session_list_mutex,TSP_STATUS_ERROR_UNKNOWN);

  return retcode;
}

const char* TSP_session_get_data_address_string_by_channel(channel_id_t channel_id)
{

  TSP_session_t* session;
  const char* data_address;

  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

	
  TSP_GET_SESSION(session, channel_id, FALSE);

  data_address = TSP_data_sender_get_data_address_string(session->session_data->sender);
    
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  STRACE_DEBUG_MORE("data_address='%s'",data_address);

    
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


int 
TSP_session_get_symbols_global_index_by_channel(channel_id_t channel_id,
						TSP_sample_symbol_info_list_t* symbol_list)
{
    TSP_session_t* session;
    GLU_handle_t* myGLU;
    int ret=TRUE;
    int* pg_indexes = calloc( symbol_list->TSP_sample_symbol_info_list_t_len, sizeof(int) );
    TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);
    TSP_GET_SESSION(session, channel_id, FALSE);    
    
    if (pg_indexes == NULL) {
      STRACE_ERROR("Unable to allocate memory for global provider index");
      ret = FALSE;
      goto out;
    }
  
    /* Get global provider indexes */
    myGLU = session->session_data->glu_h;
    if (myGLU->get_pgi(myGLU,symbol_list,pg_indexes) == FALSE) {
      STRACE_ERROR("Some symbols have not been found");
      ret=FALSE;
    }
     
out:
    TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return ret;

}

int 
TSP_session_get_garbage_session(channel_id_t* channel_id) {
  int i;
  int found;
	
  TSP_LOCK_MUTEX(&X_session_list_mutex,FALSE);

  found = FALSE;
  /* There is multiple criterion for session garbage collection */  
  for( i = 0 ;  i < X_session_nb ; i++) {

    /* Do not garbage collect session in the following state */
    if ((X_session_t[i].state == TSP_SESSION_STATE_UNKNOWN) ||
	(X_session_t[i].state == TSP_SESSION_STATE_CLOSED)
	) {
      continue;
    }

    /* Garbage collect Data link broken session */
    if (X_session_t[i].state == TSP_SESSION_STATE_BROKEN_LINK ) {
      found = TRUE;
      STRACE_INFO("Garbage Collector thread found broken link session <%d>",X_session_t[i].channel_id);
      *channel_id = X_session_t[i].channel_id;
      break;
    } 

    /* 
     * Garbage collect datapool terminated session
     * either in SAMPLING or SAMPLE_DESTROY_OK state
     */
    if (NULL != X_session_t[i].session_data) {
      /* Datapool has been deleted */
      if (NULL == X_session_t[i].session_data->datapool) { 
	/* FIXME nothing to do here */
        /* X_session_t[i].session_data->datapool
	 * is the LOCAL datapool which is used when
	 * a provider (possibly PASSIVE) is giving
	 * ONE datapool per session 
	 * In the case of GLOBAL datapool this datapool
	 * is ALWAYS NULL and never used in fact.
	 * see bug #19179
	 */
	continue;
      }
      /* Datapool has been terminated */ 
      else {
	if ( (TRUE==X_session_t[i].session_data->datapool->terminated) &&
	    ((TSP_SESSION_STATE_SAMPLING                  == X_session_t[i].state)  || 
	     (TSP_SESSION_STATE_REQUEST_SAMPLE_DESTROY_OK == X_session_t[i].state)
	     )
	    ){
	  STRACE_INFO("Garbage Collector thread found Datapool TERMINATED for session <%d>",X_session_t[i].channel_id);
	  found = TRUE;
	  *channel_id = X_session_t[i].channel_id;
	}
      }
    }
    /* FIXME to be added like 'timeout on idle consumer' */
  }
	
  TSP_UNLOCK_MUTEX(&X_session_list_mutex,FALSE);

  return found;
} /* end of TSP_session_get_garbage_session */
