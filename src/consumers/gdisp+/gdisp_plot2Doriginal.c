/*!  \file 

$Id: gdisp_plot2Doriginal.c,v 1.3 2004-10-22 20:17:34 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi.

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

File      : 2D plot system.

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
 * For key manipulation.
 */
#include <gdk/gdkkeysyms.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"

#include "gdisp_plot2D.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#undef DEBUG_2D

#define EVENT_METHOD(widget,event) \
        GTK_WIDGET_CLASS(GTK_OBJECT(widget)->klass)->event

#define X_WIN_TO_PLOT(plot,x) (x)
#define Y_WIN_TO_PLOT(plot,y) ((plot)->p2dAreaHeight - (y))

#define X_PLOT_TO_WIN(plot,x) (x)
#define Y_PLOT_TO_WIN(plot,y) ((plot)->p2dAreaHeight - (y))

#define Y_SYMBOL_OFFSET 15


/*
 * Are the drop coordinates inside the X zone ?
 * 'x' and 'y' are in plot coordinates.
 */
static gboolean
gdisp_isInsideXZone (Plot2D_T *plot,
		     guint     x,
		     guint     y)
{

  /*
   * Y
   * |       Y zone
   * |
   * | /--------------------
   * |/        X zone
   * +-----------------------> X
   *
   */

  guint coefficient = 5; /* 20 % */

  /*
   * Apply filter.
   */
  if (x < plot->p2dAreaWidth / coefficient) {

    if (y < (plot->p2dAreaHeight * x / plot->p2dAreaWidth)) {

      return TRUE;

    }

  }
  else {

    if (y < plot->p2dAreaHeight / coefficient) {

      return TRUE;

    }

  }

  return FALSE;

}


/*
 * Function in order to sort symbols alphabetically' when inserting
 * them into the double-linked list.
 */
static gint
gdisp_sortSymbolByName(gconstpointer data1,
		       gconstpointer data2)
{

  Symbol_T *symbol1 = (Symbol_T*)data1,
           *symbol2 = (Symbol_T*)data2;

  return (strcmp(symbol1->sInfo.name,symbol2->sInfo.name));

}


/*
 * Copy back buffer into graphic area (front buffer).
 */
static void
gdisp_plot2DSwapBuffers (Kernel_T *kernel,
			 Plot2D_T *plot)
{

#if defined(DEBUG_2D)
  fprintf(stdout,"Swaping front and back buffers.\n");
  fflush (stdout);
#endif

  /*
   * Copy back buffer into front buffer.
   */
  gdk_draw_pixmap(plot->p2dArea->window,
		  plot->p2dGContext,
		  plot->p2dBackBuffer,
		  0,
		  0,
		  0,
		  0,
		  plot->p2dArea->allocation.width,
		  plot->p2dArea->allocation.height);

}


/*
 * Redraw back buffer content.
 */
static void
gdisp_plot2DDrawBackBuffer (Kernel_T *kernel,
			    Plot2D_T *plot)
{

  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;
  guint     xPosition  =               0;
  guint     yPosition  =               0;
  gint      lBearing   =               0;
  gint      rBearing   =               0;
  gint      width      =               0;
  gint      ascent     =               0;
  gint      descent    =               0;
  GdkColor *backColor  = (GdkColor*)NULL;
  GdkColor *xAreaColor = (GdkColor*)NULL;
  GdkPoint  xPolygon[4];


#if defined(DEBUG_2D)
  fprintf(stdout,"Drawing into back buffer.\n");
  fflush (stdout);
#endif

  /*
   * By now, just paint a rectangle according to focus.
   */
  if (plot->p2dHasFocus == TRUE) {

    backColor  = &kernel->colors[155];
    xAreaColor = &kernel->colors[154];

  }
  else {

    backColor  = &kernel->colors[150];
    xAreaColor = &kernel->colors[150];

  }

  gdk_gc_set_foreground(plot->p2dGContext,
			backColor);

  gdk_draw_rectangle(plot->p2dBackBuffer,
		     plot->p2dGContext,
		     TRUE, /* rectangle is filled */
		     0,
		     0,
		     plot->p2dArea->allocation.width,
		     plot->p2dArea->allocation.height);


  /*
   * X area.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			xAreaColor);

  xPolygon[0].x = X_PLOT_TO_WIN(plot,0                      );
  xPolygon[0].y = Y_PLOT_TO_WIN(plot,0                      );
  
  xPolygon[1].x = X_PLOT_TO_WIN(plot,plot->p2dAreaWidth  / 5);
  xPolygon[1].y = Y_PLOT_TO_WIN(plot,plot->p2dAreaHeight / 5);

  xPolygon[2].x = X_PLOT_TO_WIN(plot,plot->p2dAreaWidth     );
  xPolygon[2].y = Y_PLOT_TO_WIN(plot,plot->p2dAreaHeight / 5);

  xPolygon[3].x = X_PLOT_TO_WIN(plot,plot->p2dAreaWidth     );
  xPolygon[3].y = Y_PLOT_TO_WIN(plot,0                      );

  gdk_draw_polygon(plot->p2dBackBuffer,
		   plot->p2dGContext,
		   TRUE, /* filled */
		   xPolygon,
		   sizeof(xPolygon) / sizeof(xPolygon[0]));

  /*
   * Symbol list attached to the X axis.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[42]);

  xPosition = plot->p2dAreaWidth  - Y_SYMBOL_OFFSET;
  yPosition = plot->p2dAreaHeight - Y_SYMBOL_OFFSET;

  symbolItem = g_list_first(plot->p2dXSymbolList);
  if (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;

    xPosition -= gdk_string_width(plot->p2dFont,
				  symbol->sInfo.name);

    gdk_draw_string(plot->p2dBackBuffer,
		    plot->p2dFont,
		    plot->p2dGContext,
		    xPosition,
		    yPosition,
		    symbol->sInfo.name);

  }

  /*
   * Symbol list attached to the Y axis.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[42 /* black */]);

  xPosition = Y_SYMBOL_OFFSET;
  yPosition = Y_SYMBOL_OFFSET;

  symbolItem = g_list_first(plot->p2dYSymbolList);
  while (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;

    gdk_draw_string(plot->p2dBackBuffer,
		    plot->p2dFont,
		    plot->p2dGContext,
		    xPosition,
		    yPosition,
		    symbol->sInfo.name);

    /*
     * Underline selected symbol.
     */
    if (symbolItem == plot->p2dSelectedSymbol) {

      gdk_string_extents(plot->p2dFont,
			 symbol->sInfo.name,
			 &lBearing,
			 &rBearing,
			 &width,
			 &ascent,
			 &descent);

      gdk_draw_line(plot->p2dBackBuffer,
		    plot->p2dGContext,
		    xPosition,                /* x1 */
		    yPosition + descent,      /* y1 */
		    xPosition + width,        /* x2 */
		    yPosition + descent);     /* y2 */

    }

    yPosition += Y_SYMBOL_OFFSET;

    symbolItem = g_list_next(symbolItem);

  }

  /*
   * Black lines around the graphic area.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[42 /* black */]);

  gdk_draw_line(plot->p2dBackBuffer,
		plot->p2dGContext,
		0,                                   /* x1 */
		0,                                   /* y1 */
		plot->p2dArea->allocation.width - 1, /* x2 */
		0);                                  /* y2 */

  gdk_draw_line(plot->p2dBackBuffer,
		plot->p2dGContext,
		plot->p2dArea->allocation.width  - 1,  /* x1 */
		0,                                     /* y1 */
		plot->p2dArea->allocation.width  - 1,  /* x2 */
		plot->p2dArea->allocation.height - 1); /* y2 */
}


/*
 * Treat 'expose' X event.
 * What shall I do when the graphic area has to be refreshed ?
 */
static gboolean
gdisp_plot2DExpose (GtkWidget       *area,
		    GdkEventExpose  *event,
		    gpointer         data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;

  /*
   * Graphic area has now to be repainted.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

#if defined(DEBUG_2D)
  fprintf(stdout,"Expose event\n");
  fflush (stdout);
#endif

  gdk_gc_set_clip_rectangle(plot->p2dGContext,
			    &event->area);

  gdisp_plot2DSwapBuffers(kernel,plot);

  gdk_gc_set_clip_rectangle(plot->p2dGContext,
			    (GdkRectangle*)NULL);

  return TRUE;

}


/*
 * Treat 'configure' X event.
 * What shall I do when the graphic area has to be re-configured ?
 */
static gboolean
gdisp_plot2DConfigure (GtkWidget         *area,
		       GdkEventConfigure *event,
		       gpointer           data)
{

  Kernel_T *kernel        = (Kernel_T*)data;
  Plot2D_T *plot          = (Plot2D_T*)NULL;


  /*
   * Get plot private data.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

#if defined(DEBUG_2D)
  fprintf(stdout,"Configure event.\n");
  fflush (stdout);
#endif

  /*
   * Don't know why, but GTK generates a 'configure' event when
   * showing widgets before returning into the main X loop. Strange.
   */
  if (event->width == 1 && event->height == 1) {

#if defined(DEBUG_2D)
    fprintf(stdout,"Configure event returns suddendly.\n");
    fflush (stdout);
#endif

    return TRUE;

  }


  /*
   * Graphic area has now to be resized
   */
  plot->p2dAreaWidth  = event->width;
  plot->p2dAreaHeight = event->height;


  /*
   * Some difficulties to create the back buffer in the 'create' procedure,
   * because some window information are not yet available.
   * So create it here.
   */
  if (plot->p2dBackBuffer != (GdkPixmap*)NULL) {

    gdk_pixmap_unref(plot->p2dBackBuffer);

  }


  /*
   * Create a pixmap for double buffering.
   * Deduce depth from graphic area window.
   */
#if defined(DEBUG_2D)
  fprintf(stdout,"Creating back buffer.\n");
  fflush (stdout);
#endif

  plot->p2dBackBuffer = gdk_pixmap_new(plot->p2dArea->window,
				       plot->p2dAreaWidth,
				       plot->p2dAreaHeight,
				       -1 /* same as window */);

  assert(plot->p2dBackBuffer);


  /*
   * The configure operation is done after a resize request.
   * So refresh back buffer.
   */
  gdisp_plot2DDrawBackBuffer(kernel,plot);

  return TRUE;

}


/*
 * Treat 'enter-notify' X event.
 * What shall I do when the mouse enters the graphic area ?
 */
static gboolean
gdisp_plot2DEnterNotify (GtkWidget        *area,
			 GdkEventCrossing *event,
			 gpointer          data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;

  /*
   * Graphic area has now the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  plot->p2dHasFocus = TRUE;
  gdisp_plot2DDrawBackBuffer(kernel,plot);
  gdisp_plot2DSwapBuffers   (kernel,plot);

  return TRUE;

}


/*
 * Treat 'leave-notify' X event.
 * What shall I do when the mouse leaves the graphic area ?
 */
static gboolean
gdisp_plot2DLeaveNotify (GtkWidget        *area,
			 GdkEventCrossing *event,
			 gpointer          data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;

  /*
   * Graphic area has lost the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  plot->p2dHasFocus = FALSE;
  gdisp_plot2DDrawBackBuffer(kernel,plot);
  gdisp_plot2DSwapBuffers   (kernel,plot);

  return TRUE;

}


/*
 * Treat 'key-press' X event.
 * What shall I do when the user press a key on the graphic area ?
 */
#if defined(WAIT_FORT_GTK_BUG_CORRECTION)

static gboolean
gdisp_plot2DKeyPress (GtkWidget   *area,
		      GdkEventKey *event,
		      gpointer     data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;


  /*
   * 'key press' events on a graphic area are not well treated
   * in version 1.2 of GTK.
   * SORRY.
   */

  /*
   * Graphic area has lost the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  /*
   * Delete all symbols in case of 'c/C' key (clear all),
   * or delete one symbol in case of 'd/D' key (delete one).
   */
  switch (event->keyval) {

  case GDK_c :
  case GDK_C :
    if (plot->p2dXSymbolList != (GList*)NULL)
      g_list_free(plot->p2dXSymbolList);
    if (plot->p2dYSymbolList != (GList*)NULL)
      g_list_free(plot->p2dYSymbolList);
    plot->p2dXSymbolList     = (GList*)NULL;
    plot->p2dYSymbolList     = (GList*)NULL;
    plot->p2dSelectedSymbol  = (GList*)NULL;
    break;

  case GDK_d :
  case GDK_D :
    if (plot->p2dSelectedSymbol != (GList*)NULL) {
      plot->p2dYSymbolList = g_list_remove_link(plot->p2dYSymbolList,
						plot->p2dSelectedSymbol);
      g_list_free(plot->p2dSelectedSymbol);
      plot->p2dSelectedSymbol = (GList*)NULL;
    }
    break;

  default :
    break;

  }

  /*
   * Refresh graphic area.
   */
  gdisp_plot2DDrawBackBuffer(kernel,plot);
  gdisp_plot2DSwapBuffers   (kernel,plot);

  return TRUE;

}

#endif

/*
 * Treat 'button-press' X event.
 * What shall I do when the user press a button on the graphic area ?
 */
static gboolean
gdisp_plot2DButtonPress (GtkWidget      *area,
			 GdkEventButton *event,
			 gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;


  /*
   * Graphic area has lost the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  /*
   * Nothing to be done if button identity is not 3.
   */
  if (event->button != 3) {

    return TRUE;

  }

  /*
   * 'button3' removes the selected symbol from the list.
   * 'shift' + 'button3' removes all symbols.
   */
  if (event->state & GDK_SHIFT_MASK) {

    if (plot->p2dXSymbolList != (GList*)NULL)
      g_list_free(plot->p2dXSymbolList);

    if (plot->p2dYSymbolList != (GList*)NULL)
      g_list_free(plot->p2dYSymbolList);

    plot->p2dXSymbolList     = (GList*)NULL;
    plot->p2dYSymbolList     = (GList*)NULL;
    plot->p2dSelectedSymbol  = (GList*)NULL;

  }
  else {

    if (plot->p2dSelectedSymbol != (GList*)NULL) {

      plot->p2dYSymbolList = g_list_remove_link(plot->p2dYSymbolList,
						plot->p2dSelectedSymbol);

      g_list_free(plot->p2dSelectedSymbol);
      plot->p2dSelectedSymbol = (GList*)NULL;

    }

  }

  /*
   * Refresh graphic area.
   */
  gdisp_plot2DDrawBackBuffer(kernel,plot);
  gdisp_plot2DSwapBuffers   (kernel,plot);

  return TRUE;

}


/*
 * Treat 'motion-notify' X event.
 * What shall I do when the mouse moves over the graphic area ?
 */
static gboolean
gdisp_plot2DMotionNotify (GtkWidget      *area,
			  GdkEventMotion *event,
			  gpointer        data)
{

  Kernel_T *kernel     = (Kernel_T*)data;
  Plot2D_T *plot       = (Plot2D_T*)NULL;

  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;
  guint     xPosition  =               0;
  guint     yPosition  =               0;
  gint      lBearing   =               0;
  gint      rBearing   =               0;
  gint      width      =               0;
  gint      ascent     =               0;
  gint      descent    =               0;

#if defined(DEBUG_2D)
  fprintf(stdout,"Motion notify event.\n");
  fflush (stdout);
#endif


  /*
   * Graphic area has lost the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");


  /*
   * Return if no symbol available.
   */
  if (plot->p2dYSymbolList == (GList*)NULL) {

    return TRUE;

  }

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

  /*
   * Try to get the symbol the mouse is over.
   */
  xPosition = Y_SYMBOL_OFFSET;
  yPosition = Y_SYMBOL_OFFSET;

  symbolItem = g_list_first(plot->p2dYSymbolList);
  while (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;

    /*
     * Compute surrounding rectangle dimensions.
     */
    gdk_string_extents(plot->p2dFont,
		       symbol->sInfo.name,
		       &lBearing,
		       &rBearing,
		       &width,
		       &ascent,
		       &descent);

    if (xPosition          < event->x && event->x < xPosition + width &&
	yPosition - ascent < event->y && event->y < yPosition + descent  ) {

      if (symbolItem != plot->p2dSelectedSymbol) {

	/*
	 * This symbol becomes the new selected one, refresh graphic area.
	 */
	plot->p2dSelectedSymbol = symbolItem;

	gdisp_plot2DDrawBackBuffer(kernel,plot);
	gdisp_plot2DSwapBuffers   (kernel,plot);

      }

      /*
       * Stop here investigations, because selected symbol is found.
       */
      return TRUE;

    } /* Inside rectangle */

    yPosition += Y_SYMBOL_OFFSET;

    symbolItem = g_list_next(symbolItem);

  }

  /*
   * No symbol is selected, because the mouse is not inside any rectangle.
   * Refresh graphic area.
   */
  if (plot->p2dSelectedSymbol != (GList*)NULL) {

    plot->p2dSelectedSymbol = (GList*)NULL;

    gdisp_plot2DDrawBackBuffer(kernel,plot);
    gdisp_plot2DSwapBuffers   (kernel,plot);

  }

  return TRUE;

}


/*
 * Create a '2D plot' by providing an opaque structure to the
 * caller. This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_createPlot2D (Kernel_T *kernel)
{

#define FROZEN_RULER

  Plot2D_T *plot = (Plot2D_T*)NULL;


  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(Plot2D_T));
  assert(plot);

  /*
   * Few initialisations.
   */
  plot->p2dHasFocus = FALSE;
  plot->p2dType     = GD_PLOT_2D_X;

  /*
   * Create a table : dimension 2 x 2, homogeneous.
   * Insert a graphic area and two rulers in it.
   */
  plot->p2dTable = gtk_table_new(2,
				 2,
				 FALSE);

  plot->p2dArea = gtk_drawing_area_new();

  gtk_table_attach(GTK_TABLE(plot->p2dTable),
		   plot->p2dArea,
		   1,
		   2,
		   0,
		   1,
		   GTK_EXPAND | GTK_FILL,
		   GTK_EXPAND | GTK_FILL,
		   0,
		   0 );

  gtk_widget_set_events(plot->p2dArea,
			GDK_POINTER_MOTION_MASK      |
			GDK_POINTER_MOTION_HINT_MASK |
			GDK_ENTER_NOTIFY_MASK        |
			GDK_LEAVE_NOTIFY_MASK        |
			GDK_BUTTON_PRESS_MASK          );

  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "expose_event",
		     (GtkSignalFunc)gdisp_plot2DExpose,
		     (gpointer)kernel);

  gtk_signal_connect (GTK_OBJECT(plot->p2dArea),
		      "configure_event",
		      (GtkSignalFunc)gdisp_plot2DConfigure,
		      (gpointer)kernel); 

  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "enter_notify_event",
		     (GtkSignalFunc)gdisp_plot2DEnterNotify,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "leave_notify_event",
		     (GtkSignalFunc)gdisp_plot2DLeaveNotify,
		     (gpointer)kernel);

#if defined(WAIT_FORT_GTK_BUG_CORRECTION)
  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "key_press_event",
		     (GtkSignalFunc)gdisp_plot2DKeyPress,
		     (gpointer)kernel);
#endif

  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "button_press_event",
		     (GtkSignalFunc)gdisp_plot2DButtonPress,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->p2dArea),
		     "motion_notify_event",
		     (GtkSignalFunc)gdisp_plot2DMotionNotify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(plot->p2dArea),
		      "plotPointer",
		      (gpointer)plot);

  /*
   * The horizontal ruler goes on bottom.
   * As the mouse moves across the drawing area, a 'motion_notify_event'
   * is passed to the appropriate event handler for the ruler.
   */
  plot->p2dHRuler = gtk_hruler_new();

  gtk_ruler_set_metric(GTK_RULER(plot->p2dHRuler),
		       GTK_PIXELS);

  gtk_ruler_set_range(GTK_RULER(plot->p2dHRuler),
		      0,
		      10,
		      0,
		      10);

#if !defined(FROZEN_RULER)
  gtk_signal_connect_object(GTK_OBJECT(plot->p2dArea),
			    "motion_notify_event",
			    (GtkSignalFunc)EVENT_METHOD(plot->p2dHRuler,
							motion_notify_event),
			    GTK_OBJECT(plot->p2dHRuler));
#endif

#if defined(FROZEN_RULER)
  gtk_widget_set_sensitive(plot->p2dHRuler,
			   FALSE /* no sensitive */);
#endif

  gtk_table_attach(GTK_TABLE(plot->p2dTable),
		   plot->p2dHRuler,
		   1,
		   2,
		   1,
		   2,
		   GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		   GTK_FILL,
		   0,
		   0);
    
  /*
   * The vertical ruler goes on the left.
   * As the mouse moves across the drawing area, a 'motion_notify_event'
   * is passed to the appropriate event handler for the ruler.
   */
  plot->p2dVRuler = gtk_vruler_new();

  gtk_ruler_set_metric(GTK_RULER(plot->p2dVRuler),
		       GTK_PIXELS);

  gtk_ruler_set_range(GTK_RULER(plot->p2dVRuler),
		      -10,
		      0,
		      0,
		      10);

#if !defined(FROZEN_RULER)
  gtk_signal_connect_object(GTK_OBJECT(plot->p2dArea),
			    "motion_notify_event",
			    (GtkSignalFunc)EVENT_METHOD(plot->p2dVRuler,
							motion_notify_event),
			    GTK_OBJECT(plot->p2dVRuler));
#endif

#if defined(FROZEN_RULER)
  gtk_widget_set_sensitive(plot->p2dVRuler,
			   FALSE /* no sensitive */);
#endif

  gtk_table_attach(GTK_TABLE(plot->p2dTable),
		   plot->p2dVRuler,
		   0,
		   1,
		   0,
		   1,
		   GTK_FILL,
		   GTK_EXPAND | GTK_SHRINK | GTK_FILL,
		   0,
		   0);

  /*
   * Create a graphic context specific to this plot.
   */
  plot->p2dGContext =
    gdk_gc_new(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);
  plot->p2dFont     = kernel->fonts[GD_FONT_SMALL][GD_FONT_FIXED];


  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a '2D' plot opaque structure.
 */
static void
gdisp_destroyPlot2D(Kernel_T *kernel,
		    void     *data)
{

  Plot2D_T *plot = (Plot2D_T*)data;

  /*
   * Free symbol list.
   */
  g_list_free(plot->p2dXSymbolList);
  g_list_free(plot->p2dYSymbolList);

  /*
   * Now destroy everything.
   */
  gdk_gc_destroy    (plot->p2dGContext  );
  gdk_pixmap_unref  (plot->p2dBackBuffer);
  gtk_widget_destroy(plot->p2dArea      );
  gtk_widget_destroy(plot->p2dHRuler    );
  gtk_widget_destroy(plot->p2dVRuler    );
  gtk_widget_destroy(plot->p2dTable     );

  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(Plot2D_T));
  g_free(plot);

}


/*
 * Record the parent widget.
 */
static void
gdisp_setPlot2DParent (Kernel_T  *kernel,
		       void      *data,
		       GtkWidget *parent)
{

  Plot2D_T *plot = (Plot2D_T*)data;

  /*
   * Remember my parent.
   */
  plot->p2dParent = parent;

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setPlot2DInitialDimensions (Kernel_T *kernel,
				  void     *data,
				  guint     width,
				  guint     height)
{

  Plot2D_T *plot = (Plot2D_T*)data;

  /*
   * Remember my initial width and height.
   */
  plot->p2dAreaWidth  = width;
  plot->p2dAreaHeight = height;

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_getPlot2DTopLevelWidget (Kernel_T  *kernel,
			       void      *data)
{

  Plot2D_T *plot = (Plot2D_T*)data;

#if defined(DEBUG_2D)
  fprintf(stdout,"Getting back plot 2D top level widget.\n");
  fflush (stdout);
#endif

  return (GtkWidget*)plot->p2dTable;

}


/*
 * By now, the '2D plot' widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showPlot2D (Kernel_T  *kernel,
		  void      *data)
{

  Plot2D_T *plot = (Plot2D_T*)data;

#if defined(DEBUG_2D)
  fprintf(stdout,"Showing plot 2D.\n");
  fflush (stdout);
#endif

  /*
   * Now show everything.
   */
  gtk_widget_show(plot->p2dArea  );
  gtk_widget_show(plot->p2dHRuler);
  gtk_widget_show(plot->p2dVRuler);
  gtk_widget_show(plot->p2dTable );

  /*
   * These 'show' operations generate 'configure" events. Strange.
   */

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getPlot2DType (Kernel_T *kernel,
		     void     *data)
{

  Plot2D_T *plot = (Plot2D_T*)data;

#if defined(DEBUG_2D)
  fprintf(stdout,"Getting back plot type (2D).\n");
#endif

  /*
   * Must be GD_PLOT_2D_X. See 'create' routine.
   */
  return plot->p2dType;

}


/*
 * Record any incoming symbols.
 * The drop coordinate (0,0) is at the lower left corner of the graphic area.
 */
static void
gdisp_addSymbolsToPlot2D (Kernel_T *kernel,
			  void     *data,
			  GList    *symbolList,
			  guchar    zoneId)
{

  Plot2D_T *plot   = (Plot2D_T*)data;
  GList    *symbol =    (GList*)NULL;

#if defined(DEBUG_2D)
  fprintf(stdout,"Adding symbols to plot 2D.\n");
  fflush (stdout);
#endif

  /*
   * If drop coordinates are in the X zone, the symbol has to be
   * attached to the X axis (often time axis).
   */
  if (gdisp_isInsideXZone(plot,xDrop,yDrop) == TRUE) {

    /*
     * Only one symbol on X axis.
     * So free previous list and create another one.
     */
    g_list_free(plot->p2dXSymbolList);
    plot->p2dXSymbolList = (GList*)NULL;

    /*
     * Take the first symbol of the incoming list.
     */
    symbol = g_list_first(symbolList);
    if (symbol != (GList*)NULL) {

      plot->p2dXSymbolList = g_list_append(plot->p2dXSymbolList,
					   symbol->data);

    }

  }
  else {

    /*
     * Incoming symbols are to be attached to the Y axis.
     */

    /*
     * Loop over all incoming symbols and store only those that
     * are not already in the final list.
     */
    symbol = g_list_first(symbolList);
    while (symbol != (GList*)NULL) {

      if (g_list_find(plot->p2dYSymbolList,symbol->data) == (GList*)NULL) {

	plot->p2dYSymbolList = g_list_append(plot->p2dYSymbolList,
					     symbol->data);

      }

      symbol = g_list_next(symbol);

    }

    /*
     * Sort symbols by name.
     */
    plot->p2dYSymbolList = g_list_sort(plot->p2dYSymbolList,
				       gdisp_sortSymbolByName);

  }

  /*
   * Refresh graphic area only if back buffer exists.
   */
  if (plot->p2dBackBuffer != (GdkPixmap*)NULL) {

    gdisp_plot2DDrawBackBuffer(kernel,plot);
    gdisp_plot2DSwapBuffers   (kernel,plot);

  }

}


/*
 * Broadcast all symbols.
 */
static GList*
gdisp_getSymbolsFromPlot2D (Kernel_T *kernel,
			    void     *data,
			    gchar     axis)
{

  Plot2D_T *plot = (Plot2D_T*)data;

#if defined(DEBUG_2D)
  fprintf(stdout,"Give back the list of symbols handled by the plot 2D.\n");
  fflush (stdout);
#endif

  switch (axis) {

  case 'X' :
    return plot->p2dXSymbolList;
    break;

  case 'Y' :
    return plot->p2dYSymbolList;
    break;

  default :
    return (GList*)NULL;
    break;

  }

}


/*
 * Real time Step Action.
 */
static void
gdisp_stepOnPlot2D (Kernel_T *kernel,
		    void     *data)
{

  /*
   * Nothing to be done on 2D plot.
   */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_initPlot2DSystem (Kernel_T     *kernel,
			PlotSystem_T *plotSystem)
{

  /*
   * We must here provide all 'Plot2D' private functions
   * that remain 'static' here, but accessible from everywhere
   * via the kernel.
   */
  plotSystem->psCreate            = gdisp_createPlot2D;
  plotSystem->psDestroy           = gdisp_destroyPlot2D;
  plotSystem->psSetParent         = gdisp_setPlot2DParent;
  plotSystem->psGetTopLevelWidget = gdisp_getPlot2DTopLevelWidget;
  plotSystem->psShow              = gdisp_showPlot2D;
  plotSystem->psGetType           = gdisp_getPlot2DType;
  plotSystem->psAddSymbols        = gdisp_addSymbolsToPlot2D;
  plotSystem->psGetSymbols        = gdisp_getSymbolsFromPlot2D;
  plotSystem->psSetDimensions     = gdisp_setPlot2DInitialDimensions;
  plotSystem->psStep              = gdisp_stepOnPlot2D;

}

