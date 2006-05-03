/*

$Id: tsp_common_macros.h,v 1.1 2006-05-03 21:13:34 erk Exp $

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

#ifndef _TSP_COMMON_MACROS_H
#define _TSP_COMMON_MACROS_H

#define TSP_TYPE_DELETE_IMPLEMENT(type,shorttype)	\
  \
  int32_t						\
  TSP_##shorttype##_delete ( type ** shorttype ) {	\
							\
    int32_t retcode;					\
    assert(shorttype);					\
    retcode = TSP_STATUS_OK;				\
							\
    if(NULL!=*shorttype) {				\
      retcode=TSP_##shorttype##_finalize(*shorttype);	\
      if (TSP_STATUS_OK!=retcode){			\
	return retcode;					\
      }							\
							\
      free(*shorttype);					\
      *shorttype=NULL;					\
    }							\
    return retcode;					\
  }							

#define TSP_ASI_STATIC_INITIALIZER {		\
    TSP_UNDEFINED_VERSION_ID,			\
      -1,					\
      NULL,					\
      TSP_STATUS_ERROR_NOT_INITIALIZED		\
      }

#define TSP_ASD_STATIC_INITIALIZER {		\
    TSP_UNDEFINED_VERSION_ID,			\
      -1,					\
      TSP_STATUS_ERROR_NOT_INITIALIZED		\
}

#define TSP_SSILIST_STATIC_INITIALIZER  {	\
    0,						\
      NULL					\
      }

#define TSP_AS_STATIC_INITIALIZER  {		\
    TSP_UNDEFINED_VERSION_ID,			\
      -1,					\
      -1,					\
      -1,					\
      TSP_SSILIST_STATIC_INITIALIZER,		\
      0.0,					\
      0,					\
      0,					\
      -1,					\
      TSP_STATUS_ERROR_NOT_INITIALIZED		\
      }


#endif /* _TSP_COMMON_MACROS_H */
