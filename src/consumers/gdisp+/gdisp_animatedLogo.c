/*

$Id: gdisp_animatedLogo.c,v 1.4 2006-07-31 19:59:07 esteban Exp $

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

File      : Animated Logo.

-----------------------------------------------------------------------
*/


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


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Clear logo drawing area.
 */
static void
gdisp_clearLogoArea ( Kernel_T *kernel )
{

  if (kernel->widgets.mainBoardLogoArea->window != (GdkWindow*)NULL) {
    gdk_window_clear(kernel->widgets.mainBoardLogoArea->window);
  }

}


/*
 * Draw the current logo into the graphic area.
 */
static gboolean
gdisp_animateSmallLogo ( Kernel_T *kernel )
{

  Pixmap_T *pixmap     = (Pixmap_T*)NULL;
  guint     areaWidth  = kernel->widgets.mainBoardLogoArea->allocation.width;
  guint     areaHeight = kernel->widgets.mainBoardLogoArea->allocation.height;

  /*
   * Pixmap to be drawn.
   */
  pixmap = *kernel->widgets.mainBoardCurrentLogo;

  gdk_draw_pixmap(kernel->widgets.mainBoardLogoArea->window,
		  kernel->widgets.mainBoardLogoContext,
		  pixmap->pixmap,
		  0,
		  0,
		  (areaWidth  - pixmap->width ) / 2,
		  (areaHeight - pixmap->height) / 2,
		  pixmap->width,
		  pixmap->height);

  kernel->widgets.mainBoardCurrentLogo++;

  if (kernel->widgets.mainBoardCurrentLogo ==
      kernel->widgets.mainBoardLogoTable + GD_ANIMATED_LOGO_NB) {

    kernel->widgets.mainBoardCurrentLogo = kernel->widgets.mainBoardLogoTable;

  }

  return TRUE; /* keep on */

}


/*
 * Animate large logo while sampling is off.
 */
static gboolean
gdisp_animateLargeLogo ( Kernel_T *kernel)
{

#define GD_LOGO_X_OFFSET 4
  guint     areaWidth  = kernel->widgets.mainBoardLogoArea->allocation.width;
  guint     areaHeight = kernel->widgets.mainBoardLogoArea->allocation.height;
  Pixmap_T *pixmap     = kernel->widgets.mainBoardLargeLogo;
  gboolean  keepOn     = TRUE;

  /*
   * Draw large pixmap on graphic area.
   */
  gdk_draw_pixmap(kernel->widgets.mainBoardLogoArea->window,
		  kernel->widgets.mainBoardLogoContext,
		  pixmap->pixmap,
		  kernel->widgets.mainBoardLargeLogoX,
		  0,
		  0,
		  0,
		  areaWidth,
		  areaHeight);

  /*
   * Logo is going to the right.
   */
  if (kernel->widgets.mainBoardLargeLogoGoesLeft == FALSE) {

    kernel->widgets.mainBoardLargeLogoX += GD_LOGO_X_OFFSET;

    if (kernel->widgets.mainBoardLargeLogoX >= (pixmap->width - areaWidth)) {
      kernel->widgets.mainBoardLargeLogoX        = pixmap->width - areaWidth;
      kernel->widgets.mainBoardLargeLogoGoesLeft = TRUE;
      /* stop animation for a while */
      keepOn = FALSE;
    }

  }

  /*
   * Logo is going to the left.
   */
  if (kernel->widgets.mainBoardLargeLogoGoesLeft == TRUE) {

    kernel->widgets.mainBoardLargeLogoX -= GD_LOGO_X_OFFSET;

    if (kernel->widgets.mainBoardLargeLogoX <= 0) {
      kernel->widgets.mainBoardLargeLogoX        = 0;
      kernel->widgets.mainBoardLargeLogoGoesLeft = FALSE;
    }

  }

  return keepOn;

}


/*
 * Main animation routine.
 */
static gint
gdisp_animateLogo ( void *data )
{

  Kernel_T *kernel = (Kernel_T*)data;
  gint      keepOn = 1;

  /* sampling on */
  if (kernel->samplingThreadMustExit == FALSE) {
    keepOn = (gint)gdisp_animateSmallLogo(kernel);
  }
  /* sampling off */
  else {
    keepOn = (gint)gdisp_animateLargeLogo(kernel);
  }

  return keepOn;

}


/*
 * Treat 'expose' X event.
 * What shall I do when the time area has to be refreshed ?
 */
static gboolean
gdisp_logoAreaExpose ( GtkWidget       *area,
		       GdkEventExpose  *event,
		       gpointer         data )
{

  Kernel_T *kernel = (Kernel_T*)data;
  gint      keepOn = 0;

  /*
   * Graphic area has now to be repainted.
   */
  gdk_gc_set_clip_rectangle(kernel->widgets.mainBoardLogoContext,
			    &event->area);

  keepOn = gdisp_animateLogo((void*)kernel);

  gdk_gc_set_clip_rectangle(kernel->widgets.mainBoardLogoContext,
			    (GdkRectangle*)NULL);

  return TRUE;

}


/*
 * High Speed Logo Management.
 */
static gint
gdisp_manageHighSpeedLogo ( void *data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Start high speed logo animation.
   */
  kernel->logoHighSpeedTimerIdentity  = gtk_timeout_add(50, /* milli-seconds */
							gdisp_animateLogo,
							(void*)kernel);

  return TRUE; /* keep on */

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ pilot board.
 */
GtkWidget*
gdisp_createAnimatedLogo (Kernel_T *kernel)
{

  GtkWidget *logoArea       = (GtkWidget*)NULL;
  guint      logoAreaWidth  = 0;
  guint      logoAreaHeight = 0;
  GtkWidget *logoFrame      = (GtkWidget*)NULL;
  GtkWidget *logoBox        = (GtkWidget*)NULL;
  GdkGC     *logoContext    = (GdkGC*)NULL;
  guint      logoCpt        = 0;
  guint      logoId[]       = { GD_PIX_animLogo1,
				GD_PIX_animLogo2,
				GD_PIX_animLogo3,
				GD_PIX_animLogo4,
				GD_PIX_animLogo5,
				GD_PIX_animLogo6,
				GD_PIX_animLogo7,
				GD_PIX_animLogo8,
				GD_PIX_animLogo9,
				GD_PIX_animLogo10,
				GD_PIX_animLogo11 };

  assert(kernel);

  /* ------------------------ LOGO FRAME  ------------------------ */

  logoFrame = gtk_frame_new((gchar*)NULL);

  gtk_frame_set_shadow_type(GTK_FRAME(logoFrame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_shadow_type(GTK_FRAME(logoFrame),GTK_SHADOW_IN);

  /* -------------------------- LOGO BOX  -------------------------- */

  logoBox = gtk_vbox_new(FALSE, /* homogeneous */
			 0      /* spacing     */ );

  gtk_container_add(GTK_CONTAINER(logoFrame),logoBox);

  gtk_widget_show(logoBox);

  /* --------------------- LOGO PIXMAP TABLE  --------------------- */

  kernel->widgets.mainBoardLargeLogo =
    gdisp_getPixmapById(kernel,
			GD_PIX_largeLogo,
			kernel->widgets.mainBoardWindow);

  /* use large logo to deduce graphic area height */
  logoAreaHeight = kernel->widgets.mainBoardLargeLogo->height;

  for (logoCpt=0; logoCpt<GD_ANIMATED_LOGO_NB; logoCpt++) {

    kernel->widgets.mainBoardLogoTable[logoCpt] =
      gdisp_getPixmapById(kernel,
			  logoId[logoCpt],
			  kernel->widgets.mainBoardWindow);

  }

  /* use small logo to deduce graphic area width */
  logoAreaWidth  = kernel->widgets.mainBoardLogoTable[0]->width;

  kernel->widgets.mainBoardCurrentLogo = kernel->widgets.mainBoardLogoTable;

  /* --------------------- LOGO GRAPHIC AREA  --------------------- */

  logoArea = gtk_drawing_area_new();
  kernel->widgets.mainBoardLogoArea = logoArea;

  gtk_drawing_area_size(GTK_DRAWING_AREA(logoArea),
			logoAreaWidth,
			logoAreaHeight);

  gtk_signal_connect(GTK_OBJECT(logoArea),
		     "expose_event",
		     (GtkSignalFunc)gdisp_logoAreaExpose,
		     (gpointer)kernel);

  gtk_box_pack_start(GTK_BOX(logoBox),
		     logoArea,
		     TRUE,
		     FALSE,
		     0);

  gtk_widget_show(logoArea);

  /* ------------------- LOGO GRAPHIC CONTEXT  ------------------- */

  logoContext =
    gdk_gc_new(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);

  kernel->widgets.mainBoardLogoContext = logoContext;

  /* ---------------------- INITIALISATIONS ---------------------- */

  kernel->widgets.mainBoardLargeLogoX        = 0;
  kernel->widgets.mainBoardLargeLogoGoesLeft = FALSE;

  /*
   * Return.
   */
  return logoFrame;

}


/*
 * Start logo animation.
 */
void
gdisp_startLogoAnimation ( Kernel_T *kernel )
{

  /* --------------------- UNREGISTER ACTION ------------------- */

  /*
   * Remove LARGE logo animation from timeout management.
   */
  if (kernel->logoTimerIdentity != 0) {

    gtk_timeout_remove(kernel->logoTimerIdentity);
    kernel->logoTimerIdentity = 0;

  }
  if (kernel->logoHighSpeedTimerIdentity != 0) {

    gtk_timeout_remove(kernel->logoHighSpeedTimerIdentity);
    kernel->logoHighSpeedTimerIdentity = 0;

  }

  /* --------------------- REGISTER ACTION ------------------- */

  /*
   * Initialisations
   */
  gdisp_clearLogoArea(kernel);

  /* gdisp_animateLogo((void*)kernel); */ /* refresh once */

  /*
   * Start SMALL logo animation
   */
  kernel->logoTimerIdentity = gtk_timeout_add(100, /* milli-seconds */
					      gdisp_animateLogo,
					      (void*)kernel);

}


/*
 * Stop logo animation.
 */
void
gdisp_stopLogoAnimation ( Kernel_T *kernel,
			  gboolean  stopAll )
{

  gint keepOn = 1;

  /* --------------------- UNREGISTER ACTION ------------------- */

  /*
   * Remove SMALL logo animation from timeout management.
   */
  if (kernel->logoTimerIdentity != 0) {

    gtk_timeout_remove(kernel->logoTimerIdentity);
    kernel->logoTimerIdentity = 0;

  }

  /* --------------------- REGISTER ACTION ------------------- */

  if (stopAll == FALSE) {

    /*
     * Initialisations
     */
    kernel->widgets.mainBoardLargeLogoX        = 0;
    kernel->widgets.mainBoardLargeLogoGoesLeft = FALSE;

    gdisp_clearLogoArea(kernel);

    keepOn = gdisp_manageHighSpeedLogo((void*)kernel);

    /*
     * Start LARGE logo animation
     */
    kernel->logoTimerIdentity = gtk_timeout_add(20000, /* milli-seconds */
						gdisp_manageHighSpeedLogo,
						(void*)kernel);

  }

}
