/* gtkplotsurface - 3d scientific plots widget for gtk+
 * Copyright 1999-2001  Adrian E. Feiguin <feiguin@ifir.edu.ar>
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <gtk/gtk.h>
#include "gtkplot.h"
#include "gtkplot3d.h"
#include "gtkplotdata.h"
#include "gtkplotsurface.h"
#include "gtkpsfont.h"
#include "gtkplotpc.h"


static void gtk_plot_surface_class_init 	(GtkPlotSurfaceClass *klass);
static void gtk_plot_surface_init 		(GtkPlotSurface *data);
static void gtk_plot_surface_destroy 		(GtkObject *object);
static void gtk_plot_surface_update_range 	(GtkPlotData *data);
static void gtk_plot_surface_real_build_mesh	(GtkPlotSurface *surface);
static void update_data 			(GtkPlotData *data, 
						 gboolean new_range);
static gboolean gtk_plot_surface_add_to_plot 	(GtkPlotData *data,
						 GtkPlot *plot);
static void gtk_plot_surface_draw 		(GtkWidget *widget, 
						 GdkRectangle *area);
static void gtk_plot_surface_draw_private 	(GtkPlotData *data);
static void gtk_plot_surface_draw_legend	(GtkPlotData *data, 
						 gint x, gint y);
static void gtk_plot_surface_real_draw		(GtkPlotSurface *data); 
static void gtk_plot_surface_draw_triangle 	(GtkPlotSurface *surface,
                                		 GtkPlotDTtriangle *triangle,
                                                 gint sign);
static void gtk_plot_surface_lighting 		(GdkColor *a, 
						 GdkColor *b, 
						 gdouble normal,
						 gdouble ambient);
static gint roundint				(gdouble x);
static void hsv_to_rgb 				(gdouble  h, 
						 gdouble  s, 
						 gdouble  v,
            					 gdouble *r, 
						 gdouble *g, 
						 gdouble *b);
static void rgb_to_hsv 				(gdouble  r, 
						 gdouble  g, 
						 gdouble  b,
            					 gdouble *h, 
						 gdouble *s, 
						 gdouble *v);
static gint compare_func                           (gpointer a, gpointer b);


static GtkDataClass *parent_class = NULL;

GtkType
gtk_plot_surface_get_type (void)
{
  static GtkType data_type = 0;

  if (!data_type)
    {
      GtkTypeInfo data_info =
      {
	"GtkPlotSurface",
	sizeof (GtkPlotSurface),
	sizeof (GtkPlotSurfaceClass),
	(GtkClassInitFunc) gtk_plot_surface_class_init,
	(GtkObjectInitFunc) gtk_plot_surface_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      data_type = gtk_type_unique (gtk_plot_data_get_type(), &data_info);
    }
  return data_type;
}

static void
gtk_plot_surface_class_init (GtkPlotSurfaceClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkPlotDataClass *data_class;
  GtkPlotSurfaceClass *surface_class;

  parent_class = gtk_type_class (gtk_plot_data_get_type ());

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  data_class = (GtkPlotDataClass *) klass;
  surface_class = (GtkPlotSurfaceClass *) klass;

  object_class->destroy = gtk_plot_surface_destroy;

  widget_class->draw = gtk_plot_surface_draw;

  data_class->update = update_data;
  data_class->add_to_plot = gtk_plot_surface_add_to_plot;
  data_class->draw_data = gtk_plot_surface_draw_private;
  data_class->draw_legend = gtk_plot_surface_draw_legend;
  surface_class->draw_triangle = gtk_plot_surface_draw_triangle;

}

static void
update_data (GtkPlotData *data, gboolean new_range)
{
  if(new_range && data->is_function) 
    GTK_PLOT_SURFACE(data)->recalc_dt = TRUE;

  GTK_PLOT_DATA_CLASS(parent_class)->update(data, new_range);
}

static void
gtk_plot_surface_update_range (GtkPlotData *data)
{
  GtkPlotSurface *surface;
  GtkPlot *plot;

  if(!data->redraw_pending) return; 

  surface = GTK_PLOT_SURFACE(data);
  plot = data->plot;


  if(!surface->recalc_dt && data->num_points > 0){
     gtk_plot_surface_recalc_nodes(surface);
     data->redraw_pending = FALSE;
     surface->recalc_dt = FALSE;
     return;
  }

  if(!data->is_function && data->num_points > 0){
     if(surface->recalc_dt)
       gtk_plot_surface_build_mesh(surface);
     else
       gtk_plot_surface_recalc_nodes(surface);

     data->redraw_pending = FALSE;
     surface->recalc_dt = FALSE;
     return;
  }

  if(data->is_function){
     gdouble xstep, ystep;
     gdouble xmin, xmax, ymin, ymax;
     gdouble x, y;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gdouble *old_x, *old_y, *old_z;
     gint nx, ny;
     gint npoints;

     xmin = GTK_PLOT(plot)->xmin;
     xmax = GTK_PLOT(plot)->xmax;
     ymin = GTK_PLOT(plot)->ymin;
     ymax = GTK_PLOT(plot)->ymax;

     xstep = surface->xstep;
     surface->nx = roundint((xmax - xmin) / xstep) + 1;

     ystep = surface->ystep;
     surface->ny = roundint((ymax - ymin) / ystep) + 1;

     npoints = surface->nx * surface->ny;
     fx = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fy = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fz = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));

     npoints = 0;
     y = ymin;
     for(ny = 0; ny < surface->ny; ny++)
       {
         x = xmin;
         for(nx = 0; nx < surface->nx; nx++)
          {
            gboolean error;
            fx[npoints] = x;
            fy[npoints] = y;
            fz[npoints] = data->function3d(plot, data, x, y, &error);

            x += xstep;
            npoints++;
          }
         y += ystep;
     }

     old_x = data->x; old_y = data->y; old_z = data->z;

     data->x = fx;
     data->y = fy;
     data->z = fz;

     data->num_points = npoints;
     gtk_plot_surface_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);

     data->x = old_x; data->y = old_y; data->z = old_z;
  }

  data->redraw_pending = FALSE;
  surface->recalc_dt = FALSE;
}

static gboolean 
gtk_plot_surface_add_to_plot (GtkPlotData *data, GtkPlot *plot)
{
  if(!data->is_function && data->num_points > 0){
    gtk_plot_surface_build_mesh(GTK_PLOT_SURFACE(data));
  }

  data->redraw_pending = TRUE;

  return TRUE;
}

static void
gtk_plot_surface_init (GtkPlotSurface *dataset)
{
  GtkWidget *widget;
  GdkColormap *colormap;
  GdkColor color;

  GTK_WIDGET_SET_FLAGS(dataset, GTK_NO_WINDOW);

  widget = GTK_WIDGET(dataset);
  colormap = gtk_widget_get_colormap(widget);

  gdk_color_parse("black", &color);
  gdk_color_alloc(colormap, &color);
  dataset->grid_foreground = color;

  gdk_color_parse("dark green", &color);
  gdk_color_alloc(colormap, &color);
  dataset->grid_background = color;

  gdk_color_parse("gray30", &color);
  gdk_color_alloc(colormap, &color);
  dataset->shadow = color;

  gdk_color_parse("blue", &color);
  gdk_color_alloc(colormap, &color);
  dataset->color = color;

  dataset->light.x = 0.;
  dataset->light.y = 0.;
  dataset->light.z = 1.;

  dataset->nx = 0;
  dataset->ny = 0;

  dataset->show_grid = TRUE;
  dataset->show_mesh = FALSE;
  dataset->transparent = FALSE;
  dataset->height_gradient = FALSE;

  dataset->ambient = 0.3;

  dataset->xstep = 0.05;
  dataset->ystep = 0.05;

  dataset->mesh_line.color = dataset->grid_foreground;
  dataset->mesh_line.line_width = 1;
  dataset->mesh_line.line_style = GTK_PLOT_LINE_SOLID;

  dataset->dt = GTK_PLOT_DT(gtk_plot_dt_new(0));
  dataset->recalc_dt = TRUE;
}

GtkWidget*
gtk_plot_surface_new (void)
{
  GtkPlotData *data;

  data = gtk_type_new (gtk_plot_surface_get_type ());

  return GTK_WIDGET (data);
}

GtkWidget*
gtk_plot_surface_new_function (GtkPlotFunc3D function)
{
  GtkWidget *dataset;

  dataset = gtk_type_new (gtk_plot_surface_get_type ());

  gtk_plot_surface_construct_function(GTK_PLOT_SURFACE(dataset), function);

  return dataset;
}

void
gtk_plot_surface_construct_function(GtkPlotSurface *surface, 
				   GtkPlotFunc3D function)
{
  GTK_PLOT_DATA(surface)->is_function = TRUE;
  GTK_PLOT_DATA(surface)->function3d = function;
}

void
gtk_plot_surface_destroy(GtkObject *object) 
{
  GtkPlotSurface *surface;
 
  surface = GTK_PLOT_SURFACE(object);

  if(surface->dt)
    gtk_object_destroy(GTK_OBJECT(surface->dt));

  if ( GTK_OBJECT_CLASS (parent_class)->destroy )
    (* GTK_OBJECT_CLASS (parent_class)->destroy) (object);
}

static void
gtk_plot_surface_draw (GtkWidget *widget, GdkRectangle *area)
{
  if(!GTK_WIDGET_VISIBLE(widget)) return;

  gtk_plot_surface_draw_private(GTK_PLOT_DATA(widget));
}

static void
gtk_plot_surface_draw_private   (GtkPlotData *data)  
{
  GtkPlot *plot;
  GtkPlotSurface *surface;

  g_return_if_fail(GTK_IS_PLOT_SURFACE(data));

  surface = GTK_PLOT_SURFACE(data);
  data = GTK_PLOT_DATA(surface);

  g_return_if_fail(GTK_PLOT_DATA(data)->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(GTK_PLOT_DATA(data)->plot));

  if(data->redraw_pending) gtk_plot_surface_update_range(data);

  plot = GTK_PLOT(data->plot);

  gtk_plot_surface_real_draw(surface);
}

static void
gtk_plot_surface_real_draw   (GtkPlotSurface *surface)  
{
  GtkWidget *widget;
  GtkPlot *plot = NULL;
  GtkPlotData *dataset;
  GList *list;

  g_return_if_fail(GTK_IS_PLOT_SURFACE(surface));

  dataset = GTK_PLOT_DATA(surface);

  g_return_if_fail(GTK_PLOT_DATA(dataset)->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(GTK_PLOT_DATA(dataset)->plot));

  plot = dataset->plot;
  widget = GTK_WIDGET(plot);

  gtk_plot_pc_gsave(plot->pc);

  if(!GTK_WIDGET_DRAWABLE(widget)) return;
  if(!GTK_WIDGET_VISIBLE(widget)) return;

  gtk_plot_set_line_attributes(plot, surface->mesh_line);

  list = surface->dt->triangles;
  while(list){
    GtkPlotDTtriangle *t;
 
    t = (GtkPlotDTtriangle *)list->data;
    GTK_PLOT_SURFACE_CLASS(GTK_OBJECT(surface)->klass)->draw_triangle(surface, t, -1.); 
    list = list->next;
  }

  gtk_plot_pc_grestore(plot->pc);
}

static void
gtk_plot_surface_draw_triangle (GtkPlotSurface *surface,
                                GtkPlotDTtriangle *triangle,
                                gint sign)
{
  GtkPlot *plot;
  GtkPlotVector side1, side2, light, normal;
  GtkPlotVector points[3];
  GtkPlotPoint t[3];
  GdkDrawable *drawable;
  GdkColor color, real_color;
  gdouble factor, norm;
  gboolean visible = TRUE;

  plot = GTK_PLOT(GTK_PLOT_DATA(surface)->plot);
  drawable = plot->drawable;

  points[0].x = triangle->na->x;
  points[0].y = triangle->na->y;
  points[0].z = triangle->na->z;
  points[1].x = triangle->nb->x;
  points[1].y = triangle->nb->y;
  points[1].z = triangle->nb->z;
  points[2].x = triangle->nc->x;
  points[2].y = triangle->nc->y;
  points[2].z = triangle->nc->z;

  t[0].x = triangle->na->px;
  t[0].y = triangle->na->py;
  t[1].x = triangle->nb->px;
  t[1].y = triangle->nb->py;
  t[2].x = triangle->nc->px;
  t[2].y = triangle->nc->py;

  side1.x = (points[1].x - points[0].x) * sign;
  side1.y = (points[1].y - points[0].y) * sign;
  side1.z = (points[1].z - points[0].z) * sign;
  side2.x = (points[2].x - points[0].x) * sign;
  side2.y = (points[2].y - points[0].y) * sign;
  side2.z = (points[2].z - points[0].z) * sign;
         
  if(surface->height_gradient){
    gtk_plot_data_get_gradient_level(GTK_PLOT_DATA(surface),
				     (triangle->na->z +
                                      triangle->nb->z +
				      triangle->nc->z) / 3.0,
				     &real_color); 
  }else{
    color = surface->color; 
    light = surface->light;
    norm = sqrt(light.x*light.x + light.y*light.y + light.z*light.z); 
    light.x /= norm;
    light.y /= norm;
    light.z /= norm;

    if(GTK_IS_PLOT3D(plot)){
      normal.x = side1.y * side2.z - side1.z * side2.y;
      normal.y = side1.z * side2.x - side1.x * side2.z;
      normal.z = side1.x * side2.y - side1.y * side2.x;

      norm = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z); 
      factor = (normal.x*light.x + normal.y*light.y + normal.z*light.z) / norm;
    } else {
      factor = 1.0;
    }

    gtk_plot_surface_lighting(&color, &real_color, 
                              factor, surface->ambient); 
  }

  if(GTK_IS_PLOT3D(plot))
    if(((t[1].x-t[0].x)*(t[2].y-t[0].y) - (t[1].y-t[0].y)*(t[2].x-t[0].x))*sign > 0)
         visible = FALSE;

  if(visible)
         gtk_plot_pc_set_color(plot->pc, &real_color);
  else
         gtk_plot_pc_set_color(plot->pc, &surface->shadow);

  gtk_plot_pc_draw_polygon(plot->pc, !surface->transparent, t, 3); 


  if(visible)
         gtk_plot_pc_set_color(plot->pc, &surface->grid_foreground);
  else
         gtk_plot_pc_set_color(plot->pc, &surface->grid_background);

  if(surface->show_mesh || !surface->dt->quadrilateral)
       gtk_plot_pc_draw_polygon(plot->pc, FALSE, t, 3); 

  if(!surface->show_mesh && surface->show_grid && surface->dt->quadrilateral)
       gtk_plot_pc_draw_line(plot->pc,  
                            t[1].x, t[1].y, t[2].x, t[2].y); 
                              
}

static void
gtk_plot_surface_draw_legend(GtkPlotData *data, gint x, gint y)
{
  GtkPlotSurface *surface;
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  GdkRectangle area;
  gint lascent, ldescent, lheight, lwidth;
  gdouble m;

  surface = GTK_PLOT_SURFACE(data);

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  m = plot->magnification;
  legend = plot->legends_attr;

  if(data->legend)
    legend.text = data->legend;
  else
    legend.text = "";

  gtk_plot_text_get_size(legend.text, legend.angle, legend.font,
                         roundint(legend.height * m), 
                         &lwidth, &lheight,
                         &lascent, &ldescent);


  if(!surface->height_gradient){
    gtk_plot_pc_set_color(plot->pc, &surface->color);

    gtk_plot_pc_draw_rectangle(plot->pc, TRUE, 
                               x, y,
                               roundint(plot->legends_line_width * m), 
                               lascent + ldescent);
  }else{
    gdouble level, step;
    gint lx = x, lstep;
    step = (data->gradient.end - data->gradient.begin) / 10;
    lstep = roundint(plot->legends_line_width * m / 10.);
    for(level = data->gradient.begin; level < data->gradient.end; level += step){
      GdkColor color;
      gtk_plot_data_get_gradient_level(data, level, &color);
      gtk_plot_pc_set_color(plot->pc, &color);

      gtk_plot_pc_draw_rectangle(plot->pc, TRUE, 
                                 lx, y,
                                 lstep, lascent + ldescent);
      lx += lstep;
    }
  }


  legend.x = (gdouble)(area.x + x + roundint((plot->legends_line_width + 4) * m))
             / (gdouble)area.width;
  legend.y = (gdouble)(area.y + y + lascent) / (gdouble)area.height;

  gtk_plot_draw_text(plot, legend);

  y += 2*lheight;
  gtk_plot_data_draw_gradient(data, x, y);
}


static gint
roundint (gdouble x)
{
 gint sign = 1;

/* if(x <= 0.) sign = -1; 
*/
 return (x+sign*.50999999471);
}


static void
gtk_plot_surface_lighting (GdkColor *a, GdkColor *b, 
                           gdouble normal, gdouble ambient)
{
  gdouble red, green, blue;
  gdouble h, s, v;

  if(normal == 1.0){
   *b = *a;
   return;
  }

  normal = MIN(fabs(normal), 1.0);

  red = a->red;
  green = a->green;
  blue = a->blue;

  rgb_to_hsv(red, green, blue, &h, &s, &v);

  s *= normal;
  v *= normal;

  s += ambient;
  v += ambient;

  hsv_to_rgb(h, MIN(s, 1.0), MIN(v, 1.0), &red, &green, &blue);

  b->red = red;
  b->green = green;
  b->blue = blue;
}


static void
hsv_to_rgb (gdouble  h, gdouble  s, gdouble  v,
            gdouble *r, gdouble *g, gdouble *b)
{
  gint i;
  gdouble f, w, q, t;

  if (s == 0.0)
    s = 0.000001;

  if (h == -1.0)
    {
      *r = v;
      *g = v;
      *b = v;
    }
  else
    {
      if (h == 360.0) h = 0.0;
      h = h / 60.0;
      i = (gint) h;
      f = h - i;
      w = v * (1.0 - s);
      q = v * (1.0 - (s * f));
      t = v * (1.0 - (s * (1.0 - f)));

      switch (i)
      {
        case 0:
          *r = v;
          *g = t;
          *b = w;
          break;
        case 1:
          *r = q;
          *g = v;
          *b = w;
          break;
        case 2:
          *r = w;
          *g = v;
          *b = t;
          break;
        case 3:
          *r = w;
          *g = q;
          *b = v;
          break;
        case 4:
          *r = t;
          *g = w;
          *b = v;
          break;
        case 5:
          *r = v;
          *g = w;
          *b = q;
          break;
      }
    }

  *r *= 65535.;
  *g *= 65535.;
  *b *= 65535.;
}

static void
rgb_to_hsv (gdouble  r, gdouble  g, gdouble  b,
            gdouble *h, gdouble *s, gdouble *v)
{
  double max, min, delta;

  r /= 65535.;
  g /= 65535.;
  b /= 65535.;

  max = r;
  if (g > max)
    max = g;
  if (b > max)
    max = b;

  min = r;
  if (g < min)
    min = g;
  if (b < min)
    min = b;

  *v = max;
  if (max != 0.0)
    *s = (max - min) / max;
  else
    *s = 0.0;

  if (*s == 0.0)
    *h = -1.0;
  else
    {
      delta = max - min;

      if (r == max)
        *h = (g - b) / delta;
      else if (g == max)
        *h = 2.0 + (b - r) / delta;
      else if (b == max)
        *h = 4.0 + (r - g) / delta;

      *h = *h * 60.0;

      if (*h < 0.0)
        *h = *h + 360;
    }
}


/******************************************
 * gtk_plot_surface_set_color
 * gtk_plot_surface_set_shadow
 * gtk_plot_surface_set_grid_foreground
 * gtk_plot_surface_set_grid_background
 * gtk_plot_surface_set_grid_visible
 * gtk_plot_surface_set_mesh_visible
 * gtk_plot_surface_get_grid_visible
 * gtk_plot_surface_get_mesh_visible
 ******************************************/

void            
gtk_plot_surface_set_color      (GtkPlotSurface *data,
                                 GdkColor *color)
{
  data->color = *color;
}

void            
gtk_plot_surface_set_shadow     (GtkPlotSurface *data,
                                 GdkColor *color)
{
  data->shadow = *color;
}

void            
gtk_plot_surface_set_grid_foreground    (GtkPlotSurface *data,
                                         GdkColor *foreground)
{
  data->grid_foreground = *foreground;
}

void            
gtk_plot_surface_set_grid_background    (GtkPlotSurface *data,
                                         GdkColor *background)
{
  data->grid_background = *background;
}

void            
gtk_plot_surface_set_grid_visible    (GtkPlotSurface *data,
                                         gboolean visible)
{
  data->show_grid = visible;
}

gboolean            
gtk_plot_surface_get_grid_visible    (GtkPlotSurface *data)
{
  return (data->show_grid);
}

void            
gtk_plot_surface_set_mesh_visible    (GtkPlotSurface *data,
                                         gboolean visible)
{
  data->show_mesh = visible;
}

gboolean            
gtk_plot_surface_get_mesh_visible    (GtkPlotSurface *data)
{
  return (data->show_mesh);
}

void            
gtk_plot_surface_set_light      (GtkPlotSurface *data,
                                 gdouble x, gdouble y, gdouble z)
{
  data->light.x = x;
  data->light.y = y;
  data->light.z = z;
}

void            
gtk_plot_surface_use_height_gradient (GtkPlotSurface *data,
                                   gboolean use_gradient)
{
  data->height_gradient = use_gradient;
}

void            
gtk_plot_surface_set_ambient      (GtkPlotSurface *data,
                                   gdouble ambient)
{
  data->ambient = ambient;
}

void            
gtk_plot_surface_set_transparent  (GtkPlotSurface *data,
                                   gboolean transparent)
{
  data->transparent = transparent;
}

/******************************************
 * gtk_plot_surface_set_points
 * gtk_plot_surface_get_points
 * gtk_plot_surface_set_x
 * gtk_plot_surface_set_y
 * gtk_plot_surface_set_z
 * gtk_plot_surface_set_dx
 * gtk_plot_surface_set_dy
 * gtk_plot_surface_set_dz
 * gtk_plot_surface_get_x
 * gtk_plot_surface_get_y
 * gtk_plot_surface_get_z
 * gtk_plot_surface_get_dx
 * gtk_plot_surface_get_dy
 * gtk_plot_surface_get_dz
 * gtk_plot_surface_set_nx
 * gtk_plot_surface_set_ny
 * gtk_plot_surface_get_nx
 * gtk_plot_surface_get_ny
 * gtk_plot_surface_set_xstep
 * gtk_plot_surface_set_ystep
 * gtk_plot_surface_get_xstep
 * gtk_plot_surface_get_ystep
 ******************************************/

void
gtk_plot_surface_set_points(GtkPlotSurface *data, 
                            gdouble *x, gdouble *y, gdouble *z,
                            gdouble *dx, gdouble *dy, gdouble *dz,
                            gint nx, gint ny)
{
  GTK_PLOT_DATA(data)->x = x;
  GTK_PLOT_DATA(data)->y = y;
  GTK_PLOT_DATA(data)->z = z;
  GTK_PLOT_DATA(data)->dx = dx;
  GTK_PLOT_DATA(data)->dy = dy;
  GTK_PLOT_DATA(data)->dz = dz;
  data->nx = nx;
  data->ny = ny;
  GTK_PLOT_DATA(data)->num_points = nx * ny;

  gtk_plot_surface_build_mesh(data);
}

void
gtk_plot_surface_get_points(GtkPlotSurface *data, 
                            gdouble **x, gdouble **y, gdouble **z,
                            gdouble **dx, gdouble **dy, gdouble **dz,
                            gint *nx, gint *ny)
{
  *x = GTK_PLOT_DATA(data)->x;
  *y = GTK_PLOT_DATA(data)->y;
  *z = GTK_PLOT_DATA(data)->z;
  *dx = GTK_PLOT_DATA(data)->dx;
  *dy = GTK_PLOT_DATA(data)->dy;
  *dz = GTK_PLOT_DATA(data)->dz;
  *nx = data->nx;
  *ny = data->ny;
}

void
gtk_plot_surface_set_x(GtkPlotSurface *data, 
                       gdouble *x) 
{
  GTK_PLOT_DATA(data)->x = x;
}


void
gtk_plot_surface_set_y(GtkPlotSurface *data, 
                       gdouble *y) 
{
  GTK_PLOT_DATA(data)->y = y;
}

void
gtk_plot_surface_set_z(GtkPlotSurface *data, 
                       gdouble *z) 
{
  GTK_PLOT_DATA(data)->z = z;
}

void
gtk_plot_surface_set_dx(GtkPlotSurface *data, 
                        gdouble *dx) 
{
  GTK_PLOT_DATA(data)->dx = dx;
}

void
gtk_plot_surface_set_dy(GtkPlotSurface *data, 
                        gdouble *dy) 
{
  GTK_PLOT_DATA(data)->dy = dy;
}

void
gtk_plot_surface_set_dz(GtkPlotSurface *data, 
                       gdouble *dz) 
{
  GTK_PLOT_DATA(data)->dz = dz;
}

gdouble *
gtk_plot_surface_get_x(GtkPlotSurface *dataset, gint *nx)
{
  *nx = dataset->nx;
  return(GTK_PLOT_DATA(dataset)->x);
}

gdouble *
gtk_plot_surface_get_y(GtkPlotSurface *dataset, gint *ny)
{
  *ny = dataset->ny;
  return(GTK_PLOT_DATA(dataset)->y);
}

gdouble *
gtk_plot_surface_get_z(GtkPlotSurface *dataset, gint *nx, gint *ny)
{
  *nx = dataset->nx;
  *ny = dataset->ny;
  return(GTK_PLOT_DATA(dataset)->z);
}

gdouble *
gtk_plot_surface_get_dz(GtkPlotSurface *dataset)
{
  return(GTK_PLOT_DATA(dataset)->dz);
}

gdouble *
gtk_plot_surface_get_dx(GtkPlotSurface *dataset)
{
  return(GTK_PLOT_DATA(dataset)->dx);
}

gdouble *
gtk_plot_surface_get_dy(GtkPlotSurface *dataset)
{
  return(GTK_PLOT_DATA(dataset)->dy);
}

void
gtk_plot_surface_set_nx(GtkPlotSurface *dataset, gint nx)
{
  dataset->nx = nx;
}

void
gtk_plot_surface_set_ny(GtkPlotSurface *dataset, gint ny)
{
  dataset->ny = ny;
}

gint
gtk_plot_surface_get_nx(GtkPlotSurface *dataset)
{
  return(dataset->nx);
}

gint
gtk_plot_surface_get_ny(GtkPlotSurface *dataset)
{
  return(dataset->ny);
}

void
gtk_plot_surface_set_xstep(GtkPlotSurface *dataset, gdouble xstep)
{
  dataset->xstep = xstep;
}

void
gtk_plot_surface_set_ystep(GtkPlotSurface *dataset, gdouble ystep)
{
  dataset->ystep = ystep;
}

gdouble
gtk_plot_surface_get_xstep(GtkPlotSurface *dataset)
{
  return (dataset->xstep);
}

gdouble
gtk_plot_surface_get_ystep(GtkPlotSurface *dataset)
{
  return (dataset->ystep);
}

void
gtk_plot_surface_build_mesh(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  GtkPlot *plot;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  if(data->is_function){
     gdouble xstep, ystep;
     gdouble xmin, xmax, ymin, ymax;
     gdouble x, y;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gdouble *old_x, *old_y, *old_z;
     gint nx, ny;
     gint npoints;

     xmin = GTK_PLOT(plot)->xmin;
     xmax = GTK_PLOT(plot)->xmax;
     ymin = GTK_PLOT(plot)->ymin;
     ymax = GTK_PLOT(plot)->ymax;

     xstep = surface->xstep;
     surface->nx = roundint((xmax - xmin) / xstep) + 1;

     ystep = surface->ystep;
     surface->ny = roundint((ymax - ymin) / ystep) + 1;

     npoints = surface->nx * surface->ny;
     fx = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fy = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));
     fz = (gdouble *)g_malloc((npoints + 1) * sizeof(gdouble));

     npoints = 0;
     y = ymin;
     for(ny = 0; ny < surface->ny; ny++)
       {
         x = xmin;
         for(nx = 0; nx < surface->nx; nx++)
          {
            gboolean error;
            fx[npoints] = x;
            fy[npoints] = y;
            fz[npoints] = data->function3d(plot, data, x, y, &error);

            x += xstep;
            npoints++;
          }
         y += ystep;
     }

     old_x = data->x; old_y = data->y; old_z = data->z;

     data->x = fx;
     data->y = fy;
     data->z = fz;

     data->num_points = npoints;
     gtk_plot_surface_real_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);
  }
  else if(data->is_iterator){
     gdouble x, y, z, a, dx, dy, dz, da;
     gchar *label;
     gdouble *fx = NULL, *fy = NULL, *fz = NULL;
     gint iter;

     if(data->iterator_mask & GTK_PLOT_DATA_X)
       fx = g_new0(gdouble, data->num_points);
     if(data->iterator_mask & GTK_PLOT_DATA_Y)
       fy = g_new0(gdouble, data->num_points);
     if(data->iterator_mask & GTK_PLOT_DATA_Z)
       fz = g_new0(gdouble, data->num_points);

     for(iter = 0; iter < data->num_points; iter++)
       {
          gboolean error;
          data->iterator (plot, data, iter,
                          &x, &y, &z, &a, &dx, &dy, &dz, &da, &label, &error);

          if(error)
            {
               break;
            }
          else
            {
              if(data->iterator_mask & GTK_PLOT_DATA_X) fx[iter] = x;
              if(data->iterator_mask & GTK_PLOT_DATA_Y) fy[iter] = y;
              if(data->iterator_mask & GTK_PLOT_DATA_Z) fz[iter] = z;
            }
       } 

     data->x = fx;
     data->y = fy;
     data->z = fz;

     gtk_plot_surface_real_build_mesh(surface);

     g_free(fx);
     g_free(fy);
     g_free(fz);
  }
  else   
     gtk_plot_surface_real_build_mesh(surface);
}

static void
gtk_plot_surface_real_build_mesh(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  GtkPlot *plot;
  gint i;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  if(data->num_points == 0) return;

  gtk_plot_dt_clear(surface->dt);

  for(i = 0; i < data->num_points; i++){
    GtkPlotDTnode node;
    node.x = data->x[i];
    node.y = data->y[i];
    node.z = 0.0;
    if(data->z) node.z = data->z[i];
    gtk_plot_dt_add_node(surface->dt, node);
  }
  gtk_plot_dt_triangulate(surface->dt);
  gtk_plot_surface_recalc_nodes(surface);
  surface->recalc_dt = FALSE;
}

void
gtk_plot_surface_recalc_nodes(GtkPlotSurface *surface)
{
  GtkPlotData *data;
  GtkPlot *plot;
  gint i;

  data = GTK_PLOT_DATA(surface);
  if(!data->plot) return;
  plot = data->plot;

  for(i= surface->dt->node_0; i < surface->dt->node_cnt; i++){
    GtkPlotDTnode *node;
    node = gtk_plot_dt_get_node(surface->dt,i);
    if(GTK_IS_PLOT3D(plot)){
      gtk_plot3d_get_pixel(GTK_PLOT3D(plot),
			   node->x, node->y, node->z,
			   &node->px, &node->py, &node->pz);
    } else {
      gtk_plot_get_pixel(plot,
			 node->x, node->y,
			 &node->px, &node->py);
      node->pz = 0.0;
    }
  }

  surface->dt->triangles = g_list_sort(surface->dt->triangles, (GCompareFunc)compare_func);
}

static gint
compare_func (gpointer a, gpointer b)
{
  GtkPlotDTtriangle *ta, *tb;
  gdouble cza, czb;

  ta = (GtkPlotDTtriangle *)a;
  tb = (GtkPlotDTtriangle *)b;
  cza = MIN(ta->na->pz, ta->nb->pz);
  cza = MIN(cza, ta->nc->pz);
  czb = MIN(tb->na->pz , tb->nb->pz);
  czb = MIN(czb , tb->nc->pz);
  return(czb > cza ? 1 : (cza == czb ? 0 : -1));
}


