/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.c,v 1.3 2002-12-05 17:36:44 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

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
