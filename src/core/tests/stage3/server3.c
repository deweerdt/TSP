#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"

#include "tsp_provider.h"

int main(int argc, char *argv[])
{

  SFUNC_NAME(main);

  int i;

  STRACE_IO(("-->IN"));

  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));
  
  if(argc == 1)
    {
      printf("USAGE %s : [ --tsp-stream-init-start file.res --tsp-stream-init-stop ]\n", argv[0]);
    }

  if(TSP_provider_init(&argc, &argv))
    {
      
      TSP_provider_run(TRUE);
    }
    
  STRACE_IO(("-->OUT"));

    
    
}

   
