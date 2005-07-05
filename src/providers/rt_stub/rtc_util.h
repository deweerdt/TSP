/*!  \file 

$Id: rtc_util.h,v 1.1 2005-07-05 14:55:11 tsp_admin Exp $

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


void fatal(char *txt);

typedef unsigned long long u64;

/* return in nanosecds */
u64 rdtsc(void);
void rtc_action(int toDo);
void rtc_init(void);
void rtc_close(void);
/* in micro seconds */
u64 rtc_read_time(void);
void rtc_wait_next_it(void);
int rt_memory_lock(int pid);
int rt_memory_unlock(int pid);
int set_real_time_priority(int pid, unsigned long mask, int sched, int prio);

 
