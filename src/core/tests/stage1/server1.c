#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_rpc.h"

#include "tsp_provider_init.h"

int main(int argc, char *argv[])
{

  SFUNC_NAME(main);

  int i;
  int host_number;

  STRACE_IO(("-->IN"));

  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

  /* Init server */
  if(TSP_provider_init(&argc, &argv))
    {
      TSP_provider_run(TRUE);
    }
    

  STRACE_IO(("-->OUT"));

    
    
}

   
