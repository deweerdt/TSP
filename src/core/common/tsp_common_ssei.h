/*

$Id: tsp_common_ssei.h,v 1.1 2006-04-07 09:30:36 erk Exp $

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

#ifndef _TSP_COMMON_SSEI_H
#define _TSP_COMMON_SSEI_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @ingroup TSP_CommonLib
 * @{
 */

#ifdef TSP_COMMON_SSEI_C
#else
#endif

BEGIN_C_DECLS

/**
 * Initialize an extended information item.
 * @param[in,out] ei the extended info structure, the structure should
 *                be allocated but the field may not
 * @param[in] key the key/name of the extended information
 * @param[in] value the value of the extended information
 */
int32_t
TSP_ei_initialize(TSP_extended_info_t* ei, const char* key, const char* value);

/**
 * Finalize an extended information item.
 * @param[in,out] ei the extended info structure, the structure should
 *                be allocated but the field may not
 */
int32_t
TSP_ei_finalize(TSP_extended_info_t* ei, const char* key, const char* value);

END_C_DECLS

#endif /* _TSP_COMMON_SSEI_H */
