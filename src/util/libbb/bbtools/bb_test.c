
#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <math.h>
#include <libgen.h>
#include <bb_core.h>
#include <bb_simple.h>

int 
main (int argc, char ** argv) {

  /* Définitions des variables */
 
  int retcode=0;
  int data_size;
  int n_data;
  int32_t* Toto;
  double* Titi;
  double* Tata;
  double* HugeArray;
  int i;
  int synchro;
  S_BB_T* mybb;

  /*
   * Analyse options de lancement
   */
  if (EOF != getopt(argc,argv,"s")) {
    synchro = 1;
    printf("Run with synchro ACTIVE\n");
  } else {
    synchro = 0;
    printf("Run with synchro INACTIVE\n");
  }
  /* Creation BB */
  /***************/
  n_data = 10000;
  data_size = n_data*8 + 5000*30*4 + 200000*8;
  if (E_NOK==bb_create(&mybb,argv[0],n_data,data_size)) {
    bb_attach(&mybb,argv[0]);
/*     bb_destroy(&mybb); */
/*     bb_create(&mybb,argv[0],n_data,data_size); */
  }

  /* Publish data in the BB */
  /**************************/
  Toto = (int32_t*) bb_simple_publish(mybb,"Toto",basename(argv[0]),1, E_BB_INT32, sizeof(int32_t),3);  
  for (i=0;i<3;++i) {
    Toto[i] = i;
  }
  for (i=0;i<3;++i) {
    printf("@Toto = 0x%x, Toto[%d] = %d\n",(unsigned int)&Toto[i],i,Toto[i]);  
  }
  
  Titi= (double*) bb_simple_publish(mybb,"Titi",basename(argv[0]),1, E_BB_DOUBLE, sizeof(double),1);
  *Titi = 3.14159;
  
  printf("@Titi = 0x%x, Titi = %f\n",(unsigned int)Titi,*Titi);
  
  Tata = (double*) bb_simple_publish(mybb,"Tata",basename(argv[0]),1, E_BB_DOUBLE, sizeof(double),9);
  for (i=0;i<9;++i) {
    Tata[i] = cos(*Titi/(i+1));
  }

#define HUGE_ARRAY
#ifdef HUGE_ARRAY
#define BIG_SIZE 200000
#else
#define BIG_SIZE 10
#endif
  HugeArray = (double*) bb_simple_publish(mybb,"HugeArray",basename(argv[0]),1, E_BB_DOUBLE, sizeof(double),BIG_SIZE);
  for (i=0;i<BIG_SIZE;++i) {
    HugeArray[i] = cos(*Titi/(i+1));
  }
  
  for (i=0;i<3;++i) {
    printf("Toto[%d] = %d\n",i,Toto[i]);  
  }
  printf("Titi = %f\n",*Titi);
  for (i=0;i<9;++i) {
    printf("Tata[%d] = %f\n",i,Tata[i]);  
  }
  
  i = 0;
  sleep(5);
  while (i<2000000) {
    Tata[0] = sin((2.0*i)/180.0);
    Tata[1] = cos((2.0*i)/180.0);
    Toto[0] = i % 1000;
    if (0==(i % 100)) {
      Toto[1] = -Toto[1];
    }     
    if ((i % 2) == 0) {
      bb_lock(mybb);
      *Titi += 1;
      printf("Titi = %f\n",*Titi);
      printf("BB locked <i=%d>\n",i);
    } else {
      *Titi += 1;
      printf("Titi = %f\n",*Titi);
      bb_unlock(mybb);      
      printf("BB unlocked\n");
      if (synchro) {
	printf("Synchro GO...");
	fflush(stdout);
	bb_simple_synchro_go(mybb,BB_SIMPLE_MSGID_SYNCHRO_COPY);
	printf("OK.\n");
/* 	printf("Synchro Wait..."); */
/* 	fflush(stdout); */
/* 	bbntb_synchro_wait(BBNTB_MSGID_SYNCHRO_COPY_ACK); */
/* 	printf("OK.\n"); */
/* 	fflush(stdout); */
      }
    }
    usleep(10000);
    ++i;
  }
  
  /* Destruction BB */
  /******************/
  retcode = bb_destroy(&mybb);

  return (retcode);
}
