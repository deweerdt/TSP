/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.h,v 1.5 2002-12-24 14:14:21 tntdev Exp $

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

Purpose   : Function calls to launch a TSP Provider program
            in the 'main'

-----------------------------------------------------------------------
 */

#ifndef _TSP_PROVIDER_INIT_H
#define _TSP_PROVIDER_INIT_H

#include "tsp_prjcfg.h"

/**
 * Main initialisation function for a TSP provider.
 * Call this function at the start of your main function
 * with the received argc/argv. Then use the modified argc/argv.
 * @param argc You must provide the real argc before using it
 * @param argv You must provider the real argv before using it
 * @return TRUE or FALSE. OK = TRUE.
 */
int TSP_provider_init(int* argc, char** argv[]);

/**
 * Main run function for a TSP provider.
 * Call this function to launch the provider after the initialisation
 * @param blocking If blocking = TRUE. This function will block forever.
 * @return TRUE or FALSE. OK = TRUE.
 */
int TSP_provider_run(int blocking);

void TSP_provider_print_usage(void);

#endif /* _TSP_PROVIDER_INIT_H */
