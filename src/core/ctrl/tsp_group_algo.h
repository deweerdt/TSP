/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo.h,v 1.3 2002-11-26 14:03:07 galles Exp $

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

/**
* Create the groups table for  a given symbol list 
* @param symbols The required symbol list
* @param out_symbols The required symbol list with information added (group id ...)
* @param out_groups The computed group table
* @param datapool The associated datapool used to link the datapool suymbols address
* to the value in the group table
* @return TRUE=OK
*/
int TSP_group_algo_create_symbols_table(const TSP_sample_symbol_info_list_t* symbols,
					TSP_sample_symbol_info_list_t* out_symbols,
					TSP_groups_t* out_groups,
					TSP_datapool_t datapool);

/**
* Get the total number of computed groups
* @return Groups number
*/                                       
int TSP_group_algo_get_group_number(TSP_groups_t* groups);

/**
* Get the biggest group size for all computed groups
* @return Biggest group size
*/                                       
int TSP_group_algo_get_biggest_group_size(TSP_groups_t* groups);
                                       
#endif /* _TSP_GROUP_ALGO_H */
