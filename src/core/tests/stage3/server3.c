#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"

#include "tsp_provider.h"

int main(int argc, char *argv[])
{

  SFUNC_NAME(main);

  int i;
  int host_number;
  char* filename;

  STRACE_IO(("-->IN"));

  STRACE_INFO(("Autodetect CPU : %d bits", sizeof(long)*8));
  
  if(argc>2)
    {
      host_number = atoi(argv[1]);
      filename = argv[2];
      if(!strcmp("_", filename))
	{
	  filename = 0;
	}
    }
  else
    {
      STRACE_ERROR(("USAGE %s : server_number (in_file_res|_)", argv[0]));
      return -1;
    }

  /* Init server */
  TSP_init(host_number, filename);
    
  STRACE_IO(("-->OUT"));

    
    
}

   
