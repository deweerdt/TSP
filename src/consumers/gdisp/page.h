#ifndef _PAGE_H_
#define _PAGE_H_

#include "support.h"

typedef struct _variable {
  variable_type type;
  GtkWidget *widget;
  gchar *text;
  gchar *legend;
  double double_value;
  int provider_global_index;
  int period;
  double duration; /**< meaningless for text widget */
  widget_type_t widget_type;  
} variable;

typedef struct _page_position {
  guint x;
  guint y;
  guint width;
  guint height;
} page_position;

typedef struct _display_page {
  GtkWindow *window;
  char *title;
  char *filename;
  GPtrArray *variables;
  gint rows;
  page_position position;
  int default_is_visible;
  int no_border;
} display_page;


#endif /* _PAGE_H_ */
