/*

$Id: server_main.c,v 1.6 2006-04-24 21:05:34 erk Exp $

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

#include <stdlib.h>
#include <strings.h>

#include "tsp_provider_init.h"

void RES_GLU_loop(void);
GLU_handle_t* GLU_resreader_create();

/**
 * @defgroup TSP_ResReader Res File Reader
 * The TSP 'RES' file reader, this provider is reading
 * a RES file providing the contained symbols as a TSP provider.
 * @ingroup TSP_Providers
 */

int main(int argc, char *argv[])
{
  char **my_argv;
  int i,my_argc;

  printf ("#========================================================================#\n");
  printf ("# Launching <res reader server> for generation of Symbols from .res file #\n");
  printf ("#========================================================================#\n");

  my_argc= argc+2;
  my_argv= (char**)calloc(my_argc, sizeof(char*));
  my_argv[0] = argv[0];
  my_argv[1] = "--tsp-stream-init-start";

  for (i=1; i<argc; i++)
    {
      my_argv[i+1]=argv[i];
    }
  my_argv[my_argc-1] = "--tsp-stream-init-stop";

  GLU_handle_t* GLU_resreader = GLU_resreader_create();

  if (TSP_STATUS_OK==TSP_provider_init(GLU_resreader,&my_argc, &my_argv)) {
    if (TSP_STATUS_OK!=TSP_provider_run(TSP_ASYNC_REQUEST_SIMPLE | TSP_ASYNC_REQUEST_NON_BLOCKING)) {
      return -1;
    }
    RES_GLU_loop();
  }

  return 0;
}

   
