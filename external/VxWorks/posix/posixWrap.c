#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <vxWorks.h>
#include <taskLib.h>
#include <semLib.h>
#include <logLib.h>

/*---------------------------------------------------------------------*/
void posixWrapInit(void)
{
  /* nothing to do, just a call to link this object to vxWorks */
}

/*---------------------------------------------------------------------*/
/* In this section we do the supposition that task = thread (POSIX) */

pthread_t pthread_self (void) 
{
/* get the thread ID */
return (taskIdSelf()) ;
}


int pthread_detach (pthread_t thread)
{
/*
     pthread_detach()  can  dynamically  reset  the   detachstate
     attribute  of a thread to PTHREAD_CREATE_DETACHED. For exam-
     ple, a thread could detach itself as follows:    
           pthread_detach(pthread_self());                      */

/* always successfull : a task is always detached */
return ((int)0) ;
}


int pthread_create (pthread_t *new_thread_ID,const pthread_attr_t *attr,void *(*start_func)(void *),void *arg) 
{
int retourcode ;
int priority ;
int options ;
int Pid ;
STATUS status ;

static int pthread_number = 0;
char pthread_name[16];

if (attr == NULL) 
    {
    /* default creation attribute in posix are 
     contentionscope   PTHREAD_SCOPE_PROCESS     Resource competition within process
     detachstate       PTHREAD_CREATE_JOINABLE   Joinable by other threads
     stackaddr         NULL                      Allocated by system
     stacksize         NULL                      1 megabyte 
     priority          NULL                      Parent (calling) thread's priority
     policy            SCHED_OTHER               Determined by system
     inheritsched      PTHREAD_EXPLICIT_SCHED    Scheduling attributes explicitly set,
                                                 for example, policy is SCHED_OTHER.
*/
    Pid = taskIdSelf ();
    status = taskPriorityGet (Pid,&priority) ;
    status = taskOptionsGet (Pid,&options) ;  
    sprintf(pthread_name, "pthread#%d", pthread_number++);
    retourcode = taskSpawn (pthread_name, /* task name */
                        priority, /* task priority */
                        options,  /* task options */
                        0x4000,   /* default stack size 16 KBytes would be enough*/     
                        (FUNCPTR) start_func,                                                     
                        arg,       /* arguments of the function */
                        Pid,	   /* The parent ID is inherited */ 
						0,0,0,0,0,0,0,0 /* the eight other arguments in vxworks */                        
                        ) ; 
	if (retourcode != ERROR)
		{
		*new_thread_ID = retourcode ;
		retourcode = OK ;
		}
	else
		retourcode = ENOMEM;
    }
else
    {
    errno = EINVAL ;  /* on considére un paramétre non nul comme invalide */
    retourcode = -1 ;
    }
return (retourcode) ;
}   

int pthread_join (pthread_t threadId ,void **status) 
{ /*    The pthread_join()  function suspend processing  of  the  calling  
    thread until the target target_thread completes.  
    target_thread must be a member  of  the  current
     process  and  it  cannot be a detached . */
STATUS statusLocal ;
statusLocal = taskIdVerify(threadId) ;
if (statusLocal == ERROR)
    {
    return -1 ;
    } 
else
    taskDelay (0) ; /* FIXME wait foreever */
    return 0 ;

}

int pthread_mutex_init (pthread_mutex_t *mp, const pthread_mutexattr_t *attr) 
{
int ret = -1 ;
SEM_ID semId ;
semId = semBCreate (SEM_Q_FIFO,SEM_FULL) ;
if (semId != NULL)
    {
    mp->mutexSemId = semId ;
    ret = 0 ;
    }
return (ret) ;
}

int pthread_mutex_lock (pthread_mutex_t *mp) 
{
int ret = -1 ;
if (mp->mutexSemId ==  0)  
	{ /* we must create the default mutex */
	ret = pthread_mutex_init (mp,NULL) ;
	}
ret = semTake (mp->mutexSemId,WAIT_FOREVER);
return (ret) ;
} 

int pthread_mutex_unlock (pthread_mutex_t *mp) 
{
int ret = -1 ;
ret = semGive (mp->mutexSemId);
return (ret) ;
} 

int pthread_mutex_destroy (pthread_mutex_t *mp) 
{
int ret = ERROR ;
ret = semDelete (mp->mutexSemId);
return (ret) ;
} 

int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t *attr)
{
int ret = -1 ;
ret = OK;
return (ret) ;
} 

int pthread_cond_wait (pthread_cond_t *cond, pthread_mutex_t *mutex) 
{
int ret = -1 ;
ret = OK;
return (ret) ;
} 



/*-----------------------------------------------------------------*/
/* MORE : some functions generally used and not defined in VXWORKS */

int gettimeofday (struct timeval *tp,void *vide)
{
struct timespec localtp;
if (clock_gettime (CLOCK_REALTIME,&localtp)==0) 
    {
    tp->tv_sec = (time_t)localtp.tv_sec ;
    tp->tv_usec = (time_t)(localtp.tv_nsec/1000) ;
    return 0 ;
    }
else
    return -1 ;
}

char *strdup (const char *s1) 
{
char *p ;
p = (char *)malloc((strlen(s1)+1)*sizeof(char)) ;
if (p != NULL) strcpy (p,s1) ;
return p ;
}
