/*!  \file 

$Id: gdisp_plot2D.c,v 1.2 2004-02-18 09:47:44 dufy Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi, Yves DUFRENNE.

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
#include <math.h>


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
                             TODO : Hard coded VALUES 
 --------------------------------------------------------------------
*/
#define GDISP_2D_MARGIN_RATIO   0.1 /* We anticipate the next 10% of futur */
#define GDISP_2D_MAX_TITLE     50   /* Nb char in title  */


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static int gdisp_verbosity=1;
#if defined(DEBUG_2D)	
#  define GDISP_TRACE(level,txt)\
	if (level<=gdisp_verbosity) fprintf(stderr,txt); 
#else
#  define GDISP_TRACE(level,txt) 	/* nothing to do */
#endif

#define EVENT_METHOD(widget,event) \
        GTK_WIDGET_CLASS(GTK_OBJECT(widget)->klass)->event

/*
 * SAMPLE is in physical unit,
 * PLOT is in logical pixel, (0,0) is bottom left,
 * WIN is in pixel with "good" GTK direction, (0,0) is upper left.
 */

#define X_WIN_TO_PLOT(plot,x) (x)
#define Y_WIN_TO_PLOT(plot,y) ((plot)->p2dAreaHeight - (y))

#define X_PLOT_TO_WIN(plot,x) (x)
#define Y_PLOT_TO_WIN(plot,y) ((plot)->p2dAreaHeight - (y))

#define Y_SYMBOL_OFFSET 15

#define X_SAMPLE_TO_PLOT(plot,xxx) ((xxx-(plot)->p2dPtMin.x)*(plot)->p2dPtSlope.x) 
#define Y_SAMPLE_TO_PLOT(plot,yyy) ((yyy-(plot)->p2dPtMin.y)*(plot)->p2dPtSlope.y) 

#define X_SAMPLE_TO_WIN(plot,x) (X_PLOT_TO_WIN(plot,X_SAMPLE_TO_PLOT(plot,x)))
#define Y_SAMPLE_TO_WIN(plot,y) (Y_PLOT_TO_WIN(plot,Y_SAMPLE_TO_PLOT(plot,y)))


/*
 * Loop over all elements of the list.
 * Each element is a TSP symbol the 'sReference' parameter of
 * which must be decremented.
 */
static void
gdisp_dereferenceSymbolList ( GList *symbolList )
{

  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop over all elements of the list.
   * Do not forget to decrement the 'sReference' of each symbol.
   */
  symbolItem = g_list_first(symbolList);

  while (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;

    symbol->sReference--;

    symbolItem = g_list_next(symbolItem);

  }

  g_list_free(symbolList);

}


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
gdisp_plot2DSwapBuffers (Kernel_T       *kernel,
			 Plot2D_T       *plot,
			 KindOfRedraw_T  drawType)
{
  GDISP_TRACE(3,"Swaping front and back buffers\n");

  /*
   * Take care of draw type.
   */
  if (drawType == GD_2D_ADD_NEW_SAMPLES) {

    /*
     * Should only repaint the area of buffer
     * that has been updated => OverPowerfull. Thanks Duf.
     */
    gdk_draw_pixmap(plot->p2dArea->window,
		    plot->p2dGContext,
		    plot->p2dBackBuffer,
		    plot->p2dPtRedrawMin.x,
		    plot->p2dPtRedrawMin.y,
		    plot->p2dPtRedrawMin.x,
		    plot->p2dPtRedrawMin.y,
		    plot->p2dPtRedrawMax.x - plot->p2dPtRedrawMin.x + 1,
		    plot->p2dPtRedrawMax.y - plot->p2dPtRedrawMin.y + 1);

  }
  else {

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

}


/*
 * Redraw back buffer content. 2 parts :
 *
 * - gdisp_plot2DDrawBackBufferBackground : 
 *	Only for background earsing, titles and Axis.
 *	For X_SCROLL, use 'gdk_draw_pixmap' for translating 90% of bitmap.
 *
 * - gdisp_plot2DDrawBackBufferCurves
 *	Plot the lines 
 *
 */
static void
gdisp_plot2DDrawBackBufferBackground (Kernel_T       *kernel,
				      Plot2D_T       *plot,
				      KindOfRedraw_T  drawType)
{

#undef WANT_TITLES

  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;
  guint     xPosition  =               0;
  guint     yPosition  =               0;
  gint      lBearing   =               0;
  gint      rBearing   =               0;
  gint      width      =               0;
  gint      ascent     =               0;
  gint      descent    =               0;
#if defined(WANT_TITLES)
  gchar     title   [GDISP_2D_MAX_TITLE];
  gchar     subTitle[GDISP_2D_MAX_TITLE];
#endif


  /*
   * We never enter this routine with 'drawType' GD_2D_ADD_NEW_SAMPLES.
   */

  /*
   * Take care of the type of the drawing process.
   */
  if (drawType == GD_2D_FULL_REDRAW) {

    GDISP_TRACE(2,"Drawing into back buffer -> background : FULL_REDRAW\n");

    gdk_gc_set_foreground(plot->p2dGContext,
			  &kernel->colors[_BLACK_]);

    gdk_draw_rectangle(plot->p2dBackBuffer,
		       plot->p2dGContext,
		       TRUE, /* rectangle is filled */
		       0,
		       0,
		       plot->p2dArea->allocation.width,
		       plot->p2dArea->allocation.height);

  } /* GD_2D_FULL_REDRAW */


  /*
   * We need to scroll the X axis.
   */
  else if (drawType == GD_2D_SCROLL_X_AXIS) {

    GDISP_TRACE(2,"Drawing into back buffer -> background : SCROLL_X\n");
    /*
     * Scroll the 90% of the backbuffer to the left.
     */
    gdk_draw_pixmap(plot->p2dBackBuffer, /* destination */
		    plot->p2dGContext,
		    plot->p2dBackBuffer, /* source      */
		    (gint)ceil(plot->p2dArea->allocation.width *
                                                     GDISP_2D_MARGIN_RATIO),
		    0,
		    0,
		    0,
		    (gint)ceil(plot->p2dArea->allocation.width *
                                               (1 - GDISP_2D_MARGIN_RATIO)), 
		    plot->p2dArea->allocation.height);

    /*
     * Erase the 10% left on the right.
     */
    gdk_gc_set_foreground(plot->p2dGContext,
			  &kernel->colors[_BLACK_]);

    gdk_draw_rectangle(plot->p2dBackBuffer,
		       plot->p2dGContext,
		       TRUE, /* rectangle is filled */
		       (gint)floor(plot->p2dArea->allocation.width *
                                                (1 - GDISP_2D_MARGIN_RATIO)), 
		       0,
		       (gint)ceil(plot->p2dArea->allocation.width *
                                                      GDISP_2D_MARGIN_RATIO), 
		       plot->p2dArea->allocation.height);

  }
  else /* GD_2D_ADD_NEW_SAMPLES */ {

    GDISP_TRACE(2,"Drawing into back buffer -> background : ADD_NEW_SAMPLES\n");

    return;

  }


  /*
   * Title
   */
#if defined(WANT_TITLES)

  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[_WHITE_]);

  snprintf(title,
	   GDISP_2D_MAX_TITLE,
	   "Y=Sin(t) : {%g,%g}", 
	   plot->p2dPtLast.x,
	   plot->p2dPtLast.y);

  gdk_string_extents(plot->p2dFont,
		     title,
		     &lBearing,
		     &rBearing,
		     &width,
		     &ascent,
		     &descent);

  gdk_draw_string(plot->p2dBackBuffer,
		  plot->p2dFont,
		  plot->p2dGContext,
		  (plot->p2dArea->allocation.width - width) / 2,
		  Y_SYMBOL_OFFSET,
		  title);
      
  snprintf(subTitle,
	   GDISP_2D_MAX_TITLE,
	   "Bornes X={%g<%g} Y={%g<%g}", 
	   plot->p2dPtMin.x,
	   plot->p2dPtMax.x, 
	   plot->p2dPtMin.y,
	   plot->p2dPtMax.y);
      
  gdk_string_extents(plot->p2dFont,
		     subTitle,
		     &lBearing,
		     &rBearing,
		     &width,
		     &ascent,
		     &descent);

  gdk_draw_string(plot->p2dBackBuffer,
		  plot->p2dFont,
		  plot->p2dGContext,
		  (plot->p2dArea->allocation.width - width) / 2,
		  Y_SYMBOL_OFFSET * 2,
		  subTitle);

#endif
      

  /*
   * TSP Symbol attached to the X axis.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[_WHITE_]);

  xPosition = plot->p2dAreaWidth  - Y_SYMBOL_OFFSET;
  yPosition = plot->p2dAreaHeight - Y_SYMBOL_OFFSET;

  if (plot->p2dXSymbolList != (GList*)NULL) {

    symbol = (Symbol_T*)plot->p2dXSymbolList->data;

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
   * TSP Symbol list attached to the Y axis.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[_WHITE_]);

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
   * X & Y axis lines around the graphic area.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[_GREY_]);


  /* Axis on X=0 */
  gdk_draw_line(plot->p2dBackBuffer,
		plot->p2dGContext,
		X_SAMPLE_TO_WIN(plot,0.0),                /* x1 */
		Y_SAMPLE_TO_WIN(plot,plot->p2dPtMin.y),   /* y1 */
		X_SAMPLE_TO_WIN(plot,0.0),                /* x2 */
		Y_SAMPLE_TO_WIN(plot,plot->p2dPtMax.y) ); /* y2 */
      
  /* Axis on Y=0 */
  gdk_draw_line(plot->p2dBackBuffer,
		plot->p2dGContext,
		X_SAMPLE_TO_WIN(plot,plot->p2dPtMin.x),   /* x1 */
		Y_SAMPLE_TO_WIN(plot,0.0),                /* y1 */
		X_SAMPLE_TO_WIN(plot,plot->p2dPtMax.x),   /* x2 */
		Y_SAMPLE_TO_WIN(plot,0.0) );              /* y2 */


  /*
   * Black lines around the graphic area.
   */
  gdk_gc_set_foreground(plot->p2dGContext,
			&kernel->colors[_BLACK_]);

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


  /*
   * Axis
   * FIXME : This piece of code must be put elsewhere,
   * but NOT here while drawing into the back-buffer.
   */
  if (plot->p2dPtMin.x != plot->p2dPtMax.x) {

    gtk_ruler_set_range(GTK_RULER(plot->p2dHRuler),
			plot->p2dPtMin.x,
			plot->p2dPtMax.x,
			plot->p2dPtMin.x,
			GDISP_WIN_T_DURATION);

  }

  if (plot->p2dPtMin.y != plot->p2dPtMax.y) {

    gtk_ruler_set_range(GTK_RULER(plot->p2dVRuler),
			-plot->p2dPtMax.y,
			-plot->p2dPtMin.y,
			0,
			plot->p2dPtMax.y - plot->p2dPtMin.y);

  }
      
}


static void
gdisp_plot2DDrawBackBufferCurves (Kernel_T       *kernel,
				  Plot2D_T       *plot,
				  KindOfRedraw_T  drawType)
{

  guint               cptDraw    =                         0;
  guint               cptPoint   =                         0;
  DoublePointArray_T *pArray     = (DoublePointArray_T*)NULL;
  DoublePoint_T      *pSample    =      (DoublePoint_T*)NULL;
  guint               startIndex =                         0;
  guint               lastIndex  =                         0;
  guint               nbPoints   =                         0;
  ShortPoint_T        lastPixel;
  ShortPoint_T        currentPixel;

  GDISP_TRACE(3,"Drawing into back buffer -> curves\n");

  /*
   * Compute bounding box to be redrawn.
   * Inversion for Y minimum and maximum, cause (0,0) is (up,left).
   */
  plot->p2dPtRedrawMin.x = X_SAMPLE_TO_WIN(plot,plot->p2dPtMax.x);
  plot->p2dPtRedrawMax.x = X_SAMPLE_TO_WIN(plot,plot->p2dPtMin.x);
  plot->p2dPtRedrawMin.y = Y_SAMPLE_TO_WIN(plot,plot->p2dPtMin.y);
  plot->p2dPtRedrawMax.y = Y_SAMPLE_TO_WIN(plot,plot->p2dPtMax.y);


  /*
   * Plot the sample lines
   */
  for (cptDraw=0; cptDraw<plot->p2dNbDraws; cptDraw++) {

    pArray = plot->p2dSampleArray[cptDraw]; 

    if (drawType == GD_2D_FULL_REDRAW) {

      /*
       * Plot all for GDISP_2D_FULL_REDRAW & GDISP_2D_SCROLL_X_AXIS.
       */
      startIndex = dparray_getFirstIndex(pArray);
      nbPoints   = dparray_getNbSamples (pArray);

    }
    else {

      /*
       * GDISP_2D_ADD_NEW_SAMPLES => restart from where we stopped earlier.
       */
      startIndex = dparray_getMarkerIndex(pArray);
      nbPoints = dparray_getLeftSamplesFromPos(pArray,
					       startIndex);

    }
    lastIndex = startIndex;
      

    /*
     * TODO : A color per draw ?
     */
    gdk_gc_set_foreground(plot->p2dGContext,
			  &kernel->colors[_GREEN_ + cptDraw * 7]);
      
    for (cptPoint=startIndex; cptPoint<startIndex+nbPoints; cptPoint++) {

      lastIndex = cptPoint;

      pSample        = DP_ARRAY_GET_SAMPLE_PTR(pArray,cptPoint);
      currentPixel.x = X_SAMPLE_TO_WIN(plot,pSample->x);
      currentPixel.y = Y_SAMPLE_TO_WIN(plot,pSample->y);

      /*
       * Store the area of backbuffer
       * that we'll need to redraw on front buffer.
       */
      if (currentPixel.x < plot->p2dPtRedrawMin.x) {
	plot->p2dPtRedrawMin.x = currentPixel.x;
      }
      if (currentPixel.y < plot->p2dPtRedrawMin.y) {
	plot->p2dPtRedrawMin.y = currentPixel.y;
      }
      if (currentPixel.x > plot->p2dPtRedrawMax.x) {
	plot->p2dPtRedrawMax.x = currentPixel.x;
      }
      if (currentPixel.y > plot->p2dPtRedrawMax.y) {
	plot->p2dPtRedrawMax.y = currentPixel.y;
      }

      if ((cptPoint > startIndex ) &&
	  (lastPixel.x != currentPixel.x || lastPixel.y != currentPixel.y)) {

	gdk_draw_line(plot->p2dBackBuffer,
		      plot->p2dGContext,
		      lastPixel.x,
		      lastPixel.y,
		      currentPixel.x,
		      currentPixel.y);

      }

      lastPixel = currentPixel;

    } /* end of loop for drawing on curve */

    /*
     * Mark the last plotted position, for next drawBackBuffer optimisation.
     */
    dparray_setMarkerIndex(pArray,
			   lastIndex - 1);

  } /* end of loop on every curves */

}


/*
 * Redraw back buffer content.
 */
static void
gdisp_plot2DDrawBackBuffer (Kernel_T       *kernel,
			    Plot2D_T       *plot,
			    KindOfRedraw_T  drawType)
{
  GDISP_TRACE(3,"Drawing into back buffer\n");

  /*
   * Do not redraw background when simply adding new samples.
   */
  if (drawType != GD_2D_ADD_NEW_SAMPLES) {
    plot->p2dPtSlope.x = 	    plot->p2dAreaWidth / (plot->p2dPtMax.x-plot->p2dPtMin.x);
    plot->p2dPtSlope.y = 	    plot->p2dAreaHeight/ (plot->p2dPtMax.y-plot->p2dPtMin.y);
    gdisp_plot2DDrawBackBufferBackground(kernel,
					 plot,
					 drawType);
  }

  /*
   * Always plot the new lines for every curve.
   */
  gdisp_plot2DDrawBackBufferCurves(kernel,
				   plot,
				   drawType);

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

  GDISP_TRACE(3,"Expose event\n");

  /*
   * Graphic area has now to be repainted.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  gdk_gc_set_clip_rectangle(plot->p2dGContext,
			    &event->area);

  gdisp_plot2DSwapBuffers(kernel,
			  plot,
			  GD_2D_FULL_REDRAW);

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

  Kernel_T *kernel = (Kernel_T*)data;
  Plot2D_T *plot   = (Plot2D_T*)NULL;

  GDISP_TRACE(1,"Configure event\n");

  /*
   * Get plot private data.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");


  /*
   * Don't know why, but GTK generates a 'configure' event when
   * showing widgets before returning into the main X loop. Strange.
   */
  if (event->width == 1 && event->height == 1) {

    GDISP_TRACE(3,"Configure event returns suddendly.\n");

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
  GDISP_TRACE(3,"Creating back buffer.\n");

  plot->p2dBackBuffer = gdk_pixmap_new(plot->p2dArea->window,
				       plot->p2dAreaWidth,
				       plot->p2dAreaHeight,
				       -1 /* same as window */);

  assert(plot->p2dBackBuffer);


  /*
   * The configure operation is done after a resize request.
   * So refresh back buffer.
   */
  gdisp_plot2DDrawBackBuffer(kernel,
			     plot,
			     GD_2D_FULL_REDRAW);

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

  Plot2D_T *plot = (Plot2D_T*)NULL;

  GDISP_TRACE(2,"Enter Notify Event\n");

  /*
   * Graphic area has now the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  plot->p2dHasFocus = TRUE;

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

  Plot2D_T *plot = (Plot2D_T*)NULL;

  GDISP_TRACE(2,"Leave Notify Event.\n");

  /*
   * Graphic area has lost the focus.
   */
  plot = (Plot2D_T*)gtk_object_get_data(GTK_OBJECT(area),
					"plotPointer");

  plot->p2dHasFocus = FALSE;

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

  GDISP_TRACE(3,"Key press event.\n");

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
    /*
     * Do not forget to decrement the reference of the Y symbol.
     */
    if (plot->p2dXSymbolList != (GList*)NULL) {
      gdisp_dereferenceSymbolList(plot->p2dXSymbolList);
    }
    if (plot->p2dYSymbolList != (GList*)NULL) {
      gdisp_dereferenceSymbolList(plot->p2dYSymbolList);
    }
    plot->p2dXSymbolList    = (GList*)NULL;
    plot->p2dYSymbolList    = (GList*)NULL;
    plot->p2dSelectedSymbol = (GList*)NULL;
    break;

  case GDK_d :
  case GDK_D :
    if (plot->p2dSelectedSymbol != (GList*)NULL) {
      plot->p2dYSymbolList = g_list_remove_link(plot->p2dYSymbolList,
						plot->p2dSelectedSymbol);
      /*
       * Do not forget to decrement the reference of the Y symbol.
       */
      gdisp_dereferenceSymbolList(plot->p2dSelectedSymbol);
      plot->p2dSelectedSymbol = (GList*)NULL;
    }
    break;

  default :
    break;

  }

  /*
   * Refresh graphic area.
   */
  gdisp_plot2DDrawBackBuffer(kernel,
			     plot,
			     GD_2DFULL_REDRAW);

  gdisp_plot2DSwapBuffers   (kernel,
			     plot,
			     GD_2D_FULL_REDRAW);

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

  GDISP_TRACE(3,"Button press event.\n");

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

    /*
     * Do not forget to decrement the reference of the Y symbol.
     */
    if (plot->p2dXSymbolList != (GList*)NULL) {
      gdisp_dereferenceSymbolList(plot->p2dXSymbolList);
    }
    if (plot->p2dYSymbolList != (GList*)NULL) {
      gdisp_dereferenceSymbolList(plot->p2dYSymbolList);
    }
    plot->p2dXSymbolList    = (GList*)NULL;
    plot->p2dYSymbolList    = (GList*)NULL;
    plot->p2dSelectedSymbol = (GList*)NULL;

  }
  else {

    if (plot->p2dSelectedSymbol != (GList*)NULL) {

      plot->p2dYSymbolList = g_list_remove_link(plot->p2dYSymbolList,
						plot->p2dSelectedSymbol);

      /*
       * Do not forget to decrement the reference of the Y symbol.
       */
      gdisp_dereferenceSymbolList(plot->p2dSelectedSymbol);
      plot->p2dSelectedSymbol = (GList*)NULL;

    }

  }

  /*
   * Refresh graphic area.
   */
  gdisp_plot2DDrawBackBuffer(kernel,
			     plot,
			     GD_2D_FULL_REDRAW);

  gdisp_plot2DSwapBuffers   (kernel,
			     plot,
			     GD_2D_FULL_REDRAW);

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

  GDISP_TRACE(3,"Motion notify event.\n");

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

	gdisp_plot2DDrawBackBuffer(kernel,
				   plot,
				   GD_2D_FULL_REDRAW);

	gdisp_plot2DSwapBuffers   (kernel,
				   plot,
				   GD_2D_FULL_REDRAW);

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

    gdisp_plot2DDrawBackBuffer(kernel,
			       plot,
			       GD_2D_FULL_REDRAW);

    gdisp_plot2DSwapBuffers   (kernel,
			       plot,
			       GD_2D_FULL_REDRAW);

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

  Plot2D_T *plot      = (Plot2D_T*)NULL;
  guint     sArrayCpt = 0;

  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(Plot2D_T));
  assert(plot);

  /*
   * Few initialisations.
   */
  plot->p2dHasFocus = FALSE;
  plot->p2dType     = GD_PLOT_2D;
  plot->p2dSubType  = GD_2D_F2T;

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
   * Allocate data for sampled points.
   */
  /* FIXME : Need to know how many draw curves you have got */
  plot->p2dNbDraws     = 3; 
  plot->p2dSampleArray = (DoublePointArray_T**)
    g_malloc0(plot->p2dNbDraws * sizeof(DoublePointArray_T*));

  for (sArrayCpt=0; sArrayCpt<plot->p2dNbDraws; sArrayCpt++) {

    plot->p2dSampleArray[sArrayCpt] =
      dparray_newSampleArray(TSP_PROVIDER_FREQ * GDISP_WIN_T_DURATION);

  }


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

  Plot2D_T *plot      = (Plot2D_T*)data;
  guint     sArrayCpt = 0;

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
   * Free data for sampled points.
   */
  for (sArrayCpt=0; sArrayCpt<plot->p2dNbDraws; sArrayCpt++) {

    dparray_freeSampleArray(plot->p2dSampleArray[sArrayCpt]);

  }
  g_free(plot->p2dSampleArray);

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

  GDISP_TRACE(3,"Getting back plot 2D top level widget.\n");

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

  GDISP_TRACE(3,"Showing plot 2D.\n");

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

  GDISP_TRACE(3,"Getting back plot type (2D).\n");

  /*
   * Must be GD_PLOT_2D_T. See 'create' routine.
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
			  guint     xDrop, /* plot coordinates */
			  guint     yDrop  /* plot coordinates */ )
{

  Plot2D_T *plot       = (Plot2D_T*)data;
  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  GDISP_TRACE(3,"Adding symbols to plot 2D.\n");

  /*
   * If drop coordinates are in the X zone, the symbol has to be
   * attached to the X axis.
   * CAUTION : we bet the X will be monotonic increasing 
   * try plot y=f(t). Realtime will warn us if not
   */
  if (gdisp_isInsideXZone(plot,
			  xDrop,
			  yDrop) == TRUE) {

    plot->p2dSubType = GD_2D_F2T;
    /*
     * Do not forget to decrement the reference of the X symbol.
     */
    gdisp_dereferenceSymbolList(plot->p2dXSymbolList);
    plot->p2dXSymbolList = (GList*)NULL;

    /*
     * Only one symbol on X axis.
     * Take the first symbol of the incoming list.
     */
    symbolItem = g_list_first(symbolList);
    if (symbolItem != (GList*)NULL) {

      plot->p2dXSymbolList = g_list_append(plot->p2dXSymbolList,
					   symbolItem->data);

      symbol = (Symbol_T*)symbolItem->data;
      symbol->sReference++;

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
    symbolItem = g_list_first(symbolList);
    while (symbolItem != (GList*)NULL) {

      if (g_list_find(plot->p2dYSymbolList,symbolItem->data) == (GList*)NULL) {

	plot->p2dYSymbolList = g_list_append(plot->p2dYSymbolList,
					     symbolItem->data);

	/*
	 * Do not forget to increment the reference of the Y symbol.
	 */
	symbol = (Symbol_T*)symbolItem->data;
	symbol->sReference++;

      }

      symbolItem = g_list_next(symbolItem);

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

    gdisp_plot2DDrawBackBuffer(kernel,
			       plot,
			       GD_2D_FULL_REDRAW);

    gdisp_plot2DSwapBuffers   (kernel,
			       plot,
			       GD_2D_FULL_REDRAW);

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

  GDISP_TRACE(3,"Give back the list of symbols handled by the plot 2D.\n");

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

  Plot2D_T *plot = (Plot2D_T*)data;

  /*
   * TODO :
   * I'm not quite sure how I will get the new data, but will see later ...
   */
  static
  gdouble        myTime    = 0.0;
  guint          cptDraw   = 0;
  guint          nbSamples = 0;
  KindOfRedraw_T drawType  = GD_2D_ADD_NEW_SAMPLES;
  DoublePoint_T  aPoint, *pLastPoint;
  guint		 n;

  /*
   * Guess we got new TSP values at each refresh cycles.
   */
  for (nbSamples=0;
       nbSamples<TSP_PROVIDER_FREQ/GDISP_REFRESH_FREQ; nbSamples++) {
    Symbol_T *symbol = (Symbol_T*)plot->p2dXSymbolList->data;  

    for (cptDraw=0; cptDraw<plot->p2dNbDraws; cptDraw++) {

      gdouble t = myTime / TSP_PROVIDER_FREQ * (cptDraw + 1) * 5;

      if (strcmp(symbol->sInfo.name,   "t") == 0 ||
	  strcmp(symbol->sInfo.name,   "T") == 0 ||
	  strcmp(symbol->sInfo.name,"time") == 0 ||
	  strcmp(symbol->sInfo.name,"Time") == 0 ||
	  strcmp(symbol->sInfo.name,"TIME") == 0    ) {

	aPoint.x = myTime;
	aPoint.y = 10.0 / (cptDraw + 1)    *
	  (sin(t) + pow(cos(t),cptDraw+1)) *
	  (1 + t / 500.0 * (1 + (gdouble)rand() / RAND_MAX));

      }
      else {

	aPoint.x = sin(myTime *(cptDraw +1) / 3.0) * myTime / (cptDraw + 1);
	aPoint.y = cos(myTime /(cptDraw +1)      ) * myTime * (cptDraw + 1);

      }
 
      dparray_addSample(plot->p2dSampleArray[cptDraw],
			&aPoint);

      /* Check if plot2D is still monotonic increasing on X */
      n = dparray_getNbSamples (plot->p2dSampleArray[cptDraw]);

      if ( (n > 1) && (plot->p2dSubType==GD_2D_F2T) ) {
	// Take the last before the one we just add
	pLastPoint = DP_ARRAY_GET_SAMPLE_PTR(plot->p2dSampleArray[cptDraw], n-2); 
	if (aPoint.x < pLastPoint->x) {
	  // FIXME : What to do if X increase like a exponentiel ?
	  plot->p2dSubType = GD_2D_F2X;
	  drawType         = GD_2D_FULL_REDRAW;
	  GDISP_TRACE(1,"gdisp_stepOnPlot2D : changing type to F2X\n");
	}
      }

      if (myTime == 0.0 && cptDraw == 0) {

	/*
	 * First time to initialize minimum and maximum.
	 */
	plot->p2dPtMin = aPoint;
	plot->p2dPtMax = aPoint;

      }
	  
      plot->p2dPtLast = aPoint;

      /*
       * I Could do this check later, but might be speeder on the fly.
       */
      if (aPoint.y < plot->p2dPtMin.y) {

	plot->p2dPtMin.y = aPoint.y * (1 + GDISP_2D_MARGIN_RATIO);
	drawType         = GD_2D_FULL_REDRAW;

      }
      if (aPoint.y > plot->p2dPtMax.y) {

	plot->p2dPtMax.y = aPoint.y * (1 + GDISP_2D_MARGIN_RATIO);
	drawType         = GD_2D_FULL_REDRAW;

      }

      switch (plot->p2dSubType) {

      case GD_2D_F2T :

	if (aPoint.x > plot->p2dPtMax.x) {

	  plot->p2dPtMax.x = aPoint.x +
	                     GDISP_WIN_T_DURATION * GDISP_2D_MARGIN_RATIO; 
	  drawType         = GD_2D_FULL_REDRAW;

	}
	if (plot->p2dPtMax.x - plot->p2dPtMin.x > GDISP_WIN_T_DURATION) {

	  plot->p2dPtMin.x = plot->p2dPtMax.x - GDISP_WIN_T_DURATION;
	  drawType         = GD_2D_SCROLL_X_AXIS;

	}	    
	break;
 
      case GD_2D_F2X :

	if (aPoint.x < plot->p2dPtMin.x) {

	  plot->p2dPtMin.x = aPoint.x * (1 + GDISP_2D_MARGIN_RATIO);
	  drawType         = GD_2D_FULL_REDRAW;

	}
	if (aPoint.x > plot->p2dPtMax.x) {

	  plot->p2dPtMax.x = aPoint.x * (1 + GDISP_2D_MARGIN_RATIO);
	  drawType         = GD_2D_FULL_REDRAW;

	}
	break;

      default : 
	break;

      }

    } /* end loop on nbDraws */
	  
    myTime += 1.0 / (gdouble)TSP_PROVIDER_FREQ; // 100Hz ?

  } // end on loop for adding datas
  
  /*
   * Put here what must be done at each step.
   */
  gdisp_plot2DDrawBackBuffer(kernel,
			     plot,
			     drawType);

  gdisp_plot2DSwapBuffers   (kernel,
			     plot,
			     drawType);

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getPlot2DInformation (Kernel_T         *kernel,
			    PlotSystemInfo_T *information)
{

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "2D Plot";
  information->psFormula     = "Y = F ( X or T )";
  information->psDescription = "A typical 2D plot that shows the evolution"
    "of several symbols (Y axis) relatively to a single one (X axis).";

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
  plotSystem->psGetInformation    = gdisp_getPlot2DInformation;

  if (getenv("GDISP_STRACE")!=NULL)
    gdisp_verbosity = atoi(getenv("GDISP_STRACE"));
  else
    gdisp_verbosity = 1;
    
}


