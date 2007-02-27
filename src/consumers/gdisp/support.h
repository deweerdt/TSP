
#ifndef _SUPPORT_H_
#define _SUPPORT_H_

#include <gtk/gtk.h>
#include <config.h>



/*
 * Public Functions.
 */

/* Don't forget to update variable_type AND variable_type_str */
typedef enum {VAR_DOUBLE, VAR_TITLE, VAR_HEXA, VAR_BIN, VAR_STRING } variable_type;
typedef enum {WIDGET_VIEW, WIDGET_DRAW } widget_type_t;

gboolean is_visible (GtkWidget *widget);
void set_style_recursively (GtkWidget *, gpointer);

/*
 * This function returns a widget in a component created by Glade.
 * Call it with the toplevel widget in the component (i.e. a window/dialog),
 * or alternatively any widget in the component, and the name of the widget
 * you want returned.
 */
GtkWidget*  lookup_widget              (GtkWidget       *widget,
                                        const gchar     *widget_name);

/* get_widget() is deprecated. Use lookup_widget instead. */
#define get_widget lookup_widget

/* Use this function to set the directory containing installed pixmaps. */
void        add_pixmap_directory       (const gchar     *directory);


/*
 * Private Functions.
 */

/* This is used to create the pixmaps in the interface. */
GtkWidget*  create_pixmap              (GtkWidget       *widget,
                                        const gchar     *filename);
#endif
