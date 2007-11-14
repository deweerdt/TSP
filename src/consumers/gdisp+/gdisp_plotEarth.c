/*

$Id: gdisp_plotEarth.c,v 1.1 2007-11-14 21:53:19 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2007 - Euskadi.

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

File      : Earth plot system.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/*
 * For key manipulation.
 */
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"

#include "gdisp_popupMenu.h"
#include "gdisp_plotEarth.h"

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Treat 'expose' X event.
 * What shall I do when the graphic area has to be refreshed ?
 */
static gboolean
gdisp_earthExpose (GtkWidget       *area,
		   GdkEventExpose  *event,
		   gpointer         data)
{

  Kernel_T *kernel       = (Kernel_T*)data;
  Earth_T  *plot         = (Earth_T*)NULL;
  gint      windowX      = 0;
  gint      windowY      = 0;
  gint      windowWidth  = 0;
  gint      windowHeight = 0;
  gint      windowDepth  = 0;

  /*
   * Graphic area has now to be repainted.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  gdk_gc_set_clip_rectangle(plot->ethGContext,
			    &event->area);

  gdk_gc_set_foreground(plot->ethGContext,
			&kernel->colors[_BLACK_]);

  gdk_window_get_geometry(plot->ethArea->window,
			  &windowX,
			  &windowY,
			  &windowWidth,
			  &windowHeight,
			  &windowDepth);

  gdk_draw_rectangle(plot->ethArea->window,
		     plot->ethGContext,
		     TRUE, /* rectangle is filled */
		     0,
		     0,
		     windowWidth,
		     windowHeight);

  /* FIXME */

  gdk_gc_set_clip_rectangle(plot->ethGContext,
			    (GdkRectangle*)NULL);

  return TRUE;

}


/*
 * Treat 'configure' X event.
 * What shall I do when the graphic area has to be re-configured ?
 */
static gboolean
gdisp_earthConfigure (GtkWidget         *area,
		      GdkEventConfigure *event,
		      gpointer           data)
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Earth_T  *plot   = (Earth_T*)NULL;

  /*
   * Get plot private data.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  /*
   * Don't know why, but GTK generates a 'configure' event when
   * showing widgets before returning into the main X loop. Strange.
   */
  if (event->width == 1 && event->height == 1) {

    return TRUE;

  }

  /*
   * Graphic area has now to be resized
   */
  plot->ethAreaWidth  = event->width;
  plot->ethAreaHeight = event->height;

  /*
   * Some difficulties to create the back buffer in the 'create' procedure,
   * because some window information are not yet available.
   * So create it here.
   */
  if (plot->ethBackBuffer != (GdkPixmap*)NULL) {

#if defined(GD_UNREF_THINGS)
    gdk_pixmap_unref(plot->ethBackBuffer);
#endif

  }

  /*
   * Create a pixmap for double buffering.
   * Deduce depth from graphic area window.
   */
  plot->ethBackBuffer = gdk_pixmap_new(plot->ethArea->window,
				       plot->ethAreaWidth,
				       plot->ethAreaHeight,
				       -1 /* same as window */);

  /*
   * The configure operation is done after a resize request.
   * So refresh back buffer.
   */

  /* FIXME */

  return TRUE;

}


/*
 * Treat 'enter-notify' X event.
 * What shall I do when the mouse enters the graphic area ?
 */
static gboolean
gdisp_earthEnterNotify (GtkWidget        *area,
			GdkEventCrossing *event,
			gpointer          data)
{

  Earth_T *plot = (Earth_T*)NULL;

  /*
   * Graphic area has now the focus.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  plot->ethHasFocus = TRUE;

  return TRUE;

}


/*
 * Treat 'leave-notify' X event.
 * What shall I do when the mouse leaves the graphic area ?
 */
static gboolean
gdisp_earthLeaveNotify (GtkWidget        *area,
			GdkEventCrossing *event,
			gpointer          data)
{

  Earth_T *plot = (Earth_T*)NULL;

  /*
   * Graphic area has lost the focus.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  plot->ethHasFocus = FALSE;

  return TRUE;

}


/*
 * Treat 'button-press' X event.
 * What shall I do when the user press a button on the graphic area ?
 */
static gboolean
gdisp_earthButtonPress (GtkWidget      *area,
			GdkEventButton *event,
			gpointer        data)
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Earth_T  *plot   = (Earth_T*)NULL;

  /*
   * Graphic area has lost the focus.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  /*
   * Nothing to be done if button identity is not 3.
   */
  if (event->button != 3) {

    return TRUE;

  }

  /*
   * 'button3'
   * 'shift' + 'button3'
   */
  if (event->state & GDK_SHIFT_MASK) {

  }
  else {

  }

  /*
   * Refresh graphic area.
   */

  /* FIXME */

  return TRUE;

}


/*
 * Treat 'motion-notify' X event.
 * What shall I do when the mouse moves over the graphic area ?
 */
static gboolean
gdisp_earthMotionNotify (GtkWidget      *area,
			 GdkEventMotion *event,
			 gpointer        data)
{

  /* Kernel_T *kernel     = (Kernel_T*)data; */
  Earth_T  *plot       = (Earth_T*)NULL;
  guint     xPosition  = 0;
  guint     yPosition  = 0;

  /*
   * Get private data.
   */
  plot = (Earth_T*)gtk_object_get_data(GTK_OBJECT(area),
				       "plotPointer");

  /*
   * Take care of hints from X Server.
   */
  if (event->is_hint) {
  
    gdk_window_get_pointer(event->window,
			   &xPosition,
			   &yPosition,
			   &event->state);

    event->x = (gdouble)xPosition;
    event->y = (gdouble)yPosition;

  }

  /* FIXME */

  return TRUE;

}


/*
 * Create a 'Earth plot' by providing an opaque structure to the
 * caller. This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_createEarth (Kernel_T *kernel)
{

  Earth_T *plot = (Earth_T*)NULL;

  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(Earth_T));

  /*
   * Few initialisations.
   */
  plot->ethHasFocus = FALSE;

  /*
   * Create a graphic area.
   */
  plot->ethArea = gtk_drawing_area_new();

#if defined(GD_AREA_WANT_ALL_EXTRA_EVENTS)

  gtk_widget_set_events(plot->ethArea,
			GDK_POINTER_MOTION_MASK      |
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_ENTER_NOTIFY_MASK        |
			GDK_LEAVE_NOTIFY_MASK        |
			GDK_BUTTON_PRESS_MASK          );

#else

  /*
   * Must be allowed for popup menu purpose and rulers.
   */
  gtk_widget_set_events(plot->ethArea,
			GDK_POINTER_MOTION_MASK      |
			GDK_BUTTON_PRESS_MASK          );

#endif

  gtk_signal_connect(GTK_OBJECT(plot->ethArea),
		     "expose_event",
		     (GtkSignalFunc)gdisp_earthExpose,
		     (gpointer)kernel);

  gtk_signal_connect (GTK_OBJECT(plot->ethArea),
		      "configure_event",
		      (GtkSignalFunc)gdisp_earthConfigure,
		      (gpointer)kernel); 

  /*
   * Store few signals in order to block them afterwards.
   */
  gtk_signal_connect(GTK_OBJECT(plot->ethArea),
		     "enter_notify_event",
		     (GtkSignalFunc)gdisp_earthEnterNotify,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->ethArea),
		     "leave_notify_event",
		     (GtkSignalFunc)gdisp_earthLeaveNotify,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->ethArea),
		     "button_press_event",
		     (GtkSignalFunc)gdisp_earthButtonPress,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->ethArea),
		     "motion_notify_event",
		     (GtkSignalFunc)gdisp_earthMotionNotify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(plot->ethArea),
		      "plotPointer",
		      (gpointer)plot);

  /*
   * Create a graphic context specific to this plot.
   */
  plot->ethGContext =
    gdk_gc_new(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);
  plot->ethFont     = kernel->fonts[GD_FONT_SMALL][GD_FONT_FIXED];

  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a 'Earth' plot opaque structure.
 */
static void
gdisp_destroyEarth(Kernel_T *kernel,
		    void     *data)
{

  Earth_T *plot = (Earth_T*)data;

  /*
   * Now destroy everything.
   */
  gdk_gc_destroy(plot->ethGContext);
#if defined(GD_UNREF_THINGS)
  gdk_pixmap_unref(plot->ethBackBuffer);
#endif
  gtk_widget_destroy(plot->ethArea);

  /*
   * Destroy Menu.
   */
  gdisp_destroyMenu(plot->ethMainMenu);

  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(Earth_T));
  g_free(plot);

}


/*
 * Record the parent widget.
 */
static void
gdisp_setEarthParent (Kernel_T  *kernel,
		      void      *data,
		      GtkWidget *parent)
{

  Earth_T *plot = (Earth_T*)data;

  /*
   * Remember my parent.
   */
  plot->ethParent = parent;

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setEarthInitialDimensions (Kernel_T *kernel,
				 void     *data,
				 guint     width,
				 guint     height)
{

  Earth_T *plot = (Earth_T*)data;

  /*
   * Remember my initial width and height.
   */
  plot->ethAreaWidth  = width;
  plot->ethAreaHeight = height;

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_getEarthTopLevelWidget (Kernel_T  *kernel,
			       void      *data)
{

  Earth_T *plot = (Earth_T*)data;

  return (GtkWidget*)plot->ethArea;

}


/*
 * Popup Menu Handler.
 */
static void
gdisp_popupMenuHandler ( Kernel_T    *kernel,
			 PopupMenu_T *menu,
			 gpointer     menuData,
			 gpointer     itemData )
{

  /* Earth_T *plot   = (Earth_T*)menuData; */
  guint    action = (guint)GPOINTER_TO_UINT(itemData);

  /*
   * Treat action.
   */
  switch (action) {

  default :

    break;

  }

}


/*
 * By now, the '2D plot' widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showEarth (Kernel_T  *kernel,
		 void      *data)
{

  Earth_T     *plot     = (Earth_T*)data;
  PopupMenu_T *subMenu  = (PopupMenu_T*)NULL;
  void        *menuItem = (void*)NULL;

  /*
   * Now show everything.
   */
  gtk_widget_show(plot->ethArea);

  /*
   * Create the dynamic menu.
   * Menu cannot be created before because the parent widget is not shown yet.
   */
  plot->ethMainMenu = gdisp_createMenu(kernel,
				       plot->ethArea,
				       gdisp_popupMenuHandler,
				       (gpointer)plot);

  /*
   * Popup-Menu.
   */
  gdisp_addMenuItem(plot->ethMainMenu,
		    GD_POPUP_ITEM,
		    "FIXME",
		    (gpointer)GUINT_TO_POINTER(0));

  /*
   * Insert a horizontal separator.
   */
  gdisp_addMenuItem(plot->ethMainMenu,
		    GD_POPUP_SEPARATOR,
		    (gchar*)NULL,
		    (gpointer)NULL);

  /*
   * Create the sub-menu.
   */
  menuItem = gdisp_addMenuItem(plot->ethMainMenu,
			       GD_POPUP_ITEM,
			       "Sub-Menu",
			       (gpointer)NULL);

  subMenu = gdisp_createMenu(kernel,
			     menuItem,
			     gdisp_popupMenuHandler,
			     (gpointer)plot);

  gdisp_addMenuItem(subMenu,
		    GD_POPUP_TITLE,
		    "Title" /* title */,
		    (gpointer)NULL);

  gdisp_addMenuItem(subMenu,
		    GD_POPUP_SEPARATOR,
		    (gchar*)NULL,
		    (gpointer)NULL);

  gdisp_addMenuItem(subMenu,
		    GD_POPUP_ITEM,
		    "item 1",
		    (gpointer)GUINT_TO_POINTER(1));

  gdisp_addMenuItem(subMenu,
		    GD_POPUP_ITEM,
		    "item 2",
		    (gpointer)GUINT_TO_POINTER(2));

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getEarthType (Kernel_T *kernel)
{

  /*
   * Must be GD_PLOT_EARTH.
   */
  return GD_PLOT_EARTH;

}


/*
 * Record any incoming symbols.
 * The drop coordinate (0,0) is at the lower left corner of the graphic area.
 */
static void
gdisp_addSymbolsToEarth (Kernel_T *kernel,
			 void     *data,
			 GList    *symbolList,
			 guchar    zoneId)
{

  /* Earth_T *plot = (Earth_T*)data; */

}


/*
 * Broadcast all symbols.
 */
static GList*
gdisp_getSymbolsFromEarth (Kernel_T *kernel,
			   void     *data,
			   gchar     axis)
{

  /* Earth_T *plot = (Earth_T*)data; */

  return (GList*)NULL;

}


/*
 * Get plot attributes in order to be saved into the configuration.
 */
#if defined(GDISP_RESERVED_FOR_THE_FUTURE)

static void
gdisp_getPlotAttributesEarth (Kernel_T *kernel,
			      void     *data,
			      GList    *attributeList)
{

  /* Earth_T *plot = (Earth_T*)data; */

}

#endif

/*
 * Set plot attributes from the configuration.
 */
static void
gdisp_setPlotAttributesEarth (Kernel_T *kernel,
			      void     *data,
			      GList    *attributeList)
{

  /* Earth_T *plot    = (Earth_T*)data; */
  gchar   *keyword = (gchar*)NULL;
  gchar   *value   = (gchar*)NULL;

  /*
   * Loop over all attributes and find correct ones.
   */
  attributeList = g_list_first(attributeList);
  while (attributeList != (GList*)NULL) {

    keyword = (gchar*)attributeList->data;

    attributeList = g_list_next(attributeList);
    value = (gchar*)attributeList->data;

    /*
     * window time duration.
     */
    if (strcmp(keyword,"FIXME") == 0) {

#if defined(EARTH_TO_BE_IMPLEMENTED)
      plot->eth... = atoi(value);
#endif

    }

    attributeList = g_list_next(attributeList);

  } /* while (attributeList != (GList*)NULL) */

}


/*
 * Get symbol attributes in order to be saved into the configuration.
 */
static void
gdisp_getSymbolAttributesEarth (Kernel_T *kernel,
				void     *data,
				Symbol_T *symbol,
				GList    *attributeList)
{

  /* Earth_T *plot = (Earth_T*)data; */

  /*
   * Return all attributes of a symbol.
   */

  /* nothing by now */

}


/*
 * Set symbol attributes from the configuration.
 */
static void
gdisp_setSymbolAttributesEarth (Kernel_T *kernel,
				 void     *data,
				 Symbol_T *symbol,
				 GList    *attributeList)
{

  /* Earth_T *plot = (Earth_T*)data; */

  /*
   * Store all attributes of a symbol.
   */

  /* nothing by now */

}


/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOnEarth (Kernel_T *kernel,
			void     *data)
{

  /* Earth_T *plo = (Earth_T*)data; */

  /*
   * Performs actions before starting steps.
   *
   * BUT we must return TRUE to the calling procedure in order to allow
   * the general step management to proceed.
   *
   * Returning FALSE means that our plot is not enabled to perform its
   * step operations, because of this or that...
   */

  return TRUE /* everything's ok */;

}


/*
 * Real time Step Action.
 */
static void
gdisp_stepOnEarth (Kernel_T *kernel,
		   void     *data)
{

  /* Earth_T *plot = (Earth_T*)data; */

}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOnEarth (Kernel_T *kernel,
		       void     *data)
{

  /* Earth_T *plot = (Earth_T*)data; */

  /*
   * Performs actions after stopping steps.
   */

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_getEarthPeriod (Kernel_T *kernel,
		       void     *data)
{

  /*
   * My period is 1000 milli-seconds.
    */
  return 1000;

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getEarthInformation (Kernel_T         *kernel,
			    PlotSystemInfo_T *information)
{

#include "pixmaps/gdisp_urlLogo.xpm"

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "Earth Plot";
  information->psFormula     = "Pos = F ( lat, lon )";
  information->psDescription = "A typical Earth plot that shows the evolution "
    "of several trajectories expressed with longitudes and latitudes.";
  information->psLogo        = gdisp_urlLogo;

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
gdisp_treatEarthSymbolValues (Kernel_T *kernel,
			      void     *data)
{

  /* Earth_T *plot = (Earth_T*)data; */

}


/*
 * Get back the zones that have been defined on that plot.
 */
static GArray*
gdisp_getEarthDropZones (Kernel_T *kernel)
{

  static GArray *ethDropZones = (GArray*)NULL;

  /*
   * Zones on Earth plots.
   */
  return ethDropZones;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/

#if defined(GD_DYNAMIC_GRAPHIC_MODULES)

void
gdisp_initGraphicSystem (Kernel_T     *kernel,
			 PlotSystem_T *plotSystem)

#else

void
gdisp_initPlotEarthSystem (Kernel_T     *kernel,
			   PlotSystem_T *plotSystem)

#endif

{

  /*
   * We must here provide all 'Earth' private functions
   * that remain 'static' here, but accessible from everywhere
   * via the kernel.
   */
  plotSystem->psCreate              = gdisp_createEarth;
  plotSystem->psDestroy             = gdisp_destroyEarth;
  plotSystem->psSetParent           = gdisp_setEarthParent;
  plotSystem->psGetTopLevelWidget   = gdisp_getEarthTopLevelWidget;
  plotSystem->psShow                = gdisp_showEarth;
  plotSystem->psGetType             = gdisp_getEarthType;
  plotSystem->psAddSymbols          = gdisp_addSymbolsToEarth;
  plotSystem->psGetSymbols          = gdisp_getSymbolsFromEarth;
  plotSystem->psGetSymbolAttributes = gdisp_getSymbolAttributesEarth;
  plotSystem->psSetSymbolAttributes = gdisp_setSymbolAttributesEarth;
  plotSystem->psSetDimensions       = gdisp_setEarthInitialDimensions;
  plotSystem->psStartStep           = gdisp_startStepOnEarth;
  plotSystem->psStep                = gdisp_stepOnEarth;
  plotSystem->psStopStep            = gdisp_stopStepOnEarth;
  plotSystem->psGetInformation      = gdisp_getEarthInformation;
  plotSystem->psTreatSymbolValues   = gdisp_treatEarthSymbolValues;
  plotSystem->psGetPeriod           = gdisp_getEarthPeriod;
  plotSystem->psGetDropZones        = gdisp_getEarthDropZones;
  plotSystem->psSetPlotAttributes   = gdisp_setPlotAttributesEarth;
  /* plotSystem->psGetPlotAttributes   = gdisp_getPlotAttributesEarth; */

}

