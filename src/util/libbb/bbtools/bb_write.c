
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
  int erreur=0;
  void* sym_value;
  S_BB_DATADESC_T sym_data_desc;
  char*    array_name;
  int32_t  array_index; 
  S_BB_T*  mybb;

  if (argc<4) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage: %s <bbname> <symbol_name> <value>\n",argv[0]);
    exit(-1);
  }
  else {
    fprintf(stdout,"%s: Trying to write symbol <%s> on blackboard <%s> value <%s>...\n",
	    argv[0],argv[2],argv[1],argv[3]);
    fflush(stdout);
  }
  
  /* Attachement BB */
  /******************/
  if (bb_attach(&mybb,argv[1]) != E_OK) {
    fprintf(stderr,"Cannot attach to blackboard <%s>!!\n",argv[1]);
    exit(-1);
  }

  /* do we want to write to an array var */
  array_name = strstr(argv[2],"[");
  if (array_name) {
    char* temp = "%d";
    char* temp2;
    temp2  = strdup(argv[2]);
    array_name  = strtok(temp2,"[");
    strncpy(sym_data_desc.name,array_name,VARNAME_MAX_SIZE);
    array_name = strtok(NULL,"]");
    sscanf(array_name,temp,&array_index);
    free(temp2);
  } else {
    array_index = 0;
    strncpy(sym_data_desc.name,argv[2],VARNAME_MAX_SIZE);  
  }
  /* 
   * Use low-level subscribe in order to discover the 
   * type of the variable
   */
  sym_value = bb_subscribe(mybb,&sym_data_desc);

  if (NULL==sym_value) {
    fprintf(stdout,"%s: symbol <%s> not found in BlackBoard <%s>\n",
	    argv[0],
	    sym_data_desc.name,
	    argv[1]);
  } else {
    if (array_index>=sym_data_desc.dimension) {
      fprintf(stdout,"%s: symbol array index <%d> out of bound (max index of <%s> is <%d>)\n",
	      argv[0],array_index,sym_data_desc.name,sym_data_desc.dimension-1);
    } else {
      bb_value_write(mybb,sym_data_desc,argv[3],array_index);
    }
  }        
  /* Detach from BB */
  /******************/
  bb_detach(&mybb);  
  
  return (erreur);
}
