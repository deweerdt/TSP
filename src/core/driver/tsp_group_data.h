/*

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_group_data.h,v 1.7 2006-03-31 12:55:19 erk Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Data structure for the TSP_group_t object.
Any module that needs to access the internal structure of the TSP_group_t object
for performance reason may include this header

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_DATA_H
#define _TSP_GROUP_DATA_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include "tsp_data_receiver.h"

#include <tsp_decoder.h>
#include <tsp_encoder.h>

struct TSP_group_item_t
{
  int provider_global_index;

  int sizeof_encoded_item;

  TSP_data_decoder_t data_decoder;

};

typedef struct TSP_group_item_t TSP_group_item_t;

struct TSP_group_t
{
  int group_len;

  int sizeof_encoded_group;
    
  TSP_group_item_t* items;
    
};

typedef struct TSP_group_t TSP_group_t;


struct TSP_group_table_t
{

  /**
   * Total number of items for all groups
   */
  int groups_summed_size;
    
  /**
   * Number of groups.
   */
  int table_len;
    
  /**
   * size of biggest group
   */
  int max_group_len;

  /** groups */
  TSP_group_t* groups;

  /** allocate room for items */
  TSP_group_item_t* items_table;
    
};

typedef struct TSP_group_table_t TSP_group_table_t;

                                       
#endif /* _TSP_GROUP_DATA_H */
