/*

$Header: /home/def/zae/tsp/tsp/src/util/libcalc/calc_func.h,v 1.6 2006-10-18 09:58:49 erk Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation fotr demo purpose

-----------------------------------------------------------------------
 */

#ifndef CALC_FUNC_H
#define CALC_FUNC_H

#include <tsp_abs_types.h>

#undef _EXPORT_TSP_CALC
#if defined (WIN32) && defined (TSP_SHARED_LIBS)
#  ifdef tsp_calc_EXPORTS
#    define _EXPORT_TSP_CALC __declspec(dllexport) 
#  else
#    define _EXPORT_TSP_CALC __declspec(dllimport) 
#  endif
#else
#  define _EXPORT_TSP_CALC
#endif

BEGIN_C_DECLS

/* return nice value for plotting */
_EXPORT_TSP_CALC double calc_func (int index, double my_time);

/* return nice value for plotting */
_EXPORT_TSP_CALC char calc_func_char (int index, double my_time);



END_C_DECLS

#endif

