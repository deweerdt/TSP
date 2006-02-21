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
/* gtkdatabox.h */

#ifndef __GTK_DATABOX_H__
#define __GTK_DATABOX_H__


#include <gdk/gdk.h>
#include <gtk/gtkvbox.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define GTK_TYPE_DATABOX            (gtk_databox_get_type ())
#define GTK_DATABOX(obj)            GTK_CHECK_CAST (obj, gtk_databox_get_type (), GtkDatabox)
#define GTK_DATABOX_CLASS(klass)    GTK_CHECK_CLASS_CAST (klass, gtk_databox_get_type (), GtkDataboxClass)
#define GTK_IS_DATABOX(obj)         GTK_CHECK_TYPE (obj, gtk_databox_get_type ())
#define GTK_IS_DATABOX_CLASS(klass) GTK_CHECK_CLASS_TYPE ((klass), GTK_TYPE_DATABOX))


typedef struct _GtkDatabox       GtkDatabox;
typedef struct _GtkDataboxClass  GtkDataboxClass;
typedef struct _GtkDataboxValue  GtkDataboxValue;
typedef struct _GtkDataboxCoord  GtkDataboxCoord;

typedef enum {
   GTK_DATABOX_NOT_DISPLAYED=0,
   GTK_DATABOX_POINTS,
   GTK_DATABOX_LINES,
   GTK_DATABOX_BARS,
} GtkDataboxDataType;

struct _GtkDataboxCoord
{
  gint x;
  gint y;
};

struct _GtkDataboxValue
{
  gfloat x;
  gfloat y;
};

struct _GtkDatabox
{
  GtkVBox box;

  GList *data;
  GtkWidget *table;
  GtkWidget *draw;
  GtkWidget *hrule;
  GtkWidget *vrule;
  GtkWidget *hscroll;
  GtkWidget *vscroll;
  GtkAdjustment *adjX;
  GtkAdjustment *adjY;
  GdkPixmap *pixmap;
  glong flags;
  guint max_points;
  GdkGC *select_gc;
  gint cross_border;
  GtkDataboxCoord size;
  GtkDataboxCoord marked;
  GtkDataboxCoord select;
  GtkDataboxValue min;
  GtkDataboxValue max;
  GtkDataboxValue top_left;
  GtkDataboxValue bottom_right;
  gboolean selection_flag;
  gfloat range_check_threshold;
};

struct _GtkDataboxClass
{
  GtkVBoxClass parent_class;

  void (* gtk_databox) (GtkDatabox *box);

  /* Funktion pointers for signals, needed (mostly) for gtk-- wrapper */
  void (* gtk_databox_zoomed) (GtkDatabox *box,GtkDataboxValue* top_left,
			       GtkDataboxValue* bottom_right);
  void (* gtk_databox_marked) (GtkDatabox *box,GtkDataboxCoord* marked);
  void (* gtk_databox_selection_started ) (GtkDatabox *box,
					   GtkDataboxCoord* marked);
  void (* gtk_databox_selection_changed ) (GtkDatabox *box,
					   GtkDataboxCoord* marked,
					   GtkDataboxCoord* select);
   void (* gtk_databox_selection_stopped )(GtkDatabox *box,
					   GtkDataboxCoord *marked,
					   GtkDataboxCoord *select);
   void (* gtk_databox_selection_canceled )(GtkDatabox *box);
 
};

guint		gtk_databox_get_type			(void);
GtkWidget*	gtk_databox_new				(void);

/* In contrast to earlier versions the show and hide functions for rulers */
/* and scrollbars create and destroy the corresponding widgets.  */

void		gtk_databox_show_rulers			(GtkDatabox *box);
void		gtk_databox_hide_rulers			(GtkDatabox *box);
void		gtk_databox_show_scrollbars		(GtkDatabox *box);
void		gtk_databox_hide_scrollbars		(GtkDatabox *box);


/* It is not very nice, but at least there is an X and Y axis :-) */
void		gtk_databox_show_cross			(GtkDatabox *box);
void		gtk_databox_hide_cross			(GtkDatabox *box);

/* Decide whether the selection box is filled or not */
void		gtk_databox_show_selection_filled	(GtkDatabox *box);
void		gtk_databox_hide_selection_filled	(GtkDatabox *box);

/* Selection is possible as default, but you may disable that feature */
void		gtk_databox_enable_selection		(GtkDatabox *box);
void		gtk_databox_disable_selection		(GtkDatabox *box);

/* Zooming into the data and back is enabled as default */
void		gtk_databox_enable_zoom			(GtkDatabox *box);
void		gtk_databox_disable_zoom		(GtkDatabox *box);

void 		gtk_databox_data_get_value		(GtkDatabox *box, GtkDataboxCoord coord, 
							GtkDataboxValue *value);
void		gtk_databox_data_get_extrema		(GtkDatabox *box, GtkDataboxValue *min, 
							GtkDataboxValue *max);
void		gtk_databox_data_get_visible_extrema	(GtkDatabox *box, 
							GtkDataboxValue *min, 
							GtkDataboxValue *max);

void 		gtk_databox_rescale			(GtkDatabox *box);
void		gtk_databox_rescale_with_values		(GtkDatabox *box, 
							GtkDataboxValue min, 
							GtkDataboxValue max);
void		gtk_databox_redraw			(GtkDatabox *box);

void		gtk_databox_set_range_check_threshold	(GtkDatabox *box, 
							gfloat threshold);

gint		gtk_databox_get_color			(GtkDatabox *box, 
							gint index, 
							GdkColor *color);
gint		gtk_databox_set_color			(GtkDatabox *box, 
							gint index, 
							GdkColor color);
gint		gtk_databox_get_data_type		(GtkDatabox *box, 
							gint index, 
							GtkDataboxDataType *type,
							guint *dot_size);
gint		gtk_databox_set_data_type		(GtkDatabox *box, 
							gint index, 
							GtkDataboxDataType type,
							guint dot_size);

gint		gtk_databox_data_add_x_y		(GtkDatabox *box, 
							guint length, 
							gfloat *X, 
							gfloat *Y, 
							GdkColor color,
							GtkDataboxDataType type,
							guint dot_size);
gint		gtk_databox_data_add_x			(GtkDatabox *box, 
							guint length, 
							gfloat *X, 
							gint shared_Y_index,
							GdkColor color,
							GtkDataboxDataType type,
							guint dot_size);
gint		gtk_databox_data_add_y			(GtkDatabox *box, 
							guint length, 
							gfloat *Y, 
							gint shared_X_index,
							GdkColor color,
							GtkDataboxDataType type,
							guint dot_size);

gint		gtk_databox_data_remove			(GtkDatabox *box, 
							gint index);
gint		gtk_databox_data_remove_all		(GtkDatabox *box);
gint		gtk_databox_data_destroy		(GtkDatabox *box, 
							gint index);
gint		gtk_databox_data_destroy_all		(GtkDatabox *box);

/* add-ons */
GtkWidget*      gtk_databox_get_drawing_area            (GtkDatabox *box);


/* These functions are now deprecated, please use the signals instead */
void 		gtk_databox_data_get_marked_value	(GtkDatabox *box, GtkDataboxValue *value);
void 		gtk_databox_data_get_delta_value	(GtkDatabox *box, GtkDataboxValue *value);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __GTK_DATABOX_H__ */

