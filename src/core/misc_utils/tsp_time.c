#include "tsp_sys_headers.h"

#include <time.h>

#include "tsp_time.h"

tsp_hrtime_t tsp_gethrtime(void)
{
#ifdef TSP_SYSTEM_HAVE_GETHRTIME
  return gethrtime();
#else
  struct timespec tp;
  gettimeofday(&tp, (void*)NULL);
  return ((tsp_hrtime_t)tp.tv_sec *  G_GINT64_CONSTANT(1000000000) + (tsp_hrtime_t)tp.tv_nsec);
#endif

}

