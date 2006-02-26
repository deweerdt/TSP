/*

$Header $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2005 Cesare BERTONA

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
Maintainer: tsp@astrium.eads.net
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
*/
#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <errno.h>


/* added for compilation compatibility */

typedef int		key_t;

/* Common IPC Definitions. */
/* Mode bits. */
#define IPC_ALLOC       0100000         /* entry currently allocated */
#define IPC_CREAT       0001000         /* create entry if key doesn't exist */
#define IPC_EXCL        0002000         /* fail if key exists */
#define IPC_NOWAIT      0004000         /* error if request must wait */

/* Keys. */
#define IPC_PRIVATE     (key_t)0        /* private key */

/* Control Commands. */

#define IPC_RMID        10      /* remove identifier */
#define IPC_SET         11      /* set options */
#define IPC_STAT        12      /* get options */



#define EIDRM		(ERRMAX+1)


#endif /* _SYS_IPC_H */
