/*!  \file 

$Id: tsp_request.h,v 1.5 2004-10-07 08:58:05 tractobob Exp $

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

#ifndef _TSP_REQUEST_H
#define _TSP_REQUEST_H

#include "tsp_prjcfg.h"
#include "tsp_sys_headers.h"

/**
 * @defgroup Request
 * @ingroup  Provider
 * The Request module is the set of all request handling API for TSP.
 * We define here the TSP request handler object and how to
 * install and use it.
 * @{
 */

/*@{*/ 
/**
 * The different methods of the TSP
 * asynchronous request handler.
 * The tsp_request_handler_ft is the object constructor
 * The tsp_request_handler_config_ft should be a function
 * which always return. The void* parameter will be allocated and 
 * filled in by the function itself.
 * The tsp_request_handler_run_ft may be passed as the start_routine
 * of pthread_create(3), it is supposed to take the configuration parameter
 * taken from the cofiguration step. This function should not return unless
 * it terminates.
 * The tsp_request_handler_stop_ft is supposed to stop the request handler
 * The tsp_request_handler_url_ft shall return the connection URL string 
*/
struct TSP_provider_request_handler_t;
typedef int   (* tsp_request_handler_config_ft)(struct TSP_provider_request_handler_t*);
typedef void* (* tsp_request_handler_run_ft)   (struct TSP_provider_request_handler_t*);
typedef int   (* tsp_request_handler_stop_ft)  (struct TSP_provider_request_handler_t*);
typedef char* (* tsp_request_handler_url_ft)   (struct TSP_provider_request_handler_t*);
/*@}*/ 

/** Request handler status */
enum TSP_request_handler_status_t
{
  /** 
   * The request handler is not installed other
   * field of the TSP_provider_request_handler_t
   * are not significant.
   */  
  TSP_RQH_STATUS_NOTINSTALLED=0,
  /** The request handler is IDLE */  
  TSP_RQH_STATUS_IDLE,
  /** The request handler is CONFIGURED */  
  TSP_RQH_STATUS_CONFIGURED,
  /** The request handler is RUNNING */  
  TSP_RQH_STATUS_RUNNING,
  /** The request handler is STOPPED */  
  TSP_RQH_STATUS_STOPPED
};
typedef enum TSP_request_handler_status_t TSP_request_handler_status_t;

/**
 * The structure TSP_provider_request_handler_t
 * define object-oriented interface to
 * a TSP request handler.
 * Every TSP request handler should implement the interface properly.
 */
struct TSP_provider_request_handler_t {

  pthread_t tid;          /**< The request handler thread Id */
  int status;             /**< The request handler status (NotInstalled,Idle, Configured, 
                                                           Running, ToBeStopped, Stopped) */
  void* config_param;     /**< The parameter used for the config step */ 

  tsp_request_handler_config_ft  config; /**< The function to be called in order to initialize the request
					    handler */
  tsp_request_handler_run_ft     run;    /**< Launch the request handler */
  tsp_request_handler_stop_ft    stop;   /**< Stop the request handler */

  tsp_request_handler_url_ft     url;    /**<Get Universal Resource Locator to connect to this request handler>**/
};

typedef struct TSP_provider_request_handler_t TSP_provider_request_handler_t;
typedef int   (* tsp_request_handler_ft)       (TSP_provider_request_handler_t*);

/**
 * The TSP rqh manager data structure.
 * It is used for manager/rqh termination synchronization.
 */
struct rqh_manager_data_t {

  pthread_cond_t cond;    /**< The condition variable used by the rqh
			    *  in order to signal the proper termination off all thread
                            */
  pthread_mutex_t mutex;   /**< The associated mutex */
  int             nb_running_rhq; /**< The number of running thread */
  TSP_provider_request_handler_t request_handlers[TSP_MAX_REQUEST_HANDLERS];
};

typedef struct rqh_manager_data_t rqh_manager_data_t;

/* Don't forget to statically initialize the structure */
static rqh_manager_data_t rqh_manager_if;
					  

/**
 * Get the maximum number
 * of TSP_provider_request_handler.
 */
int TSP_provider_rqh_manager_get_max_nb(void);

/**
 * Get the current number
 * of TSP_provider_request_handler installed.
 */
int TSP_provider_rqh_manager_get_nb(void);

/**
 * Get the current number
 * of TSP_provider_request_handler running.
 */
int TSP_provider_rqh_manager_get_nb_running(void);

/**
 * Get a request_handler
 * @return the pointer to the actual request handler
 *         pointer may be NULL if rank is invalid.
 */
TSP_provider_request_handler_t* TSP_provider_rqh_manager_get(int rank);

/**
 * Get the URL for this request handler
 * @return the string for the URL
 *         pointer may be NULL if rank is invalid or handler is KO.
 */
char* TSP_provider_rqh_manager_get_url(int rank);

/**
 * Install a TSP request handler
 * @return TRUE on success FALSE on failure
 */
int TSP_provider_rqh_manager_install(int rank, tsp_request_handler_ft rqh_constructor);

/**
 * Set-up TSP initial request handlers.
 * @return TRUE on success, FALSE on failure
 */
int TSP_provider_rqh_manager_init(void);

/**
 * Refresh TSP request handlers activities
 * this may start new request handler and/or stop
 * running ones.
 * @return TRUE on success, FALSE on failure
 */
int TSP_provider_rqh_manager_refresh(void);

/**
 * This is a blocking function calls which triggers
 * a wait on a condition variable which is signalled
 * iff no more request handlers thread are running.
 */
void TSP_provider_rqh_manager_waitend(void);

/** @} end group Request */ 

#endif /* _TSP_REQUEST_H */
