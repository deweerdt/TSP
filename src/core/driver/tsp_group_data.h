/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_group_data.h,v 1.3 2002-11-29 17:33:31 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

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

                                       
#endif /* _TSP_GROUP__DATA_H */
