/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_sys_headers.h,v 1.5 2002-12-03 11:32:45 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 
-----------------------------------------------------------------------
 */

#ifndef __TSP_SYS_HEADERS_H
#define __TSP_SYS_HEADERS_H 1

#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 199506L


#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
/*#include "fortify.h"*/

#ifdef MT_GPROF
#include "tsp_mtgprof.h"
#define pthread_create gprof_pthread_create
#endif




#endif
