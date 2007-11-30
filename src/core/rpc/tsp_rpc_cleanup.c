/*

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc_cleanup.c,v 1.3 2007-11-30 15:42:02 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 
Copyright (c) 2005 Cesare BERTONA

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNULesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
*/


#include <rpc/rpc.h>
#include <stdio.h>
#ifndef WIN32
    #include <netdb.h>
    #include <unistd.h> 
    #include <rpc/pmap_clnt.h>
#else
    #include <rpc/pmap_cln.h>
#endif

#include <tsp_sys_headers.h>
#include <tsp_glu.h>
/* 
 * FIXME RP : beurk, RPC is compiled before CTRL 
 * could export this include, how should I call 
 * Request Manager and GLU then ? 
 * All our troubles comes from using tsp_rpc.h
 * in tsp_datastruct.h
 * As soon as we have clean IDL generator
 * we will get rid of those circular include problem
 * We need 2 passes compile here, 
 * the first for IDL and the second for C compilation.
 */
#include "../ctrl/tsp_provider.h"
#include "../ctrl/tsp_request_handler.h"
/* same FIXME IDL <--> C dependency */
#include "../common/tsp_common_macros.h"

#include <tsp_server.h>
#include <tsp_rpc.h>
#include <tsp_rpc_confprogid.h>

int main(void)
{
#if defined (_WIN32)
  WSADATA WSAData;
#endif

  int servernumber;

#if defined (_WIN32)
  if (WSAStartup(MAKEWORD(2,2), &WSAData)) {
	WSACleanup();
    return -1;
   }
#endif

  for(servernumber=0; servernumber<TSP_MAX_SERVER_NUMBER; servernumber++)
    {
      svc_unregister (TSP_get_progid(servernumber), TSP_RPC_VERSION_INITIAL);
      pmap_unset (TSP_get_progid(servernumber), TSP_RPC_VERSION_INITIAL);
    }
#if defined (_WIN32)
    WSACleanup();
#endif
  return 0;
}
