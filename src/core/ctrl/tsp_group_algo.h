/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo.h,v 1.2 2002-10-01 15:21:23 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for the function used to calculate
and use groups

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_ALGO_H
#define _TSP_GROUP_ALGO_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"
#include "tsp_datapool.h"

typedef  void* TSP_groups_t;

int TSP_group_algo_create_symbols_table(const TSP_sample_symbol_info_list_t* symbols,
					TSP_sample_symbol_info_list_t* out_symbols,
					TSP_groups_t* out_groups,
					TSP_datapool_t datapool);
                                       
int TSP_group_algo_get_group_number(TSP_groups_t* groups);
                                       
#endif /* _TSP_GROUP_ALGO_H */
