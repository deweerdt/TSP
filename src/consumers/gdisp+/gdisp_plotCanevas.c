/*!  \file 

$Id: gdisp_plotCanevas.c,v 1.2 2004-03-26 21:09:17 esteban Exp $

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

File      : << extra ressource >>
            Give a skeleton that must be followed by any kind of plots.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"

#include "gdisp_<<<-canevas->>>.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/



/*
 * Create a plot by providing an opaque structure to the caller.
 * This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_create<<<-canevas->>> (Kernel_T *kernel)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)NULL;


  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a plot opaque structure.
 */
static void
gdisp_destroy<<<-canevas--->(Kernel_T *kernel,
			     void     *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Now destroy everything.
   */


  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(<<<-canevas->>>_T));
  g_free(plot);

}


/*
 * Record parent widget.
 */
static void
gdisp_set<<<-canevas->>>ParentWidget (Kernel_T  *kernel,
				      void      *data,
				      GtkWidget *parent)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Store parent widget.
   */

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_set<<<-canevas->>>InitialDimensions (Kernel_T *kernel,
					   void     *data,
					   guint     width,
					   guint     height)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Remeber here initial dimensions of the viewport.
   */

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_get<<<-canevas->>>TopLevelWidget (Kernel_T  *kernel,
					void      *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  return (GtkWidget*)<<<-topLevelWidget->>>;

}


/*
 * By now, the plot widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_show<<<-canevas->>> (Kernel_T  *kernel,
			   void      *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Now show everything.
   */

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_get<<<-canevas->>>Type (Kernel_T *kernel,
			      void     *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Return the type of the plot.
   */
  return <<<-canevas-type->>>;

}


/*
 * Record any incoming symbols.
 */
static void
gdisp_addSymbolsTo<<<-canevas->>> (Kernel_T *kernel,
				   void     *data,
				   GList    *symbolList,
				   guint     xDrop,
				   guint     yDrop)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Record here all incoming symbols.
   */

}


/*
 * Broadcast all recorded symbols.
 */
static GList*
gdisp_getSymbolsFrom<<<-canevas->>> (Kernel_T *kernel,
				     void     *data,
				     gchar     axis)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Return the list of symbols.
   */

}


/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOn<<<-canecas->>> (Kernel_T *kernel,
				  void     *data)
{

  /*
   * Do anything you want before starting steps.
   *
   * BUT we must return TRUE to the calling procedure in order to allow
   * the general step management to proceed.
   *
   * Returning FALSE means that our plot is not enabled to perform its
   * step operations, because of this or that...
   */
  return TRUE;

}


/*
 * Real time Step Action.
 */
static void
gdisp_stepOn<<<-canevas->>> (Kernel_T *kernel,
			     void     *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Do anything you want to perform steps.
   */
}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOn<<<-canevas->>> (Kernel_T *kernel,
				 void     *data)
{

  <<<-canevas->>>_T *plot = (<<<-canevas->>>_T*)data;

  /*
   * Do anything you want when steps are stopped.
   */

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_get<<<-canevas->>>Information (Kernel_T         *kernel,
				     PlotSystemInfo_T *information)
{

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "Give me a name";
  information->psFormula     = "Y = F ( X )";
  information->psDescription = "A typical skeleton for plots";

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_get<<<-canevas->>>Period (Kernel_T         *kernel,
				void             *data)
{

  /*
   * My period is 1000 milli-seconds.
   */
  return 1000;

}


/*
 * This procedure is called whenever all symbols have been time-tagged
 * by the corresponding provider sampling thread.
 * The last value of all symbols can now be retreived by the graphic plot.
 *
 * CAUTION : This procedure is called in another thread, compared to all
 * other procedures of the graphic plot that are called by GTK main thread.
 */
static void
gdisp_treat<<<-canevas->>>SymbolValues (Kernel_T *kernel,
					void     *data)
{

  /*
   * Take into account all last values.
   */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_init<<<-canevas->>>System (Kernel_T     *kernel,
				 PlotSystem_T *plotSystem)
{

  /*
   * We must here provide all plot private functions that remain
   * 'static' here, but accessible from everywhere via the kernel.
   */
  plotSystem->psCreate            = gdisp_create<<<-canevas->>>;
  plotSystem->psDestroy           = gdisp_destroy<<<-canevas->>>;
  plotSystem->psSetParent         = gdisp_set<<<-canevas->>>Parent;
  plotSystem->psGetTopLevelWidget = gdisp_get<<<-canevas->>>TopLevelWidget;
  plotSystem->psSetDimensions     = gdisp_set<<<-canevas->>>InitialDimensions;
  plotSystem->psShow              = gdisp_show<<<-canevas->>>;
  plotSystem->psGetType           = gdisp_get<<<-canevas->>>Type;
  plotSystem->psAddSymbols        = gdisp_addSymbolsTo<<<-canevas->>>;
  plotSystem->psGetSymbols        = gdisp_getSymbolsFrom<<<-canevas->>>;
  plotSystem->psStartStep         = gdisp_startStepOn<<<-canevas->>>;
  plotSystem->psStep              = gdisp_stepOn<<<-canevas->>>;
  plotSystem->psStopStep          = gdisp_stopStepOn<<<-canevas->>>;
  plotSystem->psGetInformation    = gdisp_get<<<-canevas->>>Information;
  plotSystem->psTreatSymbolValues = gdisp_treat<<<-canevas->>>SymbolValues;
  plotSystem->psGetPeriod         = gdisp_get<<<-canevas->>>Period;

}

