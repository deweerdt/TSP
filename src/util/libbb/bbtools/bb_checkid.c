
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>

#include <bb_utils.h>

int 
main (int argc, char ** argv) {

  /* Définitions des variables */
  int retcode=0;
  int32_t user_specific_value;
  char* shm_name;
  char* sem_name;
  char* msg_name;
  
  /* Attach to  BB */
  /*****************/
  if (argc<3) {
    fprintf(stderr,"%s : argument missing\n",argv[0]);
    fprintf(stderr,"Usage : %s <bbname> <user_specific_value>\n",argv[0]);
    exit(-1);
  }

  user_specific_value = atoi(argv[2]);
  fprintf(stdout,"Computing BB IPC Id for BB <%s> with specific user value <%d>\n",
	  argv[1],user_specific_value);

  
  shm_name = bb_utils_build_shm_name(argv[1]);
  sem_name = bb_utils_build_sem_name(argv[1]);
  msg_name = bb_utils_build_msg_name(argv[1]);

  fprintf(stdout,"SHM Key [name=<%s>] is 0x%08x\n",shm_name,bb_utils_ntok_user(shm_name,user_specific_value));
  fprintf(stdout,"SEM Key [name=<%s>] is 0x%08x\n",sem_name,bb_utils_ntok_user(sem_name,user_specific_value));
  fprintf(stdout,"MSG Key [name=<%s>] is 0x%08x\n",msg_name,bb_utils_ntok_user(msg_name,user_specific_value));

  free(msg_name);
  free(sem_name);    
  free(shm_name);

  return (retcode);
}
