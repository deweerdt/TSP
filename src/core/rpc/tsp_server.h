/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_server.h,v 1.2 2002-10-09 08:27:52 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

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
