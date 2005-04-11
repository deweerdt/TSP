
#include <string.h>

#include <ioLib.h>
#include <taskLib.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <math.h>

#include <bb_core.h>
#include <bb_simple.h>
#include <bb_tools.h>


typedef struct mytype {
  int     a;
  double  d;
  uint8_t byte;
} mytype_t;

double *DYN_0_d_qsat;		/* 4 elems */
double *ORBT_0_d_possat_m;	/* 4 elems */
double *ECLA_0_d_ecl_sol;
double *ECLA_0_d_ecl_lune;
double *POSA_0_d_DirSol;	/* 3 elems */
double *POSA_0_d_DirLun;	/* 3 elems */
double *Sequenceur_0_d_t_s;

#define BB_PUBLISH(bb_name,name,base,instance,bb_type,c_type,dim) \
  name = (c_type*) bb_simple_publish(bb_name, #name, (base), instance, bb_type, sizeof(c_type), dim); \
  printf ("INFO : BB_PUBLISH %s[%d] type %s\n", #name, dim, #c_type); \
  for (i=0;i<dim;++i) { \
    name[i] = (c_type)0; \
  }

int _stub_bb_server_stop = 0;

int stub_bb_server_launch(int synchro, char * bb_name) {

  int data_size;
  int n_data;
  S_BB_T* mybb;
  uint32_t* display_level;
  int32_t* Toto;
  double* Titi;
  double* Tata;
  double* HugeArray;
  int i;
  mytype_t* myvar;
  char*     astring;

  /* Creation BB */
  /***************/
  n_data = 1000;
  data_size = n_data*8 + 500*30*4 + 20000*8;
  if (E_NOK==bb_create(&mybb,bb_name,n_data,data_size)) {
    bb_attach(&mybb,bb_name);
  }

  /* Publish data in the BB */
  /**************************/ 

  display_level = (uint32_t*) bb_simple_publish(mybb,"display_level",bb_name,-1, E_BB_UINT32, sizeof(uint32_t),1);
  *display_level = 0;
  
  myvar = (mytype_t*) bb_simple_publish(mybb,"mytype_t_var",bb_name,-1, E_BB_USER, sizeof(mytype_t),1);
  myvar->a = 1;
  myvar->d = 3.14159;
  myvar->byte = 0xFF;

  astring = (char*) bb_simple_publish(mybb,"astring",bb_name,-1, E_BB_CHAR, sizeof(char),20);
  strncpy(astring,"A little string",20);

  Toto = (int32_t*) bb_simple_publish(mybb,"Toto",bb_name,1, E_BB_INT32, sizeof(int32_t),3);  
  for (i=0;i<3;++i) {
    Toto[i] = i;
  }
  for (i=0;i<3;++i) {
    printf("@Toto = 0x%x, Toto[%d] = %d\n",(unsigned int)&Toto[i],i,Toto[i]);  
  }
  
  Titi= (double*) bb_simple_publish(mybb,"Titi",bb_name,1, E_BB_DOUBLE, sizeof(double),1);
  *Titi = 3.14159;
  
  printf("@Titi = 0x%x, Titi = %f\n",(unsigned int)Titi,*Titi);
  
  Tata = (double*) bb_simple_publish(mybb,"Tata",bb_name,1, E_BB_DOUBLE, sizeof(double),9);
  for (i=0;i<9;++i) {
    Tata[i] = cos(*Titi/(i+1));
  }

#undef HUGE_ARRAY
#ifdef HUGE_ARRAY
#define BIG_SIZE 200000
#else
#define BIG_SIZE 10
#endif
  HugeArray = (double*) bb_simple_publish(mybb,"HugeArray",bb_name,1, E_BB_DOUBLE, sizeof(double),BIG_SIZE);
  for (i=0;i<BIG_SIZE;++i) {
    HugeArray[i] = cos(*Titi/(i+1));
  }

  BB_PUBLISH(mybb, DYN_0_d_qsat, "", -1, E_BB_DOUBLE, double, 4);
  BB_PUBLISH(mybb, ORBT_0_d_possat_m, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, ECLA_0_d_ecl_sol, "", -1,  E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, ECLA_0_d_ecl_lune, "", -1,  E_BB_DOUBLE, double, 1);
  BB_PUBLISH(mybb, POSA_0_d_DirSol, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, POSA_0_d_DirLun, "", -1,  E_BB_DOUBLE, double, 3);
  BB_PUBLISH(mybb, Sequenceur_0_d_t_s, "", -1,  E_BB_DOUBLE, double, 1);

  for (i=0;i<3;++i) {
    printf("Toto[%d] = %d\n",i,Toto[i]);  
  }
  printf("Titi = %f\n",*Titi);
  for (i=0;i<9;++i) {
    printf("Tata[%d] = %f\n",i,Tata[i]);  
  }
  
  i = 0;
  taskDelay(5*sysClkRateGet());
  while (1) {
    double dq1, dq2, teta, d=7e6 /*km*/;
    *Sequenceur_0_d_t_s = *Sequenceur_0_d_t_s + 0.01; /* 10 Hz Simu */
    teta = *Sequenceur_0_d_t_s*2*3.14/100; /* 100s for a complete turn around */
    ORBT_0_d_possat_m[0] = d*cos(teta); /* simple circular orbit */
    ORBT_0_d_possat_m[1] = d*sin(teta); /* do better later */
    ORBT_0_d_possat_m[2] = d*0.0; /* force Z axis is null */
    dq1 = cos(teta)/4;
    dq2 = sin(teta)/4;
    DYN_0_d_qsat[0] = 0.7+dq1;
    DYN_0_d_qsat[1] = 0.3-dq1;
    DYN_0_d_qsat[2] = 0.3+dq2;
    DYN_0_d_qsat[3] = -0.2-dq2;
    POSA_0_d_DirLun[1] = 0.4; /* e6 */
    POSA_0_d_DirSol[0] = 0.15; /* e3 */
    *ECLA_0_d_ecl_lune = 1;
    *ECLA_0_d_ecl_sol = 1;


    Tata[0] = sin((2.0*i)/180.0);
    Tata[1] = cos((2.0*i)/180.0);
    Toto[0] = i % 1000;
    if (0==(i % 100)) {
      Toto[1] = -Toto[1];
    }     
    if ((i % 2) == 0) {
      bb_lock(mybb);
      *Titi += 1;
      if (*display_level & 0x1) {
	printf("Titi = %f\n",*Titi);
      }
      if (*display_level & 0x2) {
	printf("BB locked <i=%d>\n",i);
      }
    } else {
      *Titi += 1;
      if (*display_level & 0x1) {
	printf("Titi = %f\n",*Titi);
      }
      bb_unlock(mybb);      
      if (*display_level & 0x2) {
	printf("BB unlocked\n");
      }
      if (synchro) {
	if (*display_level & 0x4) {
	  printf("Synchro GO...");
	  fflush(stdout);
	}
	bb_simple_synchro_go(mybb,BB_SIMPLE_MSGID_SYNCHRO_COPY);
	if (*display_level & 0x4) {
	  printf("OK.\n");
	}
      }
    }
    taskDelay(sysClkRateGet());
    ++i;
    /*    printf (".");*/
    if(_stub_bb_server_stop != 0) break;
  }
  
  return 0;
}
