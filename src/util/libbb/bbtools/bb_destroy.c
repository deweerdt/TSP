
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <bb_core.h>

int 
main (int argc, char ** argv) {

  /* Définitions des variables */
  S_BB_T* mybb=NULL;
  int retcode=0;
  
  if (argc<2) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage : %s <bbname>\n",argv[0]);
    exit(-1);
  }

  /* Attach to BB */
  /****************/
  if (E_OK != bb_attach(&mybb,argv[1])) {
    fprintf(stderr,"Cannot attach to Blackboard <%s>!!\n",argv[1]);
    exit(-1);
  }

  /* Destroy  BB */
  /***************/
  retcode = bb_destroy(&mybb);
  return (retcode);
}
