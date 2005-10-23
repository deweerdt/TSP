/*
 * Buzy Loop on VME A24 Top synchro
 * Read Time, when 100ms, add in histogram
 */

/* POSIX.1 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

/* POSIX.4 */
#include <signal.h>

/* Boue */

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sched.h>
#include <sys/signal.h>
#include <string.h>

/* Temporary include, wait for libc patch of setaffinity */
#include "affinity.h"

/* Projet */
#include "histogram.h"
#include "tsp_provider_init.h"
#include "tsp_datapool.h"
#include "tsp_sys_headers.h"
#include <tsp_glu.h>
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "calc_func.h"


GLU_handle_t* GLU_stub_create();

/* TSP glue server defines */
#define TSP_STUB_FREQ 128 /*Hz, must be a 2 power for RTC*/
#define TSP_PERIOD_US (1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS 100
/* Glue server ringbuf size */


/* Nasty static variables */
static GLU_handle_t* stub_GLU = NULL;
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;

// My Globals : Bouh
int stopit;
int rt_mode=1; /* 0 = NRT, 1  RT mode */
int it_mode=0; /* 0 = Polling on date, 1=IT , 2=IT blocking IO */
unsigned long affinity_mask=0x1; /* Run on proc #0 */

void fatal(char *txt)
{
  fprintf (stderr, "ERROR : %s\n", txt);
  exit (-1);
}

double second(void) {
	struct timeval tv;
	gettimeofday(&tv,0);
	return tv.tv_sec + 1e-6 * tv.tv_usec;
}

typedef unsigned long long u64;

/* return in nanosecds */
u64 rdtsc(void) {
	u64 tsc;
	__asm__ __volatile__("rdtsc" : "=A" (tsc));
	return tsc;
}


void rtc_action(int toDo)
{
  static int fd;
  int flags;
  char * dev_file_name = "/dev/rtc";
  long long data[3];

  switch (toDo)
    {
    case 0 : /* Open */
	flags = O_RDONLY;
	if (it_mode !=2) flags |= O_NONBLOCK; 
	fd = open(dev_file_name,flags);
	if (fd == -1) 
		fatal(dev_file_name);

	if (ioctl(fd, RTC_IRQP_SET, TSP_STUB_FREQ) == -1) {
	    fatal("ioctl(RTC_IRQP_SET) failed");
	}
	printf("Configured RTC for %d Hz, IRQ 8, it_mode=%d\n", TSP_STUB_FREQ, it_mode);

	/* Enable periodic interrupts */
	if (ioctl(fd, RTC_PIE_ON, 0) == -1)
	  fatal("ioctl(RTC_PIE_ON) failed");
	break;

    case 1: /* Close */
	if (ioctl(fd, RTC_PIE_OFF, 0) == -1)
		fatal("ioctl(RTC_PIE_OFF) failed");
	break;

    case 2: /* Poll*/
      if (it_mode!=2)
	while (read(fd,&data[0],sizeof(data)) == -1 && errno == EAGAIN);
      else
	read(fd, &data, sizeof(data));

      if (errno != EAGAIN && errno != 0)
	fatal("blocking read failed");
      break;
    }
}

void rtc_init(void)
{
  rtc_action(0);
}
 

void rtc_close(void)
{
  rtc_action(1);
}

/* in micro seconds */
u64 rtc_read_time(void)
{
  return (u64)(rdtsc()/1e3);
}

void rtc_wait_next_it(void)
{
  rtc_action(2);
}


void signalled(int sig)
{
  stopit = 1;
}

void usage(char *name)
{
  printf ("USAGE: %s [-r rtMode(0|1) ]  [-i itMode(0|1|2) ] [-m affinityMask ]\n", name);
  exit (-1);
}


int rt_memory_lock(int pid)
{
	if(mlockall(MCL_CURRENT | MCL_FUTURE)!=0){
		fatal("ERROR : rt_memory_lock : mlockall error : ");
	}
	return 0;
}

int rt_memory_unlock(int pid)
{
	if(munlockall()!=0){
		fatal("rt_memory_unlock : munlockall error : ");
	}
	return 0;
}


int set_real_time_priority(int pid, unsigned long mask)
{
	struct sched_param schp;
	unsigned int len; 

	/* Affinity to CPUs describe in mask  */
	len = sizeof(mask);

	if (sched_setaffinity(pid, len, &mask)) {
		fatal ("could not set pid's affinity.\n");
	}

	/*
	 * set the process to realtime privs
	 */
	memset(&schp, 0, sizeof(schp));
	schp.sched_priority = sched_get_priority_max(SCHED_FIFO);
	
	if (sched_setscheduler(pid, SCHED_FIFO, &schp) != 0) {
		fatal("sched_setscheduler");
	}

	return 0;
}
 

int main(int argc, char *argv[])
{
  char myopt; /* Options */
  GLU_handle_t* GLU_stub = GLU_stub_create();

  /* Init server */
  if(!TSP_provider_init(GLU_stub,&argc, &argv))
    {
      exit (-1);
    }
  
  while ((myopt = getopt(argc, argv, "hr:m:i:")) != -1) {
    switch(myopt) {
    case 'r':	rt_mode = atoi(optarg);		break;
    case 'i':	it_mode = atoi(optarg);		break;
    case 'm':	affinity_mask = atoi(optarg);	break;
    case 'h':	usage(argv[0]);			break;
    default:	break;
    }
  }

  printf ("#============================================================#\n");
  printf ("# Launching <LinuxRTServer> for %d Symbols at %dHz #\n", GLU_MAX_SYMBOLS, TSP_STUB_FREQ);
  printf ("# RTmode=%d	Mask=0x%lX \n", rt_mode, affinity_mask);
  printf ("#============================================================#\n");

  /* Open the logical device */
  printf ("=====================================\n");
  rtc_init();
  tsp_histo_init();
  tsp_histo_set_delta (10); /* Jitter of 10us*/
  if (rt_mode) {
      rt_memory_lock(0);
    }
  
  /* Can use kill -2 for stopping prog */
  signal(SIGINT, signalled);
  
  TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_BLOCKING);
  
  return 0;
}
   


/* ===================================== TSP PART ==================================== */

char* STUB_GLU_get_name(GLU_handle_t* this)
{
  if (rt_mode)
    return "LinuxRT";
  else
    return "LinuxNRT";
}
 
int  STUB_GLU_get_symbol_number(void)

{
  int i = 0;
  TSP_sample_symbol_info_t* p  = X_sample_symbol_info_list_val;
  for( p=X_sample_symbol_info_list_val; p->name!=0 ; p++) i++;
  return i;
}

void* STUB_GLU_thread(void* arg)
{
  
  static int last_missed = 0;
  int i, symbols_nb, my_time=0;
  glu_item_t item;
  unsigned long  last_us, now_us, t0_us, jitter, jitter_max=0;
  long delay;
  double retard, simtime, rtctime;

  printf (" GLU_thread :Last Time in NO RT mode !!!!\n");
  fflush (stdout);
  if (rt_mode) {
    set_real_time_priority (0 /*myself*/, affinity_mask);
  }

  symbols_nb  = STUB_GLU_get_symbol_number();

  /* Must synchronise to first IT then RAZ  handler */
  rtc_wait_next_it();
  last_us = now_us = rtc_read_time();
  rtc_wait_next_it();
  t0_us = rtc_read_time();

  /* infinite loop for symbols generation */
  stopit = 0;
  while(!stopit)
    {
      delay = 0;

      /* Wait to a new Period of time */
      switch (it_mode) 
	{
	case 0: /* Polling like a beast on time */
	  while (delay < TSP_PERIOD_US)
	    {
	      now_us = rtc_read_time();
	      delay =  (now_us - last_us); /* in us  on 64 bits */
	    }
	  break;
	case 1: /* Blocking / Non blocking IT before read */
	  rtc_wait_next_it();
	  now_us = rtc_read_time();
	  delay =  (now_us - last_us); /* in us  on 64 bits */
	  if (delay<0 || delay >= 1.5* TSP_PERIOD_US) printf ("Warning : delay=%ld, now=%lu, last=%lu\n", delay, now_us, last_us);
	  break;
	default: break;
	}
	  

      /* We got a new period, must calculate the data */
      my_time++;    
      last_us = now_us;
      jitter =  delay-TSP_PERIOD_US; 
      simtime = (double)(my_time) / (double)(TSP_STUB_FREQ);
      rtctime =  ((now_us-t0_us)/1e6);
      retard = rtctime - simtime;
      if (jitter>jitter_max) jitter_max = jitter;
      tsp_histo_enter_with_date(jitter, rtctime);

      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  item.time = my_time;
	  item.provider_global_index = i;
	  switch (i) 
	    {
	    case 0: item.value = simtime; break; /* s */
	    case 1: item.value = rtctime; break;  /* s */
	    case 2: item.value = now_us; break;  /* s */
	    case 3: item.value = (double)delay/1e3; break;   /* ms */
	    case 4: item.value = jitter; break; /* jitter us */
	    case 5: item.value = (jitter)/1e3; break; /* jitter ms */
	    case 6: item.value = (jitter_max)/1e3; break; /* jitter ms */
	    case 7: item.value = retard; break; /* jitter ms */
	    default :
		item.value = calc_func(i, my_time); break;
	    }
	  TSP_datapool_push_next_item(&item);
	}
      
      
      /* Finalize the datapool state with new time : Ready to send */
      TSP_datapool_push_commit(my_time, GLU_GET_NEW_ITEM);

      if (rt_mode==0 && !(my_time%10))  
	 printf("TOP %d : %s=%g \t%s=%g \t%s=%g \t%s=%g", my_time,
		X_sample_symbol_info_list_val[0].name, simtime,
		X_sample_symbol_info_list_val[1].name, rtctime,
		X_sample_symbol_info_list_val[7].name, retard/1e3,
		X_sample_symbol_info_list_val[6].name, jitter_max/1e3 );
    }
 
  /* Exit the program  */
  printf ("Exiting from GLU_Thread loop because stopit=%d\n", stopit);

	    
  tsp_histo_dump(stdout , "LOOP on RTC");
 
  printf ("simtime\t= %gs\n", simtime);
  printf ("rtctime\t= %gs\n", rtctime);
  printf ("retard\t= %gs\n", retard);
  printf ("delay\t= %gms\n", (double)delay/1e3);
  printf ("max jit\t= %gms\n", jitter_max/1e3);

  exit (-1);
  return arg;

}

int STUB_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[])
{
  int i;
  char symbol_buf[50];
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS+1, sizeof (TSP_sample_symbol_info_t)) ;
  for (i=0; i<GLU_MAX_SYMBOLS; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
    }  

  /*overide name*/
  i=0;
  X_sample_symbol_info_list_val[0].name = strdup("simtime");
  X_sample_symbol_info_list_val[1].name = strdup("rtime");
  X_sample_symbol_info_list_val[2].name = strdup("btime");
  X_sample_symbol_info_list_val[3].name = strdup("delay");
  X_sample_symbol_info_list_val[4].name = strdup("jitter");
  X_sample_symbol_info_list_val[5].name = strdup("jitter_ms");
  X_sample_symbol_info_list_val[6].name = strdup("jitter_max");
  X_sample_symbol_info_list_val[7].name = strdup("retard");

  return TRUE;
}

int  STUB_GLU_get_sample_symbol_info_list(GLU_handle_t* h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  symbol_list->TSP_sample_symbol_info_list_t_len = STUB_GLU_get_symbol_number();
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
  return TRUE;
}


/* create the GLU handle instance for STUB */
GLU_handle_t* GLU_stub_create() {
  
  /* create a default GLU */
  GLU_handle_create(&stub_GLU,"RTStubbed",GLU_SERVER_TYPE_ACTIVE,TSP_STUB_FREQ);
  
  stub_GLU->initialize     = &STUB_GLU_init;
  stub_GLU->run            = &STUB_GLU_thread;
  stub_GLU->get_ssi_list   = &STUB_GLU_get_sample_symbol_info_list;
  /* override default methods */
  stub_GLU->get_name       = &STUB_GLU_get_name;

  return stub_GLU;
}
