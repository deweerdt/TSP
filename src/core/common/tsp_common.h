/*

$Id: tsp_common.h,v 1.14 2006-10-18 09:58:48 erk Exp $

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

#ifndef _TSP_COMMON_H
#define _TSP_COMMON_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>

/* All common includes */
#include <tsp_common_request.h>
#include <tsp_common_filter.h>
#include <tsp_common_ssi.h>
#include <tsp_common_ssei.h>
#include <tsp_common_as.h>

BEGIN_C_DECLS

/**
 * @defgroup TSP_CommonLib TSP Common Library
 * @ingroup TSP_CoreLib
 * The common librarie is the part of the TSP Core library
 * which is shared by provider and consumer.
 * It contains TSP datatypes definitions and helper functions.
 * @{
 */

#ifdef TSP_COMMON_C
_EXPORT_TSP_COMMON const int tsp_type_size[] = { 0,
			      sizeof(double),
			      sizeof(float),
			      sizeof(int8_t),
			      sizeof(int16_t),
			      sizeof(int32_t),
			      sizeof(int64_t),
			      sizeof(uint8_t),
			      sizeof(uint16_t),
			      sizeof(uint32_t),
			      sizeof(uint64_t),
			      sizeof(char),
			      sizeof(unsigned char),
			      sizeof(uint8_t),
			      0				
};
_EXPORT_TSP_COMMON const char* tsp_type_name[] = { "TSP_TYPE_UNKNOWN",
				"TSP_TYPE_DOUBLE",
				"TSP_TYPE_FLOAT",
				"TSP_TYPE_INT8",
				"TSP_TYPE_INT16",
				"TSP_TYPE_INT32",
				"TSP_TYPE_INT64",
				"TSP_TYPE_UINT8",
				"TSP_TYPE_UINT16",
				"TSP_TYPE_UINT32",
				"TSP_TYPE_UINT64",
				"TSP_TYPE_CHAR",
				"TSP_TYPE_UCHAR",
				"TSP_TYPE_RAW",
				"TSP_TYPE_LAST"
};
_EXPORT_TSP_COMMON const char* tsp_type_fmt[] = { "TSP_TYPE_UNKNOWN",
			       "%1.15G",
			       "%1.15G",
			       "%d",
			       "%d",
			       "%d",
			       "%ld",
			       "%u",
			       "%u",
			       "%u",
			       "%lu",
			       "%s",
			       "%s",
			       "%c",
			       "TSP_TYPE_LAST"	
};
#else
_EXTERN_TSP_COMMON const int   tsp_type_size[];
_EXTERN_TSP_COMMON const char* tsp_type_name[];
_EXTERN_TSP_COMMON const char* tsp_type_fmt[];
#undef _EXTERN_TSP_COMMON
#endif

END_C_DECLS

/** @} */

#endif /* _TSP_COMMON_H */
