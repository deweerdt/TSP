
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

#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>

#include <pthread.h>

#include "plotwindow.h"

#ifdef __OpenBSD__
#include <sys/limits.h>
#define MAXINT INT_MAX
#endif /* __OpenBSD__ */

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>


#define MARGIN 60

#define XY_MODE_MAX_POINT 200
#define MAX_SAMPLE_NUMBER 10000
#define SCROLL_RATIO 0.10
#define AUTOSCALE_RATIO 0.08

#define WIDGET_INIT_SIZE_X 120
#define WIDGET_INIT_SIZE_Y 100

/* 0.5f is there to solve round problems */
#define COORD_SAMP_TO_PIX(bias, scale, coord)    ((bias) + (double)(coord) * (scale) + 0.5f)
#define COORD_SAMP_TO_PIX_X(pw, x) COORD_SAMP_TO_PIX((pw)->mapping.x_bias, (pw)->mapping.x_scale, x)
#define COORD_SAMP_TO_PIX_Y(pw, y) COORD_SAMP_TO_PIX((pw)->mapping.y_bias, (pw)->mapping.y_scale, y)

#define IS_SAME_PIXEL(pix1, pix2)  ( (pix1.x == pix2.x) && (pix1.y == pix2.y) )

/* pixel coord boundaries inside plotting area */
#define X_LEFT_MARGIN  (MARGIN*9/10) /* need area for Y axis */
#define X_RIGHT_MARGIN (MARGIN-X_LEFT_MARGIN)
#define X_LEFT_IN(pw)  X_LEFT_MARGIN /* need area for Y axisds */
#define X_RIGHT_IN(pw) (pw->widget.allocation.width-X_RIGHT_MARGIN)
#define Y_LOW_IN(pw)   (pw->widget.allocation.height-(MARGIN/2))
#define Y_TOP_IN(pw)   (MARGIN/2)

/* pixel coord boundaries outside plotting area */
#define X_LEFT_OUT(pw)  (X_LEFT_IN(pw) - 1)
#define X_RIGHT_OUT(pw) (X_RIGHT_IN(pw) + 1)
#define Y_LOW_OUT(pw)   (Y_LOW_IN(pw) + 1)
#define Y_TOP_OUT(pw)   (Y_TOP_IN(pw) - 1)





static void plotwindow_class_init (PlotWindowClass *class);
static void plotwindow_init (PlotWindow *pw);
static void plotwindow_realize (GtkWidget *widget);
static void plotwindow_size_allocate(GtkWidget *widget, GtkAllocation *allocation);
static gint plotwindow_expose(GtkWidget *widget, GdkEventExpose *event) ;
static void render_now (PlotWindow *pw);
  
gint paint_data (void *arg);
void draw_grids(PlotWindow *pw, GdkDrawable *where, int partial);
void *background_drawing (void *arg);
gint generator( void *arg);

    
static GtkWidgetClass *parent_class = NULL;

struct GtkTypeInfo
{
  gchar			*type_name;
  guint			 object_size;
  guint			 class_size;
  GtkClassInitFunc	 class_init_func;
  GtkObjectInitFunc	 object_init_func;
  gpointer		 reserved_1;
  gpointer		 reserved_2;
  GtkClassInitFunc	 base_class_init_func;
};


GtkType
plotwindow_get_type () {
  static guint pw_type = 0;
  if (!pw_type) {
    GtkTypeInfo pw_info = {
      "PlotWindow",
      sizeof(PlotWindow),
      sizeof(PlotWindowClass),
      (GtkClassInitFunc) plotwindow_class_init,
      (GtkObjectInitFunc) plotwindow_init,
      NULL,
      NULL,
      (GtkClassInitFunc) NULL
    };
    pw_type = gtk_type_unique (gtk_widget_get_type(), &pw_info);
  }
  return pw_type;
}

/* Initializes the fields of the widget's class structure, and sets up any signals for the class */
static void
plotwindow_class_init (PlotWindowClass *class) {
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
    
  parent_class = gtk_type_class (gtk_widget_get_type ());
  widget_class->expose_event = plotwindow_expose;
  widget_class->realize = plotwindow_realize;
  widget_class->size_allocate = plotwindow_size_allocate;
  widget_class->size_allocate = plotwindow_size_allocate;
    
}

static void 
plotwindow_size_allocate (GtkWidget *widget, GtkAllocation *allocation) {
  PlotWindow *pw;
    
  g_return_if_fail (widget != NULL);
  g_return_if_fail (IS_PLOTWINDOW (widget));
  g_return_if_fail (allocation != NULL);

  widget->allocation = *allocation;
  pw = PLOTWINDOW (widget);
    
  if (GTK_WIDGET_REALIZED (widget)) {

    pw->scale_dirty = TRUE;

    gdk_window_move_resize (widget->window,
			    allocation->x, allocation->y,
			    allocation->width, allocation->height);
  }
}
  
/*inline */gint RGB (gint r, gint g, gint b) {
  return (r&0xff) << 16 | (g&0xff) << 8 | (b&0xff);
}


  
#define MAX_DEF 1280
  
/* Sets the fields of the structure to default values. */
static void
plotwindow_init (PlotWindow *pw) {
    
  pw->all_points_bbox.minX = pw->all_points_bbox.minY = MAXINT;
  pw->all_points_bbox.maxX = pw->all_points_bbox.maxY = -MAXINT;

  /* Horizontal scrolling */
  /* FIXME : for a X/Y widget, do filterY_on = FALSE */

  /*FIXME*/
  pw->time_mode = FALSE;

  pw->ready_buffer = NULL;
  pw->data_dirty = FALSE;
  pw->scale_dirty = TRUE;
  pw->scroll_duration = 0;
  pw->prev_pt_exists = FALSE;
  pw->title = "";
  pw->clear_gc = NULL;
  pw->point_gc = NULL;
  pw->scroll_gc = NULL;
  pw->axis_gc = NULL;
  pw->box_gc = NULL;
  pw->small_font = NULL;
  pw->big_font = NULL;
  pw->ready_buffer = NULL;
  pw->array_sample.nb_sample = 0;
  pw->array_sample.current = 0;
  pw->array_sample.first = 0;
  pw->array_sample.samples = NULL;
  pw->array_sample.max_nb_sample = MAX_SAMPLE_NUMBER;
  pw->points = NULL;
  gtk_widget_set_usize(GTK_WIDGET(pw),WIDGET_INIT_SIZE_X,WIDGET_INIT_SIZE_Y);
}
  

/* Creates and returns the new object */
GtkWidget *
plotwindow_new () {
  return GTK_WIDGET(gtk_type_new(plotwindow_get_type()));
}

/* Creates an X window for the widget if it has one */
static void
plotwindow_realize (GtkWidget *widget) {
  GdkWindowAttr attributes;
  PlotWindow *pw;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (IS_PLOTWINDOW (widget));
  
  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.event_mask = gtk_widget_get_events (widget) | 
    GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK | 
    GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK |
    GDK_POINTER_MOTION_HINT_MASK;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
    
  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new (widget->parent->window, &attributes, attributes_mask);
  widget->style = gtk_style_attach (widget->style, widget->window);
    
  gdk_window_set_user_data (widget->window, widget);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_ACTIVE);

  pw = PLOTWINDOW(widget);
}
  
gint paint_data (void *arg) 
{
  PlotWindow *pw = (PlotWindow *)arg;
  /* If nothing was read, return */
  /*if (!pw->data_dirty) return TRUE;*/
  gtk_widget_draw (GTK_WIDGET(pw), NULL);
  /*pw->data_dirty = FALSE;*/
  return TRUE;
}




/** 
   * Return an array sample element.
   */
#define array_sample_index(p_array, i) ((p_array)->samples[(i + (p_array)->first) % (p_array)->nb_sample ])


/** 
   * Add an array sample element.
   */
static void array_sample_add_sample(array_sample_t* array, DoublePoint* pt)
{
  (array)->samples[(array)->current] = *pt;
  (array)->current = ((array)->current + 1 ) % (array)->max_nb_sample;
    
  if((array)->nb_sample != (array)->max_nb_sample)
    {
      (array)->nb_sample++;      
      (array)->first = 0;
    }
  else
    {
      (array)->first = (array)->current;
    }
}

/**
   * Recompute the scale <--> screen mapping
   */
static void calc_mapping(PlotWindow* pw)
{
  pw->mapping.x_len = (pw->widget.allocation.width-MARGIN);
  pw->mapping.y_len = -(pw->widget.allocation.height-MARGIN);
  if ( (pw->all_points_bbox.maxX-pw->all_points_bbox.minX) != 0 )
    pw->mapping.x_scale = pw->mapping.x_len / (pw->all_points_bbox.maxX-pw->all_points_bbox.minX);
  else
    pw->mapping.x_scale = 1;
  if ( (pw->all_points_bbox.maxY-pw->all_points_bbox.minY) != 0 )
    pw->mapping.y_scale = pw->mapping.y_len / (pw->all_points_bbox.maxY-pw->all_points_bbox.minY);
  else
    pw->mapping.y_scale = 1;
  pw->mapping.x_bias = -(pw->all_points_bbox.minX*pw->mapping.x_scale)+(double)X_LEFT_IN(pw);
  pw->mapping.y_bias = -(pw->all_points_bbox.maxY*pw->mapping.y_scale)+(double)Y_TOP_IN(pw);
  pw->mapping.x_scroll_len = 
    pw->mapping.x_len * ( pw->scroll_duration / (pw->all_points_bbox.maxX-pw->all_points_bbox.minX ) ) + .5f;
}

/**
 * Transform a sample to a pixel
 */  
static void add_point_to_pixel(PlotWindow* pw, DoublePoint* pt)
{    
  /* FIXME : mechante optimisation a faire */
  int add_pixel = FALSE;
  PixelPoint pix;
  PixelPoint prev_pix;

  /* OPTIMIZE (seed end of file if optimisation needed) */
  add_pixel = TRUE;

  /* Now, must we add a pixel ? */
  if(add_pixel && pw->prev_pt_exists)
    {
      pix.x =      COORD_SAMP_TO_PIX_X(pw, pt->x);
      pix.y =      COORD_SAMP_TO_PIX_Y(pw, pt->y);
      prev_pix.x = COORD_SAMP_TO_PIX_X(pw, pw->prev_pt.x);
      prev_pix.y = COORD_SAMP_TO_PIX_Y(pw, pw->prev_pt.y);
      pw->data_dirty = TRUE;

      if( !IS_SAME_PIXEL(pix, prev_pix) )
	gdk_draw_line (pw->ready_buffer, pw->point_gc,
		       prev_pix.x, prev_pix.y,
		       pix.x, pix.y);
    }        

  pw->prev_pt = *pt;
  pw->prev_pt_exists = TRUE;
}
  

void time_mode_scroll(PlotWindow* pw)
{
  gint width, height, xsrc, xdest, y;

  /* First scroll the bbox */
  pw->all_points_bbox.maxX += pw->scroll_duration;
  pw->all_points_bbox.minX += pw->scroll_duration;

  /* Update mapping with new bbox */
  calc_mapping(pw);

  /*Scroll*/
  width = pw->mapping.x_len - pw->mapping.x_scroll_len + 1;
  height = -pw->mapping.y_len + 1 ;
  xdest = X_LEFT_IN(pw);
  xsrc = xdest + pw->mapping.x_scroll_len;
  y = Y_TOP_IN(pw);

  /* scroll plots */
  gdk_draw_pixmap (pw->ready_buffer, pw->scroll_gc, pw->ready_buffer,
		   xsrc, y,
		   xdest, y,
		   width, height);

  /* Erase old part of plots */
  gdk_draw_rectangle(pw->ready_buffer, pw->clear_gc, TRUE,
		     xdest + width, y,
		     xsrc - xdest,  height);

#if 0
  /* scroll axis */
  gdk_draw_pixmap (pw->ready_buffer, pw->scroll_gc, pw->ready_buffer,
		   xsrc, y+height+1,
		   xdest, y+height+1, 
		   width+X_RIGHT_MARGIN, MARGIN/2);
  
  /* Erase old part of axis */
  gdk_draw_rectangle(pw->ready_buffer, pw->scroll_gc, TRUE,
		     xdest+width+X_RIGHT_MARGIN-1, y+height+1,
		     xsrc-xdest, MARGIN/2);
#endif

		     /* Avoid truncadted figures on the left axis */
  gdk_draw_rectangle(pw->ready_buffer, pw->scroll_gc, TRUE,
		     X_RIGHT_MARGIN, y+height+1,
		     pw->widget.allocation.width-X_RIGHT_MARGIN, MARGIN/2);

  draw_grids(pw, pw->ready_buffer, 1);

}
  

/** Add a point 
 *
 */

void add_point(PlotWindow* pw,DoublePoint* pt)
{
  array_sample_add_sample(&pw->array_sample, pt);

  /* If the scale is already dirty no need to work more,
     we will recompute everything at next display refresh */
  if( ! pw->scale_dirty ) 
    { 
      /* Check to see if the new point stay in the grid */
      if(pt->y <= pw->all_points_bbox.maxY && 
	 pt->y >= pw->all_points_bbox.minY)
	{
	  if (pt->x <= pw->all_points_bbox.maxX && 
	      pt->x >= pw->all_points_bbox.minX)
	    {
	      /* yes. We add the point */
	      add_point_to_pixel(pw, pt);
	    }
	  else
	    {
	      if(! pw->time_mode)
		{
		  /* We must rescal everything */
		  pw->scale_dirty = TRUE;
		}
	      else
		{
		  /* time mode. Scroll everything and plot*/
		  time_mode_scroll(pw);
		  add_point_to_pixel(pw, pt);		    
		}
	    }
	}
      else
	{
	  /* We must rescal everything */
	  pw->scale_dirty = TRUE;
	}
    }       
}

/** Set title
 *
 */
void
set_title(PlotWindow *pw, const gchar* title)
{
  pw->title = title;
}
  
static void create_all_gc(PlotWindow *pw)
{
  GdkColor color;GdkColormap *colormap;

  /* Create buffer pixmap and gc*/
  if(pw->ready_buffer)
    {
      gdk_pixmap_unref(pw->ready_buffer);
    }
  pw->ready_buffer = gdk_pixmap_new(pw->widget.window, pw->widget.allocation.width, pw->widget.allocation.height, -1);

  if( pw->time_mode)
    {
      if(pw->scroll_gc) gdk_gc_unref(pw->scroll_gc);
      pw->scroll_gc = gdk_gc_new(pw->ready_buffer);
    }

  /* Create clear gc to erase the drawings */
  if(pw->clear_gc)
    {
      gdk_gc_unref(pw->clear_gc);
    }
  pw->clear_gc = gdk_gc_new(pw->ready_buffer);
  gdk_color_parse("#000000", &color);
  colormap = gtk_widget_get_colormap(&pw->widget);
  gdk_colormap_alloc_color(colormap, &color, FALSE, TRUE);
  gdk_gc_set_foreground(pw->clear_gc, &color);
	
  if(pw->buffer_gc)
    {
      gdk_gc_unref(pw->buffer_gc);    
    }
  pw->buffer_gc = gdk_gc_new(pw->widget.window);

  /* Create point gc */
  if(pw->point_gc)
    {
      gdk_gc_unref(pw->point_gc);    
    }
  pw->point_gc = gdk_gc_new(pw->ready_buffer);
  gdk_color_parse("#FFFF00", &color);
  colormap = gtk_widget_get_colormap(&pw->widget);
  gdk_colormap_alloc_color(colormap, &color, FALSE, TRUE);
  gdk_gc_set_foreground(pw->point_gc, &color);

  /* Create point gc */
  if(pw->axis_gc)
    {
      gdk_gc_unref(pw->axis_gc);    
    }
  pw->axis_gc = gdk_gc_new(pw->ready_buffer);
  gdk_color_parse("#888888", &color);
  colormap = gtk_widget_get_colormap(&pw->widget);
  gdk_colormap_alloc_color(colormap, &color, FALSE, TRUE);
  gdk_gc_set_foreground(pw->axis_gc, &color);

  /* Create box gc */
  if(pw->box_gc)
    {
      gdk_gc_unref(pw->box_gc);    
    }
  pw->box_gc = gdk_gc_new(pw->ready_buffer);
  gdk_color_parse("#FFFFFF", &color);
  colormap = gtk_widget_get_colormap(&pw->widget);
  gdk_colormap_alloc_color(colormap, &color, FALSE, TRUE);
  gdk_gc_set_foreground(pw->box_gc, &color);

  if (!pw->big_font)
    {
      pw->big_font = gdk_font_load("-misc-fixed-bold-r-normal-*-*-*-*-*-*-*-*-*");
      if (!pw->big_font)
	{
	  pw->big_font = gdk_font_load("-misc-fixed-*-*-*-*-*-*-*-*-*-*-*-*");
	}
    }
  
  if (!pw->small_font)
    {
      pw->small_font = gdk_font_load("-misc-fixed-medium-r-semicondensed-*-*-*-*-*-*-*-*-*"); /*80 a slot 6*/
      if (!pw->small_font)
	{
	  pw->small_font = gdk_font_load("-misc-fixed-*-*-*-*-*-*-*-*-*-*-*-*");
	}
    }

}

static void calc_scale(PlotWindow *pw)
{
  int n;
  DoublePoint* p;
	
  for (n=0; n < pw->array_sample.nb_sample; n++)
    {
      p = &array_sample_index(&pw->array_sample, n);
      if (n == 0) {
	pw->all_points_bbox.maxX = p->x;
	pw->all_points_bbox.minX = p->x;
	pw->all_points_bbox.maxY = p->y;
	pw->all_points_bbox.minY = p->y;
      }
      
      if (p->x > pw->all_points_bbox.maxX) pw->all_points_bbox.maxX  = p->x;
      else if (p->x < pw->all_points_bbox.minX) pw->all_points_bbox.minX = p->x;
      if (p->y > pw->all_points_bbox.maxY) pw->all_points_bbox.maxY = p->y;
      else if (p->y < pw->all_points_bbox.minY) pw->all_points_bbox.minY = p->y;	
    }

  /* avoid 'always-autoscaling-phenomena' */
  {   
    double secur_y = (pw->all_points_bbox.maxY - pw->all_points_bbox.minY) * AUTOSCALE_RATIO;
    pw->all_points_bbox.maxY += secur_y;
    pw->all_points_bbox.minY -= secur_y;
    if( ! pw->time_mode )
      {	
	double secur_x = (pw->all_points_bbox.maxX - pw->all_points_bbox.minX) * AUTOSCALE_RATIO;
	pw->all_points_bbox.maxX += secur_x;
	pw->all_points_bbox.minX -= secur_x;
      }
    else
      {
	pw->all_points_bbox.maxX = pw->all_points_bbox.minX + pw->duration_sec;
      }
  }
    
  /* ---------------------------- */
  /* Update mapping with new bbox */
  /* ---------------------------- */
  calc_mapping(pw);

  /* ---------------------------- */
  /* Update some flags            */
  /* ---------------------------- */
  pw->scale_dirty = FALSE;
  /*pw->prev_pt_exists = FALSE;*/

  /* ---------------------------- */
  /* Create all GC                */
  /* ---------------------------- */
  create_all_gc(pw);
}


#define BUFFER_SIZE 1024
#define DESCRIPTOR 0

/* It makes the necessary calls to the drawing functions to draw the exposed portion on the screen. */
static gint
plotwindow_expose (GtkWidget      *widget,
		   GdkEventExpose *event)  {

  PlotWindow *pw;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (IS_PLOTWINDOW (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);
  
  pw = PLOTWINDOW(widget);
            
  render_now(pw);
  if(pw->ready_buffer)
    {
      gdk_draw_pixmap (pw->widget.window,
		       pw->buffer_gc,
		       pw->ready_buffer,
		       0,0,
		       0,0,
		       pw->widget.allocation.width,
		       pw->widget.allocation.height);
    }

  return FALSE;
}


  
static void clear_widget(PlotWindow *pw, GdkDrawable *where)
{
  gdk_draw_rectangle(where,
		     pw->clear_gc,
		     TRUE,
		     0, 0,
		     pw->widget.allocation.width, 
		     pw->widget.allocation.height 
		     );    
}

static void draw_all_points(PlotWindow *pw, GdkDrawable *where)
{
  int i;
  int nb_pix;
  DoublePoint* pt;    

  for ( i=0, nb_pix = 0 ; i < pw->array_sample.nb_sample; i++)
    {
      pt = &array_sample_index(&pw->array_sample, i);
      pw->points[nb_pix].x = COORD_SAMP_TO_PIX_X(pw,pt->x);
      pw->points[nb_pix].y = COORD_SAMP_TO_PIX_Y(pw,pt->y);

      /* Filter identical pixels */
      if( (nb_pix == 0) ||
	  !IS_SAME_PIXEL(pw->points[nb_pix], pw->points[nb_pix-1]) )
	{
	  nb_pix++;
	}	
    }

  gdk_draw_lines(where, pw->point_gc, pw->points, nb_pix);

  /* We prepare the point by point drawing with the latest point */
  if( pw->array_sample.nb_sample > 0)
    {
      pw->prev_pt = array_sample_index(&pw->array_sample, pw->array_sample.nb_sample - 1);
      pw->prev_pt_exists = TRUE;
    }
}

static void render_now (PlotWindow *pw) {
  /* If scale must change, recompute it */
  if(pw->scale_dirty)
    {
      if( pw->array_sample.nb_sample >= 2)
	{
	  /* calc scale */
	  calc_scale(pw);	

	  /* FIXME : pw->ready_buffer est initialisé dans calc_scale */
	  clear_widget(pw, pw->ready_buffer);
	  draw_grids(pw, pw->ready_buffer, 0);
	  draw_all_points(pw, pw->ready_buffer);	    
	}
    }    
  else if(pw->data_dirty)
    {
      /*printf("data\n");*/
    }
}



/*
 * Draw Grid
 */
#define NB_GRIDS_MAX 10
void draw_grids(PlotWindow *pw, GdkDrawable *where, int partial) {
  int title_height, char_size, px, py;
  double unit, step, x, y, nb_grids;
  char buffer[512];
  
  title_height = gdk_text_height(pw->big_font,pw->title, strlen(pw->title));
    
  /* X axis (outside of the drawing area) */
  gdk_draw_line(where,
		pw->box_gc,
		X_LEFT_OUT(pw),Y_LOW_OUT(pw),
		X_RIGHT_OUT(pw),Y_LOW_OUT(pw));

  /* Y axis (outside of the drawing area) */
  gdk_draw_line(where,
		pw->box_gc,
		X_LEFT_OUT(pw),Y_TOP_OUT(pw),
		X_LEFT_OUT(pw),Y_LOW_OUT(pw));

  /* X top limit (outside of the drawing area)*/
  gdk_draw_line(where,
		pw->box_gc,
		X_LEFT_OUT(pw),Y_TOP_OUT(pw),
		X_RIGHT_OUT(pw),Y_TOP_OUT(pw));

  /* Y top limit (outside of the drawing area)*/
  gdk_draw_line(where,
		pw->box_gc,
		X_RIGHT_OUT(pw),Y_TOP_OUT(pw),
		X_RIGHT_OUT(pw),Y_LOW_OUT(pw));

  /* X grids */
  sprintf(buffer, " %g ", (double)pw->all_points_bbox.maxX);
  char_size = gdk_text_width(pw->small_font,buffer, strlen(buffer));
  nb_grids = (double)(pw->widget.allocation.width/char_size) ; /*nb digits per unit */
  if (nb_grids>NB_GRIDS_MAX)	nb_grids = NB_GRIDS_MAX;
  if (nb_grids<1)	nb_grids = 1;
  step = (pw->all_points_bbox.maxX - pw->all_points_bbox.minX)/(nb_grids); /* compute the average step */
  unit = pow(10, floor(log10(step)));  /* round it to the next 10 power */
  step = ceil(step/unit)*unit;
  if (step==0)	step=1;

  for (x = (int)(pw->all_points_bbox.minX/step)*step; x<pw->all_points_bbox.maxX; x+= step)
  {
    if ( x>pw->all_points_bbox.minX )
      {
	px = COORD_SAMP_TO_PIX_X (pw, x);
	sprintf(buffer, "%g", (double)x);
	gdk_draw_string(where, pw->small_font, pw->point_gc,
			px-(gdk_text_width(pw->small_font,buffer, strlen(buffer))/2.0), 
			(pw->widget.allocation.height)- (gdk_text_height(pw->small_font,buffer, strlen(buffer))),
			buffer);
	
	if (partial && (x < (pw->all_points_bbox.maxX-pw->scroll_duration)) )
	  {
	    /* do nothing, point is either outisde box or already traced  */
	  }
	else 
	  {
	    gdk_draw_line(where, (x==0) ? pw->box_gc : pw->axis_gc, 
			  px, Y_LOW_IN(pw),
			  px, Y_TOP_IN(pw));
	  } /* not partial */
      } /* inside box */
  } /* end for */
 
  /* Y grids  */
  char_size = gdk_text_height(pw->small_font,"9", 1);
  nb_grids = (pw->widget.allocation.height / char_size /5) ; /*2 digits per unit */
  if (nb_grids>NB_GRIDS_MAX)
    nb_grids = NB_GRIDS_MAX;
  step = (pw->all_points_bbox.maxY - pw->all_points_bbox.minY)/nb_grids; /* compute the average step */
  unit = pow(10, floor(log10(step)));  /* round it to the next 10 power */
  if (unit != 0)
    step = ceil((step)/unit)*unit;
  else 
    step = 1;
  if (step==0) step=1;

  px = COORD_SAMP_TO_PIX_X(pw, pw->all_points_bbox.maxX-pw->scroll_duration);
  for (y = (int)(pw->all_points_bbox.minY/step)*step; y<pw->all_points_bbox.maxY; y+= step)
  {
    if ( y>pw->all_points_bbox.minY )
      {
	py = COORD_SAMP_TO_PIX_Y (pw, y);
	sprintf(buffer, " %.3g ", y);
	gdk_draw_line(where, (y==0) ? pw->box_gc : pw->axis_gc,
		      partial ? px : X_LEFT_IN(pw), py,
		      X_RIGHT_IN(pw), py);
	if (!partial)
	  gdk_draw_string(where, pw->small_font, pw->point_gc,
			  X_LEFT_IN(pw)-gdk_text_width(pw->small_font,buffer,strlen(buffer)), 
			  py + gdk_text_height(pw->small_font,buffer,strlen(buffer))/2, 
			  buffer);
      } /* inside box */
  } /* end for */

  /* Title */
  gdk_draw_string(where, pw->big_font, pw->point_gc,
		  pw->widget.allocation.width/2-(gdk_text_width(pw->big_font,pw->title, strlen(pw->title))/2), 
		  title_height*2, pw->title );
}

void set_time_mode(PlotWindow* pw, double duration_sec, double frequency_hz)
{
  pw->time_mode = TRUE;
  if (frequency_hz==0)	frequency_hz=1; /* not a real frequency, but allow to plot by item */
  pw->array_sample.max_nb_sample = duration_sec * frequency_hz;
  pw->array_sample.samples = (DoublePoint*)g_malloc0(pw->array_sample.max_nb_sample * sizeof(DoublePoint));
  pw->points = (GdkPoint*)g_malloc0(pw->array_sample.max_nb_sample * sizeof(GdkPoint));
  pw->duration_sec = duration_sec;
  pw->scroll_duration = pw->duration_sec * SCROLL_RATIO;
}






/* If need to optimize the add pixel function */
#if 0
  /* At least one elements ? */
  if( latest_index >= 0 )
    {
      PixelPoint* pix_last = &g_array_index(pw->pixel, PixelPoint ,latest_index );
	
      if( pix_last->x != pix.x)
	{
	  add_pixel = TRUE;
	  x_pixel = TRUE; 
	}
      else		  
	{
	  if( pix_last->y != pix.y)
	    {
	      /* With no Y filter : same as X */
	      if( ! pw->time_mode)
		{
		  add_pixel = TRUE;
		}
	      else /* filter Y values */
		{
		  /* At least two elements ? */
		  if( latest_index >= 1 ) 
		    {
		      /* Compare two latest elements of pixel array */
		      PixelPoint* pix_prev = &g_array_index(pw->pixel, PixelPoint ,latest_index - 1);
		      if( pix_last->x !=  pix_prev->x)
			{
			  add_pixel = TRUE;
			}
		      else
			{
			  /* If the new pix is vertically between the two lastest pix,
			     we do nothing */
			  if( pix_last->y > pix_prev->y)
			    {
			      if( pix.y > pix_last->y ){ 
				pw->data_dirty = TRUE;
				pix_last->y = pix.y;
			      } 
			      else if( pix.y < pix_prev->y ){ 
				pw->data_dirty = TRUE;
				pix_prev->y = pix.y;
			      }
			    }
			  else if ( pix_last->y < pix_prev->y)
			    {
			      if( pix.y > pix_prev->y ){ 
				pw->data_dirty = TRUE;
				pix_prev->y = pix.y;
			      } 
			      else if( pix.y < pix_last->y ){ 
				pw->data_dirty = TRUE;
				pix_last->y = pix.y;
			      }       				
			    }
			  else
			    {/* bug ! */
			      assert(0);
			    }
			}			
		    }
		  else
		    {
		      add_pixel = TRUE;
		    }		      
		}		    		    
	    }		    		    		    
	}
    }
  else
    {
      add_pixel = TRUE;
    }
#endif

