/*!  \file 

$Id: tsp_filter_symbol.h,v 1.1 2005-10-23 19:08:53 erk Exp $

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

Purpose   : Symbol filtering module

-----------------------------------------------------------------------
 */

#ifndef _TSP_FILTER_SYMBOL_H
#define _TSP_FILTER_SYMBOL_H

#include <tsp_abs_types.h>
#include <tsp_provider.h>

BEGIN_C_DECLS
void TSP_filter_symbol_none(TSP_request_information_t* req_info,
			    char* filter_string,
			    TSP_answer_sample_t* ans_sample);


void TSP_filter_symbol_minimal(TSP_request_information_t* req_info,
			       char* filter_string,
			       TSP_answer_sample_t* ans_sample);

END_C_DECLS

#endif


