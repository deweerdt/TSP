/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/include/Attic/tsp_simple_trace.h,v 1.3 2002-10-01 15:40:30 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 
-----------------------------------------------------------------------
 */

#ifndef __TSP_SIMPLE_TRACE_H
#define __TSP_SIMPLE_TRACE_H

static int _strace_debug_level=-1;

#define SIMPLE_TRACE(level,src,text) \
{ \
    if (_strace_debug_level<0) \
        if (getenv ("STRACE_DEBUG") != (char *)0) \
            _strace_debug_level = atoi(getenv("STRACE_DEBUG")); \
        else \
            _strace_debug_level = 0; \
    if (_strace_debug_level >= level ) \
        { \
	        printf("%7s||%s##%s##%d: ", src, __FILE__, strace_szfunc, __LINE__); \
            printf text; printf("\n"); fflush(stdout);\
        } \
} 
				  
				 
#define SFUNC_NAME(name)  static const char* strace_szfunc = #name 

#ifdef  USE_STRACE
# define STRACE_IO(text) SIMPLE_TRACE(5, "i/o", text)
# define STRACE_DEBUG(text) SIMPLE_TRACE(4, "debug", text)
# define STRACE_INFO(text) SIMPLE_TRACE(3, "Info", text)
# define STRACE_WARNING(text) SIMPLE_TRACE(2, "WarninG", text)
# define STRACE_TEST(text) SIMPLE_TRACE(1, "TEST", text)
#else
# define STRACE_IO(text)
# define STRACE_DEBUG(text)
# define STRACE_INFO(text)
# define STRACE_WARNING(text)
# define STRACE_TEST(text)
#endif

#define STRACE_ERROR(text) SIMPLE_TRACE(0,"ERROR", text)

#endif /*_SIMPLE_TRACE_H*/
