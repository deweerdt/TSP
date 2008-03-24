/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo.h,v 1.12 2008-03-24 23:56:20 deweerdt Exp $

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

Purpose   : Interface for the function used to calculate
and use symbols groups

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_ALGO_H
#define _TSP_GROUP_ALGO_H

#include <tsp_prjcfg.h>
#include <tsp_datastruct.h>
#include <tsp_datapool.h>

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
int32_t TSP_group_algo_create_symbols_table(const TSP_sample_symbol_info_list_t* symbols,
					TSP_sample_symbol_info_list_t* out_symbols,
					TSP_groups_t* out_groups,
					TSP_datapool_t* datapool);
/**
 * Destroy the group table
 * @param groups List allocated by TSP_group_algo_create_symbols_table
 */
void TSP_group_algo_destroy_symbols_table(TSP_groups_t* groups);

/**
 * Get the total number of computed groups
 * @return Groups number
 */                                       
int TSP_group_algo_get_group_number(TSP_groups_t* groups);

/**
 * Get the biggest group size of all computed groups
 * @return Biggest group size
 */                                       
uint32_t TSP_group_algo_get_biggest_group_size(TSP_groups_t* groups);
                                       
#endif /* _TSP_GROUP_ALGO_H */
