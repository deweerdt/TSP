/*

$Id: tsp_filter_symbol.c,v 1.3 2006-06-28 12:59:04 erk Exp $

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

#include <tsp_sys_headers.h>
#include <tsp_filter_symbol.h>
#include <tsp_session.h>
#include <tsp_common.h>

void TSP_filter_symbol_none(TSP_request_information_t* req_info,
			    char* filter_string,
			    TSP_answer_sample_t* ans_sample) {
  if(TSP_session_get_sample_symbol_info_list_by_channel(req_info->channel_id,
							&(ans_sample->symbols))) {
    ans_sample->status = TSP_STATUS_OK;
  } else {
    ans_sample->status = TSP_STATUS_ERROR_SYMBOLS;
  }

}

void TSP_filter_symbol_minimal(TSP_request_information_t* req_info,
			       char* filter_string,
			       TSP_answer_sample_t* ans_sample) {
  
  ans_sample->status = TSP_STATUS_ERROR_NOT_IMPLEMENTED;
  if (0==strncmp(filter_string,MINIMAL_STRING,strlen(MINIMAL_STRING))) {
    /* nothing to do for MINIMAL */
    ans_sample->status = TSP_STATUS_OK;
  } else {
    ans_sample->status = TSP_STATUS_ERROR_SYMBOL_FILTER;
  }
}


