/*

$Id: gdisp_defaultPlot.c,v 1.7 2006-08-05 20:50:30 esteban Exp $

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

File      : Default plot system.

-----------------------------------------------------------------------
*/


/*
 * Forbid Drag and Drop operations.
 */
#undef GD_DEFAULT_PLOT_HAS_DND


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

#include "gdisp_defaultPlot.h"
#if defined(GD_DEFAULT_PLOT_HAS_DND)
#  include "gdisp_dragAndDrop.h"
#endif


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


#if defined(GD_DEFAULT_PLOT_HAS_DND)


/*
 * DND "drag_begin" handler : this is called whenever a drag starts.
 */
static void
gdisp_beginDNDCallback (GtkWidget      *graphicArea,
			GdkDragContext *dragContext,
			gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed drag begin setup code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"begin DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

}


/*
 * DND "drag_end" handler : this is called when a drag and drop has
 * completed. So this function is the last one to be called in
 * any given DND operation.
 */
static void
gdisp_endDNDCallback (GtkWidget      *graphicArea,
		      GdkDragContext *dragContext,
		      gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed drag end cleanup code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"end DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

}


/*
 * DND "drag_motion" handler is called whenever the pointer is
 * dragging over the target widget.
 */
static gboolean
gdisp_dragMotionDNDCallback (GtkWidget      *graphicArea,
			     GdkDragContext *dragContext,
			     gint            x,
			     gint            y,
			     guint           time,
			     gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed drag motion code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Motion DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

  /*
   * Set drag status to COPY action.
   */
  gdk_drag_status(dragContext,
		  GDK_ACTION_COPY,
		  time);

  return FALSE;

}


/*
 * DND "drag_data_get" handler, for handling requests for DND
 * data on the specified widget. This function is called when
 * there is need for DND data on the source, so this function is
 * responsable for setting up the dynamic data exchange buffer
 * (DDE as sometimes it is called) and sending it out.
 */
static void
gdisp_dataRequestDNDCallback (GtkWidget        *graphicArea,
			      GdkDragContext   *dragContext,
			      GtkSelectionData *selectionData,
			      guint             entryInfo,
			      guint             time,
			      gpointer          data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed data request code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Request DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

  /*
   * Nothing to be done here because no drag operation from the
   * graphic area.
   */

}


/*
 * DND "drag_data_received" handler. When 'gdisp_dataRequestDNDCallback'
 * calls 'gtk_selection_data_set()' to send out the data, this function
 * receives it and is responsible for handling it.
 *
 * This is also the only DND callback function where the given
 * inputs may reflect those of the drop target, so we need to check
 * if this is the same structure or not.
 */
static void
gdisp_dataReceivedDNDCallback (GtkWidget        *graphicArea,
			       GdkDragContext   *dragContext,
			       gint              x,
			       gint              y,
			       GtkSelectionData *selectionData,
			       guint             entryInfo,
			       guint             time,
			       gpointer          data)
{

  Kernel_T      *kernel     =      (Kernel_T*)data;
  gchar         *action     =         (gchar*)NULL;
  DefaultPlot_T *plot       = (DefaultPlot_T*)NULL;


  /*
   * Put any needed data received code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Receive DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

  /*
   * Important, check if we actually got data.
   * Sometimes errors occur and 'selectionData' is NULL.
   */
  if (selectionData == (GtkSelectionData*)NULL) {

    return;

  }
  if (selectionData->length < 0) {

    return;

  }

  /*
   * Now check if the data format type is one that we support
   * (remember, data format type, not data type).
   *
   * We check this by testing if info matches one of the info
   * values that we have defined.
   *
   * Note that we can also iterate through the atoms in :
   *
   *  GList *glist = dragContext->targets;
   *
   *  while (glist != (GList*)NULL) {
   *
   *    gchar *name = gdk_atom_name((GdkAtom)glist->data);
   *
   *    ... strcmp the name to see if it matches one that we support...
   *
   *    glist = glist->next;
   *
   *  }
   *
   */
  if (entryInfo == GD_DND_TARGET_INFO) {

    /*
     * Pick up what action to perform.
     */
    action = (gchar*)selectionData->data;
    if (strcmp(action,GD_DND_SYMBOL_LIST_EXCHANGE) ==0) {

      fprintf(stdout,"--PLOT-- %s --PLOT-- \n",action);
      fflush (stdout);

      plot = (DefaultPlot_T*)gtk_object_get_data(GTK_OBJECT(graphicArea),
						 "plotPointer");

    }
    else {

      /* nothing else is supported by now */

    }

  } /* test on 'entryInfo' */

}


/*
 * DND "drag_data_delete" handler, this function is called when
 * the data on the source `should' be deleted (ie if the DND was
 * a move).
 */
static void
gdisp_dataDestroyedDNDCallback (GtkWidget      *graphicArea,
				GdkDragContext *dragContext,
				gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed data destroyed code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Destroy DND Callback for default plot graphic area.\n");
  fflush (stdout);
#endif

  /*
   * This procedure does nothing because there is nothing to be done here.
   * No drag operation from the graphic area.
   */

}


#endif /* GD_DEFAULT_PLOT_HAS_DND */


/*
 * Redraw graphic area content.
 */
static void
gdisp_defaultPlotDrawGraphicArea (Kernel_T      *kernel,
				  DefaultPlot_T *plot)
{

  gchar   *scratchMessage = "*scratch*";
  gint     scratchWidth   = 0;
  GdkFont *scratchFont    = kernel->fonts[GD_FONT_MEDIUM][GD_FONT_FIXED];


  /*
   * By now, just paint a rectangle.
   * Write "*scratch*" in the middle of the graphic area.
   */
  if (plot->dpHasFocus == TRUE) {

    gdk_gc_set_foreground(plot->dpGContext,
			  &kernel->colors[155]);

  }
  else {

    gdk_gc_set_foreground(plot->dpGContext,
			  &kernel->colors[150]);

  }

  gdk_draw_rectangle(plot->dpArea->window,
		     plot->dpGContext,
		     TRUE, /* rectangle is filled */
		     0,
		     0,
		     plot->dpArea->allocation.width,
		     plot->dpArea->allocation.height);

  gdk_gc_set_foreground(plot->dpGContext,
			&kernel->colors[42]);

  gdk_draw_rectangle(plot->dpArea->window,
		     plot->dpGContext,
		     FALSE, /* rectangle is not filled */
		     0,
		     0,
		     plot->dpArea->allocation.width  - 1,
		     plot->dpArea->allocation.height - 1);

  scratchWidth = gdk_string_width(scratchFont,
				  scratchMessage);

  gdk_draw_string(plot->dpArea->window,
		  scratchFont,
		  plot->dpGContext,
		  (plot->dpArea->allocation.width - scratchWidth) / 2,
		  plot->dpArea->allocation.height / 2,
		  scratchMessage);
}


/*
 * Treat 'expose' X event.
 * What shall I do when the graphic area has to be refreshed ?
 */
static gboolean
gdisp_defaultPlotExpose (GtkWidget       *area,
			 GdkEventExpose  *event,
			 gpointer         data)
{

  Kernel_T      *kernel =      (Kernel_T*)data;
  DefaultPlot_T *plot   = (DefaultPlot_T*)NULL;

  /*
   * Graphic area has now to be repainted.
   */
  plot = (DefaultPlot_T*)gtk_object_get_data(GTK_OBJECT(area),
					     "plotPointer");

#if defined(NEED_CLEARING_THE_AREA)
  gdk_window_clear_area(area->window,
			event->area.x,
			event->area.y,
			event->area.width,
			event->area.height);
#endif

  gdk_gc_set_clip_rectangle(plot->dpGContext,
			    &event->area);

  gdisp_defaultPlotDrawGraphicArea(kernel,plot);

  gdk_gc_set_clip_rectangle(plot->dpGContext,
			    (GdkRectangle*)NULL);

  return TRUE;

}


/*
 * Treat 'enter-notify' X event.
 * What shall I do when the mouse enters the graphic area ?
 */
static gboolean
gdisp_defaultPlotEnterNotify (GtkWidget        *area,
			      GdkEventCrossing *event,
			      gpointer          data)
{

  Kernel_T      *kernel =      (Kernel_T*)data;
  DefaultPlot_T *plot   = (DefaultPlot_T*)NULL;

  /*
   * Graphic area has now the focus.
   */
  plot = (DefaultPlot_T*)gtk_object_get_data(GTK_OBJECT(area),
					     "plotPointer");

  plot->dpHasFocus = TRUE;
  gdisp_defaultPlotDrawGraphicArea(kernel,plot);

  return TRUE;

}


/*
 * Treat 'leave-notify' X event.
 * What shall I do when the mouse leaves the graphic area ?
 */
static gboolean
gdisp_defaultPlotLeaveNotify (GtkWidget        *area,
			      GdkEventCrossing *event,
			      gpointer          data)
{

  Kernel_T      *kernel =      (Kernel_T*)data;
  DefaultPlot_T *plot   = (DefaultPlot_T*)NULL;

  /*
   * Graphic area has lost the focus.
   */
  plot = (DefaultPlot_T*)gtk_object_get_data(GTK_OBJECT(area),
					     "plotPointer");

  plot->dpHasFocus = FALSE;
  gdisp_defaultPlotDrawGraphicArea(kernel,plot);

  return TRUE;

}


/*
 * Create a 'default plot' by providing an opaque structure to the
 * caller. This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_createDefaultPlot (Kernel_T *kernel)
{

  DefaultPlot_T  *plot = (DefaultPlot_T*)NULL;
#if defined(GD_DEFAULT_PLOT_HAS_DND)
  GtkTargetEntry  targetEntry;
#endif


  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(DefaultPlot_T));
  assert(plot);

  /*
   * Few initialisations.
   */
  plot->dpHasFocus = FALSE;

  /*
   * Create a single graphic area.
   */
  plot->dpArea = gtk_drawing_area_new();

  /*
   * Activate signals.
   */
  gtk_widget_set_events(plot->dpArea,
			GDK_POINTER_MOTION_MASK      |
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_ENTER_NOTIFY_MASK        |
			GDK_LEAVE_NOTIFY_MASK          );

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "expose_event",
		     (GtkSignalFunc)gdisp_defaultPlotExpose,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "enter_notify_event",
		     (GtkSignalFunc)gdisp_defaultPlotEnterNotify,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "leave_notify_event",
		     (GtkSignalFunc)gdisp_defaultPlotLeaveNotify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(plot->dpArea),
		      "plotPointer",
		      (gpointer)plot);


#if defined(GD_DEFAULT_PLOT_HAS_DND)


  /*
   * Drag And Drop settings.
   * Set up the 'graphic area' as a potential DND destination.
   * First we set up 'targetEntry' which is a structure which specifies
   * the kinds (which we define) of drops accepted on this widget.
   */
  targetEntry.target = GD_DND_TARGET_NAME;
  targetEntry.flags  = 0;
  targetEntry.info   = GD_DND_TARGET_INFO;

  /*
   * Set the drag destination for this widget, using the
   * above target entry types, accept only copies.
   */
  gtk_drag_dest_set(plot->dpArea,
		    GTK_DEST_DEFAULT_MOTION    |
		    GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
		    &targetEntry,
		    sizeof(targetEntry) / sizeof(GtkTargetEntry),
		    GDK_ACTION_COPY);

  /*
   * Set DND signals on 'cList'.
   */
  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_begin",
		     GTK_SIGNAL_FUNC(gdisp_beginDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_end",
		     GTK_SIGNAL_FUNC(gdisp_endDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_motion",
		     GTK_SIGNAL_FUNC(gdisp_dragMotionDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_data_get",
		     GTK_SIGNAL_FUNC(gdisp_dataRequestDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_data_received",
		     GTK_SIGNAL_FUNC(gdisp_dataReceivedDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(plot->dpArea),
		     "drag_data_delete",
		     GTK_SIGNAL_FUNC(gdisp_dataDestroyedDNDCallback),
		     kernel);


#endif /* GD_DEFAULT_PLOT_HAS_DND */


  /*
   * Create a graphic context specific to this plot.
   */
  plot->dpGContext =
    gdk_gc_new(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);


  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a 'default' plot opaque structure.
 */
static void
gdisp_destroyDefaultPlot(Kernel_T *kernel,
			 void     *data)
{

  DefaultPlot_T *plot = (DefaultPlot_T*)data;

  /*
   * Now destroy everything.
   */
  gdk_gc_destroy    (plot->dpGContext);
  gtk_widget_destroy(plot->dpArea    );

  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(DefaultPlot_T));
  g_free(plot);

}


/*
 * Record the identity of the parent widget.
 */
static void
gdisp_setParentWidget (Kernel_T  *kernel,
		       void      *data,
		       GtkWidget *parent)
{

  DefaultPlot_T *plot = (DefaultPlot_T*)data;

  /*
   * Record my parent widget.
   */
  plot->dpParent = parent;

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setDefaultPlotInitialDimensions (Kernel_T *kernel,
				       void     *data,
				       guint     width,
				       guint     height)
{

  DefaultPlot_T *plot = (DefaultPlot_T*)data;

  plot->dpAreaWidth  = width;
  plot->dpAreaHeight = height;

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_getDefaultPlotTopLevelWidget (Kernel_T  *kernel,
				    void      *data)
{

  DefaultPlot_T *plot = (DefaultPlot_T*)data;

  return (GtkWidget*)plot->dpArea;

}


/*
 * By now, the 'default plot' widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showDefaultPlot (Kernel_T  *kernel,
		       void      *data)
{

  DefaultPlot_T *plot = (DefaultPlot_T*)data;

  /*
   * Now show everything.
   */
  gtk_widget_show(plot->dpArea);

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getDefaultPlotType (Kernel_T *kernel)
{

  /*
   * Must be GD_PLOT_DEFAULT.
   */
  return GD_PLOT_DEFAULT;

}


/*
 * Record any incoming symbols.
 */
static void
gdisp_addSymbolsToDefaultPlot (Kernel_T *kernel,
			       void     *data,
			       GList    *symbolList,
			       guchar    zoneId)
{

  /*
   * Nothing to be done on default plot.
   */

}


/*
 * Broadcast all symbols.
 */
static GList*
gdisp_getSymbolsFromDefaultPlot (Kernel_T *kernel,
				 void     *data,
				 gchar     axis)
{

  /*
   * Nothing to be done on default plot.
   */

  return (GList*)NULL;

}


/*
 * Get symbol attributes in order to be saved into the configuration.
 */
static void
gdisp_getSymbolAttributesDefaultPlot (Kernel_T *kernel,
				      void     *data,
				      Symbol_T *symbol,
				      GList    *attributeList)
{

  /* DefaultPlot_T *plot = (DefaultPlot_T*)data; */

  /*
   * Return all attributes of a symbol.
   */

  /* nothing by now */

}


/*
 * Set symbol attributes from the configuration.
 */
static void
gdisp_setSymbolAttributesDefaultPlot (Kernel_T *kernel,
				      void     *data,
				      Symbol_T *symbol,
				      GList    *attributeList)
{

  /* DefaultPlot_T *plot = (DefaultPlot_T*)data; */

  /*
   * Store all attributes of a symbol.
   */

  /* nothing by now */

}


/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOnDefaultPlot (Kernel_T *kernel,
			      void     *data)
{

  /*
   * Nothing to be done on default plot, except that we must
   * return TRUE to the calling procedure in order to allow the general
   * step management to proceed.
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
gdisp_stepOnDefaultPlot (Kernel_T *kernel,
			 void     *data)
{

  /*
   * Nothing to be done on default plot.
   */

}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOnDefaultPlot (Kernel_T *kernel,
			     void     *data)
{

  /*
   * Nothing to be done on default plot.
   */

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getDefaultPlotInformation (Kernel_T         *kernel,
				 PlotSystemInfo_T *information)
{

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "Default";
  information->psFormula     = (gchar*)NULL;
  information->psDescription = "Dummy plot for graphic page creation";

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_getDefaultPlotPeriod (Kernel_T *kernel,
			    void     *data)
{

  /*
   * My period is pure science fiction...
   */
  return G_MAXINT; /* in order to avoid disturbing other plots */

}


/*
 * Get back the zones that have been defined on that plot.
 */
static GArray*
gdisp_getDefaultPlotDropZones (Kernel_T *kernel)
{

  /*
   * No zones on default plots.
   */
  return (GArray*)NULL;

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
gdisp_treatDefaultPlotSymbolValues (Kernel_T *kernel,
				    void     *data)
{

  /*
   * Nothing to be done here for text plot, because the last
   * value of all symbols is retrieved in the "step" procedure.
   */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_initDefaultPlotSystem (Kernel_T     *kernel,
			     PlotSystem_T *plotSystem)
{

  /*
   * We must here provide all 'defaultPlot' private functions
   * that remain 'static' here, but accessible from everywhere
   * via the kernel.
   */
  plotSystem->psCreate              = gdisp_createDefaultPlot;
  plotSystem->psDestroy             = gdisp_destroyDefaultPlot;
  plotSystem->psSetParent           = gdisp_setParentWidget;
  plotSystem->psGetTopLevelWidget   = gdisp_getDefaultPlotTopLevelWidget;
  plotSystem->psShow                = gdisp_showDefaultPlot;
  plotSystem->psGetType             = gdisp_getDefaultPlotType;
  plotSystem->psAddSymbols          = gdisp_addSymbolsToDefaultPlot;
  plotSystem->psSetDimensions       = gdisp_setDefaultPlotInitialDimensions;
  plotSystem->psGetSymbols          = gdisp_getSymbolsFromDefaultPlot;
  plotSystem->psGetSymbolAttributes = gdisp_getSymbolAttributesDefaultPlot;
  plotSystem->psSetSymbolAttributes = gdisp_setSymbolAttributesDefaultPlot;
  plotSystem->psStartStep           = gdisp_startStepOnDefaultPlot;
  plotSystem->psStep                = gdisp_stepOnDefaultPlot;
  plotSystem->psStopStep            = gdisp_stopStepOnDefaultPlot;
  plotSystem->psGetInformation      = gdisp_getDefaultPlotInformation;
  plotSystem->psTreatSymbolValues   = gdisp_treatDefaultPlotSymbolValues;
  plotSystem->psGetPeriod           = gdisp_getDefaultPlotPeriod;
  plotSystem->psGetDropZones        = gdisp_getDefaultPlotDropZones;

}

