/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc_confprogid.c,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

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
