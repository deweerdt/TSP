/*!  \file 

$Id: tsp_server.h,v 1.5 2003-07-15 14:42:24 erk Exp $

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

Purpose   : 

-----------------------------------------------------------------------
 */

#ifndef _TSP_SERVER_H
#define _TSP_SERVER_H

#include "tsp_prjcfg.h"

/**
 * @defgroup Server
 * The Server module is the set of functions which encapsulate
 * the RPC machinery.
 * @ingroup Provider
 * @{
 */

int TSP_rpc_request_config(void* config_param);
int TSP_rpc_request_config2(void* config_param);
void* TSP_rpc_request_run(void* config_param);
int TSP_rpc_request_stop();

/**
* Initialise command communication canal.
* @param server_number Every provider on a given host must have its own server number ;
* All must be different
* @param spawn_mode the value of the parameter specify the behavior of the TSP asynchronous
*         command link:
*            - 0 simple non blocking call, the asynchronous command will be handled
*                by a unique thread spawned by this call.
*            - 1 blocking call the command will never return
*            - 10 non-blocking with dynamic spawning of asynchronous command receiver
* @return TRUE = OK
*/
int TSP_command_init(int server_number, int spawn_mode);

/** @} end group Server */ 

#endif /* _TSP_SERVER_H */
