#include <gtk/gtk.h>
#include <glib.h>
#include <assert.h>


#include "page.h"
#include "gview.h"
#include "config.h"



extern display_page* pages;


void 
page_variables_add (int page) {
  GtkWidget* var_widget;
  GtkWidget *event_box;
  variable *new_var;
  guint row, col;
  GtkTable *table;
  GtkWidget *draw;
  gchar *text;
  double val_double = 0.0;



  /* Get the table from the window (we know its the only container) */
  table = GTK_TABLE(GTK_BIN(pages[page].window)->child);

  /* Set up the label, justification seems to have problems ... */
  switch(widget_type)
    {
    case WIDGET_VIEW :
      var_widget = gtk_label_new("");
      gtk_misc_set_alignment(GTK_MISC(var_widget), 0.5f, 0.5f);
      break;
    case WIDGET_DRAW :
      var_widget = plotwindow_new ();
      set_title(PLOTWINDOW(var_widget),var_text );
      break;
    default:
      assert(0);
    }

  gtk_widget_show(var_widget);

  event_box = gtk_event_box_new ();

  gtk_container_add(GTK_CONTAINER(event_box), var_widget);

  /*gtk_misc_set_alignment(GTK_MISC(event_box), 0.0, 0.5);*/
  
  gtk_widget_show(GTK_WIDGET(event_box));


  /* Compute the cell where we're going to put the widget */
  row = pages[page].variables->len % pages[page].rows;
  col = floor(pages[page].variables->len / pages[page].rows);

  /* Add the widget. 
     FIXME : 3,3 = widget spacing, should be a const at least. 
  */
  /* FIXME : Il faudrait rajouter non pas le label, mais un conteneur contenant le label pour changer sa col Y.D */
/*  vbox = gtk_vbox_new(TRUE, 3);
  gtk_box_pack_start_defaults(GTK_BOX(vbox), GTK_WIDGET(label));
  gtk_widget_show(vbox);*/
  gtk_table_attach (GTK_TABLE (table), GTK_WIDGET(event_box), col, col+1, row, row+1, 
                    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND),
		    (GtkAttachOptions) (GTK_FILL | GTK_EXPAND), 3, 3);

  /*gtk_widget_show(GTK_WIDGET(table));*/

  /* Setup the variable attributes */
  new_var->type = type;
  new_var->widget = var_widget;
  new_var->text = g_strdup(var_text);
  new_var->legend = legend == NULL ? NULL : g_strdup(legend);
  new_var->widget_type = widget_type;

}

