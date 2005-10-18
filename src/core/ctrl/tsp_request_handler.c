/*!  \file 

$Id: tsp_request_handler.c,v 1.3 2005-10-18 23:10:21 erk Exp $

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
Component  : Provider

-----------------------------------------------------------------------

Purpose   : TSP request handling API

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#define REQUEST_HANDLER_C
#include "tsp_request_handler.h"
#include "tsp_time.h"

int 
TSP_provider_rqh_manager_get_max_nb(void) {
  return TSP_MAX_REQUEST_HANDLERS;
}

int
TSP_provider_rqh_manager_get_nb(void) {

  int retval = 0;
  int i;
  STRACE_IO(("-->IN"));
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,-1);
  for (i=0; i<TSP_provider_rqh_manager_get_max_nb(); ++i) {
    if (rqh_manager_if.request_handlers[i].status != TSP_RQH_STATUS_NOTINSTALLED)
      ++retval;
  }
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,-1);
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_get_nb */


int 
TSP_provider_rqh_manager_get_nb_running(void) {

  int retval = 0;
  STRACE_IO(("-->IN"));

  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,-1);
  retval = rqh_manager_if.nb_running_rhq;
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,-1);

  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_get_nb_running */

TSP_provider_request_handler_t* 
TSP_provider_rqh_manager_get(int rank) {

  TSP_provider_request_handler_t*  retval = NULL;

  if ((rank<0) || (rank>TSP_provider_rqh_manager_get_max_nb())) {
    STRACE_ERROR(("Invalid request handler RANK=%d, max=%d",rank,TSP_provider_rqh_manager_get_max_nb()));
  }
  else {
    retval = &rqh_manager_if.request_handlers[rank];
  }
  
  return retval;
} /* End of TSP_provider_rqh_manager_get */

char* 
TSP_provider_rqh_manager_get_url(int rank) {

  TSP_provider_request_handler_t*  rqh_p;

  rqh_p = TSP_provider_rqh_manager_get(rank);
  if(rqh_p)
    return rqh_p->url(rqh_p);
  else
    return NULL;

} /* End of TSP_provider_rqh_manager_get_url */

int 
TSP_provider_rqh_manager_install(int rank, tsp_request_handler_ft rqh_constructor) {

  int retval = TRUE;
  TSP_provider_request_handler_t* rqh_p;
  STRACE_IO(("-->IN"));

  rqh_p = TSP_provider_rqh_manager_get(rank);
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  if (!rqh_p) {
    retval=FALSE;
  }
  else {
    /* Verify the adressed rqh was not already installed */
    if (rqh_p->status != TSP_RQH_STATUS_NOTINSTALLED) {
      STRACE_ERROR(("Could not replace an installed request handler"));
      retval = FALSE;
    }
    else {
       /* Really install handler */
      if (!rqh_constructor(rqh_p)) {
	STRACE_ERROR(("Invalid request handler constructor"));
	retval = FALSE;
      }
    } /* (rqh_p->status == TSP_RQH_STATUS_RUNNING) [else] */
  } /* if  (!rqh_p) [else] */

  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_install */

int 
TSP_provider_rqh_manager_init(void) {

  int retval = TRUE;
  int i;
  STRACE_IO(("-->IN"));

  /* Initialise rqh manager data structure, mutex, cond var... 
   * Note that we do not test returned code from those
   * function since they should ALWAYS return 0.
   * see pthread_cond_init(3), pthread_mutex_init(3)
   */
  pthread_cond_init(&rqh_manager_if.cond,(pthread_condattr_t *)NULL);  
  pthread_mutex_init(&rqh_manager_if.mutex,(const pthread_mutexattr_t *)NULL);
  
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,-1);
  
  rqh_manager_if.nb_running_rhq = 0;
  /* RAZ handlers array */ 
  for (i=0;i<TSP_provider_rqh_manager_get_max_nb();++i) {
    rqh_manager_if.request_handlers[i].tid                = 0;
    rqh_manager_if.request_handlers[i].status             = TSP_RQH_STATUS_NOTINSTALLED;
    rqh_manager_if.request_handlers[i].config_param       = NULL;
    rqh_manager_if.request_handlers[i].config             = NULL;
    rqh_manager_if.request_handlers[i].run                = NULL;
    rqh_manager_if.request_handlers[i].stop               = NULL;
    rqh_manager_if.request_handlers[i].url                = NULL;
  }
  
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,-1);
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_init */


int 
TSP_provider_rqh_manager_refresh(void) {

  int retval = TRUE, timeout;
  TSP_provider_request_handler_t *rqh_p;
  int rank, nb, tstatus;

  /* 
   * configure and run all TSP request handlers
   * which are installed, but not running
   */
  STRACE_IO(("-->IN"));
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,FALSE);

  nb = TSP_provider_rqh_manager_get_max_nb();
  rank = 0;
  while (rank < nb && TRUE == retval)
    {
      rqh_p = TSP_provider_rqh_manager_get(rank);

      if (rqh_p->status == TSP_RQH_STATUS_IDLE)
	{
	  retval = rqh_p->config(rqh_p);
	  if (retval)
	    {
	      tstatus = pthread_create(&rqh_p->tid, NULL, rqh_p->run, rqh_p);
	
	      if (0 == tstatus)
		{
		  /* wait for handler to be really running, 1 second */
		  timeout = 100;
		  while(rqh_p->status != TSP_RQH_STATUS_RUNNING && timeout)
		    {
		      tsp_usleep(10000);
		      timeout --;
		    }

		  if(rqh_p->status == TSP_RQH_STATUS_RUNNING)
		    {
		      rqh_manager_if.nb_running_rhq++;
		      /*FIXME: should not reference rpc explicitely */
		      /*STRACE_INFO(("Request handler # %d started with URL %s", rank, TSP_rpc_request_url(rqh_p)));*/
		    }
		  else
		    {
		      STRACE_ERROR(("Request handler # %d could not start properly", rank));
		    }
		}
	    }
	}
      rank++;
    } /* End while loop over configured request handlers */

  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  STRACE_IO(("-->OUT"));
  return retval;
}  /* End of TSP_provider_rqh_manager_refresh */



int TSP_provider_rqh_manager_end(void)
{
  int timeout;
  TSP_provider_request_handler_t *rqh_p;
  int rank, nb;

  /* 
   * call stop for all TSP request handlers
   * that are running
   */
  STRACE_IO(("-->IN"));
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,FALSE);

  nb = TSP_provider_rqh_manager_get_max_nb();
  for(rank = 0; rank < nb; rank++)
    {
      rqh_p = TSP_provider_rqh_manager_get(rank);

      if (rqh_p->status == TSP_RQH_STATUS_RUNNING)
	{
	  rqh_p->stop(rqh_p);

	  /* wait for handler to be really stopped, 1 second */
	  timeout = 100;
	  while(rqh_p->status != TSP_RQH_STATUS_STOPPED && timeout)
	    {
	      tsp_usleep(10000);
	      timeout --;
	    }

	  if(rqh_p->status != TSP_RQH_STATUS_STOPPED)
	    {
	      STRACE_ERROR(("Could not stop handler # %d, canceling", rank));
	      pthread_cancel(rqh_p->tid);
	    }
	  rqh_manager_if.nb_running_rhq--;
	}
    } /* End while loop over running request handlers */

  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  STRACE_IO(("-->OUT"));
  return TRUE;
} /* end of TSP_provider_rqh_manager_end */


void TSP_provider_rqh_manager_waitend(void) {

  STRACE_IO(("-->INT"));

  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,);
  while (rqh_manager_if.nb_running_rhq > 0) {
    pthread_cond_wait(&rqh_manager_if.cond,&rqh_manager_if.mutex);
  }
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,);
  STRACE_DEBUG(("No more Request Hanndler running -->terminate"));
  STRACE_IO(("-->OUT"));
} /* end of TSP_provider_rqh_manager_waitend */
