/*

$Header: /home/def/zae/tsp/tsp/src/core/misc_utils/tsp_services.h,v 1.1 2006-10-18 09:58:48 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Alan PRAT 

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
Component : Services

-----------------------------------------------------------------------

Purpose   : Interface for tsp services functions

-----------------------------------------------------------------------
 */
#ifndef _TSP_SERVICES_H
#define _TSP_SERVICES_H

#undef _EXPORT_TSP_SERVICES
#if defined (WIN32) && defined (TSP_SHARED_LIBS)
#  ifdef tsp_services_EXPORTS
#    define _EXPORT_TSP_SERVICES __declspec(dllexport) 
#  else
#    define _EXPORT_TSP_SERVICES __declspec(dllimport) 
#  endif
#else
#  define _EXPORT_TSP_SERVICES
#endif

#endif
