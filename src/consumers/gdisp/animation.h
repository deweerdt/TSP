#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include <gtk/gtk.h>
#include <glib.h>



struct anim_data_t
{
  GdkPixmap** img;
  GdkBitmap** mask;
  GtkPixmap* pixmap;
  int current_img;
};

typedef struct anim_data_t anim_data_t;

anim_data_t* animation_create(GdkColormap* colormap);
void animation_next(anim_data_t* data);


#endif /*_ANIMATION_H_*/
