/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
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
#ifndef __PLOTWINDOW_H__
#define __PLOTWINDOW_H__


#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>
#include <values.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  

#define RTP_MIN_RANGE 1E-100
#define RTP_MIN_RES 1E-12

typedef struct _DoublePoint {
  double x,y;
} DoublePoint;

typedef struct _PixelPoint {
  int x,y;
} PixelPoint;


typedef struct _BBox {
  gdouble minX, maxX, minY, maxY;
} BBox;

typedef struct _RtpMapping {
  double xscale, yscale;
  double xbias, ybias;
  BBox viewPort;
} RtpMapping;

typedef struct _draw_grids_args {
  void *pw;
  GdkEventExpose *event;
  GdkGC *gc;
  BBox viewPort;
  GdkDrawable * drawable;
} draw_grids_args;


  typedef struct _PW_Mapping {
    gdouble x_len;
    gdouble y_len;
    gdouble x_scale;
    gdouble y_scale;
    gdouble x_bias;
    gdouble y_bias;
    gint x_scroll_len;
  } PW_Mapping;


  typedef struct _array_sample_t
  {
    DoublePoint* samples;
    int          nb_sample;
    int          current;
    int          first;
    int          max_nb_sample;
  }array_sample_t;

  /***********************
   *  Plot Window Widget
   ***********************/
#define PLOTWINDOW(obj)          GTK_CHECK_CAST (obj, plotwindow_get_type (), PlotWindow)
#define PLOTWINDOW_CLASS(klass)  GTK_CHECK_CLASS_CAST (klass, plotwindow_get_type (), PlotWindowClass)
#define IS_PLOTWINDOW(obj)       GTK_CHECK_TYPE (obj, plotwindow_get_type ())


#define PLOTWINDOW_PREFERRED_WIDTH 100
#define PLOTWINDOW_PREFERRED_HEIGHT 100
  typedef struct _PlotWindow PlotWindow;
  typedef struct _PlotWindowClass PlotWindowClass;

#define BUFFER_SIZE 1024

  
  struct _PlotWindow {
    GtkWidget widget;

    /** Use time_mode = FALSE for a y = f(x) widget, 
	use time_mode = TRUE for a y = f(t) widget */	
    int time_mode;
    gdouble duration_sec;
    gdouble scroll_duration;

    array_sample_t array_sample;
    GdkPoint* points;

    PW_Mapping mapping;
    BBox all_points_bbox;   

    int scale_dirty;
    gboolean data_dirty;

    DoublePoint prev_pt;
    int prev_pt_exists;

    const gchar* title;
    BBox view_port;
    int fd_input;
    
    GdkPixmap *ready_buffer;
    GdkGC *buffer_gc;
    GdkGC *clear_gc;
    GdkGC *point_gc;
    GdkGC *scroll_gc;
    GdkGC *axis_gc;
    GdkGC *box_gc;

    GdkFont *small_font;
    GdkFont *big_font;

    //  pthread_mutex_t data_points_mutex;
    pthread_t acquire_data_thread;
    pthread_t background_drawing_thread;
    
    /*  BBox _allPointsBBox; // Bounding box of all points received
	BBox _trackingPort;
	enum {AUTO_SCALE, USER_FIXED, TRACKING} _viewMode;*/
  };

  struct _PlotWindowClass {
    GtkWidgetClass parent_class;
  };
  

  GtkType plotwindow_get_type (void);
  GtkWidget* plotwindow_new (void);
  void display_rendered_scene (PlotWindow *pw, GdkEventExpose *event);

  void add_point(PlotWindow* pw, DoublePoint* pt);

  void set_time_mode(PlotWindow* pw, double duration_sec, double frequency_hz);



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __PLOTWINDOW_H__ */

