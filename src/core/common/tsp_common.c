/*

$Id: tsp_common.c,v 1.8 2006-10-18 09:58:48 erk Exp $

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Provider

-----------------------------------------------------------------------

Purpose   : Main implementation for the producer module

-----------------------------------------------------------------------
 */
#include <string.h>

#include <tsp_sys_headers.h>
#include <tsp_abs_types.h>
#include <tsp_datastruct.h>
#include <tsp_simple_trace.h>
#include <tsp_const_def.h>
#define TSP_COMMON_C
#include <tsp_common.h>

#ifdef WIN32
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif