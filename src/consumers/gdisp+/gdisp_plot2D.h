/*!  \file 

$Id: gdisp_plot2D.h,v 1.6 2004-11-16 09:32:00 dufy Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi, Yves DUFRENNE

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

File      : Definition of 'F2T 2D plot' private structures.

-----------------------------------------------------------------------
*/

#ifndef __2D_PLOT_H__
#define __2D_PLOT_H__

#include "gdisp_pointArray.h"

/*
 * Main principle of Y=f(t) Drawing :
 * There are three kinds of replotting actions :
 * ( GD_2D_FULL_REDRAW, GD_2D_SCROLL_X_AXIS, GD_2D_ADD_NEW_SAMPLES )
 */
typedef enum {

  GD_2D_ADD_NEW_SAMPLES = 0, // Still in X range => draw new added samples
  GD_2D_SCROLL_X_AXIS,       // time increase outside limit => scroll X_AXIS
  GD_2D_FULL_REDRAW          // Everything has changed => need to plot all

} KindOfRedraw_T;

typedef enum {

  GD_2D_F2T = 0,
  GD_2D_F2X,
  GD_2D_OTHER,

} KindOfPlot_T;

/*
 * Private structure of a '2D plot'.
 */
typedef struct Plot2D_T_ {

  /*
   * Attributes.
   */
  PlotType_T           p2dType;
  KindOfPlot_T         p2dSubType;
  gboolean             p2dHasFocus;
  gboolean             p2dIsWorking;
  gboolean             p2dIsFirstTime;

  /*
   * List of pointer on TSP_Symbol_T.
   */
  GList               *p2dXSymbolList;
  GList               *p2dYSymbolList;
  GList               *p2dSelectedSymbol;
  GdkWindow           *p2dXSymbolWindow;
  GdkWindow           *p2dYSymbolWindow;

  /*
   * Graphic widget.
   */
  GtkWidget           *p2dTable;
  GArray              *p2dSignalIdentities;
  gboolean             p2dSignalsAreBlocked;
  GtkWidget           *p2dArea;
  guint                p2dAreaWidth;
  guint                p2dAreaHeight;
  GtkWidget           *p2dHRuler;
  GtkWidget           *p2dVRuler;

  GdkGC               *p2dGContext;
  GdkFont             *p2dFont;
  GdkPixmap           *p2dBackBuffer;

  /*
   * Parent widget.
   */
  GtkWidget           *p2dParent;

  /*
   * Array for sample points.
   * - p2dPtMin       : The minium point in physical unit
   * - p2dPtMax       : The maximum point in physical unit
   * - p2dPtSlope     : The slope used to convert physical2pixel
   * - p2dPtLast      : The last point in physical unit
   * - p2dPtScroll    : The scroll in X or (on day Y) to do on next Redraw
   * - p2dPtRedrawMin : The minium in pixel unit, for redraw (plot back2front)
   * - p2dPtRedrawMax : The maximum in pixel unit, for redraw (plot back2front)
   *
   * GPtrArray : pointer array that grows automatically.
   *  struct GPtrArray {
   *            gpointer *pdata;
   *            guint    len;
   *  };
   * So, 'len' is the number of curves,
   *     'pdata' is the table of 'DoublePointArray_T*'.
   */
  GPtrArray           *p2dSampleArray;
  DoublePoint_T        p2dPtMin;
  DoublePoint_T        p2dPtMax;
  DoublePoint_T	       p2dPtSlope;	
  DoublePoint_T        p2dPtLast;
  DoublePoint_T        p2dPtScroll;
  ShortPoint_T         p2dPtRedrawMin;
  ShortPoint_T         p2dPtRedrawMax;

} Plot2D_T;


#endif /* __2D_PLOT_H__ */
