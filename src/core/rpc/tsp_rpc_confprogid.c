/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc_confprogid.c,v 1.2 2002-12-18 16:27:36 tntdev Exp $

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
Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_rpc_confprogid.h"

int TSP_get_progid_total_number()
{
  return TSP_MAX_SERVER_NUMBER;	
}

gint32 TSP_get_progid(gint n)
{
  return((gint32)(TSP_RPC_PROGID_BASE_COUNT)+n);
}
