/*!  \file 

$Id: gdisp_main.c,v 1.3 2004-06-17 21:07:41 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi.

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
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Graphic Tool main part.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <assert.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 * GDISP+ main part.
 */
gint
main (int argc, char **argv) 
{

  Kernel_T *gdispKernel = (Kernel_T*)NULL;


  /*
   * Set up GTK+ things, such as the default visual and color map.
   * Initialize the library for use, sets up default signal handlers, and
   * checks the arguments passed to the application on the command line.
   */
  gtk_init(&argc, &argv);


  /*
   * Allocate memory for managing GDISP+ kernel.
   * Take into account user defined arguments passed to the application
   * on the command line.
   */
  gdispKernel = gdisp_createKernel(argc,argv);


  /*
   * Create GDISP+ main board.
   * The main board is a little window that offers all GDISP+ functions.
   */
  gdisp_createMainBoard(gdispKernel);


  /*
   * Write few information into the output window before starting.
   */
  gdisp_writeInitialInformation(gdispKernel);


  /*
   * Create our colormap.
   */
  gdisp_createColormap(gdispKernel);


  /*
   * Discover all hosts that are requested by the user.
   */
  gdisp_buildHostList(gdispKernel);


  /*
   * Initialize consuming environment.
   */
  gdisp_consumingInit(gdispKernel);


  /*
   * Enter GTK main processing loop.
   * Sleep waiting for X events (such as button or key presses), timeouts,
   * file IO notifications to occur.
   */
  gtk_main();


  /*
   * Close consuming environment.
   */
  gdisp_consumingEnd(gdispKernel);


  /*
   * End of GDISP+ main part, by destroying kernel.
   */
  gdisp_destroyKernel(gdispKernel);

  return 0;

}
