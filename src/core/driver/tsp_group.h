/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_group.h,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Interface the groups management

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_H
#define _TSP_GROUP_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"

typedef  void* TSP_groups_t;

TSP_groups_t TSP_group_create_group_table
(const TSP_sample_symbol_info_list_t* symbols, int group_number);
                                       
#endif /* _TSP_GROUP_H */
