/*!  \file 

$Id: gdisp_pilotBoard.c,v 1.1 2004-03-26 21:09:17 esteban Exp $

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

File      : Graphic Tool Pilot Board.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


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
 * This callback is called whenever play / stop buttons are pressed.
 * The argument "data" is the kernel itself.
 * The only way to determine which button has been pressed, is to
 * compare the argument "widget" (the target button) to both existent
 * button pointers stored into the kernel.
 */
static void
gdisp_togglePlayModeCallback (GtkWidget *buttonWidget,
			      gpointer   data )
{

  Kernel_T *kernel     = (Kernel_T*)data;
  gboolean  hasStarted = FALSE;

  if (buttonWidget == kernel->widgets.mainBoardOkButton) {

    hasStarted = gdisp_startSamplingProcess(kernel);

    if (hasStarted == TRUE) {

      gtk_widget_hide(kernel->widgets.mainBoardOkButton  );
      gtk_widget_show(kernel->widgets.mainBoardStopButton);

    }

  }
  else if (buttonWidget == kernel->widgets.mainBoardStopButton) {

    gdisp_stopSamplingProcess(kernel);

    gtk_widget_hide(kernel->widgets.mainBoardStopButton);
    gtk_widget_show(kernel->widgets.mainBoardOkButton  );

  }

}



/*
 * Graphically show the time. Callback of a GTK timer.
 */
static void
gdisp_showTime ( Kernel_T *kernel )
{

#define GD_DIGIT_WIDTH               16
#define GD_DIGIT_START_X GD_DIGIT_WIDTH
#define GD_2PTS_WIDTH                 9
#define GD_DIGIT_HEIGHT              21

  GdkPixmap      *digitPixmap = (GdkPixmap*)NULL;
  GtkWidget      *timeArea    = (GtkWidget*)NULL;
  GdkGC          *timeContext =     (GdkGC*)NULL;

  struct timeval  localTime;

  guint           seconds     = 0;
  guint           minutes     = 0;
  guint           hours       = 0;
  guint           xPos        = 0;
  guint           yPos        = 0;
  

  /*
   * Get back current time.
   */
  gettimeofday(&localTime,(void*)NULL);

  seconds = localTime.tv_sec % 60;
  localTime.tv_sec -= seconds;
  localTime.tv_sec /= 60;

  minutes = localTime.tv_sec % 60;
  localTime.tv_sec -= minutes;
  localTime.tv_sec /= 60;

  hours   = localTime.tv_sec % 24;


  /*
   * Init.
   */
  digitPixmap = kernel->widgets.pilotBoardDigitPixmap;
  timeArea    = kernel->widgets.pilotBoardTimeArea;
  timeContext = kernel->widgets.pilotBoardTimeContext;


  /*
   * Draw hours.
   */
  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (hours / 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (hours % 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

  /*
   * Draw 2 points.
   */
  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + 12 * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_2PTS_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_2PTS_WIDTH;

  /*
   * Draw minutes.
   */
  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (minutes / 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (minutes % 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

  /*
   * Draw 2 points.
   */
  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + 12 * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_2PTS_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_2PTS_WIDTH;

  /*
   * Draw seconds.
   */
  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (seconds / 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

  gdk_draw_pixmap(timeArea->window,
		  timeContext,
		  digitPixmap,
		  GD_DIGIT_START_X + (seconds % 10) * GD_DIGIT_WIDTH,
		  yPos,
		  xPos,
		  yPos,
		  GD_DIGIT_WIDTH,
		  GD_DIGIT_HEIGHT);

  xPos += GD_DIGIT_WIDTH;

}


/*
 * Treat 'expose' X event.
 * What shall I do when the area has to be refreshed ?
 */
static gboolean
gdisp_timeAreaExpose (GtkWidget       *area,
		      GdkEventExpose  *event,
		      gpointer         data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Graphic area has now to be repainted.
   */
  gdk_gc_set_clip_rectangle(kernel->widgets.pilotBoardTimeContext,
			    &event->area);

  gdisp_showTime(kernel);

  gdk_gc_set_clip_rectangle(kernel->widgets.pilotBoardTimeContext,
			    (GdkRectangle*)NULL);

  return TRUE;

}


/*
 * Include for TSP Logo.
 */
#include "pixmaps/gdisp_stopButton.xpm"
#include "pixmaps/gdisp_okButton.xpm"
#include "pixmaps/gdisp_timeDigits.xpm"


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ pilot board.
 */
GtkWidget*
gdisp_createPilotBoard (Kernel_T *kernel)
{

  GtkWidget *pilotBox     = (GtkWidget*)NULL;
  GtkWidget *timeFrame    = (GtkWidget*)NULL;
  GtkWidget *pixmapWidget = (GtkWidget*)NULL;
  GdkPixmap *pixmap       = (GdkPixmap*)NULL;
  GdkBitmap *mask         = (GdkBitmap*)NULL;
  GtkStyle  *style        =  (GtkStyle*)NULL;
  GtkWidget *timeArea     = (GtkWidget*)NULL;
  GdkGC     *timeContext  =     (GdkGC*)NULL;
  GdkPixmap *digitPixmap  = (GdkPixmap*)NULL;

  assert(kernel);


  /* ----------------------- PILOT BOX ----------------------- */

  /*
   * Create a vertical box.
   */
  pilotBox = gtk_vbox_new(FALSE, /* homogeneous */
			  3      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(pilotBox),2);
  gtk_widget_show(pilotBox);


  /* --------------------- TIME GRAPHIC AREA --------------------- */

  timeFrame = gtk_frame_new((gchar*)NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(timeFrame),GTK_SHADOW_ETCHED_OUT);
  gtk_frame_set_shadow_type(GTK_FRAME(timeFrame),GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(pilotBox),
		     timeFrame,
		     FALSE, /* expand  */
		     FALSE, /* fill    */
		     0);    /* padding */
  gtk_widget_show(timeFrame);

  timeArea = gtk_drawing_area_new();
  kernel->widgets.pilotBoardTimeArea = timeArea;

  gtk_drawing_area_size(GTK_DRAWING_AREA(timeArea),
			6 * GD_DIGIT_WIDTH + 2 * GD_2PTS_WIDTH  /* width  */,
			GD_DIGIT_HEIGHT /* height */);

  timeContext =
    gdk_gc_new(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);

  kernel->widgets.pilotBoardTimeContext = timeContext;

  gtk_signal_connect(GTK_OBJECT(timeArea),
		     "expose_event",
		     (GtkSignalFunc)gdisp_timeAreaExpose,
		     (gpointer)kernel);

  gtk_container_add(GTK_CONTAINER(timeFrame),timeArea);

  style       = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  digitPixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_timeDigits);

  kernel->widgets.pilotBoardDigitPixmap = digitPixmap;

  gtk_widget_show(timeArea);


  /* ----------------------- PLAY BUTTON ----------------------- */

  /*
   * OK button.
   */
  style  = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_okButton);

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  pixmapWidget = gtk_pixmap_new(pixmap,mask);
  gtk_widget_show(pixmapWidget);

  /*
   * Create the button that contains the pixmap.
   */
  kernel->widgets.mainBoardOkButton = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardOkButton),
		    pixmapWidget);
  gtk_box_pack_start(GTK_BOX(pilotBox),
		     kernel->widgets.mainBoardOkButton,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.mainBoardOkButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardOkButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_togglePlayModeCallback),
		     (gpointer)kernel);

  /*
   * STOP button.
   */
  style  = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_stopButton);

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  pixmapWidget = gtk_pixmap_new(pixmap,mask);
  gtk_widget_show(pixmapWidget);

  /*
   * Create the button that contains the pixmap.
   */
  kernel->widgets.mainBoardStopButton = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardStopButton),
		    pixmapWidget);
  gtk_box_pack_start(GTK_BOX(pilotBox),
		     kernel->widgets.mainBoardStopButton,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);

  /* DO NOT SHOW kernel->widgets.mainBoardStopButton */

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardStopButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_togglePlayModeCallback),
		     (gpointer)kernel);


  /* --------------------- REGISTER ACTION ------------------- */

  /*
   * This procedure will be called every second by the kernel, in
   * the main GTK thread.
   */
  (*kernel->registerAction)(kernel,
			    gdisp_showTime);

  /*
   * Return.
   */
  return pilotBox;

}

