/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo_data.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

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

struct TSP_algo_group_item_t
{
  void* data;
    
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
    
};

typedef struct TSP_algo_table_t TSP_algo_table_t;

#endif /*_TSP_GROUP_ALGO_DATA_H*/
