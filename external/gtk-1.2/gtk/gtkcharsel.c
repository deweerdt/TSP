/* gtkcharselection - character selection dialog for gtk+
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

#include <gtk/gtk.h>

#include <string.h>
#include "gtkcharsel.h"

static void gtk_char_selection_class_init          (GtkCharSelectionClass *klass);
static void gtk_char_selection_init                (GtkCharSelection *charsel);
static void gtk_char_selection_realize 		   (GtkWidget *widget);
static void gtk_char_selection_map 		   (GtkWidget *widget);
static void new_font				   (GtkFontCombo *font_combo, 
						    gpointer data);
static void new_selection			   (GtkButton *button, 
                                                    gpointer data);

static GtkWindowClass *parent_class = NULL;


GtkType
gtk_char_selection_get_type (void)
{
  static GtkType charsel_type = 0;
  
  if (!charsel_type)
    {
      GtkTypeInfo charsel_info =
      {
	"GtkCharSelection",
	sizeof (GtkCharSelection),
	sizeof (GtkCharSelectionClass),
	(GtkClassInitFunc) gtk_char_selection_class_init,
	(GtkObjectInitFunc) gtk_char_selection_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };
      
      charsel_type = gtk_type_unique (gtk_window_get_type(), &charsel_info);
    }
  
  return charsel_type;
}

GtkWidget*
gtk_char_selection_new (void)
{
  GtkWidget *widget;

  widget = gtk_widget_new (gtk_char_selection_get_type(), NULL);

  return widget;
}

static void
gtk_char_selection_class_init (GtkCharSelectionClass *klass)
{
  GtkWidgetClass *widget_class;
  
  widget_class = (GtkWidgetClass*) klass;
  parent_class = gtk_type_class (gtk_window_get_type ());

  widget_class->realize = gtk_char_selection_realize;
  widget_class->map = gtk_char_selection_map;
}

static void
gtk_char_selection_init (GtkCharSelection *charsel)
{
  GtkWidget *main_vbox;
  GtkWidget *action_area;
  GtkWidget *frame;
  GtkWidget *separator;
  GtkWidget *label;
  gint i;

  charsel->selection = -1;

  gtk_window_set_policy(GTK_WINDOW(charsel), FALSE, FALSE, FALSE);
  gtk_window_set_title(GTK_WINDOW(charsel), "Select Character");
  gtk_container_set_border_width (GTK_CONTAINER (charsel), 10);

  main_vbox=gtk_vbox_new(FALSE, 5);
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 0);
  gtk_container_add(GTK_CONTAINER(charsel), main_vbox);
  gtk_widget_show(main_vbox);

  charsel->font_combo = GTK_FONT_COMBO(gtk_font_combo_new());
  gtk_box_pack_start(GTK_BOX(main_vbox), GTK_WIDGET(charsel->font_combo), TRUE, TRUE, 0);
  label = gtk_label_new("Font:   ");
  gtk_toolbar_prepend_element(GTK_TOOLBAR(charsel->font_combo),
                              GTK_TOOLBAR_CHILD_WIDGET,
                              label,
                              "Font", "Font", "Font",
                              NULL, NULL, NULL);
  gtk_widget_show(label);
  gtk_widget_show(GTK_WIDGET(charsel->font_combo));


  frame = gtk_frame_new(NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_IN);
  gtk_box_pack_start(GTK_BOX(main_vbox), frame, TRUE, TRUE, 0);
  gtk_widget_show(frame);


  charsel->table = GTK_TABLE(gtk_table_new(FALSE, 8, 32));
  gtk_container_add(GTK_CONTAINER(frame), GTK_WIDGET(charsel->table));
  gtk_widget_show(GTK_WIDGET(charsel->table));

  for(i = 0; i < 256; i++){
    gint x, y;
    y = i / 32;
    x = i % 32;

    charsel->button[i] = GTK_TOGGLE_BUTTON(gtk_toggle_button_new());
    gtk_container_set_border_width(GTK_CONTAINER(charsel->button[i]), 0);
    gtk_table_attach_defaults(charsel->table, 
                              GTK_WIDGET(charsel->button[i]),
                              x, x+1, y, y+1);
/*
    gtk_button_set_relief(GTK_BUTTON(charsel->button[i]), GTK_RELIEF_NONE);
*/
    gtk_widget_set_usize(GTK_WIDGET(charsel->button[i]), 18, 18);

    gtk_widget_show(GTK_WIDGET(charsel->button[i]));

    gtk_signal_connect(GTK_OBJECT(charsel->button[i]), "clicked",
                       GTK_SIGNAL_FUNC(new_selection),
                       charsel);
  }


  /* Action Area */

  separator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, TRUE, TRUE, 0);
  gtk_widget_show(separator);

  charsel->action_area = action_area = gtk_hbutton_box_new ();
  gtk_button_box_set_layout(GTK_BUTTON_BOX(action_area), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing(GTK_BUTTON_BOX(action_area), 5);
  gtk_box_pack_end (GTK_BOX (main_vbox), action_area, FALSE, FALSE, 0);
  gtk_widget_show (action_area);

  charsel->ok_button = gtk_button_new_with_label ("OK");
  gtk_box_pack_start (GTK_BOX (action_area), charsel->ok_button, TRUE, TRUE, 0);
  gtk_widget_show (charsel->ok_button);

  charsel->cancel_button = gtk_button_new_with_label ("Cancel");
  gtk_box_pack_start (GTK_BOX (action_area), charsel->cancel_button, TRUE, TRUE, 0);
  gtk_widget_show (charsel->cancel_button);

  /* Signals */

  gtk_signal_connect(GTK_OBJECT(charsel->font_combo), "changed",
                     GTK_SIGNAL_FUNC(new_font), charsel);

  new_font(charsel->font_combo, charsel); 
}

static void
gtk_char_selection_realize (GtkWidget *widget)
{
  GtkCharSelection *charsel;

  charsel = GTK_CHAR_SELECTION(widget);

  GTK_WIDGET_CLASS(parent_class)->realize(widget);
}

static void
gtk_char_selection_map (GtkWidget *widget)
{
  GtkCharSelection *charsel;

  charsel = GTK_CHAR_SELECTION(widget);

  GTK_WIDGET_CLASS(parent_class)->map(widget);

  new_font(charsel->font_combo, charsel); 
}


static void
new_font(GtkFontCombo *font_combo, gpointer data)
{
  GtkCharSelection *charsel;
  GdkColor color;
  GdkPixmap *pixmap;
  GtkWidget *wpixmap;
  gint i;
 
  charsel = GTK_CHAR_SELECTION(data);
 
  gdk_color_white(gtk_widget_get_colormap(GTK_WIDGET(charsel)), &color);

  for(i = 0; i < 256; i++){
    GtkWidget *widget;
    GtkRequisition req;
    gint width;
    gchar s[2];
    s[0] = i;
    s[1] = '\0';

    widget = GTK_WIDGET(charsel->button[i]);

    if(GTK_BIN(widget)->child)
      gtk_container_remove(GTK_CONTAINER(widget), GTK_BIN(widget)->child);

    req.width = gdk_char_width_wc(font_combo->font, s[0]);
    req.height = font_combo->font->ascent + font_combo->font->descent;
    width = MAX(req.width + 8, 3 * req.height / 2);

    if(GTK_WIDGET_MAPPED(widget)){
      pixmap = gdk_pixmap_new(widget->window, width, width, -1);
      gdk_draw_rectangle(pixmap, widget->style->white_gc, TRUE, 0, 0, width, width);
      gdk_draw_text(pixmap, font_combo->font, widget->style->fg_gc[0], width/2 - req.width/2, width/2 + (font_combo->font->ascent - font_combo->font->descent)/2, s, 1);
      wpixmap = gtk_pixmap_new(pixmap, NULL);
      gtk_container_add (GTK_CONTAINER (charsel->button[i]), wpixmap);
      gtk_widget_show(wpixmap);
      gdk_pixmap_unref(pixmap);
    }

    width += 2 * (GTK_CONTAINER(widget)->border_width + widget->style->klass->xthickness);
    gtk_widget_set_usize(widget, width, width);

    if(charsel->selection == i)
      gtk_toggle_button_set_active(charsel->button[i], TRUE);
    else
      gtk_toggle_button_set_active(charsel->button[i], FALSE);
  }

}

static void 
new_selection(GtkButton *button, gpointer data)
{
  GtkCharSelection *charsel;
  gint i;
  gint new_selection = -1;

  charsel = GTK_CHAR_SELECTION(data);

  for(i = 0; i < 256; i++){
    if(button == GTK_BUTTON(charsel->button[i])){
          new_selection = i;
          break;
    }
  }

  if(new_selection == charsel->selection){
      GTK_BUTTON(charsel->button[new_selection])->button_down=TRUE;
      GTK_TOGGLE_BUTTON(charsel->button[new_selection])->active=TRUE;
      gtk_widget_set_state(GTK_WIDGET(charsel->button[new_selection]), GTK_STATE_ACTIVE);

      return;
  }

  if(new_selection != -1){
    gtk_char_selection_set_selection(charsel, new_selection);
  } 
}

gint
gtk_char_selection_get_selection(GtkCharSelection *charsel)
{
  return (charsel->selection);
}

void
gtk_char_selection_set_selection(GtkCharSelection *charsel, gint selection)
{
  if(selection >= 256) return;

  if(charsel->selection >= 0){
      GTK_BUTTON(charsel->button[charsel->selection])->button_down=FALSE;
      GTK_TOGGLE_BUTTON(charsel->button[charsel->selection])->active=FALSE;
      gtk_widget_set_state(GTK_WIDGET(charsel->button[charsel->selection]), GTK_STATE_NORMAL);

      if(GTK_WIDGET_MAPPED(GTK_WIDGET(charsel)))
         gtk_widget_queue_draw(GTK_WIDGET(charsel->button[charsel->selection]));
  }

  charsel->selection = selection;

  if(charsel->selection >= 0){
      GTK_BUTTON(charsel->button[selection])->button_down=TRUE;
      GTK_TOGGLE_BUTTON(charsel->button[selection])->active=TRUE;
      gtk_widget_set_state(GTK_WIDGET(charsel->button[selection]), GTK_STATE_ACTIVE);

      if(GTK_WIDGET_MAPPED(GTK_WIDGET(charsel)))
         gtk_widget_queue_draw(GTK_WIDGET(charsel->button[selection]));
  }
}
