/*!  \file 

$Id: tsp_request.c,v 1.2 2004-07-28 13:05:38 mia Exp $

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
Maintainer : tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : TSP request handling API

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
#include "tsp_request.h"

int 
TSP_provider_rqh_manager_get_max_nb(void) {
  return TSP_MAX_REQUEST_HANDLERS;
}

int
TSP_provider_rqh_manager_get_nb(void) {

  SFUNC_NAME(TSP_provider_request_handlers_get_nb);

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

  SFUNC_NAME(TSP_provider_rqh_manager_get_nb_running);

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
  SFUNC_NAME(TSP_provider_rqh_manager_get);

  TSP_provider_request_handler_t*  retval = NULL;
  STRACE_IO(("-->IN"));

  if ((rank<0) || (rank>TSP_provider_rqh_manager_get_max_nb())) {
    STRACE_ERROR(("Invalid request handler RANK=%d, max=%d",rank,TSP_provider_rqh_manager_get_max_nb()));
  }
  else {
    retval = &rqh_manager_if.request_handlers[rank];
  }
  
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_get */

int 
TSP_provider_rqh_manager_install(int rank, TSP_provider_request_handler_t rqh) {
  SFUNC_NAME(TSP_provider_rqh_manager_install);

  int retval = TRUE;
  TSP_provider_request_handler_t* rqh_p;
  STRACE_IO(("-->IN"));

  rqh_p = TSP_provider_rqh_manager_get(rank);
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  if (!rqh_p) {
    retval=FALSE;
  }
  else {
    /* Verify the adressed rqh is not running */
    if (rqh_p->status == TSP_RQH_STATUS_RUNNING) {
      STRACE_ERROR(("Could not replace a running request handler"));
      retval = FALSE;
    }
    else {
      /* Verify the structure passed as a parameter */
      if ((rqh.config == NULL) || (rqh.run == NULL) || (rqh.stop ==NULL)) {
	STRACE_ERROR(("Invalid request handler descriptor"));
	retval = FALSE;
      }
      /* Really install handler */
      else {
	*rqh_p = rqh;
	rqh_p->status             = TSP_RQH_STATUS_IDLE;
	rqh_p->tid                = -1;
	rqh_p->request_handler_id = 0;
      } /* if ((rqh.config == NULL) ... [else] */
    } /* (rqh_p->status == TSP_RQH_STATUS_RUNNING) [else] */
  } /* if  (!rqh_p) [else] */

  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_install */

int 
TSP_provider_rqh_manager_init(void) {
  SFUNC_NAME(TSP_provider_rqh_manager_init);

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
    rqh_manager_if.request_handlers[i].request_handler_id = 0;
    rqh_manager_if.request_handlers[i].tid                = 0;
    rqh_manager_if.request_handlers[i].status             = TSP_RQH_STATUS_NOTINSTALLED;
    rqh_manager_if.request_handlers[i].config_param       = NULL;
    rqh_manager_if.request_handlers[i].config             = NULL;
    rqh_manager_if.request_handlers[i].run                = NULL;
    rqh_manager_if.request_handlers[i].stop               = NULL;
  }
  
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,-1);
  STRACE_IO(("-->OUT"));
  return retval;
} /* End of TSP_provider_rqh_manager_init */


int 
TSP_provider_rqh_manager_refresh(void) {
  SFUNC_NAME(TSP_provider_rqh_manager_refresh);

  int retval = TRUE;

  /* 
   * FIXME this is for testing purpose only 
   * configure and run all TSP request handlers
   * which are installed, but not running
   */
  int i = 0;
  STRACE_IO(("-->IN"));
  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,FALSE);

  while ( (i<TSP_provider_rqh_manager_get_max_nb()) &&
	  (TRUE==retval)
	  ) {
    if (rqh_manager_if.request_handlers[i].status == TSP_RQH_STATUS_IDLE) {
      retval = rqh_manager_if.request_handlers[i].config(&rqh_manager_if.request_handlers[i].config_param);
      if (TRUE == retval) {
	int tstatus;
	rqh_manager_if.request_handlers[i].status = TSP_RQH_STATUS_CONFIGURED;
	tstatus = pthread_create(&rqh_manager_if.request_handlers[i].tid,
				 NULL,
				 rqh_manager_if.request_handlers[i].run,
				 rqh_manager_if.request_handlers[i].config_param);
	
	if (0 == tstatus) {
	  rqh_manager_if.request_handlers[i].status = TSP_RQH_STATUS_RUNNING;
	  rqh_manager_if.nb_running_rhq++;
	}
      }
    }
    ++i;
  } /* End while loop over configured request handlers */

  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,FALSE);
  STRACE_IO(("-->OUT"));
  return retval;
}  /* End of TSP_provider_rqh_manager_refresh */

void TSP_provider_rqh_manager_waitend(void) {
  SFUNC_NAME(TSP_provider_rqh_manager_waitend);
  STRACE_IO(("-->INT"));

  TSP_LOCK_MUTEX(&rqh_manager_if.mutex,);
  while (rqh_manager_if.nb_running_rhq > 0) {
    pthread_cond_wait(&rqh_manager_if.cond,&rqh_manager_if.mutex);
  }
  TSP_UNLOCK_MUTEX(&rqh_manager_if.mutex,);
  STRACE_DEBUG(("No more Request Hanndler running -->terminate"));
  STRACE_IO(("-->OUT"));
} /* end of TSP_provider_rqh_manager_waitend */
