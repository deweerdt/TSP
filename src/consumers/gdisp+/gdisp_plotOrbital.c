/*!  \file 

$Id: gdisp_plotOrbital.c,v 1.1 2005-03-08 21:28:17 esteban Exp $

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

File      : A spacecraft orbiting around the Earth.

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

#include "gdisp_plotOrbital.h"


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
gdisp_createPlotOrbital (Kernel_T *kernel)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)NULL;


  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a plot opaque structure.
 */
static void
gdisp_destroyPlotOrbital(Kernel_T *kernel,
			 void     *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Now destroy everything.
   */


  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(PlotOrbital_T));
  g_free(plot);

}


/*
 * Record parent widget.
 */
static void
gdisp_setPlotOrbitalParent (Kernel_T  *kernel,
			    void      *data,
			    GtkWidget *parent)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Store parent widget.
   */

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setPlotOrbitalInitialDimensions (Kernel_T *kernel,
				       void     *data,
				       guint     width,
				       guint     height)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

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
gdisp_getPlotOrbitalTopLevelWidget (Kernel_T  *kernel,
				    void      *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  return (GtkWidget*)plot->poTable;

}


/*
 * By now, the plot widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showPlotOrbital (Kernel_T  *kernel,
		       void      *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Now show everything.
   */

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getPlotOrbitalType (Kernel_T *kernel,
			  void     *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Return the type of the plot.
   */
  return GD_PLOT_ORBITAL;

}


/*
 * Record any incoming symbols.
 */
static void
gdisp_addSymbolsToPlotOrbital (Kernel_T *kernel,
			       void     *data,
			       GList    *symbolList,
			       guchar    zoneId)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Record here all incoming symbols.
   */

}


/*
 * Broadcast all recorded symbols.
 */
static GList*
gdisp_getSymbolsFromPlotOrbital (Kernel_T *kernel,
				 void     *data,
				 gchar     axis)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Return the list of symbols.
   */
  return (GList*)NULL;

}


/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOnPlotOrbital (Kernel_T *kernel,
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
gdisp_stepOnPlotOrbital (Kernel_T *kernel,
			 void     *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Do anything you want to perform steps.
   */
}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOnPlotOrbital (Kernel_T *kernel,
			     void     *data)
{

  PlotOrbital_T *plot = (PlotOrbital_T*)data;

  /*
   * Do anything you want when steps are stopped.
   */

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getPlotOrbitalInformation (Kernel_T         *kernel,
				 PlotSystemInfo_T *information)
{

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "Orbital Plot";
  information->psFormula     = "Spacecraft + Earth";
  information->psDescription = "A spacecraft orbiting around the Earth";

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_getPlotOrbitalPeriod (Kernel_T         *kernel,
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
gdisp_treatPlotOrbitalSymbolValues (Kernel_T *kernel,
				    void     *data)
{

  /*
   * Take into account all last values.
   */

}


/*
 * Get back the zones that have been defined on that plot.
 */
static GArray*
gdisp_getPlotOrbitalDropZones (Kernel_T *kernel)
{

  /*
   * No zones on that plot.
   */
  return (GArray*)NULL;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_initOrbitalPlotSystem (Kernel_T     *kernel,
			     PlotSystem_T *plotSystem)
{

  /*
   * We must here provide all plot private functions that remain
   * 'static' here, but accessible from everywhere via the kernel.
   */
  plotSystem->psCreate            = gdisp_createPlotOrbital;
  plotSystem->psDestroy           = gdisp_destroyPlotOrbital;
  plotSystem->psSetParent         = gdisp_setPlotOrbitalParent;
  plotSystem->psGetTopLevelWidget = gdisp_getPlotOrbitalTopLevelWidget;
  plotSystem->psSetDimensions     = gdisp_setPlotOrbitalInitialDimensions;
  plotSystem->psShow              = gdisp_showPlotOrbital;
  plotSystem->psGetType           = gdisp_getPlotOrbitalType;
  plotSystem->psAddSymbols        = gdisp_addSymbolsToPlotOrbital;
  plotSystem->psGetSymbols        = gdisp_getSymbolsFromPlotOrbital;
  plotSystem->psStartStep         = gdisp_startStepOnPlotOrbital;
  plotSystem->psStep              = gdisp_stepOnPlotOrbital;
  plotSystem->psStopStep          = gdisp_stopStepOnPlotOrbital;
  plotSystem->psGetInformation    = gdisp_getPlotOrbitalInformation;
  plotSystem->psTreatSymbolValues = gdisp_treatPlotOrbitalSymbolValues;
  plotSystem->psGetPeriod         = gdisp_getPlotOrbitalPeriod;
  plotSystem->psGetDropZones      = gdisp_getPlotOrbitalDropZones;

}

