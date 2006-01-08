/*!  \file 

$Id: gdisp_pages.c,v 1.9 2006-01-08 14:21:52 erk Exp $

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

File      : Graphic page management.

-----------------------------------------------------------------------
*/


/*
 * Authorize Drag and Drop operations.
 */
#define GD_PAGE_HAS_DND


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"
#if defined(GD_PAGE_HAS_DND)
#  include "gdisp_dragAndDrop.h"
#endif


/*
 * Page border width and row/columns spacings.
 * These are used when drop operation occurs. We must deduce from X and
 * Y drop location in page window coordinates, what is the target plot.
 */
#define GD_PAGE_BORDER_WIDTH  5
#define GD_PAGE_ROW_SPACINGS 10
#define GD_PAGE_COL_SPACINGS 10
#define GD_PAGE_ICON_OFFSET  20

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Compute theorical plot dimensions.
 */
static void
gdisp_computeTheoricalPlotDimensions ( GtkWidget *pageWindow,
				       Page_T    *page,
				       guint     *plotWidth,
				       guint     *plotHeight ) {

  *plotWidth  =
    (pageWindow->allocation.width - (2 * GD_PAGE_BORDER_WIDTH) -
     ((page->pColumns - 1) * GD_PAGE_COL_SPACINGS)) / page->pColumns;

  *plotHeight =
    (pageWindow->allocation.height - (2 * GD_PAGE_BORDER_WIDTH) -
     ((page->pRows    - 1) * GD_PAGE_ROW_SPACINGS)) / page->pRows;

}


/*
 * Graphic page destruction.
 */
static void
gdisp_destroyGraphicPage ( Kernel_T *kernel,
			   Page_T   *page )
{

  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;

  /*
   * Remove the page from the kernel list.
   */
  kernel->pageList = g_list_remove(kernel->pageList,
				   (gpointer)page);

  /*
   * Destroy all plots.
   */
  plotSystemData = page->pPlotSystemData;
  while (plotSystemData <
	 page->pPlotSystemData + (page->pRows * page->pColumns)) {

    (*plotSystemData->plotSystem->psDestroy)(kernel,
					     plotSystemData->plotData);

    plotSystemData->plotData = (void*)NULL;

    plotSystemData++;

  }

  /*
   * Delete resources.
   */
  gtk_widget_destroy(page->pTable );
  gtk_widget_destroy(page->pWindow);

  g_string_free(page->pName,TRUE);

  g_free(page->pPlotSystemData);

  memset(page,0,sizeof(Page_T));
  g_free(page);

}


/*
 * The "delete_event" occurs when the window manager sens this event
 * to the application, usually by the "close" option, or on the titlebar.
 * Returning TRUE means that we do not want to have the "destroy" event 
 * emitted, keeping GDISP+ running. Returning FALSE, we ask that "destroy"
 * be emitted, which in turn will call the "destroy" signal handler.
 */
static gint
gdisp_manageDeleteEventFromWM ( GtkWidget *pageWindow,
				GdkEvent  *event,
				gpointer   data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Allow the window manager to close graphic page windows
   * only if sampling is off.
   */
  return (kernel->samplingThreadMustExit == FALSE ? TRUE : FALSE);

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdisp_destroySignalHandler ( GtkWidget *pageWindow,
			     gpointer   data )
{

  Kernel_T *kernel        = (Kernel_T*)data;

  GString  *messageString =  (GString*)NULL;
  GList    *pageItem      =    (GList*)NULL;
  Page_T   *page          =   (Page_T*)NULL;


  /*
   * Search the corresponding graphic page in the kernel.
   */
  pageItem = g_list_first(kernel->pageList);
  while (pageItem != (GList*)NULL) {

    page = (Page_T*)pageItem->data;
    if (page->pWindow == pageWindow) {

      /*
       * We have found the graphic page the window belongs to.
       */
      break; /* out of while */

    }

    page     = (Page_T*)NULL;
    pageItem = g_list_next(pageItem);

  }

  /*
   * We have found the graphic page the window belongs to.
   */
  if (page != (Page_T*)NULL) {

    /*
     * Destroy the graphic page.
     */
    gdisp_destroyGraphicPage(kernel,
			     page);

    /*
     * Message.
     */
    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "Graphic page correctly destroyed (%d remaining).",
		     g_list_length(kernel->pageList));
    kernel->outputFunc(kernel,messageString,GD_MESSAGE);

  }


}


/*
 * Treat 'expose' X event.
 * What shall I do when the page has to be re-drawn ?
 */
#if defined(GD_PAGE_TREAT_EXPOSE_EVENT)

static gboolean
gdisp_handlePageExpose ( GtkWidget      *pWindow,
			 GdkEventExpose *event,
			 gpointer        data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * The only way I have found to keep the main board and the databook
   * on the top of all other windows, is to trace page window expose
   * events.
   */
  if (kernel->widgets.dataBookWindow != (GtkWidget*)NULL) {
    gdk_window_raise(GTK_WIDGET(kernel->widgets.dataBookWindow)->window);
  }
  if (kernel->widgets.mainBoardWindow != (GtkWidget*)NULL) {
    gdk_window_raise(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);
  }

  return TRUE;

}

#endif

#if defined(GD_PAGE_HAS_DND)


/*
 * DND "drag_begin" handler : this is called whenever a drag starts.
 */
static void
gdisp_beginDNDCallback (GtkWidget      *pageWindow,
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
  fprintf(stdout,"begin DND Callback for page window.\n");
  fflush (stdout);
#endif

  /*
   * Nothing to be done here because no drag operation from the
   * page window.
   */

}


/*
 * DND "drag_end" handler : this is called when a drag and drop has
 * completed. So this function is the last one to be called in
 * any given DND operation.
 */
static void
gdisp_endDNDCallback (GtkWidget      *pageWindow,
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
  fprintf(stdout,"end DND Callback for page window.\n");
  fflush (stdout);
#endif

  /*
   * Nothing to be done here because no drag operation from the
   * page window.
   */

}


/*
 * Analyse mouse position over a plot.
 */
static gboolean
gdisp_analysePositionOverPlot ( Kernel_T          *kernel,
				PlotSystemData_T  *plotSystemData,
				gdouble            xPosition,
				gdouble            yPosition,
				PlotSystemZone_T **zone )
{

  GArray           *plotZones       =           (GArray*)NULL;
  guint             currentZoneId   =                       0;
  PlotSystem_T     *plotSystem      =     (PlotSystem_T*)NULL;
  PlotSystemZone_T *currentZone     = (PlotSystemZone_T*)NULL;
  gboolean          isInsideZone    =                   FALSE;
  gboolean          canDrop         =                    TRUE;

  /*
   * Retreive plot zones.
   * Default plot ? If it is, use the currently selected plot type
   * in order to get its zones.
   */
  *zone = (PlotSystemZone_T*)NULL;

  if ((*plotSystemData->plotSystem->psGetType)
      (kernel,plotSystemData->plotData) == GD_PLOT_DEFAULT) {

    /* change plot system */
    plotSystem = &kernel->plotSystems[kernel->currentPlotType];
    plotZones = (*plotSystem->psGetDropZones)(kernel);

  }
  else {

    plotZones = (*plotSystemData->plotSystem->psGetDropZones)(kernel);

  }

  /*
   * If there is no zone, drop is allowed everywhere.
   * If one or several zones exist, drop is allowed according to the
   * zone the mouse is over.
   * If one or several zones exist and the mouse is over none of them,
   * drop is refused.
   */
  if (plotZones != (GArray*)NULL) {

    /*
     * Search the zone the mouse is over.
     */
    while (currentZoneId < plotZones->len && isInsideZone == FALSE) {

      currentZone = &g_array_index(plotZones,PlotSystemZone_T,currentZoneId);

      isInsideZone = gdisp_positionIsInsideZone(currentZone,
						xPosition,
						yPosition);

      if (isInsideZone == FALSE) {
	currentZoneId++;
      }

    } /* loop over all declared zones */

    /*
     * Proceed if we have found a zone.
     */
    if (isInsideZone == TRUE) {

      *zone   = currentZone;
      canDrop = currentZone->pszAcceptDrops;

    }
    else {

      *zone   = (PlotSystemZone_T*)NULL;
      canDrop = FALSE;

    }

  } /* plotZones != (GArray*)NULL */

  return canDrop;

}


/*
 * DND "drag_motion" handler is called whenever the pointer is
 * dragging over the target widget.
 */
static gboolean
gdisp_dragMotionDNDCallback (GtkWidget      *pageWindow,
			     GdkDragContext *dragContext,
			     gint            xPositionInPageWindow,
			     gint            yPositionInPageWindow,
			     guint           time,
			     gpointer        data)
{

  Kernel_T         *kernel          =         (Kernel_T*)data;
  Page_T           *page            =           (Page_T*)NULL;

  guint             plotWidth       =                       0;
  guint             plotHeight      =                       0;
  guint             nColumn         =                       0;
  guint             nRow            =                       0;
  guint             xPositionInPlot =                       0;
  guint             yPositionInPlot =                       0;
  gdouble           xRatioInPlot    =                     0.0;
  gdouble           yRatioInPlot    =                     0.0;

  PlotSystemData_T *plotSystemData  = (PlotSystemData_T*)NULL;
  guint             plotIdentity    =                       0;
  gboolean          canDrop         =                   FALSE;
  PlotSystemZone_T *currentZone     = (PlotSystemZone_T*)NULL;

  gint              windowAttrMask  =                       0;
  GdkWindowAttr     windowAttr;

  Pixmap_T         *iconPixmap      =         (Pixmap_T*)NULL;


  /*
   * Put any needed drag motion code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,
	  "data Motion DND Callback for page window (%d,%d).\n",
	  xPositionInPageWindow,
	  yPositionInPageWindow);
  fflush (stdout);
#endif

  /*
   * Set drag status to COPY action.
   * We always copy the symbols, NOT moving them from the list to the plot.
   */
  gdk_drag_status(dragContext,
		  GDK_ACTION_COPY,
		  time);

  /*
   * Compute the size of each plot of the table.
   */
  page = (Page_T*)gtk_object_get_data(GTK_OBJECT(pageWindow),
				      "pageInformation");

  gdisp_computeTheoricalPlotDimensions(pageWindow,
				       page,
				       &plotWidth,
				       &plotHeight);

  /*
   * Deduce on what plot the drop operation occurs.
   */
  if (xPositionInPageWindow > GD_PAGE_BORDER_WIDTH &&
      yPositionInPageWindow > GD_PAGE_BORDER_WIDTH    ) {

    nColumn =
      (xPositionInPageWindow - GD_PAGE_BORDER_WIDTH) /
      (plotWidth  + GD_PAGE_COL_SPACINGS);

    nRow    = 
      (yPositionInPageWindow - GD_PAGE_BORDER_WIDTH) /
      (plotHeight + GD_PAGE_ROW_SPACINGS);

    /*
     * Am I over a plot ?
     */
    if ((xPositionInPageWindow <
	 (nColumn * (plotWidth  + GD_PAGE_COL_SPACINGS) + plotWidth )) &&
	(yPositionInPageWindow <
	 (nRow    * (plotHeight + GD_PAGE_ROW_SPACINGS) + plotHeight))) {

      /*
       * Yes, I am over a plot.
       * Compute drop coordinates in the plot reference.
       */
      xPositionInPlot = (xPositionInPageWindow - GD_PAGE_BORDER_WIDTH) -
	(nColumn * (plotWidth  + GD_PAGE_COL_SPACINGS));

      yPositionInPlot = (yPositionInPageWindow - GD_PAGE_BORDER_WIDTH) -
	(nRow    * (plotHeight + GD_PAGE_ROW_SPACINGS));

      yPositionInPlot = plotHeight - yPositionInPlot;

      /*
       * GDK windows may have different sizes, compute the drop
       * coordinates as a percentage of the plot dimensions.
       */
      xRatioInPlot = (gdouble)xPositionInPlot / (gdouble)plotWidth;
      yRatioInPlot = (gdouble)yPositionInPlot / (gdouble)plotHeight;

      /*
       * Retreive current plot handle.
       */
      plotIdentity   = nRow * page->pColumns + nColumn;
      plotSystemData = &page->pPlotSystemData[plotIdentity];

      /*
       * Analyse position over plot.
       */
      canDrop = gdisp_analysePositionOverPlot(kernel,
					      plotSystemData,
					      xRatioInPlot,
					      yRatioInPlot,
					      &currentZone);

    } /* i am over a plot */

  }

  /*
   * Have we already built the icon window ?
   * No ? So create it.
   * Yes ? Check if the parent has changed (drag over several windows).
   */
  if (kernel->dndIconWindow == (GdkWindow*)NULL) {

    memset(&windowAttr,0,sizeof(GdkWindowAttr));

    windowAttr.event_mask  = GDK_EXPOSURE_MASK;
    windowAttr.x           = xPositionInPageWindow - GD_PAGE_ICON_OFFSET;
    windowAttr.y           = yPositionInPageWindow - GD_PAGE_ICON_OFFSET;
    windowAttr.width       = GD_PAGE_ICON_OFFSET;
    windowAttr.height      = GD_PAGE_ICON_OFFSET;
    windowAttr.window_type = GDK_WINDOW_CHILD;
    windowAttr.wclass      = GDK_INPUT_OUTPUT;
    windowAttr.visual      = kernel->visual;
    windowAttr.colormap    = kernel->colormap;
    windowAttrMask         = GDK_WA_X        |
                             GDK_WA_Y        |
                             GDK_WA_COLORMAP |
                             GDK_WA_VISUAL;

    kernel->dndIconWindowParent = GTK_WIDGET(page->pWindow)->window;

    kernel->dndIconWindow = gdk_window_new(kernel->dndIconWindowParent,
					   &windowAttr,
					   windowAttrMask);

    gdk_window_show(kernel->dndIconWindow);

    kernel->dndIconWindowGc = gdk_gc_new(kernel->dndIconWindow);

  }
  else {

    /*
     * D&D Icon window exists. Check if the icon window must be reparented.
     * It is typically the case when the mouse crosses window borders during
     * the drag process.
     */
    if (kernel->dndIconWindowParent != GTK_WIDGET(page->pWindow)->window) {

      kernel->dndIconWindowParent = GTK_WIDGET(page->pWindow)->window;

      gdk_window_reparent(kernel->dndIconWindow,
			  kernel->dndIconWindowParent,
			  xPositionInPageWindow - GD_PAGE_ICON_OFFSET,
			  yPositionInPageWindow - GD_PAGE_ICON_OFFSET);

    } /* end of reparent operation */

  }

  /*
   * Move icon window according to mouse position.
   * Update content with the correct pixmap according to drop authorization.
   */
  if (currentZone == (PlotSystemZone_T*)NULL) {

    if (canDrop == TRUE) {

      iconPixmap = gdisp_getPixmapById(kernel,
				       GD_PIX_okButton2,
				       page->pWindow);

    }
    else {

      iconPixmap = gdisp_getPixmapById(kernel,
				       GD_PIX_error,
				       page->pWindow);

    }

  }
  else {

      iconPixmap = gdisp_getPixmapByAddr(kernel,
					 currentZone->pszIcon,
					 page->pWindow);

  }

  kernel->dndIconPixmap     = iconPixmap->pixmap;
  kernel->dndIconPixmapMask = iconPixmap->mask;

  gdk_window_move_resize(kernel->dndIconWindow,
			 xPositionInPageWindow - iconPixmap->width,
			 yPositionInPageWindow - iconPixmap->height,
			 iconPixmap->width,
			 iconPixmap->height);

  gdk_draw_pixmap(kernel->dndIconWindow,
		  kernel->dndIconWindowGc,
		  kernel->dndIconPixmap,
		  0,
		  0,
		  0,
		  0,
		  iconPixmap->width,
		  iconPixmap->height);

  gdk_window_shape_combine_mask(kernel->dndIconWindow,
				kernel->dndIconPixmapMask,
				0,
				0);

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
gdisp_dataRequestDNDCallback (GtkWidget        *pageWindow,
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
  fprintf(stdout,"data Request DND Callback for page window.\n");
  fflush (stdout);
#endif

  /*
   * Nothing to be done here because no drag operation from the
   * page window.
   */

}


/*
 * Finalize Drag & Drop operation according to Drag & Drop scope.
 */
static void
gdisp_finalizeDragAndDropOperation(Kernel_T         *kernel,
				   Page_T           *page,
				   PlotSystemData_T *plotSystemData,
				   guint             plotWidth,
				   guint             plotHeight,
				   guint             nColumn,
				   guint             nRow,
				   guchar            zoneId)
{

  GtkWidget        *plotTopLevel        =    (GtkWidget*)NULL;
  GString          *messageString       =      (GString*)NULL;
  PlotSystem_T     *requestedPlotSystem = (PlotSystem_T*)NULL;
  gchar            *plotName            =        (gchar*)NULL;
  PlotSystemInfo_T  plotInformation;


  /*
   * If the plot we are over, is a 'default plot', replace
   * it by the plot the type of which is currently selected.
   */
  if ((*plotSystemData->plotSystem->psGetType)
                     (kernel,plotSystemData->plotData) == GD_PLOT_DEFAULT) {

    /*
     * Destroy this default plot.
     * Create a new one and attach it correctly to the table.
     * Set up its parent and finally show it.
     */

    /* change plot system */
    requestedPlotSystem = &kernel->plotSystems[kernel->currentPlotType];

    /* check out if this kind of plot is fully supported... */
    if (requestedPlotSystem->psIsSupported == FALSE) {

      messageString = g_string_new((gchar*)NULL);

      /*
       * CAUTION :
       * 'psGetInformation' may not exist... as plot is not fully supported.
       */
      if (requestedPlotSystem->psGetInformation != (aFunction_T)NULL) {

	(*requestedPlotSystem->psGetInformation)(kernel,
						 &plotInformation);

	plotName = plotInformation.psName != (gchar*)NULL ?
                                   plotInformation.psName : "unknown";

      }
      else {

	plotName = "unknown";

      }

      g_string_sprintf(messageString,
		       "Requested graphic plot not fully supported (%s).",
		       plotName);

      kernel->outputFunc(kernel,messageString,GD_ERROR);

      return;

    }

    /* destroy */
    (*plotSystemData->plotSystem->psDestroy)(kernel,
					     plotSystemData->plotData);

    /* re-create */
    plotSystemData->plotSystem = requestedPlotSystem;
    plotSystemData->plotData   =
                       (*plotSystemData->plotSystem->psCreate)(kernel);

    /* set parent widget */
    (*plotSystemData->plotSystem->psSetParent)(kernel,
					       plotSystemData->plotData,
					       page->pWindow);

    /* set initial dimensions */
    (*plotSystemData->plotSystem->psSetDimensions)(kernel,
						   plotSystemData->plotData,
						   plotWidth,
						   plotHeight);

    /* attach new plot */
    plotTopLevel =
      (*plotSystemData->plotSystem->psGetTopLevelWidget)
                                      (kernel,plotSystemData->plotData);

    gtk_table_attach_defaults(GTK_TABLE(page->pTable),
			      plotTopLevel,
			      nColumn,
			      nColumn + 1,
			      nRow,
			      nRow    + 1);

    /* show it */
    (*plotSystemData->plotSystem->psShow)(kernel,
					  plotSystemData->plotData);

  } /* current plot is a 'default plot' */

  /*
   * Add symbols to the plot we are over.
   */
  (*plotSystemData->plotSystem->psAddSymbols)(kernel,
					      plotSystemData->plotData,
					      kernel->dndSelection,
					      zoneId);

  /*
   * Do not forget to update symbols <-> providers assignments.
   */
  (*kernel->assignSymbolsToProviders)(kernel);

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
gdisp_dataReceivedDNDCallback (GtkWidget        *pageWindow,
			       GdkDragContext   *dragContext,
			       gint              xPositionInPageWindow,
			       gint              yPositionInPageWindow,
			       GtkSelectionData *selectionData,
			       guint             entryInfo,
			       guint             time,
			       gpointer          data)
{

  Kernel_T         *kernel          =         (Kernel_T*)data;
  gchar            *action          =            (gchar*)NULL;
  Page_T           *page            =           (Page_T*)NULL;
  GList            *pageItem        =            (GList*)NULL;

  guint             plotWidth       =                       0;
  guint             plotHeight      =                       0;
  guint             nColumn         =                       0;
  guint             nRow            =                       0;
  guint             xPositionInPlot =                       0;
  guint             yPositionInPlot =                       0;
  gdouble           xRatioInPlot    =                     0.0;
  gdouble           yRatioInPlot    =                     0.0;

  PlotSystemData_T *plotSystemData  = (PlotSystemData_T*)NULL;
  guint             plotIdentity    =                       0;

  gboolean          canDrop         =                   FALSE;
  PlotSystemZone_T *currentZone     = (PlotSystemZone_T*)NULL;


  /*
   * Put any needed data received code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,
	  "data Receive DND Callback for page window (%d,%d).\n",
	  xPositionInPageWindow,
	  yPositionInPageWindow);
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

      /*
       * Compute the size of each plot of the table.
       */
      page = (Page_T*)gtk_object_get_data(GTK_OBJECT(pageWindow),
					  "pageInformation");

      gdisp_computeTheoricalPlotDimensions(pageWindow,
					   page,
					   &plotWidth,
					   &plotHeight);

      /*
       * Deduce on what plot the drop operation occurs.
       */
      if (xPositionInPageWindow > GD_PAGE_BORDER_WIDTH &&
	  yPositionInPageWindow > GD_PAGE_BORDER_WIDTH    ) {

	nColumn =
	  (xPositionInPageWindow - GD_PAGE_BORDER_WIDTH) /
	  (plotWidth  + GD_PAGE_COL_SPACINGS);

	nRow    = 
	  (yPositionInPageWindow - GD_PAGE_BORDER_WIDTH) /
	  (plotHeight + GD_PAGE_ROW_SPACINGS);

	/*
	 * Am I over a plot ?
	 */
	if ((xPositionInPageWindow <
	     (nColumn * (plotWidth  + GD_PAGE_COL_SPACINGS) + plotWidth )) &&
	    (yPositionInPageWindow <
	     (nRow    * (plotHeight + GD_PAGE_ROW_SPACINGS) + plotHeight))) {

	  /*
	   * Yes, I am over a plot.
	   * Compute drop coordinates in the plot reference.
	   */
	  xPositionInPlot = (xPositionInPageWindow - GD_PAGE_BORDER_WIDTH) -
	    (nColumn * (plotWidth  + GD_PAGE_COL_SPACINGS));

	  yPositionInPlot = (yPositionInPageWindow - GD_PAGE_BORDER_WIDTH) -
	    (nRow    * (plotHeight + GD_PAGE_ROW_SPACINGS));

	  yPositionInPlot = plotHeight - yPositionInPlot;

	  /*
	   * For BROADCAST purpose, as GDK windows may have different
	   * sizes, compute the drop coordinates as a percentage of the
	   * plot dimensions.
	   */
	  xRatioInPlot = (gdouble)xPositionInPlot / (gdouble)plotWidth;
	  yRatioInPlot = (gdouble)yPositionInPlot / (gdouble)plotHeight;

	  /*
	   * Take care of Drag & Drop operation scope.
	   */
	  switch (kernel->dndScope) {

	  case GD_DND_UNICAST :

	    /*
	     * Retreive current plot handle.
	     */
	    plotIdentity   = nRow * page->pColumns + nColumn;
	    plotSystemData = &page->pPlotSystemData[plotIdentity];

	    /*
	     * Analyse position over plot.
	     */
	    canDrop = gdisp_analysePositionOverPlot(kernel,
						    plotSystemData,
						    xRatioInPlot,
						    yRatioInPlot,
						    &currentZone);

	    /*
	     * Finalize Drag & Drop operation according to Drag & Drop scope.
	     */
	    if (canDrop == TRUE)
	      gdisp_finalizeDragAndDropOperation(kernel,
						 page,
						 plotSystemData,
						 plotWidth,
						 plotHeight,
						 nColumn,
						 nRow,
						 currentZone ?
						 currentZone->pszId : 0);

	    break;

	  case GD_DND_MULTICAST :

	    /*
	     * Loop over all graphic plots of the current page.
	     */
	    for (nRow=0; nRow<page->pRows; nRow++) {

	      for (nColumn=0; nColumn<page->pColumns; nColumn++) {

		plotIdentity   = nRow * page->pColumns + nColumn;
		plotSystemData = &page->pPlotSystemData[plotIdentity];

		canDrop = gdisp_analysePositionOverPlot(kernel,
							plotSystemData,
							xRatioInPlot,
							yRatioInPlot,
							&currentZone);

		if (canDrop == TRUE)
		  gdisp_finalizeDragAndDropOperation(kernel,
						     page,
						     plotSystemData,
						     plotWidth,
						     plotHeight,
						     nColumn,
						     nRow,
						     currentZone ?
						     currentZone->pszId : 0);

	      } /* columns */

	    } /* rows */

	    break;

	  case GD_DND_BROADCAST :

	    /*
	     * Loop over all graphic plots of all existing graphic pages.
	     * The difference between the BROADCAST mode and the two previous
	     * ones is that we loop here upon all graphic pages that MAY have
	     * different sizes.
	     * So "plotWidth", "plotHeight" and drop coordinates have to be
	     * recomputed for each page.
	     */
	    pageItem = g_list_first(kernel->pageList);
	    while (pageItem != (GList*)NULL) {

	      page       = (Page_T*)pageItem->data;
	      pageWindow = page->pWindow;

	      gdisp_computeTheoricalPlotDimensions(pageWindow,
						   page,
						   &plotWidth,
						   &plotHeight);

	      xPositionInPlot = (guint)(xRatioInPlot * (gdouble)plotWidth );
	      yPositionInPlot = (guint)(yRatioInPlot * (gdouble)plotHeight);

	      for (nRow=0; nRow<page->pRows; nRow++) {

		for (nColumn=0; nColumn<page->pColumns; nColumn++) {

		  plotIdentity   = nRow * page->pColumns + nColumn;
		  plotSystemData = &page->pPlotSystemData[plotIdentity];

		  canDrop = gdisp_analysePositionOverPlot(kernel,
							  plotSystemData,
							  xRatioInPlot,
							  yRatioInPlot,
							  &currentZone);

		  if (canDrop == TRUE) {

		    gdisp_finalizeDragAndDropOperation(kernel,
						       page,
						       plotSystemData,
						       plotWidth,
						       plotHeight,
						       nColumn,
						       nRow,
						       currentZone ?
						       currentZone->pszId : 0);

		  } /* if */

		} /* columns */

	      } /* rows */

	      pageItem = g_list_next(pageItem);

	    } /* while */

	    break;

	  default :

	    break;

	  } /* end of switch (kernel->dndScope) */

	} /* Am I over a plot ? */

      }

    }
    else {

      /* nothing else is supported by now */

    }

  } /* test on 'entryInfo' */

  /*
   * Now all is done, what about removing icon window ?
   */
  if (kernel->dndIconWindow != (GdkWindow*)NULL) {

    gdk_window_destroy(kernel->dndIconWindow);
    kernel->dndIconWindow       = (GdkWindow*)NULL;
    kernel->dndIconWindowParent = (GdkWindow*)NULL;
    kernel->dndIconPixmap       = (GdkPixmap*)NULL;
    kernel->dndIconPixmapMask   = (GdkBitmap*)NULL;

    gdk_gc_destroy(kernel->dndIconWindowGc);
    kernel->dndIconWindowGc = (GdkGC*)NULL;

  }

}


/*
 * DND "drag_data_delete" handler, this function is called when
 * the data on the source `should' be deleted (ie if the DND was
 * a move).
 */
static void
gdisp_dataDestroyedDNDCallback (GtkWidget      *pageWindow,
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
  fprintf(stdout,"data Destroy DND Callback for page window.\n");
  fflush (stdout);
#endif

  /*
   * This procedure does nothing because there is nothing to be done here.
   * No drag operation from the page window.
   */

}


#endif /* GD_PAGE_HAS_DND */


/*
 * Routine that acts as a callback to retreive the symbols of a plot.
 */
static void
gdisp_getSymbolsInOnePlot ( Kernel_T         *kernel,
			    Page_T           *page,
			    PlotSystemData_T *plotSystemData,
			    void             *userData )
{

  GArray            *plotZones     = (GArray*)NULL;
  guint              currentZoneId = 0;
  PlotSystemZone_T  *currentZone   = (PlotSystemZone_T*)NULL;
  GList            **symbolList    = (GList**)userData;
  GList             *symbolItem    =  (GList*)NULL;


  /*
   * Ask for referenced symbols. Take care of zones !!!
   */
  plotZones = (*plotSystemData->plotSystem->psGetDropZones)(kernel);
 
  if (plotZones != (GArray*)NULL) {

    /*
     * Loop over all declared zones.
     */
    while (currentZoneId < plotZones->len) {

      currentZone =
	&g_array_index(plotZones,PlotSystemZone_T,currentZoneId);

      symbolItem =
	(*plotSystemData->plotSystem->psGetSymbols)(kernel,
						    plotSystemData->plotData,
						    currentZone->pszId);

      /* add symbol to the final list if not already present */
      /* BEGIN */
      symbolItem = g_list_first(symbolItem);
      while (symbolItem != (GList*)NULL) {
	if (g_list_find(*symbolList,symbolItem->data) == (GList*)NULL) {
	  *symbolList = g_list_append(*symbolList,
				      symbolItem->data);
	}
	symbolItem = g_list_next(symbolItem);
      }
      /* END */

      currentZoneId++;

    } /* loop over all declared zones */

  } /* plotZones != (GArray*)NULL */

  else {

    /*
     * No zone.
     */
    symbolItem =
      (*plotSystemData->plotSystem->psGetSymbols)(kernel,
						  plotSystemData->plotData,
						  0 /* dummy zone id */);

    /* add symbol to the final list if not already present */
    /* BEGIN */
    symbolItem = g_list_first(symbolItem);
    while (symbolItem != (GList*)NULL) {
      if (g_list_find(*symbolList,symbolItem->data) == (GList*)NULL) {
	*symbolList = g_list_append(*symbolList,
				    symbolItem->data);
      }
      symbolItem = g_list_next(symbolItem);
    }
    /* END */

  } /* end else */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Add a plot to a graphic page.
 */
PlotSystemData_T*
gdisp_addPlotToGraphicPage (Kernel_T   *kernel,
			    Page_T     *page,
			    PlotType_T  plotType,
			    guint       plotRow,
			    guint       plotNbRows,
			    guint       plotColumn,
			    guint       plotNbColumns)
{

  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;

  /*
   * Check.
   */
  if (page == (Page_T*)NULL) {
    return (PlotSystemData_T*)NULL;
  }
  if (page->pCurrentNbPlotSystems == (page->pRows * page->pColumns)) {
    return (PlotSystemData_T*)NULL;
  }

  /*
   * Find the good position in page memory.
   */
  plotSystemData = &page->pPlotSystemData[page->pCurrentNbPlotSystems];

  /*
   * Set up requested plot type.
   */
  plotSystemData->plotSystem = &kernel->plotSystems[plotType];

  /*
   * Store information.
   */
  plotSystemData->plotRow       = plotRow;
  plotSystemData->plotNbRows    = plotNbRows;
  plotSystemData->plotColumn    = plotColumn;
  plotSystemData->plotNbColumns = plotNbColumns;
  plotSystemData->plotCycle     = G_MAXINT;

  /*
   * Create the plot in memory.
   */
  plotSystemData->plotData =
                       (*plotSystemData->plotSystem->psCreate)(kernel);

  /*
   * Increase the number of already created plots.
   */
  page->pCurrentNbPlotSystems++;

  return plotSystemData;

}


/*
 * Create GDISP+ graphic page.
 */
Page_T*
gdisp_allocateGraphicPage (Kernel_T *kernel,
			   gchar    *pageTitle,
			   guint     pageRows,
			   guint     pageColumns)
{

  Page_T *newPage = (Page_T*)NULL;

  /*
   * Allocate memory for this new graphic page.
   * Insert the new graphic page into the kernel page list.
   */
  newPage = (Page_T*)g_malloc0(sizeof(Page_T));
  assert(newPage);

  newPage->pRows    = pageRows;
  newPage->pColumns = pageColumns;

  if (pageTitle == (gchar*)NULL) {

    newPage->pName = g_string_new("");
    assert(newPage->pName);

    g_string_sprintf(newPage->pName,
		     "Graphic Page #%d",
		     g_list_length(kernel->pageList) + 1);

  }
  else {

    newPage->pName = g_string_new(pageTitle);

  }

  /* --------------------- PLOT SYSTEMS --------------------- */

  /*
   * Allocate memory for plot systems.
   * When the graphic page is created, all that it has is default plots.
   */
  newPage->pPlotSystemData =
    (PlotSystemData_T*)g_malloc0(newPage->pRows *
				 newPage->pColumns * sizeof(PlotSystemData_T));
  assert(newPage->pPlotSystemData);

  /*
   * Now that all has been created within the graphic page,
   * insert the graphic page into the kernel page list.
   */
  kernel->pageList = g_list_append(kernel->pageList,
				   (gpointer)newPage);
  assert(kernel->pageList);

  /*
   * Return the created page.
   */
  return newPage;

}


/*
 * Create GDISP+ graphic page (gtk graphic part).
 */
void
gdisp_finalizeGraphicPageCreation (Kernel_T *kernel,
				   Page_T   *newPage)
{

  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;
  GtkWidget        *plotTopLevel   =        (GtkWidget*)NULL;
  GdkGeometry       pWindowHints;
#if defined(GD_PAGE_HAS_DND)
  GtkTargetEntry    targetEntry;
#endif
  guint             plotWidth      = 0;
  guint             plotHeight     = 0;
  gint              screenWidth    = 0;
  gint              screenHeight   = 0;
  gint              row            = 0;
  gint              column         = 0;


  /*
   * Now, deal with graphic page top-level window.
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  screenWidth  = gdk_screen_width ();
  screenHeight = gdk_screen_height();

  newPage->pWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  assert(newPage->pWindow);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdisp_manageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdisp_destroySignalHandler),
		     (gpointer)kernel);

#if defined(GD_PAGE_TREAT_EXPOSE_EVENT)
  gtk_signal_connect (GTK_OBJECT(newPage->pWindow),
		      "expose_event",
		      (GtkSignalFunc)gdisp_handlePageExpose,
		      (gpointer)kernel); 
#endif

#if defined(GD_PAGE_HAS_DND)


  /*
   * Drag And Drop settings.
   * Set up the 'page window' as a potential DND destination.
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
  gtk_drag_dest_set(newPage->pWindow,
		    GTK_DEST_DEFAULT_MOTION    |
		    GTK_DEST_DEFAULT_HIGHLIGHT | GTK_DEST_DEFAULT_DROP,
		    &targetEntry,
		    sizeof(targetEntry) / sizeof(GtkTargetEntry),
		    GDK_ACTION_COPY);

  /*
   * Set DND signals on 'cList'.
   */
  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_begin",
		     GTK_SIGNAL_FUNC(gdisp_beginDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_end",
		     GTK_SIGNAL_FUNC(gdisp_endDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_motion",
		     GTK_SIGNAL_FUNC(gdisp_dragMotionDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_data_get",
		     GTK_SIGNAL_FUNC(gdisp_dataRequestDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_data_received",
		     GTK_SIGNAL_FUNC(gdisp_dataReceivedDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "drag_data_delete",
		     GTK_SIGNAL_FUNC(gdisp_dataDestroyedDNDCallback),
		     kernel);

#endif /* GD_PAGE_HAS_DND */

  gtk_object_set_data(GTK_OBJECT(newPage->pWindow),
		      "pageInformation",
		      (gpointer)newPage);


  /*
   * Set up window size, title and border width.
   */
#define GD_WINDOW_MAX_INITIAL_WIDTH  400
#define GD_WINDOW_MAX_INITIAL_HEIGHT 300
  gtk_widget_set_usize(GTK_WIDGET(newPage->pWindow),
		       MIN(3 * screenWidth  / 4,GD_WINDOW_MAX_INITIAL_WIDTH),  /* width  */
		       MIN(3 * screenHeight / 4,GD_WINDOW_MAX_INITIAL_HEIGHT)); /* height */

  gtk_window_set_title(GTK_WINDOW(newPage->pWindow),
		       newPage->pName->str);

  gtk_container_set_border_width(GTK_CONTAINER(newPage->pWindow),
				 GD_PAGE_BORDER_WIDTH);

  pWindowHints.min_aspect = (gdouble)screenWidth / (gdouble)screenHeight;
  pWindowHints.max_aspect = pWindowHints.min_aspect;

  gtk_widget_show(newPage->pWindow);

#if defined(GD_KEEP_ASPECT_RATIO)
  /* FIXME : should be a user preference */
  gdk_window_set_geometry_hints(GTK_WIDGET(newPage->pWindow)->window,
				&pWindowHints,
				GDK_HINT_ASPECT);
#endif

  /*
   * Create a table with the correct dimensions.
   */
  newPage->pTable = gtk_table_new(newPage->pRows,
				  newPage->pColumns,
				  TRUE /* homogeneous */);

  gtk_table_set_row_spacings(GTK_TABLE(newPage->pTable),
			     GD_PAGE_ROW_SPACINGS);
  gtk_table_set_col_spacings(GTK_TABLE(newPage->pTable),
			     GD_PAGE_COL_SPACINGS);

  gtk_container_add(GTK_CONTAINER(newPage->pWindow),
		    newPage->pTable);

  gtk_widget_show(newPage->pTable);


  /*
   * Compute theorical plot dimensions.
   */
  gdisp_computeTheoricalPlotDimensions(newPage->pWindow,
				       newPage,
				       &plotWidth,
				       &plotHeight);


  /*
   * Attach each graphic plots to the correct place.
   */
  plotSystemData = newPage->pPlotSystemData;

  for (row=0; row<newPage->pRows; row++) {

    for (column=0; column<newPage->pColumns; column++) {

      (*plotSystemData->plotSystem->psSetParent)(kernel,
						 plotSystemData->plotData,
						 newPage->pWindow);

      (*plotSystemData->plotSystem->psSetDimensions)(kernel,
						     plotSystemData->plotData,
						     plotWidth,
						     plotHeight);

      plotTopLevel =
	(*plotSystemData->plotSystem->psGetTopLevelWidget)
	                                 (kernel,plotSystemData->plotData);

      gtk_table_attach_defaults(
		   GTK_TABLE(newPage->pTable),
		   plotTopLevel,
		   plotSystemData->plotColumn,
		   plotSystemData->plotColumn + plotSystemData->plotNbColumns,
		   plotSystemData->plotRow,
		   plotSystemData->plotRow + plotSystemData->plotNbRows);

      plotSystemData++;

    } /* columns */

  } /* rows */


  /*
   * Show all plots.
   */
  plotSystemData = newPage->pPlotSystemData;
  while (plotSystemData <
	 newPage->pPlotSystemData + (newPage->pRows * newPage->pColumns)) {

    (*plotSystemData->plotSystem->psShow)(kernel,
					  plotSystemData->plotData);

    plotSystemData++;

  }

}


/*
 * Create GDISP+ graphic page (menu callback).
 */
void
gdisp_createGraphicPage (gpointer factoryData,
			 guint    pageDimension)
{

  Kernel_T         *kernel         =  (Kernel_T*)factoryData;
  Page_T           *newPage        =           (Page_T*)NULL;
  PlotSystem_T     *defaultSystem  =     (PlotSystem_T*)NULL;
  GString          *messageString  =          (GString*)NULL;
  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;
  gint              row            = 0;
  gint              column         = 0;

  /* --------------------- BEGIN --------------------- */

  assert(kernel);

  /*
   * Custom dimension management is not done yet.
   */
  if (pageDimension == 0) {
    return;
  }

  /*
   * CAUTION.
   * Before anything, we must check out whether 'default plot' plot system
   * is fully supported. Without default plot system, the graphic page
   * can not be created.
   */
  defaultSystem = &kernel->plotSystems[GD_PLOT_DEFAULT];
  if (defaultSystem->psIsSupported == FALSE) {

    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "Default plot system not fully supported.");
    kernel->outputFunc(kernel,messageString,GD_ERROR);

    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "Aborting graphic page creation.");
    kernel->outputFunc(kernel,messageString,GD_WARNING);

    return;

  }

  /*
   * Allocate memory for this new graphic page.
   * Insert the new graphic page into the kernel page list.
   */
  newPage = gdisp_allocateGraphicPage(kernel,
				      (gchar*)NULL, /* title */
				      pageDimension,
				      pageDimension);
				       
  for (row=0; row<newPage->pRows; row++) {

    for (column=0; column<newPage->pColumns; column++) {

      plotSystemData = gdisp_addPlotToGraphicPage (kernel,
						   newPage,
						   GD_PLOT_DEFAULT,
						   row,
						   1, /* nb rows */
						   column,
						   1); /* nb columns */

    }

  }

  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Create GTK stuffs.
   */
  gdisp_finalizeGraphicPageCreation(kernel,
				    newPage);

  /*
   * Message.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "Graphic page correctly created (%d).",
		   g_list_length(kernel->pageList));
  kernel->outputFunc(kernel,messageString,GD_MESSAGE);

}


/*
 * Return the whole set of symbols affected to plots in pages.
 */
GList*
gdisp_getSymbolsInPages (Kernel_T *kernel)
{

  GList *symbolList = (GList*)NULL;


  /*
   * Loop over all plots defined in the kernel.
   */
  gdisp_loopOnGraphicPlots (kernel,
			    gdisp_getSymbolsInOnePlot,
			    (void*)&symbolList);

  return symbolList;

}


/*
 * Destroy all graphic pages.
 */
void
gdisp_destroyAllGraphicPages ( Kernel_T *kernel )
{

  GList  *pageItem = (GList*)NULL;
  Page_T *page     = (Page_T*)NULL;

  /*
   * Loop upon all graphic pages of the kernel.
   */
  do {

    pageItem = g_list_first(kernel->pageList);

    if (pageItem != (GList*)NULL) {

      page = (Page_T*)pageItem->data;

      /*
       * Destroy the graphic page.
       */
      gdisp_destroyGraphicPage(kernel,
			       page);

    }

  } while (pageItem != (GList*)NULL);

}
