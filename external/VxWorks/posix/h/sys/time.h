#ifndef _SYS_TIME_H
#define _SYS_TIME_H

#include <sys/types.h>
#include <sys/times.h>

/*
 * Operations on timevals.
 *
 * NB: timercmp does not work for >= or <=.
 */
#define timerisset(tvp)         ((tvp)->tv_sec || (tvp)->tv_usec)
#define timercmp(tvp, uvp, cmp) \
        /* CSTYLED */ \
        ((tvp)->tv_sec cmp (uvp)->tv_sec || \
        ((tvp)->tv_sec == (uvp)->tv_sec && \
        /* CSTYLED */ \
        (tvp)->tv_usec cmp (uvp)->tv_usec))
 
#define timerclear(tvp)         (tvp)->tv_sec = (tvp)->tv_usec = 0

int gettimeofday (struct timeval *tp,void *vide) ;

#endif /* _SYS_TIME_H */
