#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"

#include "tsp_provider.h"


int main(int argc, char *argv[])
{

  SFUNC_NAME(main);

  int i;
  int host_number;

  STRACE_IO(("-->IN"));

  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));

  if (argc>1)
    {   
      host_number = atoi(argv[1]);
    }
  else
    {
      STRACE_ERROR(("First parameter must the server number on the host"));
      return -1;
    }

  /* Init server */
  TSP_init(host_number, NULL);
    
  STRACE_IO(("-->OUT"));

    
    
}

   
