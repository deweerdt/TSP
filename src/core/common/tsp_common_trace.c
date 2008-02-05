/*

$Id: tsp_common_trace.c,v 1.1 2008-02-05 18:54:09 rhdv Exp $

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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <assert.h>

#include "tsp_common_trace.h"

static void default_trace_func(int level, const char *name, const char *file, const char *func, int line, const char *format, va_list args);

static unsigned long int strace_debug_level = 0;
static TSP_trace_func trace = default_trace_func;


static const char *level_name(int level)
{
    switch(level) {
    case STRACE_LEVEL_ERROR:
	return "ERROR";
    case STRACE_LEVEL_WARNING:
	return "Warning";
    case STRACE_LEVEL_INFO:
	return "Info";
    case STRACE_LEVEL_REQUEST:
	return "Request";
    case STRACE_LEVEL_DEBUG:
	return "Debug";
    case STRACE_LEVEL_DEBUG_MORE:
	return "Debug More";
    case STRACE_LEVEL_TEST:
	return "Test";
    }
    return "???";
}


static void default_trace_func(int level, const char *name, const char *file, const char *func, int line, const char *format, va_list args)
{
    fprintf(stderr, "%7s||%s##%s##%d: ", name, file, func, line);
    vfprintf(stderr, format, args);
    putc('\n', stderr);
    fflush(stderr);
}

static void _trace(int level, const char *file, const char *func, int line, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    trace(level, level_name(level), file, func, line, format, args);
    va_end(args);
}

static void init_strace_debug_level(void)
{
    char *strace_env = getenv("STRACE_DEBUG");
    if (strace_env == NULL) return;

    char *end;
    int new_level = strtoul(strace_env, &end, 0);
    assert(errno != EINVAL); /* programming error, base contains an unsupported value */
    if (*end != 0) {
	_trace(STRACE_LEVEL_ERROR, __FILE__, __func__, __LINE__, "STRACE_DEBUG environment variable has trailing garbage");
	return;
    }
    
    if (errno == ERANGE) {
	_trace(STRACE_LEVEL_ERROR, __FILE__, __func__, __LINE__, "STRACE_DEBUG environment variable value out of range");
	return;
    }

    strace_debug_level = new_level;
}

void TSP_trace(int level, const char *file, const char *func, int line, const char *format, ...)
{
    static bool first_time = true;

    if (first_time) {
	first_time = false;
	init_strace_debug_level();
    }
    if ((0==level) || (strace_debug_level & level) ) {
	va_list args;
	va_start(args, format);
	trace(level, level_name(level), file, func, line, format, args);
	va_end(args);
    }		    
}

void TSP_trace_install(TSP_trace_func trace_func)
{
    trace = trace_func;
}

void TSP_trace_set_debug_level(unsigned long int level)
{
    strace_debug_level = level;
}
