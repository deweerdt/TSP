/* gtkplotcsurface - csurface plots widget for gtk+
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
#include "gtkplotcsurface.h"
#include "gtkpsfont.h"

static void gtk_plot_csurface_class_init 	(GtkPlotCSurfaceClass *klass);
static void gtk_plot_csurface_init 		(GtkPlotCSurface *data);
static void gtk_plot_csurface_destroy 		(GtkObject *object);
static void gtk_plot_csurface_get_legend_size	(GtkPlotData *data, 
						 gint *width, gint *height);
static void gtk_plot_csurface_draw_legend	(GtkPlotData *data, 
						 gint x, gint y);
static void gtk_plot_csurface_draw_private 	(GtkPlotData *data);
static void gtk_plot_csurface_draw_triangle 	(GtkPlotSurface *surface,
                                		 GtkPlotDTtriangle *triangle,
                                                 gint sign);
static void gtk_plot_csurface_draw_gradient 	(GtkPlotData *surface,
 						 gint x, gint y);
static void gtk_plot_csurface_lighting 		(GdkColor *a, 
						 GdkColor *b, 
						 gdouble normal,
						 gdouble ambient);
static void clear_polygons			(GtkPlotCSurface *surface);
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

static GtkPlotSurfaceClass *parent_class = NULL;

typedef struct
{
  GtkPlotPoint p[4];
  GtkPlotDTtriangle *t;
  gint n;
  GdkColor color;
  gboolean cut_level;
} GtkPlotPolygon;
  

GtkType
gtk_plot_csurface_get_type (void)
{
  static GtkType data_type = 0;

  if (!data_type)
    {
      GtkTypeInfo data_info =
      {
	"GtkPlotCSurface",
	sizeof (GtkPlotCSurface),
	sizeof (GtkPlotCSurfaceClass),
	(GtkClassInitFunc) gtk_plot_csurface_class_init,
	(GtkObjectInitFunc) gtk_plot_csurface_init,
	/* reserved 1*/ NULL,
        /* reserved 2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      data_type = gtk_type_unique (gtk_plot_surface_get_type(), &data_info);
    }
  return data_type;
}

static void
gtk_plot_csurface_class_init (GtkPlotCSurfaceClass *klass)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkPlotDataClass *data_class;
  GtkPlotSurfaceClass *surface_class;

  parent_class = gtk_type_class (gtk_plot_surface_get_type ());

  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  data_class = (GtkPlotDataClass *) klass;
  surface_class = (GtkPlotSurfaceClass *) klass;

  object_class->destroy = gtk_plot_csurface_destroy;

  data_class->draw_legend = gtk_plot_csurface_draw_legend;
  data_class->get_legend_size = gtk_plot_csurface_get_legend_size;
/*
  data_class->draw_gradient = NULL;
*/
  data_class->draw_data = gtk_plot_csurface_draw_private;
  surface_class->draw_triangle = gtk_plot_csurface_draw_triangle;
}


static void
gtk_plot_csurface_init (GtkPlotCSurface *dataset)
{
  GtkWidget *widget;
  GdkColormap *colormap;

  GTK_WIDGET_SET_FLAGS(dataset, GTK_NO_WINDOW);

  widget = GTK_WIDGET(dataset);
  colormap = gtk_widget_get_colormap(widget);

  GTK_PLOT_DATA(dataset)->show_gradient = TRUE;

  dataset->lines_visible = TRUE;
  dataset->project_xy = TRUE;
  dataset->lines_only = FALSE;
  dataset->levels_start = NULL; 
  dataset->levels_end = NULL; 
  dataset->sublevels_start = NULL; 
  dataset->sublevels_end = NULL; 
  dataset->num_levels = 0;
  dataset->num_sublevels = 0;

  dataset->polygons = NULL;
  dataset->npolygons = 0;

  dataset->levels_line = GTK_PLOT_SURFACE(dataset)->mesh_line;
  dataset->sublevels_line = GTK_PLOT_SURFACE(dataset)->mesh_line;
}

static void 
gtk_plot_csurface_destroy 		(GtkObject *object)
{
  GtkPlotCSurface *surface;

  surface = GTK_PLOT_CSURFACE(object);

  if(surface->levels_start) g_free(surface->levels_start);
  if(surface->levels_end) g_free(surface->levels_end);
  if(surface->sublevels_start) g_free(surface->sublevels_start);
  if(surface->sublevels_end) g_free(surface->sublevels_end);

  clear_polygons(surface);

  GTK_OBJECT_CLASS(parent_class)->destroy(object);
}

GtkWidget*
gtk_plot_csurface_new (void)
{
  GtkPlotData *data;

  data = gtk_type_new (gtk_plot_csurface_get_type ());

  return GTK_WIDGET (data);
}

GtkWidget*
gtk_plot_csurface_new_function (GtkPlotFunc3D function)
{
  GtkWidget *data;

  data = gtk_type_new (gtk_plot_csurface_get_type ());

  gtk_plot_csurface_construct_function(GTK_PLOT_CSURFACE(data), function);

  return data;
}

void
gtk_plot_csurface_construct_function (GtkPlotCSurface *data, 
                                      GtkPlotFunc3D function)
{
  GTK_PLOT_DATA(data)->is_function = TRUE;
  GTK_PLOT_DATA(data)->function3d = function;
}

static void 
gtk_plot_csurface_draw_private 	(GtkPlotData *data)
{
  GtkPlot *plot;
  GtkPlotSurface *surface;
  GtkPlotCSurface *csurface;

  surface = GTK_PLOT_SURFACE(data);
  csurface = GTK_PLOT_CSURFACE(data);
  plot = data->plot;

  if(csurface->levels_start) g_free(csurface->levels_start); 
  if(csurface->levels_end) g_free(csurface->levels_end); 
  if(csurface->sublevels_start) g_free(csurface->sublevels_start); 
  if(csurface->sublevels_end) g_free(csurface->sublevels_end); 
  clear_polygons(csurface);
  csurface->polygons = (gpointer)g_new0(GtkPlotPolygon, 1);

  csurface->levels_start = g_new0(GtkPlotVector, 1);
  csurface->levels_end = g_new0(GtkPlotVector, 1);
  csurface->sublevels_start = g_new0(GtkPlotVector, 1);
  csurface->sublevels_end = g_new0(GtkPlotVector, 1);
  csurface->num_levels = 0;
  csurface->num_sublevels = 0;

  GTK_PLOT_DATA_CLASS(parent_class)->draw_data(data); 

  gtk_plot_set_line_attributes(plot, csurface->levels_line);

  if(!csurface->lines_only){
    gint i;
    for(i = 0; i < csurface->npolygons; i++)
    {
      GtkPlotPolygon *poly;

      poly = (GtkPlotPolygon *)csurface->polygons[i];

      gtk_plot_pc_set_color(data->plot->pc, &poly->color);
      gtk_plot_pc_draw_polygon(data->plot->pc, !surface->transparent, poly->p, poly->n);
    
      if(csurface->lines_visible || surface->show_mesh || surface->show_grid)
        gtk_plot_pc_set_color(data->plot->pc, &surface->grid_foreground);

      if(GTK_IS_PLOT3D(data->plot) && csurface->lines_visible && poly->cut_level)
        gtk_plot_pc_draw_line(data->plot->pc, 
    	   	              poly->p[0].x, poly->p[0].y, 
                              poly->p[1].x, poly->p[1].y);

      if(surface->show_mesh || !surface->dt->quadrilateral){
/*
        gtk_plot_pc_draw_polygon(data->plot->pc, FALSE, poly->p, poly->n);
*/
        gtk_plot_pc_draw_line(data->plot->pc, 
    	   	              poly->p[1].x, poly->p[1].y, 
                              poly->p[2].x, poly->p[2].y);
        if(poly->n == 3)
          gtk_plot_pc_draw_line(data->plot->pc, 
    	     	                poly->p[0].x, poly->p[0].y, 
                                poly->p[2].x, poly->p[2].y);
        else
          gtk_plot_pc_draw_line(data->plot->pc, 
    	     	                poly->p[1].x, poly->p[1].y, 
                                poly->p[3].x, poly->p[3].y);
      }

      if(!surface->show_mesh && surface->show_grid && surface->dt->quadrilateral){
        gtk_plot_pc_draw_line(data->plot->pc, 
                              poly->t->nb->px, poly->t->nb->py, 
                              poly->t->nc->px, poly->t->nc->py); 
      }
    }
  }

  if(!GTK_IS_PLOT3D(data->plot)){
    gtk_plot_pc_set_color(data->plot->pc, &surface->grid_foreground);
    if(csurface->lines_visible){
      gint i;
      for(i = 0; i < csurface->num_levels; i++)
         gtk_plot_pc_draw_line(data->plot->pc, 
                               csurface->levels_start[i].x, 
                               csurface->levels_start[i].y,
                               csurface->levels_end[i].x,
                               csurface->levels_end[i].y);
      gtk_plot_set_line_attributes(plot, csurface->sublevels_line);
      for(i = 0; i < csurface->num_sublevels; i++)
         gtk_plot_pc_draw_line(data->plot->pc, 
                               csurface->sublevels_start[i].x, 
                               csurface->sublevels_start[i].y,
                               csurface->sublevels_end[i].x,
                               csurface->sublevels_end[i].y);
    }
  }

}

static void
gtk_plot_csurface_draw_triangle (GtkPlotSurface *surface,
                                 GtkPlotDTtriangle *triangle,
                                 gint sign)
{
  GtkPlot *plot;
  GtkPlotData *data;
  GtkPlotCSurface *csurface;
  GtkPlotVector points[3], side[3], c[2], light, normal;
  GdkDrawable *drawable;
  GtkPlotPoint t[3], poly[4], proj[2];
  GtkPlotPolygon *polygon = NULL;
  GdkColor color;
  GdkColor color_h1, color_h2;
  gdouble x, y, z;
  gint i, n, level, sublevel;
  gdouble factor, norm;
  gint mask, vertex[3];
  gdouble min, max, step;
  gint nlevels, nsublevels;
  gdouble h, h_next;

  csurface = GTK_PLOT_CSURFACE(surface);
  data = GTK_PLOT_DATA(surface);
  plot = GTK_PLOT(data->plot);
  drawable = plot->drawable;

  min = data->gradient.begin;
  max = data->gradient.end;
  nlevels = data->gradient.nmajorticks;
  nsublevels = data->gradient.nminorticks;
  step = (data->gradient.end - data->gradient.begin) / (nlevels * (nsublevels + 1));
  data->gradient.end += step;

  light = surface->light;
  norm = sqrt(light.x*light.x + light.y*light.y + light.z*light.z); 
  light.x /= norm;
  light.y /= norm;
  light.z /= norm;

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

  side[0].x = (points[1].x - points[0].x) * sign;
  side[0].y = (points[1].y - points[0].y) * sign;
  side[0].z = (points[1].z - points[0].z) * sign;
  side[1].x = (points[2].x - points[0].x) * sign;
  side[1].y = (points[2].y - points[0].y) * sign;
  side[1].z = (points[2].z - points[0].z) * sign;
  
  if(GTK_IS_PLOT3D(plot)){       
    normal.x = side[0].y * side[1].z - side[0].z * side[1].y;
    normal.y = side[0].z * side[1].x - side[0].x * side[1].z;
    normal.z = side[0].x * side[1].y - side[0].y * side[1].x;

    norm = sqrt(normal.x*normal.x + normal.y*normal.y + normal.z*normal.z); 
    factor = (normal.x*light.x + normal.y*light.y + normal.z*light.z) / norm;
  } else {
    factor = 1.0;
  }

  side[0].x = (points[1].x - points[0].x);
  side[0].y = (points[1].y - points[0].y);
  side[0].z = (points[1].z - points[0].z);
  side[1].x = (points[2].x - points[1].x);
  side[1].y = (points[2].y - points[1].y);
  side[1].z = (points[2].z - points[1].z);
  side[2].x = (points[0].x - points[2].x);
  side[2].y = (points[0].y - points[2].y);
  side[2].z = (points[0].z - points[2].z);

  h = data->gradient.end;
  h_next = h - step;

  if(points[0].z <= min || points[1].z <= min || points[2].z <= min){ 
    if(!csurface->lines_only){
      gtk_plot_data_get_gradient_level(data, min - step, &color); 
      gtk_plot_csurface_lighting(&color, &color_h1, factor, 1.); 

      polygon = g_new0(GtkPlotPolygon, 1);
      polygon->n = 3;
      polygon->color = color_h1;
      polygon->p[0] = t[0]; 
      polygon->p[1] = t[1]; 
      polygon->p[2] = t[2]; 
      polygon->t = triangle; 
      csurface->polygons = g_realloc(csurface->polygons, 
                                    (csurface->npolygons + 1) * sizeof(GtkPlotPolygon));
      csurface->polygons[csurface->npolygons++] = polygon;
    }
  }
  if(points[0].z > max || points[1].z > max || points[2].z > max){ 
    if(!csurface->lines_only){
      gtk_plot_data_get_gradient_level(data, max + step, &color); 
      gtk_plot_csurface_lighting(&color, &color_h1, factor, 1.); 
      gtk_plot_pc_set_color(plot->pc, &color_h1);

      polygon = g_new0(GtkPlotPolygon, 1);
      polygon->n = 3;
      polygon->color = color_h1;
      polygon->p[0] = t[0]; 
      polygon->p[1] = t[1]; 
      polygon->p[2] = t[2]; 
      polygon->t = triangle; 
      csurface->polygons = g_realloc(csurface->polygons, 
                                    (csurface->npolygons + 1) * sizeof(GtkPlotPolygon));
      csurface->polygons[csurface->npolygons++] = polygon;
    }
  }

  for(level = nlevels + 1; level >= 0; level--){
   for(sublevel = nsublevels + 1; sublevel > 0; sublevel--){

    if(points[0].z <= h && points[1].z <= h && points[2].z <= h &&
      (points[0].z > h_next || points[1].z > h_next || points[2].z > h_next)){
      if(!csurface->lines_only){
        gtk_plot_data_get_gradient_level(data, h, &color); 
        gtk_plot_csurface_lighting(&color, &color_h1, factor, 1.); 

        polygon = g_new0(GtkPlotPolygon, 1);
        polygon->n = 3;
        polygon->color = color_h1;
        polygon->p[0] = t[0]; 
        polygon->p[1] = t[1]; 
        polygon->p[2] = t[2]; 
        polygon->t = triangle;
        csurface->polygons = g_realloc(csurface->polygons, 
                                    (csurface->npolygons + 1) * sizeof(GtkPlotPolygon));
        csurface->polygons[csurface->npolygons++] = polygon;
      }
    }

    if(points[0].z > h_next || points[1].z > h_next || points[2].z > h_next){
      if(!csurface->lines_only){
        gtk_plot_data_get_gradient_level(data, h_next, &color); 
        gtk_plot_csurface_lighting(&color, &color_h2, factor, 1.); 
        gtk_plot_pc_set_color(plot->pc, &color_h2);
      }

      n = 0;
      mask = 0;
      for(i = 0; i < 3; i++){
        gdouble a;
 
        a = -1.0;
        if(side[i].z != 0.0) 
            a = (h_next - points[i].z) / side[i].z; 
  
        if(a >= 0. && a <= 1.0){
          mask |= 1 << i;
          c[n].x = points[i].x + side[i].x * a;
          c[n].y = points[i].y + side[i].y * a;
          c[n].z = h_next;
          n++;
        }
      }

      if(n >= 2){
        switch(mask){
          case 5:
             vertex[0] = 0;
             vertex[1] = 2;
             vertex[2] = 1;
             break;
          case 3:
             vertex[0] = 1;
             vertex[1] = 2;
             vertex[2] = 0;
             break;
          case 6:
             vertex[0] = 2;
             vertex[1] = 0;
             vertex[2] = 1;
             break;
          default:
             if(c[0].z == c[1].z) c[1] = c[2];
             vertex[0] = 0;
             vertex[1] = 2;
             vertex[2] = 1;
             break;
        }

        if(GTK_IS_PLOT3D(plot)){
          gtk_plot3d_get_pixel(GTK_PLOT3D(plot), 
                               c[0].x, c[0].y, c[0].z,
                               &x, &y, &z);
          poly[0].x = x;
          poly[0].y = y;
          gtk_plot3d_get_pixel(GTK_PLOT3D(plot),
                               c[0].x, c[0].y, GTK_PLOT3D(plot)->origin.z,
                               &x, &y, &z);
          proj[0].x = x;
          proj[0].y = y;
          gtk_plot3d_get_pixel(GTK_PLOT3D(plot), 
                               c[1].x, c[1].y, c[1].z,
                               &x, &y, &z);
          poly[1].x = x;
          poly[1].y = y;
          gtk_plot3d_get_pixel(GTK_PLOT3D(plot),
                               c[1].x, c[1].y, GTK_PLOT3D(plot)->origin.z,
                               &x, &y, &z);
          proj[1].x = x;
          proj[1].y = y;
        } else {
          gtk_plot_get_pixel(GTK_PLOT(plot), 
                             c[0].x, c[0].y, &x, &y);
          poly[0].x = x;
          poly[0].y = y;
          gtk_plot_get_pixel(GTK_PLOT(plot), 
                             c[1].x, c[1].y, &x, &y);
          poly[1].x = x;
          poly[1].y = y;
        }
  
        if(GTK_IS_PLOT3D(plot) && csurface->project_xy){
          if(sublevel > 1){
            gtk_plot_set_line_attributes(plot, csurface->sublevels_line);
          }else{
            gtk_plot_set_line_attributes(plot, csurface->levels_line);
          }
          gtk_plot_pc_set_color(plot->pc, &color_h2);
 
          gtk_plot_pc_draw_line(plot->pc, 
   	   	                proj[0].x, proj[0].y, proj[1].x, proj[1].y);
        }


        if(!csurface->lines_only){
          if(points[vertex[0]].z <= h_next){
            poly[2] = t[vertex[0]];

            polygon = g_new0(GtkPlotPolygon, 1);
            polygon->n = 3;
            polygon->color = color_h2;
            polygon->p[0] = poly[0]; 
            polygon->p[1] = poly[1]; 
            polygon->p[2] = poly[2]; 
            polygon->cut_level = TRUE;
            polygon->t = triangle; 

            csurface->polygons = g_realloc(csurface->polygons, 
                                    (csurface->npolygons + 1) * sizeof(GtkPlotPolygon));
            csurface->polygons[csurface->npolygons++] = polygon;
          }else{
            poly[2] = t[vertex[1]];
            poly[3] = t[vertex[2]];

            polygon = g_new0(GtkPlotPolygon, 1);
            polygon->n = 4;
            polygon->color = color_h2;
            polygon->p[0] = poly[0]; 
            polygon->p[1] = poly[1]; 
            polygon->p[2] = poly[2]; 
            polygon->p[3] = poly[3]; 
            polygon->cut_level = TRUE;
            polygon->t = triangle; 

            csurface->polygons = g_realloc(csurface->polygons, 
                                    (csurface->npolygons + 1) * sizeof(GtkPlotPolygon));
            csurface->polygons[csurface->npolygons++] = polygon;
          }
        }
    
        if(!GTK_IS_PLOT3D(plot)){
          if(sublevel > 1){
            csurface->num_sublevels++;
            csurface->sublevels_start = g_realloc(csurface->sublevels_start, 
                                            csurface->num_sublevels * sizeof(GtkPlotVector));
            csurface->sublevels_end = g_realloc(csurface->sublevels_end, 
                                            csurface->num_sublevels * sizeof(GtkPlotVector));
            csurface->sublevels_start[csurface->num_sublevels - 1].x = poly[0].x;
            csurface->sublevels_start[csurface->num_sublevels - 1].y = poly[0].y;
            csurface->sublevels_end[csurface->num_sublevels - 1].x = poly[1].x;
            csurface->sublevels_end[csurface->num_sublevels - 1].y = poly[1].y;

          }else{
            csurface->num_levels++;
            csurface->levels_start = g_realloc(csurface->levels_start, 
                                            csurface->num_levels * sizeof(GtkPlotVector));
            csurface->levels_end = g_realloc(csurface->levels_end, 
                                            csurface->num_levels * sizeof(GtkPlotVector));
            csurface->levels_start[csurface->num_levels - 1].x = poly[0].x;
            csurface->levels_start[csurface->num_levels - 1].y = poly[0].y;
            csurface->levels_end[csurface->num_levels - 1].x = poly[1].x;
            csurface->levels_end[csurface->num_levels - 1].y = poly[1].y;
          }
        }
  
      }
    }

    h -= step;
    h_next -= step;
   }
  }


  data->gradient.begin = min;
  data->gradient.end = max;
  data->gradient.nmajorticks = nlevels;
}

static void
clear_polygons(GtkPlotCSurface *surface)
{
  if(surface->polygons) g_free(surface->polygons);
  surface->polygons = NULL;
  surface->npolygons = 0;
}

static void
gtk_plot_csurface_draw_gradient(GtkPlotData *data, gint x, gint y)
{
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  GdkRectangle area;
  GdkColor color;
  GdkFont *font;
  GList *family;
  gint numf;
  gint lascent, ldescent;
  gdouble m;
  gdouble step;
  gdouble min, max, nlevels;
  gdouble h;
  gint level;

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  if(!data->show_gradient) return;

  min = data->gradient.begin;
  max = data->gradient.end;
  nlevels = data->gradient.nmajorticks;


  plot = data->plot;
  area.x = GTK_WIDGET(plot)->allocation.x;
  area.y = GTK_WIDGET(plot)->allocation.y;
  area.width = GTK_WIDGET(plot)->allocation.width;
  area.height = GTK_WIDGET(plot)->allocation.height;

  m = plot->magnification;
  legend = plot->legends_attr;
  legend.text = "";

  gtk_psfont_get_families(&family, &numf);
  font = gtk_psfont_get_gdkfont(legend.font, roundint(legend.height * m));
  
  lascent = font->ascent;
  ldescent = font->descent;
  gdk_font_unref(font);

  legend.x = (gdouble)(area.x + x + roundint((plot->legends_line_width + 4) * m)) / (gdouble)area.width;

  step = (data->gradient.end - data->gradient.begin) / nlevels;
  data->gradient.end += step;

  h = data->gradient.end;

  for(level = nlevels + 2; level > 0; level--){
      gchar text[20];

      gtk_plot_data_get_gradient_level(data, h, &color); 
      gtk_plot_pc_set_color(plot->pc, &color);
  
      gtk_plot_pc_draw_rectangle(plot->pc, TRUE, 
                                 x, y,
                                 roundint(plot->legends_line_width * m), 
                                 lascent + ldescent);

      legend.y = (gdouble)(area.y + y + lascent + (lascent + ldescent) / 2) / (gdouble)area.height;
  
      if(level > 1){
           gdouble val = h - step;
           if(fabs(val) < pow(10, -data->legends_precision)) val = 0.0f;
           sprintf(text, "%.*f", data->legends_precision,  val);
           legend.text = text;
           gtk_plot_draw_text(plot, legend);
      }
  
      y += lascent + ldescent;
      h -= step;
  }

  data->gradient.begin = min;
  data->gradient.end = max;
  data->gradient.nmajorticks = nlevels;
}


static void
gtk_plot_csurface_get_legend_size(GtkPlotData *data, gint *width, gint *height)
{
  GtkPlotSurface *surface;
  GtkPlotCSurface *csurface;
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  gint lascent, ldescent, lheight, lwidth;
  gdouble m;
  gint level;

  surface = GTK_PLOT_SURFACE(data);
  csurface = GTK_PLOT_CSURFACE(data);

  g_return_if_fail(data->plot != NULL);
  g_return_if_fail(GTK_IS_PLOT(data->plot));

  plot = data->plot;

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

  *width = lwidth + roundint(12 * m);
  *height = MAX(lheight, roundint(data->symbol.size * m));

  if(data->show_gradient){
    for(level = data->gradient.nmajorticks; level >= 0; level--){
      gdouble h;
      gchar text[20];
  
      h = data->gradient.major_values[level];
  
      sprintf(text, "%.*f", data->legends_precision,  h);
      legend.text = text;
  
      gtk_plot_text_get_size(legend.text, legend.angle, legend.font,
                             roundint(legend.height * m), 
                             &lwidth, &lheight,
                             &lascent, &ldescent);
  
      *width = MAX(*width, lwidth + roundint((plot->legends_line_width + 12) * m));
    }
    *height += (data->gradient.nmajorticks + 2) * (lascent + ldescent);
  }

}

static void
gtk_plot_csurface_draw_legend(GtkPlotData *data, gint x, gint y)
{
  GtkPlotSurface *surface;
  GtkPlotCSurface *csurface;
  GtkPlot *plot = NULL;
  GtkPlotText legend;
  GdkRectangle area;
  gint lascent, ldescent, lheight, lwidth;
  gdouble m;

  surface = GTK_PLOT_SURFACE(data);
  csurface = GTK_PLOT_CSURFACE(data);

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


  legend.x = (gdouble)(area.x + x + roundint(4 * m))
             / (gdouble)area.width;
  legend.y = (gdouble)(area.y + y + lascent) / (gdouble)area.height;

  gtk_plot_draw_text(plot, legend);

  y += lheight;

  gtk_plot_csurface_draw_gradient(data, x, y);
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
gtk_plot_csurface_lighting (GdkColor *a, GdkColor *b, 
                           gdouble normal, gdouble ambient)
{
  gdouble red, green, blue;
  gdouble h, s, v;

  *b = *a;
  return;

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



/***********************************

 ***********************************/

void
gtk_plot_csurface_set_lines_visible (GtkPlotCSurface *csurface, gboolean visible)
{
  csurface->lines_visible = visible;
}

gboolean
gtk_plot_csurface_get_lines_visible (GtkPlotCSurface *csurface)
{
  return (csurface->lines_visible);
}

void
gtk_plot_csurface_set_lines_only (GtkPlotCSurface *csurface, gboolean lines)
{
  csurface->lines_only = lines;
}

gboolean
gtk_plot_csurface_lines_only (GtkPlotCSurface *csurface)
{
  return (csurface->lines_only);
}

void
gtk_plot_csurface_set_labels_visible (GtkPlotCSurface *csurface, gboolean visible)
{
  csurface->labels_visible = visible;
}

gboolean
gtk_plot_csurface_labels_visible (GtkPlotCSurface *csurface)
{
  return (csurface->labels_visible);
}
