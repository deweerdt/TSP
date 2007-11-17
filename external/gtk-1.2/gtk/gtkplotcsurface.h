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

#ifndef __GTK_PLOT_CSURFACE_H__
#define __GTK_PLOT_CSURFACE_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "gtkplot.h"
#include "gtkplotdt.h"
#include "gtkplotsurface.h"
#include "gtkplotpc.h"


#define GTK_PLOT_CSURFACE(obj)        GTK_CHECK_CAST (obj, gtk_plot_csurface_get_type (), GtkPlotCSurface)
#define GTK_TYPE_PLOT_CSURFACE        (gtk_plot_csurface_get_type ())
#define GTK_PLOT_CSURFACE_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, gtk_plot_csurface_get_type, GtkPlotCSurfaceClass)
#define GTK_IS_PLOT_CSURFACE(obj)     GTK_CHECK_TYPE (obj, gtk_plot_csurface_get_type ())
#define GTK_PLOT_CSURFACE_FLAGS(plot)         (GTK_PLOT_CSURFACE(plot)->flags)
#define GTK_PLOT_CSURFACE_SET_FLAGS(plot,flag) (GTK_PLOT_CSURFACE_FLAGS(plot) |= (flag))
#define GTK_PLOT_CSURFACE_UNSET_FLAGS(plot,flag) (GTK_PLOT_CSURFACE_FLAGS(plot) &= ~(flag))

#define GTK_PLOT_CSURFACE_TRANSPARENT(plot) (GTK_PLOT_CSURFACE_FLAGS(plot) & GTK_PLOT_CSURFACE_TRANSPARENT)

typedef struct _GtkPlotCSurface		GtkPlotCSurface;
typedef struct _GtkPlotCSurfaceClass	GtkPlotCSurfaceClass;

struct _GtkPlotCSurface
{
  GtkPlotSurface surface;

  gboolean lines_visible;  /* show contour level lines */
  gboolean project_xy;     /* project on x-y plane */
  gboolean lines_only;     /* draw contour lines only */
  gboolean labels_visible; /* show contour labels */

  GtkPlotVector *levels_start, *levels_end;  /* contour lines */
  GtkPlotVector *sublevels_start, *sublevels_end;  /* contour lines */
  gint num_levels;
  gint num_sublevels;

  gpointer *polygons;
  gint npolygons;

  GtkPlotLine levels_line;
  GtkPlotLine sublevels_line;
};

struct _GtkPlotCSurfaceClass
{
  GtkPlotSurfaceClass parent_class;
};

/* PlotCSurface */

GtkType		gtk_plot_csurface_get_type	(void);
GtkWidget*	gtk_plot_csurface_new		(void);
GtkWidget*	gtk_plot_csurface_new_function	(GtkPlotFunc3D function);

void		gtk_plot_csurface_construct_function (GtkPlotCSurface *csurface,
					 	      GtkPlotFunc3D function);

/* gint		gtk_plot_csurface_get_legends_precision (GtkPlotCSurface *csurface); */
 
void		gtk_plot_csurface_set_lines_visible (GtkPlotCSurface *csurface,
						     gboolean visible);
gboolean	gtk_plot_csurface_get_lines_visible (GtkPlotCSurface *csurface);
void		gtk_plot_csurface_set_lines_only    (GtkPlotCSurface *csurface,
						     gboolean lines);
gboolean	gtk_plot_csurface_lines_only 	(GtkPlotCSurface *csurface);
void		gtk_plot_csurface_set_labels_visible(GtkPlotCSurface *csurface,
						     gboolean visible);
gboolean	gtk_plot_csurface_labels_visible    (GtkPlotCSurface *csurface);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GTK_PLOT_CSURFACE_H__ */
