/*

$Id: tsp_common_filter.h,v 1.1 2006-04-07 09:30:36 erk Exp $

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

#ifndef _TSP_COMMON_FILTER_H
#define _TSP_COMMON_FILTER_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/**
 * @defgroup TSP_CommonLib_Filter TSP Request Filtering Parameters
 * @ingroup TSP_CommonLib
 * @{
 */

/**
 * The filter kind used in 
 * TSP_request_filtered_information.
 */
typedef enum TSP_filter_kind {
  TSP_FILTER_NONE=0,
  TSP_FILTER_MINIMAL,
  TSP_FILTER_SIMPLE,
  TSP_FILTER_REGEX,
  TSP_FILTER_XPATH,
  TSP_FILTER_SQL,
  TSP_FILTER_LAST
} TSP_filter_kind_t;

#define MINIMAL_STRING           "minimal"
#define SIMPLE_CASE_SENSITIVE    "simple_withcase"
#define SIMPLE_CASE_INSENSITIVE  "simple_nocase"

BEGIN_C_DECLS

/** @} */

END_C_DECLS

#endif /* _TSP_COMMON_FILTER_H */
