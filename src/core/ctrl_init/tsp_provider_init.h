#ifndef _TSP_PROVIDER_INIT_H
#define _TSP_PROVIDER_INIT_H

#include "tsp_prjcfg.h"

int TSP_provider_init(int* argc, char** argv[]);

int TSP_provider_run(int blocking);

void TSP_provider_print_usage(void);

#endif /* _TSP_PROVIDER_INIT_H */
