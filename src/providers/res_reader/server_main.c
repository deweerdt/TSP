#/*!  \file 

$Id: server_main.c,v 1.1 2003-01-31 18:22:25 tsp_admin Exp $

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
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Allow the output of a datapool of symbols from res file

-----------------------------------------------------------------------
*/


#include "tsp_provider_init.h"

int main(int argc, char *argv[])
{
  printf ("#========================================================================#\n");
  printf ("# Launching <res reader server> for generation of Symbols from .res file #\n");
  printf ("#========================================================================#\n");

  if (argc == 1)
    {
      printf("USAGE %s : [ --tsp-stream-init-start file.res --tsp-stream-init-stop ]\n", argv[0]);
    }

  if (TSP_provider_init(&argc, &argv))
    {
      TSP_provider_run(TRUE);
    }

  return 0;
}

   
