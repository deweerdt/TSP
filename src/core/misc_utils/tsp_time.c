#include "tsp_sys_headers.h"

#include <time.h>
#include <sys/time.h>

#include "tsp_time.h"

tsp_hrtime_t tsp_gethrtime(void)
{
  SFUNC_NAME(tsp_gethrtime);

#ifdef TSP_SYSTEM_HAVE_GETHRTIME
  return gethrtime();
#else
  struct timeval tp;
  gettimeofday(&tp, (void*)NULL);
   return ((tsp_hrtime_t)tp.tv_sec *  G_GINT64_CONSTANT(1000000000)
	   + (tsp_hrtime_t)tp.tv_usec*G_GINT64_CONSTANT(1000));
#endif
}


int tsp_usleep(int useconds)
{

#ifdef TSP_SYSTEM_HAVE_NANOSLEEP
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = useconds * 1000;
  return nanosleep(&ts, NULL);
#else

#ifdef TSP_SYSTEM_HAVE_THREADSAFE_USLEEP
  return usleep(useconds);
#else
  ERROR__you_should_try_to_find_some_kind_of_wait_function
#endif /*TSP_SYSTEM_HAVE_THREADSAFE_USLEEP*/

#endif /*TSP_SYSTEM_HAVE_NANOSLEEP*/

}
