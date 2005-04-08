/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_sys_headers.h,v 1.18 2005-04-08 14:52:21 le_tche Exp $

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
Maintainer : tsp@astrium.eads.net
Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 
-----------------------------------------------------------------------
 */

#ifndef __TSP_SYS_HEADERS_H
#define __TSP_SYS_HEADERS_H 1

#ifdef __FreeBSD__
#include <rpc/rpc.h>
#endif /* __FreeBSD__ */

#ifdef __OpenBSD__
#include <rpc/rpc.h>
/* OpenBSD's rpcgen has timeout support and declares _rpcsvcdirty as extern */
int _rpcsvcdirty;
#endif /* __OpenBSD__ */

#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE
#define _POSIX_C_SOURCE 199506L
#endif

#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <errno.h>
#include <netdb.h>
/*#include "fortify.h"*/


/* Used for gprof to work for linux multi-thread programs.
Do not use this with others targets */
#ifdef MT_GPROF
#include "tsp_mtgprof.h"
#define pthread_create gprof_pthread_create
#endif

/* OSF1 / Alpha */
#if defined (__osf__) && defined (__alpha)
# define strtoll strtol
# define strtoull strtoul
#endif /* OSF1 / Alpha */

#include <pthread.h>
#include <sys/time.h>
#include <sys/param.h>

#ifdef VXWORKS
/* VXWORKS part */
# include <hostLib.h>
# include <sockLib.h>
# include <logLib.h>
# include <tickLib.h>
# include <taskLib.h>
/* posix part */
# include <libgen.h>
# include <strings.h>
#endif /* VXWORKS */

#endif
