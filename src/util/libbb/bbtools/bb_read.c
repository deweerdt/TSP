
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
  char*    array_name;
  int32_t  array_index; 

  if (argc<3) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage: %s <bbname> <symbol_name>\n",argv[0]);
    exit(-1);
  }
  else {
    /* do we want to read a single element of an array var */
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
      array_index = -1;
      strncpy(sym_data_desc.name,argv[2],VARNAME_MAX_SIZE);  
    }
    
    if (array_name) {
      fprintf(stdout,"%s: Trying to read index <%d> of array symbol <%s> on blackboard <%s>...\n",
	      argv[0],array_index,sym_data_desc.name,argv[1]);
    } else {
      fprintf(stdout,"%s: Trying to read symbol <%s> on blackboard <%s>...\n",
	      argv[0],argv[2],argv[1]);
    }
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
  sym_value = bb_subscribe(mybb,&sym_data_desc);

  if (NULL==sym_value) {
    fprintf(stdout,"%s: symbol not found\n",argv[0]);
  } else {
    if ((array_index!=-1) && (sym_data_desc.dimension <= array_index)) {
      fprintf(stdout,"%s: index <%d> exceeding symbol array dimension <%d>\n",
	      argv[0],array_index,sym_data_desc.dimension);
    } else {
      bb_data_header_print(sym_data_desc,stdout,array_index);
      bb_value_print(mybb,sym_data_desc,stdout,array_index);
      bb_data_footer_print(sym_data_desc,stdout,array_index);
    }
  }        
  /* Detach from BB */
  /******************/
  retcode = bb_detach(&mybb);  
  
  return (retcode);
}
