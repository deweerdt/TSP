/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/misc_utils/tsp_time.h,v 1.2 2002-09-18 08:15:53 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Interface for tsp time functions

-----------------------------------------------------------------------
 */
#ifndef _TSP_TIME_H
#define _TSP_TIME_H



#include "tsp_prjcfg.h"



typedef gint64  tsp_hrtime_t;

tsp_hrtime_t tsp_gethrtime(void);

int tsp_usleep(int useconds);


#endif _TSP_TIME_H
