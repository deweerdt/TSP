

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <glib.h>

#include "cutnpast.h"

cutnpast_t* cutnpast_new(GdkWindow* window, GdkPixmap* target, GdkGC* clear_gc, gint width, gint height)
{
  
  cutnpast_t* h = (cutnpast_t*)g_malloc0(sizeof(cutnpast_t));

  h->buf = gdk_pixmap_new(window, width, height, -1);
  h->target = target;
  h->clear_gc = clear_gc;
  
  h->buf_gc = gdk_gc_new(h->buf);
  h->target_gc = gdk_gc_new(h->target);
  
  h->width = width;
  h->height = height;


}

void  cutnpast_destroy(cutnpast_t* h)
{
  gdk_gc_unref( h->buf_gc );
  gdk_gc_unref( h->target_gc );
  gdk_pixmap_unref(h->buf);
  g_free(h);
}

void cutnpast_cut_and_paste(cutnpast_t* h, gint xsrc, gint ysrc, gint xdest, gint ydest)
{

  /* Copy area on buf, and from buf to target */

  	/*gdk_draw_pixmap (h->buf,
			 h->buf_gc,
			 h->target,
			 xsrc,ysrc,
			 0, 0,
			 h->width,
			 h->height);

	gdk_draw_rectangle(h->target,
			  h->clear_gc,
			  TRUE,
			  xsrc,ysrc,
			 h->width,
			 h->height);

  	gdk_draw_pixmap (h->target,
			 h->target_gc,
			 h->buf,
			 0,0,
			 xdest, ydest,
			 -1,
			 -1);*/

  gdk_draw_pixmap (h->target,
		   h->target_gc,
		   h->target,
		   xsrc,ysrc,
		   xdest, ydest,
		   h->width,
		   h->height);


}
