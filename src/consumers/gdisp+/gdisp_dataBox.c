/*

$Id: gdisp_dataBox.c,v 1.4 2006-07-31 19:59:07 esteban Exp $

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

#include <gdk/gdkx.h>

#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 * Type definition.
 */
typedef enum {

  GD_SYMBOL_NAME_COLUMN = 0,
  GD_LEGEND_MAX_COLUMN

} DataBox_Column_T;

typedef struct DataBox_T_ {

  /*
   * Kernel.
   */
  Kernel_T  *dbKernel;

  /*
   * Graphics.
   */
  guchar     dbMode; /* standalone or embedded */
  GtkWidget *dbWindow;
  GtkWidget *dbParent;
  GtkWidget *dbDataBox;
  GtkWidget *dbDataBoxArea;
  GtkStyle  *dbStyle;
  GtkWidget *dbCList;
  GdkGC     *dbGc;
  GdkFont   *dbFont;
  GdkWindow *dbTooltip;

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

#define GD_DB_STANDALONE  1
#define GD_DB_EMBEDDED    2

/* Offset between the mouse position and the tooltip window */
#define GD_DB_X_OFFSET   15
#define GD_DB_Y_OFFSET   15
#define GD_DB_W_OFFSET    2
#define GD_DB_H_OFFSET    2

/* Symbol icon size */
#define GD_DB_ICON_SIZE 10

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Manage symbol name window.
 */
static GdkFilterReturn
gdisp_dataBoxEventHandler ( GdkXEvent *xevent,
			    GdkEvent  *event,
			    gpointer   data )
{

  DataBox_T       *dataBox       = (DataBox_T*)data;
  GdkWindow       *tooltipWindow = (GdkWindow*)NULL;
  XEvent          *xEvent        = (XEvent*)NULL;
  GdkFilterReturn  returnFilter  = GDK_FILTER_REMOVE;
  gint             windowX       = 0;
  gint             windowY       = 0;
  gint             windowWidth   = 0;
  gint             windowHeight  = 0;
  gint             windowDepth   = 0;
  GdkRectangle     exposeArea;

  /*
   * CAUTION !!!! CAUTION !!!! CAUTION !!!! CAUTION !!!! CAUTION !!!!
   *
   * Due to a strange way of processing events within Gdk, the second
   * parameter "event" (a GdkEvent pointer) must be used carefully.
   * The only field that is correct is the "window" field. All other
   * fields must not be used, because they are UNINITIALIZED !!!
   * All other information must be taken from the first argument "xevent"
   * that must be casted to a X11 native "XEvent" structure.
   */
  tooltipWindow = event->any.window;
  xEvent        = (XEvent*)xevent;

  switch (xEvent->type) {

  case Expose :

    exposeArea.x      = xEvent->xexpose.x;
    exposeArea.y      = xEvent->xexpose.y;
    exposeArea.width  = xEvent->xexpose.width;
    exposeArea.height = xEvent->xexpose.height;

    gdk_gc_set_clip_rectangle(dataBox->dbGc,
			      &exposeArea);

    gdk_gc_set_foreground(dataBox->dbGc,
			  &dataBox->dbKernel->colors[_RED_]);

    gdk_window_get_geometry(tooltipWindow,			    
			    &windowX,
			    &windowY,
			    &windowWidth,
			    &windowHeight,
			    &windowDepth);

    gdk_draw_rectangle(tooltipWindow,	       
		       dataBox->dbGc,
		       TRUE, /* rectangle is filled */
		       0,
		       0,
		       windowWidth,
		       windowHeight);

    gdk_gc_set_clip_rectangle(dataBox->dbGc,
			      (GdkRectangle*)NULL);

    break;

  default :
    break;

  }

  return returnFilter;

}


/*
 * Treat 'enter-notify' X event.
 * What shall I do when the mouse enters the graphic area ?
 */
static gboolean
gdisp_dataBoxEnterNotify (GtkWidget        *area,
			  GdkEventCrossing *event,
			  gpointer          data)
{

  DataBox_T       *dataBox        = (DataBox_T*)data;
  GdkVisual       *visual         = (GdkVisual*)NULL;
  GdkColormap     *colormap       = (GdkColormap*)NULL;
  gint             lBearing       = 0;
  gint             rBearing       = 0;
  gint             stringWidth    = 0;
  gint             ascent         = 0;
  gint             descent        = 0;
  gint             windowAttrMask = 0;
  GdkWindowAttr    windowAttr;
  GtkDataboxCoord  mouseCoord;
  GtkDataboxValue  mouseValue;
  gchar            stringBuffer[128];

  /*
   * Compute tooltip window dimensions.
   */
  mouseCoord.x = event->x;
  mouseCoord.y = event->y;
  gtk_databox_data_get_value(GTK_DATABOX(dataBox->dbDataBox),
			     mouseCoord,
			     &mouseValue);
  sprintf(stringBuffer,
	  "%f,%f",
	  mouseValue.x,
	  mouseValue.y);

  gdk_string_extents(dataBox->dbFont,
		     stringBuffer,
		     &lBearing,
		     &rBearing,
		     &stringWidth,
		     &ascent,
		     &descent);

  /*
   * Create the flying Gdk window.
   */
  visual   = gdk_window_get_visual  (area->window);
  colormap = gdk_window_get_colormap(area->window);

  memset(&windowAttr,0,sizeof(GdkWindowAttr));

  windowAttr.event_mask  = 0; /* GDK_EXPOSURE_MASK; */
  windowAttr.x           = event->x + GD_DB_X_OFFSET;
  windowAttr.y           = event->y + GD_DB_Y_OFFSET;
  windowAttr.width       = stringWidth      + (2 * GD_DB_W_OFFSET);
  windowAttr.height      = ascent + descent + (2 * GD_DB_H_OFFSET);
  windowAttr.window_type = GDK_WINDOW_CHILD;
  windowAttr.wclass      = GDK_INPUT_OUTPUT;
  windowAttr.visual      = visual;
  windowAttr.colormap    = colormap;
  windowAttrMask         = GDK_WA_X        | GDK_WA_Y      |
                           GDK_WA_COLORMAP | GDK_WA_VISUAL;

  dataBox->dbTooltip = gdk_window_new(area->window,
				      &windowAttr,
				      windowAttrMask);

  if (dataBox->dbTooltip == (GdkWindow*)NULL) {
    return TRUE;
  }

  /*
   * Assign user data to it.
   */
  gdk_window_set_user_data(dataBox->dbTooltip,
			   (gpointer)dataBox);

  /*
   * Set up event handler.
   */
  gdk_window_add_filter(dataBox->dbTooltip,
			gdisp_dataBoxEventHandler,
			(gpointer)dataBox);

  /*
   * Show the window.
   */
  gdk_window_show(dataBox->dbTooltip);

  return TRUE;

}


/*
 * Treat 'leave-notify' X event.
 * What shall I do when the mouse leaves the graphic area ?
 */
static gboolean
gdisp_dataBoxLeaveNotify (GtkWidget        *area,
			  GdkEventCrossing *event,
			  gpointer          data)
{

  DataBox_T *dataBox = (DataBox_T*)data;

  /*
   * Destroy tooltip window.
   */
  if (dataBox->dbTooltip != (GdkWindow*)NULL) {
    gdk_window_destroy(dataBox->dbTooltip);
  }

  dataBox->dbTooltip = (GdkWindow*)NULL;

  return TRUE;

}


/*
 * Treat 'motion-notify' X event.
 * What shall I do when the mouse moves over the graphic area ?
 */
static gboolean
gdisp_dataBoxMotionNotify(GtkWidget      *area,
			  GdkEventMotion *event,
			  gpointer        data)
{

  DataBox_T       *dataBox        = (DataBox_T*)data;
  gint             lBearing       = 0;
  gint             rBearing       = 0;
  gint             stringWidth    = 0;
  gint             ascent         = 0;
  gint             descent        = 0;
  guint            tooltipWidth   = 0;
  guint            tooltipHeight  = 0;
  GtkDataboxCoord  mouseCoord;
  GtkDataboxValue  mouseValue;
  gchar            stringBuffer[128];


#if defined(GD_DATABOX_DUMP)

  GtkDataboxCoord  coord;
  GtkDataboxValue  value;
  GtkDataboxValue  minimum;
  GtkDataboxValue  maximum;

  printf("--------------------------------------------------\n");
  printf("Position %dx%d\n",
	 (gint)event->x,
	 (gint)event->y);

  coord.x = event->x;
  coord.y = event->y;
  gtk_databox_data_get_value(GTK_DATABOX(dataBox->dbDataBox),
			     coord,
			     &value);
  printf("Value    %f,%f\n",
	 value.x,
	 value.y);

  gtk_databox_data_get_extrema(GTK_DATABOX(dataBox->dbDataBox),
			       &minimum,
			       &maximum);
  printf("Extrema  %f,%f -> %f,%f\n",
	 minimum.x,
	 minimum.y,
	 maximum.x,
	 maximum.y);

  gtk_databox_data_get_visible_extrema(GTK_DATABOX(dataBox->dbDataBox),
			       &minimum,
			       &maximum);
  printf("Visible Extrema  %f,%f -> %f,%f\n",
	 minimum.x,
	 minimum.y,
	 maximum.x,
	 maximum.y);

#endif

  /*
   * Destroy tooltip window.
   */
  if (dataBox->dbTooltip != (GdkWindow*)NULL) {

    /*
     * Compute tooltip window dimensions.
     */
    mouseCoord.x = event->x;
    mouseCoord.y = event->y;
    gtk_databox_data_get_value(GTK_DATABOX(dataBox->dbDataBox),
			       mouseCoord,
			       &mouseValue);

    sprintf(stringBuffer,
	    "%f,%f",
	    mouseValue.x,
	    mouseValue.y);

    gdk_string_extents(dataBox->dbFont,
		       stringBuffer,
		       &lBearing,
		       &rBearing,
		       &stringWidth,
		       &ascent,
		       &descent);

    tooltipWidth  = stringWidth      + (2 * GD_DB_W_OFFSET);
    tooltipHeight = ascent + descent + (2 * GD_DB_H_OFFSET);

    gdk_window_move_resize(dataBox->dbTooltip,
			   (gint)event->x + GD_DB_X_OFFSET,
			   (gint)event->y + GD_DB_Y_OFFSET,
			   tooltipWidth,
			   tooltipHeight);

    gdk_gc_set_foreground(dataBox->dbGc,
			  &dataBox->dbKernel->colors[_BLACK_]);

    gdk_draw_rectangle(dataBox->dbTooltip,
		       dataBox->dbGc,
		       TRUE, /* rectangle is filled */
		       0,
		       0,
		       tooltipWidth,
		       tooltipHeight);

    gdk_gc_set_foreground(dataBox->dbGc,
			  &dataBox->dbKernel->colors[_WHITE_]);

    gdk_draw_rectangle(dataBox->dbTooltip,
		       dataBox->dbGc,
		       FALSE, /* rectangle is not filled */
		       0,
		       0,
		       tooltipWidth  - 1,
		       tooltipHeight - 1);

    gdk_draw_string(dataBox->dbTooltip,
		    dataBox->dbFont,
		    dataBox->dbGc,
		    GD_DB_W_OFFSET,          /* X Position */
		    GD_DB_H_OFFSET + ascent, /* Y Position */
		    stringBuffer);

  }

  return TRUE;

}


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
  GtkWidget *frame       = (GtkWidget*)NULL;

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
  dataBox->dbKernel     = kernel;
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
     * Create a frame.
     */
    frame = gtk_frame_new((gchar*)NULL);

    gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

    gtk_container_add(GTK_CONTAINER(dataBox->dbWindow),frame);

    gtk_widget_show(frame);

    /*
     * Create vertical packing box.
     */
    vBox = gtk_vbox_new(FALSE, /* homogeneous */
			2      /* spacing     */ );

    gtk_container_set_border_width(GTK_CONTAINER(vBox),5);

    gtk_container_add(GTK_CONTAINER(frame),vBox);

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

  gtk_signal_connect(GTK_OBJECT(dataBox->dbDataBox),
		     "motion-notify-event",
		     (GtkSignalFunc)gdisp_dataBoxMotionNotify,
		     (gpointer)dataBox);
		     
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

  dataBox->dbDataBoxArea =
    gtk_databox_get_drawing_area(GTK_DATABOX(dataBox->dbDataBox));

  gtk_widget_set_style(dataBox->dbDataBoxArea,
		       dataBox->dbStyle);

  /*
   * Connect signal for tooltip window.
   */
  gtk_signal_connect(GTK_OBJECT(dataBox->dbDataBoxArea),
		     "enter-notify-event",
		     (GtkSignalFunc)gdisp_dataBoxEnterNotify,
		     (gpointer)dataBox);

  gtk_signal_connect(GTK_OBJECT(dataBox->dbDataBoxArea),
		     "leave-notify-event",
		     (GtkSignalFunc)gdisp_dataBoxLeaveNotify,
		     (gpointer)dataBox);

  /*
   * Selection area is filled.
   */
  gtk_databox_show_selection_filled(GTK_DATABOX(dataBox->dbDataBox));

  /*
   * Show databox.
   */
  gtk_widget_show(dataBox->dbDataBox);

  /*
   * Create graphic context and fonts.
   */
  dataBox->dbGc   = gdk_gc_new(dataBox->dbDataBoxArea->window);
  dataBox->dbFont = kernel->fonts[GD_FONT_SMALL][GD_FONT_FIXED];


  /*
   * Separator.
   */
  separator = gtk_hseparator_new();

  gtk_box_pack_start(GTK_BOX(vBox),
		     separator,
		     FALSE, /* expand  */
		     TRUE,  /* fill    */
		     0);    /* padding */

  gtk_widget_show(separator);

  /*
   * Legend.
   */
  dataBox->dbCList = gtk_clist_new(GD_LEGEND_MAX_COLUMN);

  gtk_clist_set_shadow_type(GTK_CLIST(dataBox->dbCList),
			    GTK_SHADOW_IN);

  gtk_clist_set_column_title(GTK_CLIST(dataBox->dbCList),
			     GD_SYMBOL_NAME_COLUMN,
			     "Symbol");

  gtk_clist_set_sort_column(GTK_CLIST(dataBox->dbCList),
			    GD_SYMBOL_NAME_COLUMN);

  gtk_clist_set_sort_type(GTK_CLIST(dataBox->dbCList),
			  GTK_SORT_ASCENDING);

  gtk_clist_set_auto_sort(GTK_CLIST(dataBox->dbCList),
			  TRUE); /* Auto-sort is allowed */

  gtk_clist_set_button_actions(GTK_CLIST(dataBox->dbCList),
			       0, /* left button */
			       GTK_BUTTON_IGNORED);

  gtk_clist_column_titles_hide(GTK_CLIST(dataBox->dbCList));

  gtk_box_pack_start(GTK_BOX(vBox),
		     dataBox->dbCList,
		     FALSE, /* expand  */
		     TRUE,  /* fill    */
		     0);    /* padding */

  gtk_widget_show(dataBox->dbCList);

  /*
   * In standalone mode, add a separator, with 'done' button under.
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
   * Check parameter.
   */
  if (dataBox == (DataBox_T*)NULL) {
    return;
  }

  /*
   * Delete graphic context.
   */
  if (dataBox->dbGc != (GdkGC*)NULL) {
    gdk_gc_destroy(dataBox->dbGc);
  }

  /*
   * Check standalone mode.
   */
  if (dataBox->dbMode == GD_DB_STANDALONE) {

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
			GdkColor *yColor )
{

  DataBox_T *dataBox = (DataBox_T*)dataBoxVoid;
  GdkPixmap *pixmap  = (GdkPixmap*)NULL;
  guint      curveId = 0;
  guint      theRow  = 0;
  gchar     *rowData   [GD_LEGEND_MAX_COLUMN];

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
				       *yColor,
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
				     *yColor,
				     GTK_DATABOX_LINES,
				     1 /* dot size */);

  }

  /*
   * Insert information into the legend.
   */
  rowData[0] = yName;
  theRow = gtk_clist_append(GTK_CLIST(dataBox->dbCList),
			    rowData);

  pixmap = gdk_pixmap_new(dataBox->dbDataBoxArea->window,
			  GD_DB_ICON_SIZE, /* width  */
			  GD_DB_ICON_SIZE, /* height */
			  -1  /* the one of the databox area */);

  gdk_gc_set_foreground(dataBox->dbGc,
			yColor);

  gdk_draw_rectangle((GdkDrawable*)pixmap,
		     dataBox->dbGc,
		     1,   /* Filled */
		     0,   /* X      */
		     0,   /* Y      */
		     15,  /* Width  */
		     15); /* Height */

  gtk_clist_set_pixtext(GTK_CLIST(dataBox->dbCList),
			theRow,
			GD_SYMBOL_NAME_COLUMN,
			rowData[0],
			10, /* spacing */
			pixmap,
			(GdkBitmap*)NULL);

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
