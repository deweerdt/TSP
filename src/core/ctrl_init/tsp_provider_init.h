/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl_init/tsp_provider_init.h,v 1.3 2002-12-05 17:36:44 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Function calls to launch a TSP Provider program
            in the 'main'

-----------------------------------------------------------------------
 */

#ifndef _TSP_PROVIDER_INIT_H
#define _TSP_PROVIDER_INIT_H

#include "tsp_prjcfg.h"

int TSP_provider_init(int* argc, char** argv[]);

int TSP_provider_run(int blocking);

void TSP_provider_print_usage(void);

#endif /* _TSP_PROVIDER_INIT_H */
