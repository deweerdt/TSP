/*!  \file 

$Id: server_main.c,v 1.4 2004-10-04 08:59:09 tractobob Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server, for stub test
	    Allow the output of a datapool of 1000 symbols, 
	    cyclically generated at 100Hz

-----------------------------------------------------------------------
*/

#include "tsp_provider_init.h"

int main(int argc, char *argv[])
{
  printf ("#===================================================================#\n");
  printf ("# Launching <StubbedServer> for generation of 1000 Symbols at 100Hz #\n");
  printf ("#===================================================================#\n");

  /* Init server */
  if(TSP_provider_init(&argc, &argv))
    {
      TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING);
      TSP_provider_urls(TSP_PUBLISH_URLS_PRINT | TSP_PUBLISH_URLS_FILE);
      sigwait();
      TSP_provider_end();
    }

  return 0;
}
  

   
