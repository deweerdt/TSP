
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <math.h>
#include <bb_tsp_provider.h>

int 
main (int argc, char ** argv) {

/*   if (bb_attach()) { */
/*     exit(-1); */
/*   } */
  if (argc<2) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage: %s <bbname>\n",argv[0]);
    exit(-1);
  }
  
  bb_tsp_provider_initialise(&argc,&argv,1,argv[1]);

/*   return bb_detach(); */
  return 0;
}
