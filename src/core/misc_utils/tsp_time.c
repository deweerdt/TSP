/*

$Header: /home/def/zae/tsp/tsp/src/core/misc_utils/tsp_time.c,v 1.11 2006-10-18 09:58:48 erk Exp $

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
Maintainer : tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for tsp time functions

-----------------------------------------------------------------------
 */

#include <time.h>

#include "tsp_sys_headers.h"

#include "tsp_time.h"

#if defined (WIN32)
#include <rpc/rpc.h>
#endif
tsp_hrtime_t tsp_gethrtime()
{
#ifdef TSP_SYSTEM_HAVE_GETHRTIME
  return gethrtime();
#else
  struct timeval tp;
  gettimeofday(&tp, (void*)NULL);
   return ((tsp_hrtime_t)tp.tv_sec *  TSP_INT64_CONSTANT(1000000000)
	   + (tsp_hrtime_t)tp.tv_usec*TSP_INT64_CONSTANT(1000));
#endif
}


int tsp_usleep(int useconds)
{

#ifdef TSP_SYSTEM_HAVE_NANOSLEEP
  struct timespec ts;
  ts.tv_sec = useconds / 1000000;
  ts.tv_nsec = (useconds % 1000000) * 1000;
  return nanosleep(&ts, NULL);
#else

#ifdef TSP_SYSTEM_HAVE_THREADSAFE_USLEEP
  return usleep(useconds);
#else
  
#ifdef TSP_SYSTEM_USE_WIN32_SLEEP
  Sleep(useconds / 1000);
  return 0;
#else
  ERROR__you_should_try_to_find_some_kind_of_wait_function
#endif /*TSP_SYSTEM_HAVE_SLEEP*/

#endif /*TSP_SYSTEM_HAVE_THREADSAFE_USLEEP*/

#endif /*TSP_SYSTEM_HAVE_NANOSLEEP*/

}

