/*!  \file 

$Id: gdisp_dataBox.c,v 1.1 2006-02-21 22:11:00 esteban Exp $

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

File      : Databox management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>

#include <gtk/gtk.h>
#include <gtk/gtkdatabox.h>

#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 * Type definition.
 */
typedef struct DataBox_T_ {

  /*
   * Graphics.
   */
  guchar     dbMode; /* standalone or embedded */
  GtkWidget *dbWindow;
  GtkWidget *dbParent;
  GtkWidget *dbDataBox;
  GtkStyle  *dbStyle;

  /*
   * X Axis.
   */
  gboolean   dbXDataIsSet;
  gfloat    *dbXData;
  guint      dbXIndex;

  /*
   * Number of values.
   */
  guint      dbNbValues;

} DataBox_T;

#define GD_DB_STANDALONE 1
#define GD_DB_EMBEDDED   2

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/




/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create the dataBox.
 */
gpointer
gdisp_createDataBox ( Kernel_T  *kernel,
		      GtkWidget *dbParent )
{

  DataBox_T *dataBox     = (DataBox_T*)NULL;
  GtkWidget *vBox        = (GtkWidget*)NULL;
  GtkWidget *separator   = (GtkWidget*)NULL;
  GtkWidget *buttonBar   = (GtkWidget*)NULL;
  GtkWidget *doneButton  = (GtkWidget*)NULL;
  GtkWidget *drawingArea = (GtkWidget*)NULL;

  /*
   * Check parent type. Only a vertical packing box is allowed.
   */
  if (dbParent != (GtkWidget*)NULL && GTK_IS_VBOX(dbParent) == FALSE) {
    return (gpointer)NULL;
  }

  /*
   * Allocate memory.
   */
  dataBox = (DataBox_T*)g_malloc0(sizeof(DataBox_T));

  if (dataBox == (DataBox_T*)NULL) {
    return (void*)NULL;
  }

  /*
   * Init.
   */
  dataBox->dbXDataIsSet = FALSE;

  /*
   * Create main window if parent widget is not given.
   */
  if (dbParent == (GtkWidget*)NULL) {

    /*
     * Set up mode.
     */
    dataBox->dbMode = GD_DB_STANDALONE;

    /*
     * Create top-level window.
     */
    dataBox->dbWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_widget_set_usize(dataBox->dbWindow,400,300);

    gtk_signal_connect(GTK_OBJECT(dataBox->dbWindow),
		       "destroy",
		       GTK_SIGNAL_FUNC(gtk_widget_destroyed),
		       &dataBox->dbWindow);

    gtk_container_border_width(GTK_CONTAINER(dataBox->dbWindow),5);

    gtk_widget_show(dataBox->dbWindow);

    /*
     * Create vertical packing box.
     */
    vBox = gtk_vbox_new(FALSE, /* homogeneous */
			2      /* spacing     */ );

    gtk_container_add(GTK_CONTAINER(dataBox->dbWindow),vBox);

    gtk_widget_show(vBox);

    /*
     * The vertical packing box is the parent.
     */
    dataBox->dbParent = vBox;

  }
  else {

    /*
     * Set up mode.
     */
    dataBox->dbMode = GD_DB_EMBEDDED;

    /*
     * Simply remember the parent widget.
     */
    dataBox->dbParent = dbParent;

  }

  /*
   * Create the data box.
   */
  dataBox->dbDataBox = gtk_databox_new();

  gtk_signal_connect(GTK_OBJECT(dataBox->dbDataBox),
		     "destroy",
		     GTK_SIGNAL_FUNC(gtk_databox_data_destroy_all),
		     (gpointer)NULL);

  gtk_box_pack_start(GTK_BOX(dataBox->dbParent),
		     dataBox->dbDataBox,
		     TRUE,  /* expand  */
		     TRUE,  /* fill    */
		     0);    /* padding */

  /*
   * Change databox look & feel.
   */
  dataBox->dbStyle = gtk_style_copy(gtk_widget_get_default_style());
  dataBox->dbStyle = gtk_style_ref(dataBox->dbStyle);

  dataBox->dbStyle->base[GTK_STATE_NORMAL]   = kernel->colors[_BLACK_];
  dataBox->dbStyle->fg  [GTK_STATE_NORMAL]   = kernel->colors[_WHITE_];
  dataBox->dbStyle->bg  [GTK_STATE_NORMAL]   = kernel->colors[_BLACK_];

  dataBox->dbStyle->base[GTK_STATE_ACTIVE]   = kernel->colors[_BLACK_];
  dataBox->dbStyle->fg  [GTK_STATE_ACTIVE]   = kernel->colors[_WHITE_];
  dataBox->dbStyle->bg  [GTK_STATE_ACTIVE]   = kernel->colors[_BLACK_];

  dataBox->dbStyle->base[GTK_STATE_SELECTED] = kernel->colors[_WHITE_];
  dataBox->dbStyle->fg  [GTK_STATE_SELECTED] = kernel->colors[_BLACK_];
  dataBox->dbStyle->bg  [GTK_STATE_SELECTED] = kernel->colors[_WHITE_];

  drawingArea = gtk_databox_get_drawing_area(GTK_DATABOX(dataBox->dbDataBox));
  gtk_widget_set_style(drawingArea,dataBox->dbStyle);

  /*
   * Show databox.
   */
  gtk_widget_show(dataBox->dbDataBox);

  /*
   * In standalone mode, add a separator, with 'done' buton under.
   */
  if (dataBox->dbMode == GD_DB_STANDALONE) {

    separator = gtk_hseparator_new();

    gtk_box_pack_start(GTK_BOX(vBox),
		       separator,
		       FALSE, /* expand  */
		       TRUE,  /* fill    */
		       0);    /* padding */

    gtk_widget_show(separator);

    buttonBar = gdisp_createButtonBar(kernel,
				      dataBox->dbParent,
				      (GtkWidget**)NULL, /* apply */
				      &doneButton);

    gtk_box_pack_start(GTK_BOX(vBox),
		       buttonBar,
		       FALSE, /* expand  */
		       TRUE,  /* fill    */
		       0);    /* padding */

    gtk_signal_connect_object(GTK_OBJECT(doneButton),
			      "clicked",
			      GTK_SIGNAL_FUNC(gdisp_destroyDataBox),
			      (gpointer)dataBox);

  }

  /*
   * Return the opaque structure.
   */
  return (gpointer)dataBox;

}


/*
 * Set databox title.
 */
void
gdisp_setDataBoxTitle ( gpointer  dataBoxVoid,
			gchar    *dataBoxTitle )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Check standalone mode.
   */
  if (dataBox->dbWindow != (GtkWidget*)NULL &&
      GTK_IS_WINDOW(dataBox->dbWindow) == TRUE) {

    gtk_window_set_title(GTK_WINDOW(dataBox->dbWindow),
			 dataBoxTitle);

  }

}


/*
 * Set databox title.
 */
void
gdisp_setDataBoxDimensions ( gpointer  dataBoxVoid,
			     guint     dataBoxWidth,
			     guint     dataBoxHeight )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Check standalone mode.
   */
  if (dataBox->dbWindow != (GtkWidget*)NULL &&
      GTK_IS_WINDOW(dataBox->dbWindow) == TRUE) {

    gtk_widget_set_usize(dataBox->dbWindow,
			 dataBoxWidth,
			 dataBoxHeight);

  }

}


/*
 * Destroy databox.
 */
void
gdisp_destroyDataBox ( gpointer dataBoxVoid )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Check standalone mode.
   */
  if (dataBox != (DataBox_T*)NULL && dataBox->dbMode == GD_DB_STANDALONE) {

    gtk_widget_destroy(dataBox->dbWindow);

    if (dataBox->dbStyle != (GtkStyle*)NULL) {
#if defined(GD_UNREF_THINGS)
      gtk_style_unref(dataBox->dbStyle);
#endif
    }

    g_free(dataBox);

  }

}


/*
 * Give the total number of values.
 */
void
gdisp_setDataBoxNbValues ( gpointer  dataBoxVoid,
			   guint     nbValues)
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Remember the number of values.
   */
  dataBox->dbNbValues = nbValues;

}


/*
 * Give X data to the databox.
 */
void
gdisp_setDataBoxXData ( gpointer  dataBoxVoid,
			gchar    *xName,
			gfloat   *xData)
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Remember table, do not copy it.
   */
  dataBox->dbXData = xData;

}


/*
 * Give Y data to the databox.
 */
guint
gdisp_addDataBoxYData ( gpointer  dataBoxVoid,
			gchar    *yName,
			gfloat   *yData,
			GdkColor  yColor )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;
  guint      curveId = 0;

  /*
   * X data must have been given before.
   */
  if (dataBox->dbXData == (gfloat*)NULL) {
    return (guint)0;
  }

  /*
   * Create the curve with the associated X data.
   */
  if (dataBox->dbXDataIsSet == FALSE) {

    curveId = gtk_databox_data_add_x_y(GTK_DATABOX(dataBox->dbDataBox),
				       dataBox->dbNbValues,
				       dataBox->dbXData,
				       yData,
				       yColor,
				       GTK_DATABOX_LINES, 
				       1 /* dot size */);

    dataBox->dbXIndex     = curveId;
    dataBox->dbXDataIsSet = TRUE;

  }
  else {

    curveId = gtk_databox_data_add_y(GTK_DATABOX(dataBox->dbDataBox),
				     dataBox->dbNbValues,
				     yData,
				     dataBox->dbXIndex,
				     yColor,
				     GTK_DATABOX_LINES,
				     1 /* dot size */);

  }

  return curveId;

}


/*
 * Rescale data box.
 */
void
gdisp_rescaleDataBox ( gpointer dataBoxVoid )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;

  /*
   * Rescale data box.
   */
  gtk_databox_rescale(GTK_DATABOX(dataBox->dbDataBox));

}
