/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/include/Attic/tsp_simple_trace.h,v 1.8 2004-08-31 09:58:52 dufy Exp $

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
static int _strace_debug_level = -1;

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
				  
				 

#ifdef  USE_STRACE
# define SFUNC_NAME(name)  static const char* strace_szfunc = #name 
# define STRACE_IO(text) SIMPLE_TRACE(5, "i/o", text)
# define STRACE_DEBUG(text) SIMPLE_TRACE(4, "debug", text)
# define STRACE_INFO(text) SIMPLE_TRACE(3, "Info", text)
# define STRACE_TEST(text) SIMPLE_TRACE(1, "TEST", text)
#else
# define SFUNC_NAME(name)  static const char* strace_szfunc = #name
# define STRACE_IO(text)
# define STRACE_DEBUG(text)
# define STRACE_INFO(text)
# define STRACE_TEST(text)
#endif

# define STRACE_WARNING(text) SIMPLE_TRACE(2, "WarninG", text)
#define STRACE_ERROR(text) SIMPLE_TRACE(0,"ERROR", text)

#endif /*_SIMPLE_TRACE_H*/
