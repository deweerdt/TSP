/*!  \file 

$Id: gdisp_graphics.c,v 1.1 2004-03-26 21:09:17 esteban Exp $

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

File      : Information / Actions upon available graphic plots.

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

#define GD_TABLE_SPACING 20

#include "pixmaps/gdisp_gdispLogo.xpm"

/*
 * This callback is called whenever a "press button" is pressed.
 * The argument "data" is the kernel itself.
 */
static void
gdisp_changePlotTypeCallback(GtkWidget *buttonWidget,
			     gpointer   data )
{

  Kernel_T         *kernel        =     (Kernel_T*)data;
  PlotSystem_T     *plotSystem    = (PlotSystem_T*)NULL;
  GString          *messageString = (GString*)NULL;
  PlotSystemInfo_T  plotInformation;


  /*
   * Store new current plot type.
   */
  kernel->currentPlotType =
    (PlotType_T)gtk_object_get_user_data(GTK_OBJECT(buttonWidget));

  /*
   * The callback has been called. So the corresponding plot type
   * is fully supported. Otherwise, the "press button" would not have
   * been created.
   */
  plotSystem = &kernel->plotSystems[kernel->currentPlotType];
  (*plotSystem->psGetInformation)(kernel,
				  &plotInformation);

  /*
   * Write into the output window.
   * Keep in mind that the GString is released into the
   * 'gdispOutputWrite' function.
   * Do not 'g_string_free' it.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "%s is now selected.",
		   plotInformation.psName);
  kernel->outputFunc(kernel,messageString,GD_MESSAGE);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ graphic plot list.
 */
void
gdisp_createGraphicList ( Kernel_T  *kernel,
			  GtkWidget *parent )
{

  PlotType_T        plotType       = GD_PLOT_DEFAULT;
  PlotSystem_T     *plotSystem     = (PlotSystem_T*)NULL;
  PlotSystemInfo_T  plotInformation;
  GtkWidget        *frame          = (GtkWidget*)NULL;
  GtkWidget        *scrolledWindow = (GtkWidget*)NULL;
  GtkWidget        *box            = (GtkWidget*)NULL;
  GtkWidget        *table          = (GtkWidget*)NULL;
  GtkWidget        *pressButton    = (GtkWidget*)NULL;
  GtkTooltips      *toolTipGroup   = (GtkTooltips*)NULL;
  guint             line           = 0;
  guint             lineNumber     = 0;
  guint             column         = 0;
  guint             columnNumber   = 0;

  GtkWidget        *pixmapWidget   = (GtkWidget*)NULL;
  GdkPixmap        *pixmap         = (GdkPixmap*)NULL;
  GdkBitmap        *mask           = (GdkBitmap*)NULL;
  GtkStyle         *style          =  (GtkStyle*)NULL;


  /* ------------------------ TOOLTIP GROUP ------------------------ */

  /*
   * Create the group of tooltips.
   * Look downwards for the association of tooltips and widgets.
   */
  toolTipGroup = gtk_tooltips_new();


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for graphic logos.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new(" Available Graphic Plots ");
  gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(parent),frame);
  gtk_widget_show(frame);


  /* -------- SCROLLED WINDOW FOR THE LIST OF GRAPHIC PLOTS  -------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_container_add(GTK_CONTAINER(frame),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /* --------- BOX FOR ADDING SPACE INTO THE SCROLLED WINDOW ---------- */

  /*
   * We need a vertical packing box for managing the table.
   */
  box = gtk_vbox_new(FALSE, /* homogeneous */
		     0      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(box),
			     GD_TABLE_SPACING);

  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					box);

  gtk_widget_show(box);


  /* ------------ TABLE CONTAINER FOR GRAPHIC PLOTS  ------------ */

  /*
   * Create a table with the correct dimensions.
   * 2 columns x (GD_MAX_PLOT / 2) lines.
   */
  lineNumber   = GD_MAX_PLOT / 2;
  columnNumber = 2;

  table = gtk_table_new(lineNumber,
			columnNumber,
			TRUE /* homogeneous */);
  gtk_table_set_row_spacings(GTK_TABLE(table),
			     GD_TABLE_SPACING);
  gtk_table_set_col_spacings(GTK_TABLE(table),
			     GD_TABLE_SPACING);

  gtk_box_pack_start(GTK_BOX(box),
		     table,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);
  /* gtk_container_add(GTK_CONTAINER(box),table); */

  gtk_widget_show(table);


  /* ---------------- INSERT GRAPHIC PLOT TYPES  ---------------- */

  /* Do not start with default plot */
  plotType++;

  for (line=0; line<lineNumber; line++) {

    for (column=0; column<columnNumber; column++) {

      if (plotType < GD_MAX_PLOT) {

	plotSystem = &kernel->plotSystems[plotType];

	if (plotSystem->psIsSupported == TRUE) {

	  /*
	   * Get back information from graphic plot.
	   */
	  memset(&plotInformation,0,sizeof(PlotSystemInfo_T));
	  (*plotSystem->psGetInformation)(kernel,
					  &plotInformation);

	  /*
	   * Instanciate a press button.
	   */
	  pressButton = gtk_button_new();

	  /*
	   * Associate the press button with the plot type it represents.
	   * Attach a "clicked" callback.
	   */
	  gtk_object_set_user_data(GTK_OBJECT(pressButton),
				   (gpointer)plotType);

	  gtk_signal_connect(GTK_OBJECT(pressButton),
			     "clicked",
			     GTK_SIGNAL_FUNC(gdisp_changePlotTypeCallback),
			     (gpointer)kernel);

	  /**********************************************************/

	  /*
	   * Use GDK services to create GDISP+ Logo (XPM format).
	   */
	  style  = gtk_widget_get_style(kernel->widgets.dataBookWindow);
	  if (plotInformation.psLogo == (gchar**)NULL) {

	    pixmap = gdk_pixmap_create_from_xpm_d(
				      kernel->widgets.dataBookWindow->window,
				      &mask,
				      &style->bg[GTK_STATE_NORMAL],
				      (gchar**)gdisp_gdispLogo);

	  }
	  else {

	    pixmap = gdk_pixmap_create_from_xpm_d(
				      kernel->widgets.dataBookWindow->window,
				      &mask,
				      &style->bg[GTK_STATE_NORMAL],
				      (gchar**)plotInformation.psLogo);

	  }

	  /*
	   * Create a pixmap widget to contain the pixmap.
	   */
	  pixmapWidget = gtk_pixmap_new(pixmap,mask);
	  gtk_container_add(GTK_CONTAINER(pressButton),
			    pixmapWidget);
	  gtk_widget_show(pixmapWidget);

	  /**********************************************************/

	  /*
	   * Set up a requested size for the press button.
	   */
	  gtk_widget_set_usize(pressButton,
			       150,
			       150);

	  /*
	   * Associate a tooltip information to the press button.
	   */
	  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
			       pressButton,
			       plotInformation.psDescription,
			       "");

	  /*
	   * Attach the press button to its parent widget.
	   */
	  gtk_table_attach_defaults(GTK_TABLE(table),
				    pressButton,
				    column,
				    column + 1,
				    line,
				    line   + 1);

	  /*
	   * Show the press button.
	   */
	  gtk_widget_show(pressButton);

	} /* graphic plot is supported */

      } /* do not exceed GD_MAX_PLOT */

      plotType++;

    } /* columns */

  } /* lines */

}


