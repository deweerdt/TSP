#/*!  \file 

$Id: server_main.c,v 1.2 2003-02-28 14:34:27 tsp_admin Exp $

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
  char **my_argv;
  int i,my_argc;

  printf ("#========================================================================#\n");
  printf ("# Launching <res reader server> for generation of Symbols from .res file #\n");
  printf ("#========================================================================#\n");

  if (argc>1 && !strcmp(argv[1],"-help"))
    {
      printf("USAGE %s :  file.res [ -eof 0/1 0=always loop, 1=exit on end of file]\n", argv[0]);
      exit(0);
    }
      
  my_argc= argc+2;
  my_argv= (char**)calloc(my_argc, sizeof(char*));
  my_argv[0] = argv[0];
  my_argv[1] = "--tsp-stream-init-start";

  for (i=1; i<argc; i++)
    {
      my_argv[i+1]=argv[i];
    }
  my_argv[my_argc-1] = "--tsp-stream-init-stop";

  if (TSP_provider_init(&my_argc, &my_argv))
    {
      TSP_provider_run(TRUE);
    }

  return 0;
}

   
