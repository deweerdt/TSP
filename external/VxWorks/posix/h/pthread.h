/* pthread.h - header for POSIX threads (pthreads) */
 
/* Copyright 1984-2002 Wind River Systems, Inc. */
 
/*
modification history
--------------------
01c,22oct01,jgn  add definition of PTHREAD_STACK_MIN (SPR #71110)
01b,11sep00,jgn  split into kernel & user level parts (SPR #33375)
01a,17jul00,jgn  created from DOT-4 version
*/
 
#ifndef __INCpthreadh
#define __INCpthreadh

/* includes */

#include "vxWorks.h"
#include "semLib.h"
#include "signal.h"
#include "timers.h"
#include "sched.h"

#if defined(__cplusplus)
extern "C" {
#endif	/* __cplusplus */

/* defines */

#define _POSIX_THREADS				1
#define _POSIX_THREAD_PRIORITY_SCHEDULING	1
#define _POSIX_THREAD_ATTR_STACKSIZE		1
#define _POSIX_THREAD_ATTR_STACKADDR		1

#ifdef _POSIX_THREAD_PROCESS_SHARED
#define PTHREAD_PROCESS_PRIVATE	0
#define PTHREAD_PROCESS_SHARED	1
#endif /* _POSIX_THREAD_PROCESS_SHARED */

#ifdef _POSIX_THREAD_ATTR_STACKSIZE
#define PTHREAD_STACK_MIN		4096	/* suggested minimum */
#endif

#define	PTHREAD_INHERIT_SCHED		0	/* implementation default */
#define	PTHREAD_EXPLICIT_SCHED		1
#define PTHREAD_SCOPE_PROCESS		2
#define PTHREAD_SCOPE_SYSTEM		3	/* implementation default */


#define PTHREAD_ONCE_INIT		{0}

#define PTHREAD_INITIALIZED		1	/* object can be used */
#define PTHREAD_DESTROYED		-1	/* object status */
#define PTHREAD_MUTEX_INITIALIZER	{NULL, TRUE, 0, 0, 0, 0, \
    {PTHREAD_INITIALIZED, PTHREAD_PRIO_INHERIT, 0}}
#define PTHREAD_COND_INITIALIZER	{NULL, TRUE, 0, 0, NULL}

#define	PTHREAD_CREATE_DETACHED		0
#define	PTHREAD_CREATE_JOINABLE		1	/*.4a and implementation default */

#define PTHREAD_CANCEL_ENABLE		0
#define PTHREAD_CANCEL_DISABLE		1

#define PTHREAD_CANCEL_ASYNCHRONOUS	0
#define PTHREAD_CANCEL_DEFERRED		1

#define PTHREAD_CANCELED		((void *)-1)

#define _POSIX_THREAD_THREAD_MAX	0	/* unlimited, not checked */
#define _POSIX_THREAD_KEYS_MAX		256     
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS  4

/* to make some function returns more readable
 */
#define _RETURN_PTHREAD_SUCCESS		0

#define SIGCANCEL SIGCNCL

/* Internal definitions */

#define VALID			0x01
#define STACK_PASSED_IN		0x02
#define JOINABLE		0x04
#define JOINER_WAITING		0x08
#define TASK_EXITED		0x10

/* typedefs */

typedef unsigned long pthread_t;

typedef struct {
	int	threadAttrStatus;		/* status flag		*/
	size_t	threadAttrStacksize;		/* stack size		*/	
	void	*threadAttrStackaddr;		/* stack address	*/
	int	threadAttrDetachstate;		/* detach state		*/
	int	threadAttrContentionscope;	/* contention scope	*/
	int	threadAttrInheritsched;		/* inherit scheduler	*/
	int	threadAttrSchedpolicy;		/* scheduling policy	*/
	char 	*threadAttrName;	/* task name - VxWorks extension */
        struct sched_param	threadAttrSchedparam;
						/* sched param struct	*/
	} pthread_attr_t;


typedef struct {
	int	condAttrStatus;			/* status flag		*/
#ifdef _POSIX_THREAD_PROCESS_SHARED
	int	condAttrPshared;		/* process-shared attr	*/
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	} pthread_condattr_t;


typedef struct {
	int	mutexAttrStatus;		/* status flag		*/
#ifdef _POSIX_THREAD_PROCESS_SHARED
	int	mutexAttrPshared;		/* process-shared attr	*/
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	int	mutexAttrProtocol;		/* inherit or protect	*/
	int	mutexAttrPrioceiling;		/* priority ceiling	*/
						/* (protect only)	*/
	} pthread_mutexattr_t;


/* values for mutexAttrProtocol */

#define PTHREAD_PRIO_NONE       0
#define PTHREAD_PRIO_INHERIT	1
#define PTHREAD_PRIO_PROTECT	2

typedef struct {
	SEM_ID			mutexSemId;
        int                     mutexValid;
        int                     mutexInitted;
        int                     mutexIniting;
	int			mutexCondRefCount;
	int			mutexSavPriority;
	pthread_mutexattr_t	mutexAttr;
	} pthread_mutex_t;

typedef struct {
	SEM_ID			condSemId;
        int                     condValid;
        int                     condInitted;
        int                     condIniting;
	int			condRefCount;
	pthread_mutex_t         *condMutex;
#ifdef _POSIX_THREAD_PROCESS_SHARED
	pthread_condattr_t	condAttr;
#endif	/* _POSIX_THREAD_PROCESS_SHARED */
	} pthread_cond_t;

typedef unsigned long	pthread_key_t;

typedef struct {
	int	onceInitialized;
	int	onceMutex;	/* shdbe sizeof thing that vxTas acts on */
	} pthread_once_t;

/* internal typedefs */

typedef struct cleanup
    {
 
    struct cleanup *next;
    void(*routine)(void *);
    void *arg;
 
    } cleanupHandler;
 
typedef struct
    {
    const void **       privateData;
    int                 privateDataCount;
    int                 taskId;
    unsigned long       flags;
    SEM_ID              exitJoinSemId;
    SEM_ID              mutexSemId;
    SEM_ID              cancelSemId;
    int                 priority;
    int                 cancelstate;
    int                 canceltype;
    int                 cancelrequest;
    void *              exitStatus;
    cleanupHandler *    handlerBase;
    pthread_cond_t *    cvcond;
    } internalPthread;

/*
 * Section 3 Process Primitives	
 */

int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
int pthread_kill(pthread_t thread, int sig);

/*
 * Section 11.3 Mutexes
 */

int pthread_mutexattr_init(pthread_mutexattr_t *attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t *attr);

#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int *pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int pshared);
#endif	/* _POSIX_THREAD_PROCESS_SHARED */

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);

int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/*
 * Section 11.4 Condition variables
 */

int pthread_condattr_init(pthread_condattr_t *attr);
int pthread_condattr_destroy(pthread_condattr_t *attr);

#ifdef _POSIX_THREAD_PROCESS_SHARED
int pthread_condattr_getpshared(const pthread_condattr_t *attr, int *pshared);
int pthread_condattr_setpshared(pthread_condattr_t *attr, int pshared);
#endif	/* _POSIX_THREAD_PROCESS_SHARED */

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);

int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex,
			   const struct timespec *abstime);

/*
 * Section 13.5 Thread scheduling
 */

#ifdef _POSIX_THREAD_PRIORITY_SCHEDULING
int pthread_attr_setscope(pthread_attr_t *attr, int contentionscope);
int pthread_attr_getscope(const pthread_attr_t *attr, int *contentionscope);

int pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched);
int pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched);

int pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy);
int pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy);

int pthread_attr_setschedparam(pthread_attr_t *attr,
	const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr,
	struct sched_param *param);

int pthread_getschedparam(pthread_t thread, int *policy,
	struct sched_param *param);
int pthread_setschedparam(pthread_t thread, int policy,
	const struct sched_param *param);
#endif	/* _POSIX_THREAD_PRIORITY_SCHEDULING */

#if defined (_POSIX_THREAD_PRIO_INHERIT) || defined (_POSIX_THREAD_PRIO_PROTECT)
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *attr, int protocol);
int pthread_mutexattr_getprotocol(pthread_mutexattr_t *attr, int *protocol);
#endif /* _POSIX_THREAD_PRIO_INHERIT || _POSIX_THREAD_PRIO_PROTECT */

#ifdef _POSIX_THREAD_PRIO_PROTECT
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *attr,
	int prioceiling);
int pthread_mutexattr_getprioceiling(pthread_mutexattr_t *attr,
	int *prioceiling);

int pthread_mutex_setprioceiling(pthread_mutex_t *attr, int prioceiling,
	int *old_ceiling);
int pthread_mutex_getprioceiling(pthread_mutex_t *attr, int *prioceiling);
#endif	/* _POSIX_THREAD_PRIO_PROTECT */

/*
 * Section 16 Thread management
 */

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
#ifdef _POSIX_THREAD_ATTR_STACKSIZE
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize);
#endif	/* _POSIX_THREAD_ATTR_STACKSIZE */
#ifdef _POSIX_THREAD_ATTR_STACKADDR
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr);
int pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr);
#endif	/* _POSIX_THREAD_ATTR_STACKADDR */
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate);

void pthread_exit(void *value_ptr);

int pthread_create (pthread_t *pThread,
		    const pthread_attr_t *pAttr,
		    void * (*start_routine)(void *),
		    void *arg);

int pthread_join(pthread_t thread, void **status);

int pthread_detach(pthread_t thread);

pthread_t pthread_self(void);

int pthread_equal(pthread_t t1, pthread_t t2);

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/*
 * Section 17 Thread-specific data
 */

int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));

int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);

int pthread_key_delete(pthread_key_t key);

/*
 * Section 18 Thread cancellation
 */

int pthread_cancel(pthread_t thread);
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
void pthread_testcancel(void);
void pthread_cleanup_push(void (*routine)(void *), void *arg);
void pthread_cleanup_pop(int execute);

/* VxWorks support routines */

extern void pthreadLibInit();

/* Kernel support routine prototypes */

extern STATUS	_pthreadLibInit(FUNCPTR deleteUserTaskEntry);

extern int	_pthreadCreate (pthread_t *pThread,
		                const pthread_attr_t *pAttr,
		                void * (*wrapperFunc)(void *),
		                void * (*start_routine)(void *),
		                void *arg,
				int priNumMode);

extern int	_pthreadSetCancelType (int type, int * oldtype);

extern int	_pthreadSemOwnerGet (SEM_ID	semId);

#if defined(__cplusplus)
}
#endif	/* __cplusplus */

#endif /* __INCpthreadh */
