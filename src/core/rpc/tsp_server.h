/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.h,v 1.3 2002-12-18 16:27:36 tntdev Exp $

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

Purpose   : 

-----------------------------------------------------------------------
 */

#ifndef _TSP_SERVER_H
#define _TSP_SERVER_H

#include "tsp_prjcfg.h"


/**
* Initialise command canal communication.
* @param server_number Every provider on a given host must have its own server number ;
* All must be different
* @param blocking if blocking = true, the program will be blocked in this fonction forever
* @return TRUE = OK
*/
int TSP_command_init(int server_number, int blocking);

#endif /* _TSP_SERVER_H */
