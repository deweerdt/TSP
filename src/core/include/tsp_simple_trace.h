/*

$Header: /home/def/zae/tsp/tsp/src/core/include/Attic/tsp_simple_trace.h,v 1.22 2006-10-21 08:48:00 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : Some trace macro

-----------------------------------------------------------------------
 */

#ifndef __TSP_SIMPLE_TRACE_H
#define __TSP_SIMPLE_TRACE_H

#include <stdio.h>
#include <stdlib.h>

#define SIMPLE_TRACE_FUNC(level,src,func,text) \
{ \
    static int _strace_debug_level = -1; \
    if (_strace_debug_level<0) { \
        if (getenv ("STRACE_DEBUG") != (char *)0) {\
            _strace_debug_level = atoi(getenv("STRACE_DEBUG")); \
        } else {\
            _strace_debug_level = 0; }\
    }\
    if ((0==level) || (_strace_debug_level & level) ) {		       \
           printf("%7s||%s##%s##%d: ", src, __FILE__, func, __LINE__); \
           printf text; printf("\n"); fflush(stdout);\
    }\
} 
#if defined(__GNUC__) || defined(_MSC_VER)
#  define SIMPLE_TRACE(level,src,text) SIMPLE_TRACE_FUNC(level,src,__FUNCTION__,text)
#else 
#  define SIMPLE_TRACE(level,src,text) SIMPLE_TRACE_FUNC(level,src,"where?",text)
#endif
				  
#ifdef  USE_STRACE
# define STRACE_TEST(text) SIMPLE_TRACE(32, "TEST", text)
# define STRACE_DEBUG_MORE(text) SIMPLE_TRACE(16, "Debug More", text)
# define STRACE_DEBUG(text) SIMPLE_TRACE(8, "Debug", text)
# define STRACE_REQUEST(text) SIMPLE_TRACE(4, "TSP Request", text)
# define STRACE_INFO(text) SIMPLE_TRACE(2, "Info", text)
#else
# define STRACE_TEST(text)
# define STRACE_DEBUG_MORE(text)
# define STRACE_DEBUG(text)
# define STRACE_REQUEST(text)
# define STRACE_INFO(text)
#endif

#define STRACE_WARNING(text) SIMPLE_TRACE(1, "WarninG", text)
#define STRACE_ERROR(text) SIMPLE_TRACE(0,"ERROR", text)

#endif /*_SIMPLE_TRACE_H*/
