/* 
 * pthread_create wrapper for gprof compatibility
 * Provided by Samuel Hocevar at http://sam.zoy.org/doc/programming/gprof.html
 *
 * needed headers: <pthread.h>
 *                 <sys/time.h>
 */


#include "tsp_mtgprof.h"
#ifndef VXWORKS
/* Same prototype as pthread_create; use some #define magic to
 * transparently replace it in other files */
int gprof_pthread_create(pthread_t * thread, pthread_attr_t * attr,
                         void * (*start_routine)(void *), void * arg)
{
    wrapper_t wrapper_data;
    int i_return;

    /* Initialize the wrapper structure */
    wrapper_data.start_routine = start_routine;
    wrapper_data.arg = arg;
#if defined (_WIN32)
    /* FIXME under Windows : interval timer */
#else
    getitimer(ITIMER_PROF, &wrapper_data.itimer);
#endif
    pthread_cond_init(&wrapper_data.wait, NULL);
    pthread_mutex_init(&wrapper_data.lock, NULL);
    pthread_mutex_lock(&wrapper_data.lock);

    /* The real pthread_create call */
    i_return = pthread_create(thread, attr, &wrapper_routine,
                                            &wrapper_data);

    /* If the thread was successfully spawned, wait for the data
     * to be released */
    if(i_return == 0)
    {
        pthread_cond_wait(&wrapper_data.wait, &wrapper_data.lock);
    }

    pthread_mutex_unlock(&wrapper_data.lock);
    pthread_mutex_destroy(&wrapper_data.lock);
    pthread_cond_destroy(&wrapper_data.wait);

    return i_return;
}

/* The wrapper function in charge for setting the itimer value */
static void * wrapper_routine(void * data)
{
    /* Put user data in thread-local variables */
    void * (*start_routine)(void *) = ((wrapper_t*)data)->start_routine;
    void * arg = ((wrapper_t*)data)->arg;

    /* Set the profile timer value */
#if defined (_WIN32)
    /* FIXME under Windows : interval timer */
#else
    setitimer(ITIMER_PROF, &((wrapper_t*)data)->itimer, NULL);
#endif

    /* Tell the calling thread that we don't need its data anymore */
    pthread_mutex_lock(&((wrapper_t*)data)->lock);
    pthread_cond_signal(&((wrapper_t*)data)->wait);
    pthread_mutex_unlock(&((wrapper_t*)data)->lock);

    /* Call the real function */
    return start_routine(arg);
}
#endif /* VXWORKS */
