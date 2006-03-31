/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo_data.h,v 1.6 2006-03-31 12:55:19 erk Exp $

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

Purpose   :  Data structure for the TSP_algo_group_t object.
Any module that needs to access the internal structure of the TSP_group_algo_t object
for performance reason may include this header

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_ALGO_DATA_H
#define _TSP_GROUP_ALGO_DATA_H

#include "tsp_data_sender.h"
#include <tsp_encoder.h>

struct TSP_algo_group_item_t
{
  void* data;
  int   dimension;
  TSP_data_encoder_t data_encoder;
};

typedef struct TSP_algo_group_item_t TSP_algo_group_item_t;

struct TSP_algo_group_t
{
  int group_len;
    
  TSP_algo_group_item_t* items;
    
};

typedef struct TSP_algo_group_t TSP_algo_group_t;

struct TSP_algo_table_t
{
  int groups_summed_size;
    
  int table_len;
    
  TSP_algo_group_t* groups;

  int max_group_len;

  TSP_algo_group_item_t* all_items;
    
};

typedef struct TSP_algo_table_t TSP_algo_table_t;

#endif /*_TSP_GROUP_ALGO_DATA_H*/
