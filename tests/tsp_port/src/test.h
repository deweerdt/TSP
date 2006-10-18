#ifndef _TEST_H_
#define _TEST_H_

#include <pthread.h>

typedef struct wrapper_s
{
    int id;    

    pthread_mutex_t lock;
    pthread_cond_t  wait;

} wrapper_t;

static void * wrapper_routine(void *);

#endif