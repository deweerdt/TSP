/* GtkDatabox - An extension to the gtk+ library
 * Copyright (C) 1998-2000 Dr. Roland Bock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/* gtkdatabox.c */

#include <string.h>
#include "gtk/gtksignal.h"
#include "gtk/gtktable.h"
#include "gtk/gtktogglebutton.h"
#include "gtk/gtkdrawingarea.h"
#include "gtk/gtkhscrollbar.h"
#include "gtk/gtkvscrollbar.h"
#include "gtk/gtkdatabox.h"
#include "gtk/gtkruler.h"
#include "gtk/gtkhruler.h"
#include "gtk/gtkvruler.h"

enum {
  GTK_DATABOX_SHOW_CROSS=0,
  GTK_DATABOX_SHOW_RULERS,
  GTK_DATABOX_SHOW_SCROLLBARS,
  GTK_DATABOX_ENABLE_SELECTION,
  GTK_DATABOX_SHOW_SELECTION_FILLED,
  GTK_DATABOX_ENABLE_ZOOM,
  GTK_DATABOX_REDRAW_REQUEST,
};

enum {
  GTK_DATABOX_DATA_HAS_GC=0,
};

typedef struct _GtkDataboxData   GtkDataboxData;

struct _GtkDataboxData
{
  gfloat *X;			/* X (normally left/right) values for each set */
  gfloat *Y;			/* Y intensity values */
  guint length;			/* Number of data points */
  GtkDataboxDataType type;	/* How this data set is to be displayed */
  guint size;
  glong flags;
  GdkColor color;
  GdkGC *gc;
};

#define EVENT_METHOD(i, x) GTK_WIDGET_CLASS(GTK_OBJECT(i)->klass)->x

static void gtk_databox_class_init          	(GtkDataboxClass *klass);
static void gtk_databox_init                	(GtkDatabox *box);
static gint gtk_databox_destroy_callback	(GtkWidget *widget, 
						GtkDatabox *box);
static gint gtk_databox_expose_callback		(GtkWidget *widget, 
						GdkEventExpose *event, 
						GtkDatabox *box);
static gint gtk_databox_configure_callback	(GtkWidget *widget, 
						GdkEventConfigure *event, 
						GtkDatabox *box);
static void gtk_databox_zoom_to_selection	(GtkWidget *widget, 
						GtkDatabox *box);
static void gtk_databox_zoom_out		(GtkWidget *widget, 
						GtkDatabox *box);
static void gtk_databox_zoom_home		(GtkWidget *widget, 
						GtkDatabox *box);
static void gtk_databox_zoomed			(GtkWidget *widget, 
						GtkDatabox *box,
						gboolean redraw_flag);
static void gtk_databox_x_adjustment_callback	(GtkWidget *widget, 
						GtkDatabox *box);
static void gtk_databox_y_adjustment_callback	(GtkWidget *widget, 
						GtkDatabox *box);
static gint gtk_databox_button_press_callback	(GtkWidget *widget, 
						GdkEventButton *event, 
						GtkDatabox *box);
static gint gtk_databox_button_release_callback	(GtkWidget *widget, 
						GdkEventButton *event, 
						GtkDatabox *box);
static gint gtk_databox_motion_notify_callback	(GtkWidget *widget, 
						GdkEventMotion *event, 
						GtkDatabox *box);
static void gtk_databox_draw_request_full	(GtkWidget *widget, 
						gboolean now, 
						GtkDatabox *box);
static gint gtk_databox_draw_selection		(GtkWidget *widget, 
						GtkDatabox *box, 
						GdkRectangle *rect);
static void gtk_databox_draw            	(GtkWidget *widget, 
						GtkDatabox *box, 
						GdkEventExpose *event);
static void gtk_databox_draw_points 		(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor); 
static void gtk_databox_draw_points_range_check	(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor); 
static void gtk_databox_get_edge_value		(gfloat x1, gfloat y1,
						gfloat x2, gfloat y2,
						gfloat min_x, gfloat min_y,
						gfloat max_x, gfloat max_y,
						gfloat *x_edge, gfloat *y_edge);
static void gtk_databox_draw_lines 		(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor); 
static void gtk_databox_draw_lines_range_check	(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor); 
static void gtk_databox_draw_bars               (GtkDatabox *box,
                                                GtkDataboxData *data,
                                                GdkPoint *points,
                                                GtkDataboxValue top_left,
                                                GtkDataboxValue bottom_right,
                                                GtkDataboxValue factor);
static void gtk_databox_draw_bars_range_check  (GtkDatabox *box,
                                                GtkDataboxData *data,
                                                GdkPoint *points,
                                                GtkDataboxValue top_left,
                                                GtkDataboxValue bottom_right,
                                                GtkDataboxValue factor);
static void gtk_databox_new_data_gc		(GtkWidget *widget, 
						GtkDatabox *box, 
						GtkDataboxData *data);
static void gtk_databox_draw_cross_xy 		(GtkWidget *widget, 
						GtkDatabox *box, 
						GtkDataboxValue factor);
static void gtk_databox_update_x_ruler		(GtkDatabox *box);
static void gtk_databox_update_y_ruler		(GtkDatabox *box);
static void gtk_databox_data_calc_extrema	(GtkDatabox *box, 
						GtkDataboxValue *min, 
						GtkDataboxValue *max);
static gint gtk_databox_check_x_links		(GList *list, 
						gfloat *values);
static gint gtk_databox_check_y_links		(GList *list, 
						gfloat *values);
static void gtk_databox_destroy_data		(GtkDatabox *box,
						GtkDataboxData *data, 
						GList *list, 
						gboolean free_flag);
static gint gtk_databox_data_destroy_with_flag	(GtkDatabox *box, 
						gint index, 
						gboolean free_flag);
static gint gtk_databox_data_destroy_all_with_flag(GtkDatabox *box, 
						gboolean free_flag);
enum {
  GTK_DATABOX_ZOOMED_SIGNAL,
  GTK_DATABOX_MARKED_SIGNAL,
  GTK_DATABOX_SELECTION_STARTED_SIGNAL,
  GTK_DATABOX_SELECTION_CHANGED_SIGNAL,
  GTK_DATABOX_SELECTION_STOPPED_SIGNAL,
  GTK_DATABOX_SELECTION_CANCELED_SIGNAL,
  LAST_SIGNAL
};

static guint gtk_databox_signals[LAST_SIGNAL] = { 0 };

guint
gtk_databox_get_type ()
{
  static guint box_type = 0;

  if (!box_type)
    {
      GtkTypeInfo box_info =
      {
        "GtkDatabox",
        sizeof (GtkDatabox),
        sizeof (GtkDataboxClass),
        (GtkClassInitFunc) gtk_databox_class_init,
        (GtkObjectInitFunc) gtk_databox_init,
        (GtkArgSetFunc) NULL,
        (GtkArgGetFunc) NULL
      };

      box_type = gtk_type_unique (gtk_vbox_get_type (), &box_info);
    }

  return box_type;
}

static void
gtk_databox_class_init (GtkDataboxClass *class)
{
  GtkObjectClass *object_class=NULL;

  object_class = (GtkObjectClass*) class;

  gtk_databox_signals[GTK_DATABOX_ZOOMED_SIGNAL] = gtk_signal_new ("gtk_databox_zoomed",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_marshal_NONE__POINTER_POINTER, 
                                         GTK_TYPE_NONE, 
                                         2, GTK_TYPE_POINTER, GTK_TYPE_POINTER);
  gtk_databox_signals[GTK_DATABOX_MARKED_SIGNAL] = gtk_signal_new ("gtk_databox_marked",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_marshal_NONE__POINTER, 
                                         GTK_TYPE_NONE, 
                                         1, GTK_TYPE_POINTER);
  gtk_databox_signals[GTK_DATABOX_SELECTION_STARTED_SIGNAL] = gtk_signal_new ("gtk_databox_selection_started",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_marshal_NONE__POINTER, 
                                         GTK_TYPE_NONE, 
                                         1, GTK_TYPE_POINTER);
  gtk_databox_signals[GTK_DATABOX_SELECTION_CHANGED_SIGNAL] = gtk_signal_new ("gtk_databox_selection_changed",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_marshal_NONE__POINTER_POINTER, 
                                         GTK_TYPE_NONE, 
                                         2, GTK_TYPE_POINTER, GTK_TYPE_POINTER);
  gtk_databox_signals[GTK_DATABOX_SELECTION_STOPPED_SIGNAL] = gtk_signal_new ("gtk_databox_selection_stopped",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_marshal_NONE__POINTER_POINTER, 
                                         GTK_TYPE_NONE, 
                                         2, GTK_TYPE_POINTER, GTK_TYPE_POINTER);
  gtk_databox_signals[GTK_DATABOX_SELECTION_CANCELED_SIGNAL] = gtk_signal_new ("gtk_databox_selection_canceled",
                                         GTK_RUN_FIRST,
                                         object_class->type,
                                         GTK_SIGNAL_OFFSET (GtkDataboxClass, gtk_databox),
                                         gtk_signal_default_marshaller, GTK_TYPE_NONE, 
                                         0);


  gtk_object_class_add_signals (object_class, gtk_databox_signals, LAST_SIGNAL);

  class->gtk_databox = NULL;
  class->gtk_databox_zoomed = NULL;
  class->gtk_databox_marked = NULL;
  class->gtk_databox_selection_started = NULL;
  class->gtk_databox_selection_changed = NULL;
  class->gtk_databox_selection_stopped = NULL;
  class->gtk_databox_selection_canceled = NULL;

}

static void
gtk_databox_init (GtkDatabox *box)
{
  GtkWidget *widget=NULL;

  box->table = gtk_table_new (3, 3, FALSE);
  gtk_container_add (GTK_CONTAINER(box), box->table);
  gtk_widget_show (box->table);

  widget=box->draw=gtk_drawing_area_new();
  gtk_widget_set_events(widget, GDK_BUTTON_PRESS_MASK
  				 | GDK_BUTTON_RELEASE_MASK
                                 | GDK_POINTER_MOTION_MASK
				 | GDK_POINTER_MOTION_HINT_MASK
				);
  gtk_signal_connect(GTK_OBJECT (widget), "destroy",
                      GTK_SIGNAL_FUNC (gtk_databox_destroy_callback),
                      box);
  gtk_signal_connect(GTK_OBJECT (widget), "configure_event",
                      GTK_SIGNAL_FUNC (gtk_databox_configure_callback),
                      box);
  gtk_signal_connect(GTK_OBJECT (widget), "expose_event",
                      GTK_SIGNAL_FUNC (gtk_databox_expose_callback),
                      box);
  gtk_signal_connect(GTK_OBJECT (widget), "button_press_event",
                      GTK_SIGNAL_FUNC (gtk_databox_button_press_callback),
                      box);
  gtk_signal_connect(GTK_OBJECT (widget), "button_release_event",
                      GTK_SIGNAL_FUNC (gtk_databox_button_release_callback),
                      box);
  gtk_signal_connect(GTK_OBJECT (widget), "motion_notify_event",
                      GTK_SIGNAL_FUNC (gtk_databox_motion_notify_callback),
                      box);
  gtk_widget_set_usize(widget, 20,30);
  
  gtk_table_attach(GTK_TABLE(box->table), widget,
                        1,2,
                        1,2,
                        GTK_FILL|GTK_EXPAND|GTK_SHRINK,
                        GTK_FILL|GTK_EXPAND|GTK_SHRINK,
                        0,0);
  gtk_widget_show(widget);

  box->adjX = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.9, 1.0));
  box->adjY = GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 1.0, 0.1, 0.9, 1.0));
  gtk_object_ref(GTK_OBJECT(box->adjX));
  gtk_object_ref(GTK_OBJECT(box->adjY));

  gtk_signal_connect(GTK_OBJECT(box->adjY), "value_changed",
                          GTK_SIGNAL_FUNC (gtk_databox_y_adjustment_callback),
                          box);
  gtk_signal_connect(GTK_OBJECT(box->adjX), "value_changed",
                          GTK_SIGNAL_FUNC (gtk_databox_x_adjustment_callback),
                          box);

  box->flags=0;
  gtk_databox_show_rulers(box);
  gtk_databox_show_scrollbars(box);
  gtk_databox_show_cross(box);
  gtk_databox_enable_zoom(box);
  gtk_databox_enable_selection(box);
  gtk_databox_hide_selection_filled(box);
  gtk_databox_set_range_check_threshold(box, 0.25);
  
  box->pixmap=NULL;
  box->data=NULL;
  box->max_points=0;
  box->select_gc=NULL;
  box->cross_border=10;
  gtk_databox_rescale(box);
}

GtkWidget*
gtk_databox_new ()
{
  return GTK_WIDGET(gtk_type_new(gtk_databox_get_type()));
}

void
gtk_databox_show_rulers (GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  if (!(box->flags & (1<<GTK_DATABOX_SHOW_RULERS))) {
    box->hrule = gtk_hruler_new();
    gtk_ruler_set_metric( GTK_RULER(box->hrule), GTK_PIXELS );
    gtk_ruler_set_range( GTK_RULER(box->hrule), 1.5, -0.5, 0.5, 20);
    gtk_signal_connect_object_while_alive(GTK_OBJECT(box->draw), "motion_notify_event",
                           (GtkSignalFunc) EVENT_METHOD(box->hrule, motion_notify_event),
                           GTK_OBJECT(box->hrule) );

    box->vrule = gtk_vruler_new();
    gtk_ruler_set_metric( GTK_RULER(box->vrule), GTK_PIXELS );
    gtk_ruler_set_range( GTK_RULER(box->vrule), 1.5, -0.5, 0.5, 20);
    gtk_signal_connect_object_while_alive(GTK_OBJECT(box->draw), "motion_notify_event",
                           (GtkSignalFunc) EVENT_METHOD(box->vrule, motion_notify_event),
                           GTK_OBJECT(box->vrule) );

    gtk_table_attach(GTK_TABLE(box->table), box->hrule, 
  		1, 2, 
  		0, 1,
		GTK_EXPAND|GTK_SHRINK|GTK_FILL, 
		GTK_FILL, 
		0, 0 );
    gtk_table_attach(GTK_TABLE(box->table), box->vrule, 
  		0, 1, 
  		1, 2,
  		GTK_FILL, 
  		GTK_EXPAND|GTK_SHRINK|GTK_FILL, 
  		0, 0 );

    gtk_widget_show(box->hrule);
    gtk_widget_show(box->vrule);
    box->flags|=1<<GTK_DATABOX_SHOW_RULERS;
  }
}

void
gtk_databox_hide_rulers (GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));
  
  if (box->flags & (1<<GTK_DATABOX_SHOW_RULERS)) {
    gtk_widget_destroy(box->hrule); box->hrule=NULL;
    gtk_widget_destroy(box->vrule); box->vrule=NULL;
  }
  box->flags&=~(1<<GTK_DATABOX_SHOW_RULERS);
}

void
gtk_databox_show_scrollbars (GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  if (!(box->flags & (1<<GTK_DATABOX_SHOW_SCROLLBARS))) {
    box->hscroll = gtk_hscrollbar_new(box->adjX);
    box->vscroll = gtk_vscrollbar_new(box->adjY);
    gtk_table_attach(GTK_TABLE(box->table), box->hscroll,
                        1,2,
                        2,3,
                        GTK_FILL|GTK_EXPAND|GTK_SHRINK,
                        GTK_FILL,
                        0,0);
    gtk_table_attach(GTK_TABLE(box->table), box->vscroll,
                        2,3,
                        1,2,
                        GTK_FILL,
                        GTK_FILL|GTK_EXPAND|GTK_SHRINK,
                        0,0);

    gtk_widget_show(box->hscroll);
    gtk_widget_show(box->vscroll);
  }
  box->flags|=1<<GTK_DATABOX_SHOW_SCROLLBARS;
}

void
gtk_databox_hide_scrollbars (GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  if ((box->flags & (1<<GTK_DATABOX_SHOW_SCROLLBARS))) {
    gtk_widget_destroy(box->hscroll);
    gtk_widget_destroy(box->vscroll);
  }
  box->flags&=~(1<<GTK_DATABOX_SHOW_SCROLLBARS);
}

void
gtk_databox_show_cross(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags|=1<<GTK_DATABOX_SHOW_CROSS;
}

void
gtk_databox_hide_cross(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags&=~(1<<GTK_DATABOX_SHOW_CROSS);
}

void
gtk_databox_enable_selection(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags|=1<<GTK_DATABOX_ENABLE_SELECTION;
}

void
gtk_databox_disable_selection(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags&=~(1<<GTK_DATABOX_ENABLE_SELECTION);
  
  box->selection_flag=0;
  gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_CANCELED_SIGNAL]);
          
}

void
gtk_databox_show_selection_filled(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));
  
  box->flags|=1<<GTK_DATABOX_SHOW_SELECTION_FILLED;
}

void
gtk_databox_hide_selection_filled(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags&=~(1<<GTK_DATABOX_SHOW_SELECTION_FILLED);
}

void
gtk_databox_enable_zoom(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags|=1<<GTK_DATABOX_ENABLE_ZOOM;
}

void
gtk_databox_disable_zoom(GtkDatabox *box)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->flags&=~(1<<GTK_DATABOX_ENABLE_ZOOM);
}

void
gtk_databox_set_range_check_threshold(GtkDatabox *box, gfloat threshold)
{
  g_return_if_fail(GTK_IS_DATABOX(box));
  
  box->range_check_threshold=threshold;
}

                                                        
static gint
gtk_databox_destroy_callback(GtkWidget *widget, GtkDatabox *box)
{
  if (box->pixmap) 
    gdk_pixmap_unref(box->pixmap);
  if (box->select_gc) {
    gdk_gc_destroy(box->select_gc);
  }
  gtk_object_unref(GTK_OBJECT(box->adjX));
  gtk_object_unref(GTK_OBJECT(box->adjY));
  return 0;
}

static gint
gtk_databox_configure_callback(GtkWidget *widget, GdkEventConfigure *event,
                    GtkDatabox *box)
{
  gdk_window_get_size (widget->window, &(box->size.x), &(box->size.y));

  if (box->pixmap)
    gdk_pixmap_unref(box->pixmap);

  box->pixmap = gdk_pixmap_new(widget->window,
                          box->size.x,
                          box->size.y,
                          -1);

  gdk_draw_rectangle (box->pixmap,
                      widget->style->bg_gc[0],
                      TRUE,
                      0, 0,
                      box->size.x,
                      box->size.y);

  if (box->selection_flag) {
    box->selection_flag=0;
    gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_CANCELED_SIGNAL]);
  }
  gtk_databox_zoomed(widget, box, FALSE);
  
  return TRUE;
}

static gint
gtk_databox_expose_callback(GtkWidget *widget, GdkEventExpose *event,
                    GtkDatabox *box)
{
  gtk_databox_draw(box->draw, box, event);

  gdk_draw_pixmap(widget->window,
      widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
      box->pixmap,
      event->area.x, event->area.y,
      event->area.x, event->area.y,
      event->area.width, event->area.height);
  
  return TRUE;
}

static gint
gtk_databox_button_press_callback(GtkWidget *widget, GdkEventButton *event, GtkDatabox *box)
{
  gint x;
  gint y;
  guint button;
  GdkRectangle rect;


  if (event->type!=GDK_BUTTON_PRESS)
    return FALSE;

  button=event->button;
  x=event->x;
  y=event->y;

  if (box->selection_flag) {
    rect.x = MIN(box->marked.x , box->select.x);
    rect.y = MIN(box->marked.y , box->select.y);
    rect.width = MAX(box->marked.x , box->select.x) - rect.x + 1;
    rect.height = MAX(box->marked.x , box->select.x) - rect.y + 1;

    gtk_databox_draw_selection(box->draw, box, &rect);
  } 

  if (button==1 || button==2) {
    if (box->selection_flag) {
      if (rect.x<x && x<MAX(box->marked.x, box->select.x)
       && rect.y<y && y<MAX(box->marked.y, box->select.y))
      {
        box->selection_flag=0;
        gtk_databox_zoom_to_selection(widget, box);
      } else {
        gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_CANCELED_SIGNAL]);
      }
    }
  } else if (button==3) {
    if (event->state&GDK_SHIFT_MASK){
      gtk_databox_zoom_home(widget, box);
    } else {
      gtk_databox_zoom_out(widget, box);
    }
  }
  box->marked.x=x;
  box->marked.y=y;
  gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_MARKED_SIGNAL], &box->marked);
  
  box->selection_flag=0;

  return TRUE;
}

static gint
gtk_databox_button_release_callback(GtkWidget *widget, GdkEventButton *event, GtkDatabox *box)
{
  if (event->type!=GDK_BUTTON_RELEASE)
    return FALSE;

  if (box->selection_flag) {
    gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_STOPPED_SIGNAL], &box->marked, &box->select);
  }

  return TRUE;
}

static gint
gtk_databox_motion_notify_callback(GtkWidget *widget, GdkEventMotion *event,GtkDatabox *box)
{
  gint x, y;
  GdkModifierType state;
  
  x = event->x;
  y = event->y;
  state = event->state;

  if (event->is_hint || (event->window != widget->window))
               gdk_window_get_pointer (widget->window, &x, &y, &state);

  if (state & GDK_BUTTON1_MASK 
      && (box->flags & (1<<GTK_DATABOX_ENABLE_SELECTION))) {
    GdkRectangle rect;
    gint width;
    gint height;
    
    gdk_window_get_size (widget->window, &width, &height);
    x=MAX(0,MIN(width-1,x));
    y=MAX(0,MIN(height-1,y));

    if (box->selection_flag) {
      /* Clear current selection from pixmap */
      gtk_databox_draw_selection(box->draw, box, NULL);
    } else {
      box->selection_flag=1;
      box->marked.x=x;
      box->marked.y=y;
      box->select.x=x;
      box->select.y=y;
      gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_STARTED_SIGNAL], &box->marked);
    }

    /* Determine the exposure rectangle (covering old selection and new) */
    rect.x = MIN(MIN(box->marked.x , box->select.x), x);
    rect.y = MIN(MIN(box->marked.y , box->select.y), y);
    rect.width = MAX(MAX(box->marked.x , box->select.x), x) - rect.x + 1;
    rect.height = MAX(MAX(box->marked.y , box->select.y), y) - rect.y + 1;

    box->select.x=x;
    box->select.y=y;

    /* Draw new selection */
    gtk_databox_draw_selection(box->draw, box, &rect);

    gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_SELECTION_CHANGED_SIGNAL], &box->marked, &box->select);
  }

  return TRUE;
}

void
gtk_databox_data_get_value(GtkDatabox *box, GtkDataboxCoord point, GtkDataboxValue *coord)
{
  coord->x=box->top_left.x
  	+point.x*(box->bottom_right.x-box->top_left.x)/box->size.x;
  coord->y=box->top_left.y
  	+point.y*(box->bottom_right.y-box->top_left.y)/box->size.y;
}

void
gtk_databox_data_get_marked_value(GtkDatabox *box, GtkDataboxValue *coord)
{
  gtk_databox_data_get_value(box, box->marked, coord);
}

void
gtk_databox_data_get_delta_value(GtkDatabox *box, GtkDataboxValue *coord)
{
  GtkDataboxValue drooc;

  gtk_databox_data_get_value(box, box->marked, &drooc);
  gtk_databox_data_get_value(box, box->select, coord);
  coord->x-=drooc.x;
  coord->y-=drooc.y;
}

static void
gtk_databox_data_calc_extrema(GtkDatabox *box, GtkDataboxValue *min, GtkDataboxValue *max) 
{
  gint i;
  GtkDataboxData *data=NULL;
  GList *list=NULL;
  GtkDataboxValue border;
  
  g_return_if_fail(GTK_IS_DATABOX(box) && min && max);
  
  if (!box->data) {
    min->x=-0.5;
    min->y=-0.5;
    max->x=1.5;
    max->y=1.5;
    return;
  }

  list=box->data;
  if (list) data=(GtkDataboxData *) list->data;
  else data=NULL;

  min->x=data->X[0];
  min->y=data->Y[0];
  max->x=data->X[0];
  max->y=data->Y[0];

  while(data) {
    for (i=0; i<data->length; i++) {
      min->x=MIN(data->X[i], min->x);
      max->x=MAX(data->X[i], max->x);
    }
    for (i=0; i<data->length; i++) {
      min->y=MIN(data->Y[i], min->y);
      max->y=MAX(data->Y[i], max->y);
    }
    list=g_list_next(list);
    if (list) data=(GtkDataboxData *) list->data;
    else data=NULL;
  }

  border.x=(max->x-min->x)/10.;
  border.y=(max->y-min->y)/10.;

  min->x=min->x-border.x;
  min->y=min->y-border.y;
  max->x=max->x+border.x;
  max->y=max->y+border.y;

  return;
}

void
gtk_databox_data_get_extrema(GtkDatabox *box, GtkDataboxValue *min, GtkDataboxValue *max) 
{
  g_return_if_fail(GTK_IS_DATABOX(box) && min && max);
  
  *min=box->min;
  *max=box->max;

  return;
}

void
gtk_databox_data_get_visible_extrema(GtkDatabox *box, GtkDataboxValue *min, GtkDataboxValue *max) 
{
  GtkDataboxValue pre_min;
  GtkDataboxValue pre_max;
  GtkDataboxCoord top_left;
  GtkDataboxCoord bottom_right;
  
  top_left.x=top_left.y=0;
  bottom_right.x=box->size.x-1;
  bottom_right.y=box->size.y-1;
  
  g_return_if_fail(GTK_IS_DATABOX(box) && min && max);
  
  gtk_databox_data_get_value(box, top_left, &pre_min);
  gtk_databox_data_get_value(box, bottom_right, &pre_max);
  
  min->x=MIN(pre_min.x, pre_max.x);
  min->y=MIN(pre_min.y, pre_max.y);
  max->x=MAX(pre_min.x, pre_max.x);
  max->y=MAX(pre_min.y, pre_max.y);

  return;
}

void
gtk_databox_rescale_with_values(GtkDatabox *box, GtkDataboxValue min, GtkDataboxValue max)
{
  g_return_if_fail(GTK_IS_DATABOX(box));

  box->min.x=min.x;
  box->max.x=max.x;
  box->min.y=min.y;
  box->max.y=max.y;
  
  if (box->max.x-box->min.x<1e-10) {
    box->min.x-=0.5e-10;
    box->max.x+=0.5e-10;
  }
  if (box->max.y-box->min.y<1e-10) {
    box->min.y-=0.5e-10;
    box->max.y+=0.5e-10;
  }

  gtk_databox_zoom_home(box->draw, box);
}

void
gtk_databox_rescale(GtkDatabox *box)
{
  GtkDataboxValue min, max;
  
  g_return_if_fail(GTK_IS_DATABOX(box));
  
  gtk_databox_data_calc_extrema(box, &min, &max);

  gtk_databox_rescale_with_values(box, min, max);
}

void
gtk_databox_redraw(GtkDatabox *box)
{
  gtk_databox_draw_request_full(box->draw, TRUE, box);
}


static void
gtk_databox_zoom_to_selection(GtkWidget *widget, GtkDatabox *box)
{
  if (!(box->flags&(1<<GTK_DATABOX_ENABLE_ZOOM))) return;

  box->adjX->lower=0;
  box->adjY->lower=0;
  box->adjX->value+=(gfloat)(MIN(box->marked.x, box->select.x))*box->adjX->page_size/box->size.x;
  box->adjY->value+=(gfloat)(MIN(box->marked.y, box->select.y))*box->adjY->page_size/box->size.y;
  box->adjX->page_size*=(gfloat)(ABS(box->marked.x-box->select.x)+1)/box->size.x;
  box->adjY->page_size*=(gfloat)(ABS(box->marked.y-box->select.y)+1)/box->size.y;
  box->adjX->upper=1.0;
  box->adjY->upper=1.0;
  box->adjY->step_increment=box->adjY->page_size/20;
  box->adjY->page_increment=box->adjY->page_size*0.9;
  box->adjX->step_increment=box->adjX->page_size/20;
  box->adjX->page_increment=box->adjX->page_size*0.9;

  gtk_databox_zoomed(widget, box, TRUE);
}

static void
gtk_databox_zoom_out(GtkWidget *widget, GtkDatabox *box)
{
  if (!(box->flags&(1<<GTK_DATABOX_ENABLE_ZOOM))) return;

  box->adjX->lower=0;
  box->adjY->lower=0;
  box->adjX->page_size=MIN(1.0, box->adjX->page_size*2);
  box->adjY->page_size=MIN(1.0, box->adjY->page_size*2);
  box->adjX->value=(box->adjX->page_size==1.0)? 0:
  			(MAX(0, (box->adjX->value-box->adjX->page_size/4)));
  box->adjY->value=(box->adjY->page_size==1.0)? 0:
  			(MAX(0, (box->adjY->value-box->adjY->page_size/4)));
  box->adjX->upper=1.0;
  box->adjY->upper=1.0;
  box->adjY->step_increment=box->adjY->page_size/20;
  box->adjY->page_increment=box->adjY->page_size*0.9;
  box->adjX->step_increment=box->adjX->page_size/20;
  box->adjX->page_increment=box->adjX->page_size*0.9;

  gtk_databox_zoomed(widget, box, TRUE);
}

static void
gtk_databox_zoom_home(GtkWidget *widget, GtkDatabox *box)
{
  
  if (!(box->flags&(1<<GTK_DATABOX_ENABLE_ZOOM))) return;

  box->selection_flag=0;

  box->adjX->lower=0;
  box->adjY->lower=0;
  box->adjX->page_size=1.0;
  box->adjY->page_size=1.0;
  box->adjX->value=0;
  box->adjY->value=0;
  box->adjX->upper=1.0;
  box->adjY->upper=1.0;
  box->adjY->step_increment=box->adjY->page_size/20;
  box->adjY->page_increment=box->adjY->page_size*0.9;
  box->adjX->step_increment=box->adjX->page_size/20;
  box->adjX->page_increment=box->adjX->page_size*0.9;

  gtk_databox_zoomed(widget, box, TRUE);
}

static void
gtk_databox_zoomed(GtkWidget *widget, GtkDatabox *box, gboolean redraw_flag)
{
  if (!(box->flags&(1<<GTK_DATABOX_ENABLE_ZOOM))) return;

  box->flags|=1<<GTK_DATABOX_REDRAW_REQUEST;  

  gtk_adjustment_changed(box->adjX);
  gtk_adjustment_changed(box->adjY);
  gtk_databox_x_adjustment_callback(widget, box);
  gtk_databox_y_adjustment_callback(widget, box);

  if (redraw_flag) {
    box->flags&=~(1<<GTK_DATABOX_REDRAW_REQUEST);
    gtk_databox_draw_request_full(box->draw, TRUE, box);
  }

  gtk_signal_emit(GTK_OBJECT(box), gtk_databox_signals[GTK_DATABOX_ZOOMED_SIGNAL], &box->top_left, &box->bottom_right);
}

static void
gtk_databox_x_adjustment_callback(GtkWidget *widget, GtkDatabox *box)
{
  if (box->adjX->page_size==1) {
    box->top_left.x=box->min.x;
    box->bottom_right.x=box->max.x;
  } else {
    box->top_left.x=box->min.x+(box->max.x-box->min.x)*box->adjX->value;
    box->bottom_right.x=box->top_left.x+(box->max.x-box->min.x)*box->adjX->page_size;
  }

  gtk_databox_update_x_ruler(box);
  gtk_databox_draw_request_full(box->draw, TRUE, box);
}

static void
gtk_databox_y_adjustment_callback(GtkWidget *widget, GtkDatabox *box)
{
  if (box->adjY->page_size==1) {
    box->top_left.y=box->max.y;
    box->bottom_right.y=box->min.y;
  } else {
    box->top_left.y=box->max.y-(box->max.y-box->min.y)*box->adjY->value;
    box->bottom_right.y=box->top_left.y-(box->max.y-box->min.y)*box->adjY->page_size;
  }

  gtk_databox_update_y_ruler(box);
  gtk_databox_draw_request_full(box->draw, TRUE, box);
}

static void
gtk_databox_update_x_ruler(GtkDatabox *box)
{
  if (box->flags & (1<<GTK_DATABOX_SHOW_RULERS)) {
    gtk_ruler_set_range( GTK_RULER(box->hrule), 
  		box->top_left.x, box->bottom_right.x, 
  		0.5*(box->top_left.x+box->bottom_right.x), 20);
  }
}

static void
gtk_databox_update_y_ruler(GtkDatabox *box)
{
  if (box->flags & (1<<GTK_DATABOX_SHOW_RULERS)) {
    gtk_ruler_set_range( GTK_RULER(box->vrule), 
  		box->top_left.y, box->bottom_right.y, 
  		0.5*(box->top_left.y+box->bottom_right.y), 20);
  }
}

/*static void
gtk_databox_draw_request(GtkWidget *widget, gboolean now, GdkRectangle redraw_rect, GtkDatabox *box)
{
  box->redraw_rect.width=MIN(box->size.x-1, MAX(box->redraw_rect.x+box->redraw_rect.width,
  				redraw_rect.x+redraw_rect.width)
  			-MIN(box->redraw_rect.x, redraw_rect.x));
  box->redraw_rect.height=MIN(box->size.y-1, MAX(box->redraw_rect.y+box->redraw_rect.height,
  				redraw_rect.y+redraw_rect.height)
  			-MIN(box->redraw_rect.y, redraw_rect.y));
  box->redraw_rect.x=MAX(0, MIN(box->redraw_rect.x, redraw_rect.x));
  box->redraw_rect.y=MAX(0, MIN(box->redraw_rect.y, redraw_rect.y));

  if (now) gtk_widget_draw(widget, &redraw_rect);
}
*/
static void
gtk_databox_draw_request_full(GtkWidget *widget, gboolean now, GtkDatabox *box)
{
  GdkRectangle redraw_rect;

  redraw_rect.x=0;
  redraw_rect.y=0;
  redraw_rect.width=box->size.x;
  redraw_rect.height=box->size.y;
  
  if (box->flags&(1<<GTK_DATABOX_REDRAW_REQUEST)) {
    return;
  }
  
  box->flags|=1<<GTK_DATABOX_REDRAW_REQUEST;

  if (now) gtk_widget_draw(widget, &redraw_rect);
}

static gint
gtk_databox_draw_selection(GtkWidget *widget, GtkDatabox *box, GdkRectangle *rect)
{
  if (!box->select_gc) {
    GdkGCValues values;
    GdkColormap* colormap;
    GdkColor color;

    color.red=65535;
    color.green=65535;
    color.blue=65535;
    colormap=gtk_widget_get_colormap(widget);
    gdk_color_alloc(colormap, &color);
    values.foreground=color;
    values.function=GDK_XOR;
    box->select_gc=gdk_gc_new_with_values(widget->window,
			&values,
			GDK_GC_FUNCTION | GDK_GC_FOREGROUND);
  }


  gdk_draw_rectangle(box->pixmap,
                   box->select_gc,
                   box->flags & (1<<GTK_DATABOX_SHOW_SELECTION_FILLED),
                   MIN(box->marked.x , box->select.x),
                   MIN(box->marked.y , box->select.y),
                   ABS(box->marked.x - box->select.x),
                   ABS(box->marked.y - box->select.y));

  if (rect)
    gdk_draw_pixmap(widget->window,
		widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
		box->pixmap,
                rect->x, rect->y,
                rect->x, rect->y,
                rect->width,
                rect->height);

    
  return TRUE;
}

gint
gtk_databox_get_color(GtkDatabox *box, gint index, GdkColor *color)
{
  GtkDataboxData *data=NULL;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  g_return_val_if_fail(color, -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, index);
  g_return_val_if_fail(data, -1);

  *color=data->color;
  
  return 0;
}

gint
gtk_databox_get_data_type(GtkDatabox *box, gint index, GtkDataboxDataType *type, guint *dot_size)
{
  GtkDataboxData *data=NULL;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  g_return_val_if_fail(type, -1);
  g_return_val_if_fail(dot_size, -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, index);
  g_return_val_if_fail(data, -1);

  *type=data->type;
  *dot_size=data->size;
  
  return 0;
}

gint
gtk_databox_set_data_type(GtkDatabox *box, gint index, GtkDataboxDataType type, guint dot_size)
{
  GtkDataboxData *data=NULL;
  GdkGCValues values;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, index);
  g_return_val_if_fail(data, -1);
  
  if (data->gc) {
    gdk_gc_get_values(data->gc, &values);
    gdk_gc_set_line_attributes(data->gc, dot_size, values.line_style, values.cap_style, values.join_style);
  }
  
  data->type=type;
  data->size=dot_size;
  
  return 0;
}

gint
gtk_databox_set_color(GtkDatabox *box, gint index, GdkColor color)
{
  GtkDataboxData *data=NULL;
  GtkWidget *widget=NULL;
  GdkColormap* colormap=NULL;
  gboolean color_allocate_success;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  widget=box->draw;
  g_return_val_if_fail(GTK_IS_WIDGET(widget), -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, index);
  g_return_val_if_fail(data, -1);

  if(data->flags&(1<<GTK_DATABOX_DATA_HAS_GC)) {
    colormap=gtk_widget_get_colormap(widget);
    g_return_val_if_fail(colormap, -1);
    gdk_colormap_free_colors(colormap, &data->color, 1);
    data->color=color;
    gdk_colormap_alloc_colors(colormap, &data->color, 1, FALSE, TRUE, &color_allocate_success);
    g_return_val_if_fail(color_allocate_success, -1);
    gdk_gc_set_foreground(data->gc, &data->color);
  } else {
    data->color=color;
  }
  
  return 0;
}

static void
gtk_databox_new_data_gc(GtkWidget *widget, GtkDatabox *box, GtkDataboxData *data)
{
  GdkGCValues values;
  gboolean color_allocate_success;
  GdkColormap* colormap=NULL;
  
  g_return_if_fail(GTK_IS_DATABOX(box));
  g_return_if_fail(GTK_IS_WIDGET(widget));
  g_return_if_fail(data);

  colormap=gtk_widget_get_colormap(widget);
  g_return_if_fail(colormap);
  gdk_colormap_alloc_colors(colormap, &data->color, 1, FALSE, TRUE, &color_allocate_success);
  g_return_if_fail(color_allocate_success);

  values.foreground=data->color;
  values.function=GDK_COPY;
  values.line_width=data->size;
  data->gc=gdk_gc_new_with_values(widget->window,
  				&values,
  				GDK_GC_FUNCTION | GDK_GC_FOREGROUND | GDK_GC_LINE_WIDTH);
  data->flags|=1<<GTK_DATABOX_DATA_HAS_GC;
}

static void
gtk_databox_draw_cross_xy (GtkWidget *widget, GtkDatabox *box, GtkDataboxValue factor)
{
  gint x=0;
  gint y=0;
  char *str=NULL;
  gboolean xflag=FALSE;
  gboolean yflag=FALSE;
   
  if (0>=box->top_left.x && 0<box->bottom_right.x) {
    x=(0-box->top_left.x)*factor.x;
    if (x>=box->cross_border && x<box->size.x-box->cross_border) {
      gdk_draw_line(box->pixmap, widget->style->fg_gc[0], 
           x, box->cross_border, x, box->size.y-box->cross_border);
      xflag=TRUE;
    }
  }
  if (0<=box->top_left.y && 0>box->bottom_right.y) {
    y=(0-box->top_left.y)*factor.y;
    if (y>=box->cross_border && y<box->size.y-box->cross_border) {
      gdk_draw_line(box->pixmap, widget->style->fg_gc[0], 
           box->cross_border, y, box->size.x-box->cross_border, y);
      yflag=TRUE;
    }
  }
  
  if (xflag && yflag) {
    str = "0";
    x-=gdk_text_width(widget->style->font, str, strlen(str));
    y+=gdk_text_height(widget->style->font, str, strlen(str)) + 2;
    gdk_draw_string(box->pixmap, widget->style->font, widget->style->fg_gc[0], x, y, str);
  }
}

static void
gtk_databox_draw(GtkWidget *widget, GtkDatabox *box, GdkEventExpose *event)
{
  GList *list=NULL;
  GtkDataboxData *data=NULL;
  GtkDataboxValue factor;
  GdkPoint *points=NULL;
  
  box->flags&=~(1<<GTK_DATABOX_REDRAW_REQUEST);
  box->flags=(box->flags);
  
  g_return_if_fail(GTK_IS_DATABOX(box));
  if (!GTK_WIDGET_VISIBLE(widget)) return;

  gdk_draw_rectangle (box->pixmap,
                      widget->style->bg_gc[0],
                      TRUE,
                      0, 0,
                      box->size.x,
                      box->size.y);

  if (!box->data || !box->max_points) return;

  factor.x=box->size.x/(box->bottom_right.x-box->top_left.x);
  factor.y=box->size.y/(box->bottom_right.y-box->top_left.y);
  
  points=g_new0(GdkPoint, box->max_points*2); /* lines and bars need segments
  						(1 segment equals 2 points) */
  
  /*  Draw last data set first so first is on top */
  list=g_list_last(box->data);
  if (list) data=(GtkDataboxData *) list->data;
  else data=NULL;
  
  while(data) {
    if (!data->gc || !(data->flags&(1<<GTK_DATABOX_DATA_HAS_GC))) {
      gtk_databox_new_data_gc(widget, box, data);
    }
    if (data->length) {
      switch(data->type) {
        case GTK_DATABOX_POINTS:
          if (box->adjX->page_size*box->adjY->page_size<box->range_check_threshold) { 
            gtk_databox_draw_points_range_check(box, data, points, box->top_left, box->bottom_right, factor);
          } else {
            gtk_databox_draw_points(box, data, points, box->top_left, box->bottom_right, factor);
          }
          break; 
        case GTK_DATABOX_LINES:
          if (box->adjX->page_size*box->adjY->page_size<box->range_check_threshold) { 
            gtk_databox_draw_lines_range_check(box, data, points, box->top_left, box->bottom_right, factor);
          } else {
            gtk_databox_draw_lines(box, data, points, box->top_left, box->bottom_right, factor);
          }
          break;
        case GTK_DATABOX_BARS:
          if (box->adjX->page_size*box->adjY->page_size<box->range_check_threshold) { 
            gtk_databox_draw_bars_range_check(box, data, points, box->top_left, box->bottom_right, factor);
          } else {
            gtk_databox_draw_bars(box, data, points, box->top_left, box->bottom_right, factor);
          }
          break;
        default: 
        break;
      }
    }
    list=g_list_previous(list);
    if (list) data=(GtkDataboxData *) list->data;
    else data=NULL;
  }

  if (box->flags&(1<<GTK_DATABOX_SHOW_CROSS)) {
    gtk_databox_draw_cross_xy(widget, box, factor);
  }
  
  if (points) g_free((gpointer) points);

  if (box->selection_flag) {
    gtk_databox_draw_selection(widget, box, NULL);
  }

  return;
}

static void 
gtk_databox_draw_points 		(GtkDatabox *box, 
					GtkDataboxData *data, 
					GdkPoint *points,
					GtkDataboxValue top_left, 
					GtkDataboxValue bottom_right,
					GtkDataboxValue factor) 
{
  gint i;
    
  for (i=0; i<data->length; i++) {
    points[i].x=(gint16) ((data->X[i]-top_left.x)*factor.x);
    points[i].y=(gint16) ((data->Y[i]-top_left.y)*factor.y);
  }

  if (data->size<2) {
    /* More than 2^16 points will cause X IO error on most XServers
       (Hint from Paul Barton-Davis <pbd@Op.Net>) */
    for (i=0; i<data->length; i+=65536) {
        gdk_draw_points(box->pixmap,
        data->gc,
      	points+i, MIN(65536, data->length-i));
    }
  } else {
    for (i=0; i<data->length; i++) {
      /* Why on earth is there no gdk_draw_rectangles?? */
      gdk_draw_rectangle(box->pixmap,
  	    	data->gc,
      		TRUE,
       		points[i].x-data->size/2,
        	points[i].y-data->size/2,
        	data->size,
        	data->size);
    }
  }
}

static void 
gtk_databox_draw_points_range_check	(GtkDatabox *box, 
					GtkDataboxData *data, 
					GdkPoint *points,
					GtkDataboxValue top_left, 
					GtkDataboxValue bottom_right,
					GtkDataboxValue factor) 
{
  gfloat x, y;
  gint count;
  gint i;
    
  count=0;
  for (i=0; i<data->length; i++) {
    x=data->X[i];
    if (x>=top_left.x && x<=bottom_right.x) {
      y=data->Y[i];
      if (y>=bottom_right.y && y<=top_left.y) {
        points[count].x=(gint16) ((x-top_left.x)*factor.x);
        points[count].y=(gint16) ((y-top_left.y)*factor.y);
        count++;
      }
    }
  }
  if (data->size<2) {
    /* More than 2^16 points will cause X IO error on most XServers
       (Hint from Paul Barton-Davis <pbd@Op.Net>) */
    for (i=0; i<count; i+=65536) {
      gdk_draw_points(box->pixmap,
		data->gc,
      		points+i, MIN(65536, count-i));
    }
  } else {
    for (i=0; i<count; i++) {
      gdk_draw_rectangle(box->pixmap,
        	data->gc,
        	TRUE,
        	points[i].x-data->size/2,
        	points[i].y-data->size/2,
        	data->size,
        	data->size);
    }
  }
}

static void 
gtk_databox_draw_lines 		(GtkDatabox *box, 
					GtkDataboxData *data, 
					GdkPoint *points,
					GtkDataboxValue top_left, 
					GtkDataboxValue bottom_right,
					GtkDataboxValue factor) 
{
  gint i;
    
  for (i=0; i<data->length; i++) {
    points[i].x=(gint16) ((data->X[i]-top_left.x)*factor.x);
    points[i].y=(gint16) ((data->Y[i]-top_left.y)*factor.y);
  }

  /* More than 2^16 points will cause X IO error on most XServers
     (Hint from Paul Barton-Davis <pbd@Op.Net>) */
  for (i=0; i<data->length; i+=65535) {
      gdk_draw_lines(box->pixmap,
      data->gc,
      points+i, MIN(65536, data->length-i));
  }
}

static void
gtk_databox_get_edge_value	(gfloat x1, gfloat y1,
					gfloat x2, gfloat y2,
					gfloat min_x, gfloat min_y,
					gfloat max_x, gfloat max_y,
					gfloat *x_edge, gfloat *y_edge)
/* Finds the point at the edge of the drawing area where the line line between
   point1 and point2 crosses. It is supposed that point1 is inside and point2 
   is outside the drawing area */
{
  gfloat mx, my;

  *x_edge=x1;
  *y_edge=y1;
  
  if (x1!=x2) {
    my=(y1-y2)/(x1-x2);
    *x_edge=x2=MIN(MAX(x2, min_x), max_x);
    *y_edge=y2=y1-(x1-x2)*my;
  }
  if (y1!=y2) {
    mx=(x1-x2)/(y1-y2);
    *y_edge=y2=MIN(MAX(y2, min_y), max_y);
    *x_edge=x2=x1-(y1-y2)*mx;
  }
}

static void 
gtk_databox_draw_lines_range_check	(GtkDatabox *box, 
					GtkDataboxData *data, 
					GdkPoint *points,
					GtkDataboxValue top_left, 
					GtkDataboxValue bottom_right,
					GtkDataboxValue factor) 
{
  gfloat x, y;
  gfloat x_old, y_old;
  gfloat x_edge, y_edge;
  GdkSegment *segments=(GdkSegment *) points;
  gint count;
  gint last_point_out_flag=0;
  gint i;
    
/* FIXME: Drawing lines instead of segments might be even nicer ?? 
	But difficulties with drawing over the edge ... */

  x_old=data->X[0];
  y_old=data->Y[0];
  if (x_old>=top_left.x && x_old<=bottom_right.x &&
    		y_old>=bottom_right.y && y_old<=top_left.y) {
    last_point_out_flag=0;
  } else {
    last_point_out_flag=1;
  }
  
  count=0;
  for (i=1; i<data->length; i++) {
    x=data->X[i];
    y=data->Y[i];
    if (x>=top_left.x && x<=bottom_right.x &&
      		y>=bottom_right.y && y<=top_left.y) {
      if (!last_point_out_flag) {
        /* actual point is INSIDE drawing area, last point was INSIDE */

        segments[count].x1=(gint16) ((x-top_left.x)*factor.x);
        segments[count].y1=(gint16) ((y-top_left.y)*factor.y);
        segments[count].x2=(gint16) ((x_old-top_left.x)*factor.x);
        segments[count].y2=(gint16) ((y_old-top_left.y)*factor.y);
        count++;
      } else {
        /* actual point is INSIDE drawing area, last point was OUTSIDE */

        gtk_databox_get_edge_value(x, y, x_old, y_old, 
        			top_left.x, bottom_right.y,
        			bottom_right.x, top_left.y,
        			&x_edge, &y_edge);

        segments[count].x1=(gint16) ((x-top_left.x)*factor.x);
        segments[count].y1=(gint16) ((y-top_left.y)*factor.y);
        segments[count].x2=(gint16) ((x_edge-top_left.x)*factor.x);
        segments[count].y2=(gint16) ((y_edge-top_left.y)*factor.y);
        count++;
      }
      last_point_out_flag=0;
    } else if (!last_point_out_flag) {
      /* actual point is OUTSIDE drawing area, last point was INSIDE */

      gtk_databox_get_edge_value(x_old, y_old, x, y, 
              			top_left.x, bottom_right.y,
        			bottom_right.x, top_left.y,
      				&x_edge, &y_edge);

      segments[count].x1=(gint16) ((x_old-top_left.x)*factor.x);
      segments[count].y1=(gint16) ((y_old-top_left.y)*factor.y);
      segments[count].x2=(gint16) ((x_edge-top_left.x)*factor.x);
      segments[count].y2=(gint16) ((y_edge-top_left.y)*factor.y);
      count++;
      last_point_out_flag=1;
    } else {
      /* actual point is OUTSIDE drawing area, last point was OUTSIDE */
      if ((x<top_left.x && x_old<top_left.x) ||
      	(x>bottom_right.x && x_old>bottom_right.x) ||
      	(y>top_left.y && y_old>top_left.y) ||
      	(y<bottom_right.y && y_old<bottom_right.y)) {
      	  /* Do nothing */
      } else {
        gtk_databox_get_edge_value(x, y, x_old, y_old, 
        			top_left.x, bottom_right.y,
        			bottom_right.x, top_left.y,
      				&x_edge, &y_edge);
        if (x_edge>=top_left.x && x_edge<=bottom_right.x &&
      	    y_edge<=top_left.y && y_edge>=bottom_right.y &&
      	    x_edge>=MIN(x, x_old) && x_edge<=MAX(x, x_old) &&
      	    y_edge>=MIN(y, y_old) && y_edge<=MAX(y, y_old)) {
          x_old=x_edge;
          y_old=y_edge;
          gtk_databox_get_edge_value(x_old, y_old, x, y, 
        			top_left.x, bottom_right.y,
        			bottom_right.x, top_left.y,
      				&x_edge, &y_edge);
          
          segments[count].x1=(gint16) ((x_old-top_left.x)*factor.x);
          segments[count].y1=(gint16) ((y_old-top_left.y)*factor.y);
          segments[count].x2=(gint16) ((x_edge-top_left.x)*factor.x);
          segments[count].y2=(gint16) ((y_edge-top_left.y)*factor.y);
          count++;
          last_point_out_flag=1;
        }
      }
    }
    x_old=data->X[i];
    y_old=data->Y[i];
  }
  for (i=0; i<count; i+=65536) {  
    gdk_draw_segments(box->pixmap,
                data->gc,
      		segments+i, MIN(65536, count-i));
  }
}

static void gtk_databox_draw_bars 		(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor) 
{
  gint i;
  GdkSegment *segments=(GdkSegment *) points;
  gfloat axis=0;
    
  axis=(gint16) ((0-top_left.y)*factor.y);
  for (i=0; i<data->length; i++) {
    segments[i].x1=segments[i].x2=(gint16) ((data->X[i]-top_left.x)*factor.x);
    segments[i].y1=axis;
    segments[i].y2=(gint16) ((data->Y[i]-top_left.y)*factor.y);
  }
  for (i=0; i<data->length; i+=65536) { 
    gdk_draw_segments(box->pixmap,
		data->gc,
      		segments, MIN(65536, data->length-i));
  }
}

static void gtk_databox_draw_bars_range_check	(GtkDatabox *box, 
						GtkDataboxData *data, 
						GdkPoint *points,
						GtkDataboxValue top_left, 
						GtkDataboxValue bottom_right,
						GtkDataboxValue factor) 
{
  gfloat x, y;
  gint count;
  gint i;
  GdkSegment *segments=(GdkSegment *) points;
  gfloat axis=0;
  gfloat temp_y=0;
    
  count=0;
  axis=((0-top_left.y)*factor.y);
  for (i=0; i<data->length; i++) {
    x=data->X[i];
    if (x>=top_left.x && x<=bottom_right.x) {
      y=data->Y[i];
      segments[count].x1=segments[count].x2=(gint16) ((x-top_left.x)*factor.x);
      temp_y=(gint16) ((y-top_left.y)*factor.y);
      segments[count].y1=(gint16) MAX(-1, MIN(axis, temp_y));
      segments[count].y2=(gint16) MIN(box->size.x, MAX(axis, temp_y));
      count++;
    }
  }
  for (i=0; i<count; i+=65536) { 
    gdk_draw_segments(box->pixmap,
		data->gc,
      		segments, MIN(65536, count-i));
  }
}

gint
gtk_databox_data_add_x_y(GtkDatabox *box, guint length, 
			gfloat *X, gfloat *Y, 
			GdkColor color, GtkDataboxDataType type,
			guint dot_size)
{
  GtkDataboxData *data;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  g_return_val_if_fail(X, -1);
  g_return_val_if_fail(Y, -1);
  g_return_val_if_fail(length, -1);
  
  box->max_points=MAX(length, box->max_points);
  data=g_new0(GtkDataboxData, 1);
  
  data->X=X;
  data->Y=Y;
  data->length=length;
  data->type=type;
  data->size=dot_size;
  data->flags=0;
  data->color=color;
  data->gc=NULL;
  
    
  box->data=g_list_append(box->data, data);
  
  return g_list_length(box->data)-1;
}

gint
gtk_databox_data_add_x(GtkDatabox *box, guint length, 
			gfloat *X, gint shared_Y_index,
			GdkColor color, GtkDataboxDataType type,
			guint dot_size)
{
  GtkDataboxData *data;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  g_return_val_if_fail(X, -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, shared_Y_index);
  g_return_val_if_fail(data, -1);
  g_return_val_if_fail(data->length==length, -1);
  
  return gtk_databox_data_add_x_y(box, length, X, data->Y, color, type, dot_size);
}

gint
gtk_databox_data_add_y(GtkDatabox *box, guint length, 
			gfloat *Y, gint shared_X_index,
			GdkColor color, GtkDataboxDataType type,
			guint dot_size)
{
  GtkDataboxData *data;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), -1);
  g_return_val_if_fail(Y, -1);
  
  data=(GtkDataboxData *) g_list_nth_data(box->data, shared_X_index);
  g_return_val_if_fail(data, -1);
  g_return_val_if_fail(data->length==length, -1);
  
  return gtk_databox_data_add_x_y(box, length, data->X, Y, color, type, dot_size);
}

static gint
gtk_databox_check_x_links(GList *list, gfloat *values)
{
  GtkDataboxData *data;
  gint counter=0;
  
  if (list) data=(GtkDataboxData *) list->data;
  else return 0; 
  
  while(data) {
    if (data->X==values) counter++;
    list=g_list_next(list);
    if (list) data=(GtkDataboxData *) list->data;
    else data=NULL; 
  }
  
  return counter;    
}

static gint
gtk_databox_check_y_links(GList *list, gfloat *values)
{
  GtkDataboxData *data;
  gint counter=0;
  
  if (list) data=(GtkDataboxData *) list->data;
  else return 0; 
  
  while(data) {
    if (data->Y==values) counter++;
    list=g_list_next(list);
    if (list) data=(GtkDataboxData *) list->data;
    else data=NULL; 
  }
  
  return counter;    
}

static void
gtk_databox_destroy_data(GtkDatabox *box, GtkDataboxData *data, GList *list, gboolean free_flag)
{
  GdkColormap *colormap;

  if (free_flag && gtk_databox_check_x_links(box->data, data->X)==1) {
    g_free(data->X);
  }
  if (free_flag && gtk_databox_check_y_links(box->data, data->Y)==1) {
    g_free(data->Y);
  }
  if (data->flags&(1<<GTK_DATABOX_DATA_HAS_GC)) {
    colormap=gtk_widget_get_colormap(box->draw);
    gdk_colormap_free_colors(colormap, &data->color, 1);
  }
  if (data->gc)
    gdk_gc_destroy(data->gc);
  
  g_free(data);
    
}

gint
gtk_databox_data_destroy_all_with_flag(GtkDatabox *box, gboolean free_flag)
{
  GList *list=NULL;
  GtkDataboxData *data=NULL;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), 0);

  if (!box->data) return 0;
  
  list=box->data;
  if (list) data=(GtkDataboxData *) list->data;
  else data=NULL;
  
  while(data) {
    gtk_databox_destroy_data(box, data, list, free_flag);
    
    list=g_list_next(list);
    if (list) data=(GtkDataboxData *) list->data;
    else data=NULL;
  }
  
  g_list_free(box->data);
  
  box->data=NULL;
  box->max_points=0;
  
  return 0;  
}

gint
gtk_databox_data_destroy_with_flag(GtkDatabox *box, gint index, gboolean free_flag)
{
  GList *list=NULL;
  GtkDataboxData *data=NULL;
  
  g_return_val_if_fail(GTK_IS_DATABOX(box), 0);

  if (!box->data) return -1;
  
  list=g_list_nth(box->data, index);
  if (list) data=(GtkDataboxData *) list->data;
  else return -1;
  
  gtk_databox_destroy_data(box, data, list, free_flag);
    
  box->data=g_list_remove_link(box->data, list);
  g_list_free_1(list);
  
  return 0;  
}

gint
gtk_databox_data_remove_all(GtkDatabox *box)
{
  return gtk_databox_data_destroy_all_with_flag(box, FALSE);
}

gint
gtk_databox_data_remove(GtkDatabox *box, gint index)
{
  return gtk_databox_data_destroy_with_flag(box, index, FALSE);
}

gint
gtk_databox_data_destroy_all(GtkDatabox *box)
{
  return gtk_databox_data_destroy_all_with_flag(box, TRUE);
}

gint
gtk_databox_data_destroy(GtkDatabox *box, gint index)
{
  return gtk_databox_data_destroy_with_flag(box, index, TRUE);
}

/* add-ons */
GtkWidget*
gtk_databox_get_drawing_area(GtkDatabox *box)
{
  return box->draw;
}
