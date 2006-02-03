/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_group.h,v 1.5 2006-02-03 20:46:22 erk Exp $

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

Purpose   : Interface for the groups management

-----------------------------------------------------------------------
 */

#ifndef _TSP_GROUP_H
#define _TSP_GROUP_H

#include "tsp_prjcfg.h"

#include "tsp_datastruct.h"

/** The group object handle */
typedef  void* TSP_groups_t;

/**
 * Creation of the group object.
 * @param symbols The symbol list that will be used to create the groups.
 * @param group_number The total number of groups
 * @return The group object handle
 */
TSP_groups_t TSP_group_create_group_table (const TSP_sample_symbol_info_list_t* symbols,
					   int group_number);

void TSP_group_delete_group_table(TSP_groups_t groups);                                    
#endif /* _TSP_GROUP_H */
