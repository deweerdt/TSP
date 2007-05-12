/*

$Id: tsp_common_status.h,v 1.3 2007-05-12 22:10:41 erk Exp $

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
Component  : Common

-----------------------------------------------------------------------

Purpose   : Main interface for the producer module

-----------------------------------------------------------------------
 */

#ifndef _TSP_COMMON_STATUS_H
#define _TSP_COMMON_STATUS_H

#include <stdio.h>

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @defgroup TSP_CommonLib_STATUS TSP Status Handling Library
 * @ingroup TSP_CommonLib
 * @{
 */

#ifdef TSP_COMMON_STATUS_C
#else
#endif

BEGIN_C_DECLS

_EXPORT_TSP_COMMON int32_t
TSP_status_print(FILE* fs, int32_t TSPStatus);

_EXPORT_TSP_COMMON const char*
TSP_status_string(int32_t TSPStatus);

_EXPORT_TSP_COMMON int32_t
TSP_status_sprint(char* buffer, int32_t buffer_len, int32_t TSPStatus);

/* compatibility define */
#define TSP_STATUS_print TSP_status_print
#define TSP_STATUS_sprint TSP_status_sprint

/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_STATUS_H */
