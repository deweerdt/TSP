/*!  \file 

$Id: tsp_client.h,v 1.8 2004-11-09 22:33:01 erk Exp $

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

Purpose   : 

-----------------------------------------------------------------------
 */

#ifndef _TSP_CLIENT
#define _TSP_CLIENT

#include "tsp_prjcfg.h"

#include "tsp_rpc.h"

/**
 * @defgroup Client
 * The Client module is the set of functions which encapsulate
 * the RPC machinery.
 * @ingroup Consumer
 * @{
 */


typedef  void* TSP_server_t;

int TSP_remote_open_server( const char *protocol,
			    const char *target_name,
			    const char *server_name,
			    const int server_id, 
			    TSP_server_t* server,
			    TSP_server_info_string_t server_info);

/**
 * Close a server.
 * @param server the server that must be close.
 */
void TSP_remote_close_server(TSP_server_t server);

/**
 * Max server number.
 * Get how many server can exists on a given host.
 * @return Max server number
 */
int TSP_get_server_max_number(void);

TSP_answer_open_t* TSP_request_open(const TSP_request_open_t* req_open,
				    TSP_server_t server);

/**
 * Close the session for a remote_opened provider.
 * @param req_close the informations tout close the session
 * @param server the server
 * @return The action result (TRUE or FALSE)
 */
int TSP_request_close(const TSP_request_close_t* req_close,
		      TSP_server_t server);	
		      
TSP_answer_sample_t * TSP_request_information(
					      const TSP_request_information_t* req_info,
					      TSP_server_t server);
	
TSP_answer_sample_t * TSP_request_sample(
					 const TSP_request_sample_t* req_sample,
					 TSP_server_t server);

TSP_answer_sample_init_t * TSP_request_sample_init(
					      const TSP_request_sample_init_t* req_sample,
					      TSP_server_t server);

TSP_answer_sample_destroy_t * TSP_request_sample_destroy(const TSP_request_sample_destroy_t* req_sample,
							 TSP_server_t server);

/** @} end group Client */ 

#endif /* _TSP_CLIENT */
