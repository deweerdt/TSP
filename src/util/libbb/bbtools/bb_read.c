
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <bb_core.h>

int 
main (int argc, char ** argv) {

  /* Définitions des variables */
  int retcode=0;
  void* sym_value;
  S_BB_DATADESC_T sym_data_desc;
  S_BB_T* mybb;

  if (argc<3) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage: %s <bbname> <symbol_name>\n",argv[0]);
    exit(-1);
  }
  else {
    fprintf(stdout,"%s: Trying to read symbol <%s> on blackboard <%s>...\n",
	    argv[0],argv[2],argv[1]);
    fflush(stdout);
  }
  
  /* Attach to BB */
  /****************/
  if (E_OK != bb_attach(&mybb,argv[1])) {
    fprintf(stderr,"Cannot attach to blackboard <%s>!!\n",argv[1]);
    exit(-1);
  }

  /* 
   * Use low-level subscribe in order to discover the 
   * type of the variable
   */
  strncpy(sym_data_desc.name,argv[2],VARNAME_MAX_SIZE);
  sym_value = bb_subscribe(mybb,&sym_data_desc);

  if (NULL==sym_value) {
    fprintf(stdout,"%s: symbol not found\n",argv[0]);
  } else {
    bb_data_print(mybb,sym_data_desc,stdout);
  }        
  /* Detach from BB */
  /******************/
  retcode = bb_detach(&mybb);  
  
  return (retcode);
}
