/*

$Id: gdisp_splashScreen.c,v 1.2 2006-02-26 14:08:24 erk Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi

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

File      : Splash screen management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>

#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 * Type definition.
 */
typedef struct SplashData_T_ {

  GtkWidget *splashWindow;
  GdkPixmap *splashPixmap;
  guint      splashWidth;
  guint      splashHeight;

} SplashData_T;


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#include "pixmaps/gdisp_splashScreen.xpm"


/*
 * Expose event handler.
 */
static void
gdisp_splashExposeHandler ( GtkWidget      *widget,
			    GdkEventExpose *event,
			    gpointer        userData)
{

  SplashData_T *splashData = (SplashData_T*)userData;

  /*
   * Simply draw the entire pixmap into the window.
   */
  gdk_draw_pixmap(widget->window,
		  widget->style->fg_gc[GTK_STATE_NORMAL],
		  splashData->splashPixmap,
		  0,
		  0,
		  0,
		  0,
		  splashData->splashWidth,
		  splashData->splashHeight);

}


/*
 * Destroy event handler.
 */
static gint
gdisp_removeSplashScreen ( gpointer userData )
{

  SplashData_T *splashData = (SplashData_T*)userData;

  /*
   * Simply destroy top-level window.
   */
#if defined(GD_UNREF_THINGS)
  gdk_pixmap_unref(splashData->splashPixmap);
#endif
  gtk_widget_destroy(splashData->splashWindow);

  g_free(splashData);

  return FALSE; /* stop the timer */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_showSplashScreen ( void )
{

  GtkWidget    *splashDrawingArea = (GtkWidget*)NULL;
  GtkStyle     *splashStyle       = (GtkStyle*)NULL;
  GdkBitmap    *splashMask        = (GdkBitmap*)NULL;
  SplashData_T *splashData        = (SplashData_T*)NULL;
  gchar        *sizeString        = (gchar*)NULL;
  guint         splashTimer       = 0;


  /*
   * Allocate memory.
   */
  splashData = (SplashData_T*)g_malloc0(sizeof(SplashData_T));
  if (splashData == (SplashData_T*)NULL) {
    return;
  }

  /*
   * Create top-level window without decoration.
   * Place this window at the center of the screen.
   */
  splashData->splashWindow = gtk_window_new(GTK_WINDOW_POPUP);

  gtk_window_set_position(GTK_WINDOW(splashData->splashWindow),
			  GTK_WIN_POS_CENTER);

  /*
   * Deduce window width and height from information in the pixmap.
   */
  sizeString = gdisp_splashScreen[0];
  sscanf(sizeString,
	 "%d %d",
	 &splashData->splashWidth,
	 &splashData->splashHeight);

  /*
   * Create the drawing area with the correct size.
   */
  splashDrawingArea = gtk_drawing_area_new();

  gtk_drawing_area_size(GTK_DRAWING_AREA(splashDrawingArea),
			splashData->splashWidth,
			splashData->splashHeight);

  gtk_container_add(GTK_CONTAINER(splashData->splashWindow),
		    splashDrawingArea);

  gtk_signal_connect(GTK_OBJECT(splashDrawingArea),
		     "expose-event",
		     GTK_SIGNAL_FUNC(gdisp_splashExposeHandler),
		     (gpointer)splashData);

  gtk_widget_show_all(splashData->splashWindow);

  /*
   * Create the pixmap.
   */
  splashStyle = gtk_widget_get_default_style();

  splashData->splashPixmap =
    gdk_pixmap_create_from_xpm_d(splashData->splashWindow->window,
				 &splashMask, 
				 &splashStyle->bg[GTK_STATE_NORMAL],
				 gdisp_splashScreen);

  /*
   * Make the splash screen appear before entering into the GTK+ main loop.
   */
  gdisp_updateWholeGui();

  /*
   * Arm a time in order to destroy splash screen.
   */
  splashTimer = gtk_timeout_add(5000 /* in milli-seconds, thus 5 seconds */,
				gdisp_removeSplashScreen,
				(gpointer)splashData);

}
