#include "animation.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "images_xpm.h"


anim_data_t* animation_create(GdkColormap* colormap)
{

  int i;
  anim_data_t* data;


  data = (anim_data_t*)g_malloc0(sizeof(anim_data_t));
  data->current_img = 0;
  data->img = (GdkPixmap**)g_malloc0(ANIMATION_NB_IMAGES*sizeof(GdkPixmap*));
  data->mask = (GdkBitmap**)g_malloc0(ANIMATION_NB_IMAGES*sizeof(GdkBitmap*));

  for(i = 0 ; i < ANIMATION_NB_IMAGES ; i++)
    {

      data->img[i] = gdk_pixmap_colormap_create_from_xpm_d(NULL, 
							   colormap,
							   &data->mask[i],
							   NULL, 
							   animation_images_xpm[i]);

      data->pixmap = GTK_PIXMAP(gtk_pixmap_new(data->img[data->current_img], data->mask[data->current_img]));      

    }
  
  return data;
}

void animation_next(anim_data_t* data)
{
  data->current_img = (data->current_img + 1) % ANIMATION_NB_IMAGES;
  gtk_pixmap_set(data->pixmap, data->img[data->current_img], data->mask[data->current_img]);      
   
}
