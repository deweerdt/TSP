/*!  \file 

$Id: gdisp_main.c,v 1.5 2004-10-04 08:57:26 tractobob Exp $

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
Maintainer: tsp@astrium.eads.net
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
#include <unistd.h>
#include <libgen.h> /* basename function */


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Usage.
 */
static void
gdisp_usage ( Kernel_T *kernel,
	      gchar    *applicationName )
{

  /*
   * Print application usage.
   */
  fprintf(stdout,
	  "------------------------------------------------------------\n");

  fprintf(stdout,
	  "Usage : %s [ -h host1 [ -h host2 ... ] ]\n",
	  basename(applicationName));

  fprintf(stdout,
	  "  -h : insert one or several additional hosts.\n");

  fprintf(stdout,
	  "       'localhost' is always taken into account.\n");

  fprintf(stdout,
	  "------------------------------------------------------------\n");

  fflush (stdout);

}


/*
 * Analyse all arguments given by the user.
 * Returns TRUE in case of error.
 */
static gboolean
gdisp_analyseUserArguments ( Kernel_T *kernel )
{

  gint     opt      = 0;
  gboolean mustStop = FALSE;

  /*
   * The user can specify several host to be looked at, with the '-h' option.
   */
  while ((opt = getopt(kernel->argCounter,
		       kernel->argTable,
		       "h:")) != EOF) {

    switch (opt) {

    case 'h' :
      gdisp_addHost(kernel,optarg);
      break;

    default :
      mustStop = TRUE;
      break;

    } /* end switch */

  } /* end while */

  return mustStop;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * GDISP+ main part.
 */
gint
main (int argc, char **argv) 
{

  Kernel_T *gdispKernel = (Kernel_T*)NULL;
  gboolean  mustStop    = FALSE;


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
   * Discover all options given by the user.
   */
  mustStop = gdisp_analyseUserArguments(gdispKernel);
  if (mustStop == TRUE) {

    /*
     * Write the usage.
     */
    gdisp_usage(gdispKernel,argv[0]);

    /*
     * Destroy kernel, and exit.
     */
    gdisp_destroyKernel(gdispKernel);
    return -1;

  }


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
   * Destroy our colormap.
   */
  gdisp_destroyColormap(gdispKernel);


  /*
   * End of GDISP+ main part, by destroying kernel.
   */
  gdisp_destroyKernel(gdispKernel);

  return 0;

}
