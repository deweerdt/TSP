#ifndef _AFFINITY_H
#define _AFFINITY_H

#include <sched.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <sys/syscall.h> 
#include <sys/types.h> 

#define _syscall3_pic(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name (type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ( \
        "pushl %%ebx\n\t" \
        "movl %%eax,%%ebx\n\t" \
        "movl %1,%%eax\n\t" \
        "int $0x80\n\t" \
        "popl %%ebx" \
        : "=a" (__res) \
        : "i" (__NR_##name),"a" ((long)(arg1)),"c" ((long)(arg2)), "d" ((long)(arg3)) ); \
__syscall_return(type,__res); \
}
 


/*
 * provide the proper syscall information if our libc is not yet updated.
 * It is suggested you check your kernel to make sure these are right for
 * your architecture.
 */
//#ifndef __NR_sched_setaffinity

#if defined(__i386__)
#define __NR_sched_setaffinity	241
#define __NR_sched_getaffinity	242
#endif

#if defined(__powerpc__)
#define __NR_sched_setaffinity	223
#define __NR_sched_getaffinity	222
#endif

_syscall3_pic(int, sched_setaffinity, pid_t, pid, unsigned int, len,
	  unsigned long *, user_mask_ptr)
_syscall3_pic(int, sched_getaffinity, pid_t, pid, unsigned int, len,
	  unsigned long *, user_mask_ptr)

//#endif /* __NR_sched_setaffinity */

#endif /* _AFFINITY_H */
