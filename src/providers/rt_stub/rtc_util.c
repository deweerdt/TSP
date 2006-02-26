/*

$Id: rtc_util.c,v 1.2 2006-02-26 13:36:06 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Give some simple functions to use RTC

-----------------------------------------------------------------------
*/

/* POSIX.1 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

/* POSIX.4 */
#include <signal.h>
#include <sched.h>


/* Boue */
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/signal.h>

/* Temporary include, wait for libc patch of setaffinity */
#include "affinity.h"

#include "rtc_util.h"

#define RTC_STUB_FREQ	1000 /* Hz */
static int it_mode=0; /* 0 = Polling on date, 1=IT , 2=IT blocking IO */

void fatal(char *txt)
{
  fprintf (stderr, "ERROR : %s\n", txt);
  exit (-1);
}

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

	if (ioctl(fd, RTC_IRQP_SET, RTC_STUB_FREQ) == -1) {
	    fatal("ioctl(RTC_IRQP_SET) failed");
	}
	printf("Configured RTC for %d Hz, IRQ 8, it_mode=%d\n", RTC_STUB_FREQ, it_mode);

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


int set_real_time_priority(int pid, unsigned long mask, int sched, int prio)
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
	schp.sched_priority = prio;
	
	if (sched_setscheduler(pid, sched, &schp) != 0) {
		fatal("sched_setscheduler");
	}

	return 0;
}
 
