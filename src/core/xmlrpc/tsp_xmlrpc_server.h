/*

$Id: tsp_xmlrpc_server.h,v 1.4 2006-10-18 09:58:48 erk Exp $

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

#ifndef _TSP_XMLRPC_SERVER_H
#define _TSP_XMLRPC_SERVER_H

#include "tsp_prjcfg.h"

/**
 * @defgroup TSP_XMLRPCRequestHandlerLib XML-RPC Request Handler
 * The XMLRPCRequestLib module is the set of functions which encapsulate
 * the RPC machinery.
 * @ingroup TSP_RequestHandlerLib
 * @{
 */


int TSP_xmlrpc_request(TSP_provider_request_handler_t* cthis);
int TSP_xmlrpc_request_config(TSP_provider_request_handler_t* cthis);
void* TSP_xmlrpc_request_run(TSP_provider_request_handler_t* cthis);
int TSP_xmlrpc_request_stop(TSP_provider_request_handler_t* cthis);
char* TSP_xmlrpc_request_url(TSP_provider_request_handler_t* cthis);

/** @} end group TSP_XMLRPCRequestHandlerLib */ 

#endif /* _TSP_XMLRPC_SERVER_H */
