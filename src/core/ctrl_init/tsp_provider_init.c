/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.c,v 1.4 2002-12-18 16:27:23 tntdev Exp $

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

Purpose   : Function calls to launch a TSP Provider program
            in the 'main'

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_provider.h"
#include "tsp_server.h"

int TSP_provider_init(int* argc, char** argv[])
{
  SFUNC_NAME(TSP_provider_init);

  int ret;
  STRACE_IO(("-->IN"));

  ret = TSP_provider_private_init(argc, argv);

  STRACE_IO(("-->OUT"));
  return ret;

}

int TSP_provider_run(int blocking)
{
  SFUNC_NAME(TSP_provider_run);

  int ret = FALSE;
  STRACE_IO(("-->IN"));


  if(TSP_provider_is_initialized())
    {            
      int server_number = TSP_provider_get_server_number();
      ret = TSP_command_init(server_number, blocking);
    }
  else
    {
      STRACE_ERROR(("Call TSP_provider_init first, and then call TSP_provider_run ! "))
    }
  
  STRACE_IO(("-->OUT"));
  return ret;  
  
}


void TSP_provider_print_usage(void)
{
   printf(TSP_ARG_PROVIDER_USAGE"\n");
}
