/*!  \file 

$Id: gdisp_pages.c,v 1.1 2004-02-04 20:32:10 esteban Exp $

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


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * The "delete_event" occurs when the window manager sens this event
 * to the application, usually by the "close" option, or on the titlebar.
 * Returning TRUE means that we do not want to have the "destroy" event 
 * emitted, keeping GDISP+ running. Returning FALSE, we ask that "destroy"
 * be emitted, which in turn will call the "destroy" signal handler.
 */
static gint
gdispManageDeleteEventFromWM (GtkWidget *pageWindow,
			      GdkEvent  *event,
			      gpointer   data)
{

  return FALSE;

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdispDestroySignalHandler (GtkWidget *pageWindow,
			   gpointer   data)
{

  Kernel_T      *kernel        = (Kernel_T*)data;

  PlotSystem_T **plotSystem    = (PlotSystem_T**)NULL;
  void         **plotData      =         (void**)NULL;
  GString       *messageString =  (GString*)NULL;
  GList         *pageItem      =    (GList*)NULL;
  Page_T        *page          =   (Page_T*)NULL;


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
     * Remove the page from the kernel list.
     */
    kernel->pageList = g_list_remove(kernel->pageList,
				     (gpointer)page);

    /*
     * Destroy all plots.
     */
    plotSystem = page->pPlotSystems;
    plotData   = page->pPlotData;
    while (plotSystem <
	   page->pPlotSystems + (page->pRows * page->pColumns)) {

      (*(*plotSystem)->psDestroy)(kernel,*plotData);

      *plotData = (void*)NULL;

      plotSystem++; plotData++;

    }

    /*
     * Delete resources.
     */
    gtk_widget_destroy(page->pWindow);
    g_string_free(page->pName,TRUE);

    g_free(page->pPlotSystems);
    g_free(page->pPlotData   );

    memset(page,0,sizeof(Page_T));
    g_free(page);

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

  Kernel_T *kernel = (Kernel_T*)data;

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
gdisp_finalizeDragAndDropOperation(Kernel_T      *kernel,
				   Page_T        *page,
				   PlotSystem_T **plotSystem,
				   void         **plotData,
				   guint          plotWidth,
				   guint          plotHeight,
				   guint          nColumn,
				   guint          nRow,
				   guint          xPositionInPlot,
				   guint          yPositionInPlot)
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
  if ((*(*plotSystem)->psGetType)(kernel,*plotData) == GD_PLOT_DEFAULT) {

    /*
     * Destroy this default plot.
     * Create a new one and attach it correctly to the table.
     * Set up its parent and finally show it.
     */

    /* change plot system */
    requestedPlotSystem = &kernel->plotSystems[GD_PLOT_2D];

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
    (*(*plotSystem)->psDestroy)(kernel,
				*plotData);

    /* re-create */
    *plotSystem = requestedPlotSystem;
    *plotData   = (*(*plotSystem)->psCreate)(kernel);

    /* set parent widget */
    (*(*plotSystem)->psSetParent)(kernel,
				  *plotData,
				  page->pWindow);

    /* set initial dimensions */
    (*(*plotSystem)->psSetDimensions)(kernel,
				      *plotData,
				      plotWidth,
				      plotHeight);

    /* attach new plot */
    plotTopLevel = (*(*plotSystem)->psGetTopLevelWidget)(kernel,
							 *plotData);

    gtk_table_attach_defaults(GTK_TABLE(page->pTable),
			      plotTopLevel,
			      nColumn,
			      nColumn + 1,
			      nRow,
			      nRow    + 1);

    /* show it */
    (*(*plotSystem)->psShow)(kernel,*plotData);

  } /* current plot is a 'default plot' */

  /*
   * Add symbols to the plot we are over.
   */
  (*(*plotSystem)->psAddSymbols)(kernel,
				 *plotData,
				 kernel->dndSelection,
				 xPositionInPlot,
				 yPositionInPlot);

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

  Kernel_T      *kernel          =      (Kernel_T*)data;
  gchar         *action          =         (gchar*)NULL;
  Page_T        *page            =        (Page_T*)NULL;
  GList         *pageItem        =         (GList*)NULL;

  guint          plotWidth       =                    0;
  guint          plotHeight      =                    0;
  guint          nColumn         =                    0;
  guint          nRow            =                    0;
  guint          xPositionInPlot =                    0;
  guint          yPositionInPlot =                    0;

  PlotSystem_T **plotSystem      = (PlotSystem_T**)NULL;
  void         **plotData        =         (void**)NULL;
  guint          plotIdentity    =                    0;


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

      plotWidth  =
	(pageWindow->allocation.width - (2 * GD_PAGE_BORDER_WIDTH) -
	 ((page->pColumns - 1) * GD_PAGE_COL_SPACINGS)) / page->pColumns;

      plotHeight =
	(pageWindow->allocation.height - (2 * GD_PAGE_BORDER_WIDTH) -
	 ((page->pRows    - 1) * GD_PAGE_ROW_SPACINGS)) / page->pRows;

      /*
       * Deduce on what plot the drop operation occurs.
       */
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
	xPositionInPlot = xPositionInPageWindow -
	  (nColumn * (plotWidth  + GD_PAGE_COL_SPACINGS));

	yPositionInPlot = yPositionInPageWindow -
	  (nRow    * (plotHeight + GD_PAGE_ROW_SPACINGS));

	yPositionInPlot = plotHeight - yPositionInPlot;

	/*
	 * Take care of Drag & Drop operation scope.
	 */
	switch (kernel->dndScope) {

	case GD_DND_UNICAST :

	  /*
	   * Retreive current plot handle.
	   */
	  plotIdentity = nRow * page->pColumns + nColumn;

	  plotSystem   = &page->pPlotSystems[plotIdentity];
	  plotData     = &page->pPlotData   [plotIdentity];

	  /*
	   * Finalize Drag & Drop operation according to Drag & Drop scope.
	   */
	  gdisp_finalizeDragAndDropOperation(kernel,
					     page,
					     plotSystem,
					     plotData,
					     plotWidth,
					     plotHeight,
					     nColumn,
					     nRow,
					     xPositionInPlot,
					     yPositionInPlot);

	  break;

	case GD_DND_MULTICAST :

	  /*
	   * Loop over all graphic plots of the current page.
	   */
	  for (nRow=0; nRow<page->pRows; nRow++) {

	    for (nColumn=0; nColumn<page->pColumns; nColumn++) {

	      plotIdentity = nRow * page->pColumns + nColumn;

	      plotSystem   = &page->pPlotSystems[plotIdentity];
	      plotData     = &page->pPlotData   [plotIdentity];

	      gdisp_finalizeDragAndDropOperation(kernel,
						 page,
						 plotSystem,
						 plotData,
						 plotWidth,
						 plotHeight,
						 nColumn,
						 nRow,
						 xPositionInPlot,
						 yPositionInPlot);

	      plotSystem++; plotData++;

	    } /* columns */

	  } /* rows */

	  break;

	case GD_DND_BROADCAST :

	  /*
	   * Loop over all graphic plots of all existing graphic pages.
	   */
	  pageItem = g_list_first(kernel->pageList);
	  while (pageItem != (GList*)NULL) {

	    page = (Page_T*)pageItem->data;

	    for (nRow=0; nRow<page->pRows; nRow++) {

	      for (nColumn=0; nColumn<page->pColumns; nColumn++) {

		plotIdentity = nRow * page->pColumns + nColumn;

		plotSystem   = &page->pPlotSystems[plotIdentity];
		plotData     = &page->pPlotData   [plotIdentity];

		gdisp_finalizeDragAndDropOperation(kernel,
						   page,
						   plotSystem,
						   plotData,
						   plotWidth,
						   plotHeight,
						   nColumn,
						   nRow,
						   xPositionInPlot,
						   yPositionInPlot);

		plotSystem++; plotData++;

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
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ graphic page.
 */
void
gdisp_createGraphicPage (gpointer factoryData,
			 guint    pageDimension)
{

  Kernel_T        *kernel        =      (Kernel_T*)factoryData;
  Page_T          *newPage       =        (Page_T*)NULL;
  GString         *messageString =       (GString*)NULL;
  PlotSystem_T   **plotSystem    = (PlotSystem_T**)NULL;
  PlotSystem_T    *defaultSystem =  (PlotSystem_T*)NULL;
  void           **plotData      =         (void**)NULL;
  GtkWidget       *plotTopLevel  =     (GtkWidget*)NULL;
  GdkGeometry      pWindowHints;
#if defined(GD_PAGE_HAS_DND)
  GtkTargetEntry   targetEntry;
#endif

  gint           screenWidth   =                 0;
  gint           screenHeight  =                 0;
  gint           row           =                 0;
  gint           column        =                 0;


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
  newPage = (Page_T*)g_malloc0(sizeof(Page_T));
  assert(newPage);

  newPage->pRows    = pageDimension;
  newPage->pColumns = pageDimension;

  newPage->pName    = g_string_new("Graphic Page");
  assert(newPage->pName);

  kernel->pageList = g_list_append(kernel->pageList,
				   (gpointer)newPage);
  assert(kernel->pageList);

  /* --------------------- PLOT SYSTEMS --------------------- */

  /*
   * Allocate memory for plot systems.
   * When the graphic page is created, all that it has is default plots.
   */
  newPage->pPlotSystems =
    (PlotSystem_T**)g_malloc0(newPage->pRows *
			      newPage->pColumns * sizeof(PlotSystem_T*));
  assert(newPage->pPlotSystems);

  newPage->pPlotData =
    (void**)g_malloc0(newPage->pRows * newPage->pColumns * sizeof(void*));
  assert(newPage->pPlotData);

  plotSystem = newPage->pPlotSystems;
  plotData   = newPage->pPlotData;

  while (plotSystem <
	 newPage->pPlotSystems + (newPage->pRows * newPage->pColumns)) {

    *plotSystem = &kernel->plotSystems[GD_PLOT_DEFAULT];
    *plotData   = (*(*plotSystem)->psCreate)(kernel);

    plotSystem++; plotData++;

  }

  /* ------------------------ MAIN WINDOW ------------------------ */

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
		     GTK_SIGNAL_FUNC(gdispManageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(newPage->pWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdispDestroySignalHandler),
		     (gpointer)kernel);


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

  gtk_object_set_data(GTK_OBJECT(newPage->pWindow),
		      "pageInformation",
		      (gpointer)newPage);


#endif /* GD_PAGE_HAS_DND */


  /*
   * Set up window size, title and border width.
   */
  gtk_widget_set_usize(GTK_WIDGET(newPage->pWindow),
		       3 * screenWidth  / 4,  /* width  */
		       3 * screenHeight / 4); /* height */

  gtk_window_set_title(GTK_WINDOW(newPage->pWindow),
		       "GDISP+ Graphic Page.");

  gtk_container_set_border_width(GTK_CONTAINER(newPage->pWindow),
				 GD_PAGE_BORDER_WIDTH);

  pWindowHints.min_aspect = (gdouble)screenWidth / (gdouble)screenHeight;
  pWindowHints.max_aspect = pWindowHints.min_aspect;

  gtk_widget_show(newPage->pWindow);

  gdk_window_set_geometry_hints(GTK_WIDGET(newPage->pWindow)->window,
				&pWindowHints,
				GDK_HINT_ASPECT);
				

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

  gtk_widget_show   (newPage->pTable);


  /*
   * Attach each graphic plots to the correct place.
   */
  plotSystem = newPage->pPlotSystems;
  plotData   = newPage->pPlotData;

  for (row=0; row<newPage->pRows; row++) {

    for (column=0; column<newPage->pColumns; column++) {

      (*(*plotSystem)->psSetParent)(kernel,*plotData,newPage->pWindow);

      plotTopLevel = (*(*plotSystem)->psGetTopLevelWidget)(kernel,*plotData);

      gtk_table_attach_defaults(GTK_TABLE(newPage->pTable),
				plotTopLevel,
				column,
				column + 1,
				row,
				row    + 1);

      plotSystem++; plotData++;

    } /* columns */

  } /* rows */


  /*
   * Show all plots.
   */
  plotSystem = newPage->pPlotSystems;
  plotData   = newPage->pPlotData;
  while (plotSystem <
	 newPage->pPlotSystems + (newPage->pRows * newPage->pColumns)) {

    (*(*plotSystem)->psShow)(kernel,*plotData);

    plotSystem++; plotData++;

  }

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

  GList         *pageItem      =         (GList*)NULL,
                *symbolItem    =         (GList*)NULL,
                *symbolList    =         (GList*)NULL;
  Page_T        *page          =        (Page_T*)NULL;
  PlotSystem_T **plotSystem    = (PlotSystem_T**)NULL;
  void         **plotData      =         (void**)NULL;

  /*
   * Loop over all pages defined in the kernel.
   */
  pageItem = g_list_first(kernel->pageList);
  while (pageItem != (GList*)NULL) {

    page = (Page_T*)pageItem->data;

    /*
     * Loop over all plots of a given page.
     */
    plotSystem = page->pPlotSystems;
    plotData   = page->pPlotData;
    while (plotSystem <
	   page->pPlotSystems + (page->pRows * page->pColumns)) {

      /*
       * Read the list of symbols on the X axis.
       */
      symbolItem = (*(*plotSystem)->psGetSymbols)(kernel,*plotData,'X');
      symbolItem = g_list_first(symbolItem);

      while (symbolItem != (GList*)NULL) {

	if (g_list_find(symbolList,symbolItem->data) == (GList*)NULL) {

	  symbolList = g_list_append(symbolList,
				     symbolItem->data);

	}

	symbolItem = g_list_next(symbolItem);

      }


      /*
       * Read the list of symbols on the Y axis.
       */
      symbolItem = (*(*plotSystem)->psGetSymbols)(kernel,*plotData,'Y');
      symbolItem = g_list_first(symbolItem);

      while (symbolItem != (GList*)NULL) {

	if (g_list_find(symbolList,symbolItem->data) == (GList*)NULL) {

	  symbolList = g_list_append(symbolList,
				     symbolItem->data);

	}

	symbolItem = g_list_next(symbolItem);

      }

      plotSystem++; plotData++;

    }

    pageItem = g_list_next(pageItem);

  }

  return symbolList;

}

