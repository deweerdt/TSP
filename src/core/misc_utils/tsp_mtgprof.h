/* 
 * pthread_create wrapper for gprof compatibility
 * Provided by Samuel Hocevar at http://sam.zoy.org/doc/programming/gprof.html
 *
 * needed headers: <pthread.h>
 *                 <sys/time.h>
 */

#include <pthread.h>
#if defined (WIN32)
/*
 * Names of the interval timers, and structure
 * defining a timer setting.
 */
#define	ITIMER_REAL	0
#define	ITIMER_VIRTUAL	1
#define	ITIMER_PROF	2
/*
 * Structure used under UNIX which are not defined under Windows
 */
/* define also in tsp_time.c because used into */
typedef 	 long suseconds_t;
struct timeval
{
	time_t		tv_sec;		/* seconds */
	suseconds_t	tv_usec;	/* and microseconds */
};
/* end of the define also in tsp_time.c */
struct	itimerval
{
	struct	timeval it_interval;	/* timer interval */
	struct	timeval it_value;	/* current value */
};
#else
#include <sys/time.h>
#endif

#ifndef __TSP_MTGPROG
#define __TSP_MTGPROG 1

#ifndef VXWORKS
typedef struct wrapper_s
{
    void * (*start_routine)(void *);
    void * arg;

    pthread_mutex_t lock;
    pthread_cond_t  wait;

    struct itimerval itimer;

} wrapper_t;

static void * wrapper_routine(void *);

int gprof_pthread_create(pthread_t * thread, pthread_attr_t * attr,
                         void * (*start_routine)(void *), void * arg);

#endif /* VXWORKS */

#endif /* __TSP_MTGPROG */
