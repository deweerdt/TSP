/*

$Id: tsp_common_trace.h,v 1.3 2008-10-03 07:49:29 rhdv Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2008 Robert de Vries

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : common

-----------------------------------------------------------------------

Purpose   : Tracing and error/warning message generation

-----------------------------------------------------------------------
 */

#ifndef _TSP_COMMON_TRACE_H
#define _TSP_COMMON_TRACE_H

#include <stdarg.h>
#include <tsp_prjcfg.h>

typedef void (*TSP_trace_func)(int level, const char *source, const char *file, const char *func, int line, const char *format, va_list args);

#if __GNUC__ >= 3
#define PRINTF_ATTRIBUTE __attribute__ ((__format__ (__printf__, 5, 6)))
#else
#define PRINTF_ATTRIBUTE
#endif

_EXPORT_TSP_COMMON void TSP_trace(int level, const char *file, const char *func, int line, const char *format, ...) PRINTF_ATTRIBUTE;

void TSP_trace_install(TSP_trace_func trace_func);

void TSP_trace_set_debug_level(unsigned long int level);


#ifndef S_SPLINT_S
#if defined(__GNUC__) || defined(_MSC_VER)
#define __func__ __FUNCTION__
#else
#define __func__ "where?"
#endif
#endif	/* S_SPLINT_S */

#define STRACE_LEVEL_ERROR       (0)
#define STRACE_LEVEL_WARNING     (1)
#define STRACE_LEVEL_INFO        (2)
#define STRACE_LEVEL_REQUEST     (4)
#define STRACE_LEVEL_DEBUG       (8)
#define STRACE_LEVEL_DEBUG_MORE (16)
#define STRACE_LEVEL_TEST       (32)

#ifdef  USE_STRACE
# define STRACE_TEST(...)	TSP_trace(STRACE_LEVEL_TEST,       __FILE__, __func__, __LINE__, __VA_ARGS__)
# define STRACE_DEBUG_MORE(...)	TSP_trace(STRACE_LEVEL_DEBUG_MORE, __FILE__, __func__, __LINE__, __VA_ARGS__)
# define STRACE_DEBUG(...)	TSP_trace(STRACE_LEVEL_DEBUG,      __FILE__, __func__, __LINE__, __VA_ARGS__)
# define STRACE_REQUEST( ...)	TSP_trace(STRACE_LEVEL_REQUEST,    __FILE__, __func__, __LINE__, __VA_ARGS__)
# define STRACE_INFO(...)	TSP_trace(STRACE_LEVEL_INFO,       __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
# define STRACE_TEST(...)
# define STRACE_DEBUG_MORE(...)
# define STRACE_DEBUG(...)
# define STRACE_REQUEST(...)
# define STRACE_INFO(...)
#endif

#define STRACE_WARNING(...)	 TSP_trace(STRACE_LEVEL_WARNING, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define STRACE_ERROR(...)	 TSP_trace(STRACE_LEVEL_ERROR,   __FILE__, __func__, __LINE__, __VA_ARGS__)

#endif	/* _TSP_COMMON_TRACE_H */
