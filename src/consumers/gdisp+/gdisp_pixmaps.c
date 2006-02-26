/*

$Id: gdisp_pixmaps.c,v 1.6 2006-02-26 14:08:23 erk Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer: tsp@astrium-space.com
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Pixmap utilities.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             PIXMAP INCLUSION
 --------------------------------------------------------------------
*/

#include "pixmaps/gdisp_gdispLogo.xpm"
#include "pixmaps/gdisp_okButton.xpm"
#include "pixmaps/gdisp_okButton2.xpm"
#include "pixmaps/gdisp_stopButton.xpm"
#include "pixmaps/gdisp_timeDigits.xpm"
#include "pixmaps/gdisp_2dLogo.xpm"
#include "pixmaps/gdisp_textLogo.xpm"
#include "pixmaps/gdisp_stubLogo.xpm"
#include "pixmaps/gdisp_resLogo.xpm"
#include "pixmaps/gdisp_collapsedNode.xpm"
#include "pixmaps/gdisp_expandedNode.xpm"
#include "pixmaps/gdisp_applyLogo.xpm"
#include "pixmaps/gdisp_doneLogo.xpm"
#include "pixmaps/gdisp_warningLogo.xpm"
#include "pixmaps/gdisp_errorLogo.xpm"
#include "pixmaps/gdisp_infoLogo.xpm"
#include "pixmaps/gdisp_magentaBall.xpm"
#include "pixmaps/gdisp_cyanBall.xpm"
#include "pixmaps/gdisp_yellowBall.xpm"
#include "pixmaps/gdisp_blueBall.xpm"
#include "pixmaps/gdisp_greenBall.xpm"
#include "pixmaps/gdisp_redBall.xpm"
#include "pixmaps/gdisp_gdispAnimLogo1.xpm"
#include "pixmaps/gdisp_gdispAnimLogo2.xpm"
#include "pixmaps/gdisp_gdispAnimLogo3.xpm"
#include "pixmaps/gdisp_gdispAnimLogo4.xpm"
#include "pixmaps/gdisp_gdispAnimLogo5.xpm"
#include "pixmaps/gdisp_gdispAnimLogo6.xpm"
#include "pixmaps/gdisp_gdispAnimLogo7.xpm"
#include "pixmaps/gdisp_gdispAnimLogo8.xpm"
#include "pixmaps/gdisp_gdispAnimLogo9.xpm"
#include "pixmaps/gdisp_gdispAnimLogo10.xpm"
#include "pixmaps/gdisp_gdispAnimLogo11.xpm"
#include "pixmaps/gdisp_gdispLargeLogo.xpm"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static gint
gdisp_checkPixmapById ( gconstpointer listItemData,
			gconstpointer pixmapIdAddr )
{

  Pixmap_T  *pixmap   =  (Pixmap_T*)listItemData;
  Pixmap_ID *pixmapId = (Pixmap_ID*)pixmapIdAddr;

  return (pixmap->id == (*pixmapId) ? 0 : 1);

}


static gint
gdisp_checkPixmapByAddr ( gconstpointer listItemData,
			  gconstpointer pixmapAddr )
{

  Pixmap_T  *pixmap     = (Pixmap_T*)listItemData;
  gchar    **pixmapData = (gchar**)pixmapAddr;

  return (pixmap->data == pixmapData ? 0 : 1);

}


static Pixmap_T*
gdisp_createPixmap ( Kernel_T   *kernel,
		     Pixmap_ID   pixmapId,
		     gchar     **pixmapAddr,
		     GtkWidget  *pixmapParent )
{

  Pixmap_T *requestedPixmap = (Pixmap_T*)NULL;
  GtkStyle *style           = (GtkStyle*)NULL;

  /*
   * All available pixmaps.
   */
  gchar **gdisp_pixmapAddrs[GD_PIX_NbPixmaps] = { gdisp_gdispLogo,
						  gdisp_okButton,
						  gdisp_okButton2,
						  gdisp_stopButton,
						  gdisp_timeDigits,
						  gdisp_2dLogo,
						  gdisp_textLogo,
						  gdisp_stubLogo,
						  gdisp_resLogo,
						  gdisp_collapsedNode,
						  gdisp_expandedNode,
						  gdisp_applyLogo,
						  gdisp_doneLogo,
						  gdisp_warningLogo,
						  gdisp_errorLogo,
						  gdisp_infoLogo,
						  gdisp_magentaBall,
						  gdisp_cyanBall,
						  gdisp_yellowBall,
						  gdisp_blueBall,
						  gdisp_greenBall,
						  gdisp_redBall,
						  gdisp_gdispAnimLogo1,
						  gdisp_gdispAnimLogo2,
						  gdisp_gdispAnimLogo3,
						  gdisp_gdispAnimLogo4,
						  gdisp_gdispAnimLogo5,
						  gdisp_gdispAnimLogo6,
						  gdisp_gdispAnimLogo7,
						  gdisp_gdispAnimLogo8,
						  gdisp_gdispAnimLogo9,
						  gdisp_gdispAnimLogo10,
						  gdisp_gdispAnimLogo11,
						  gdisp_gdispLargeLogo };


  /*
   * Allocate memory for this new pixmap.
   */
  requestedPixmap = (Pixmap_T*)g_malloc0(sizeof(Pixmap_T));
  assert(requestedPixmap);

  if (pixmapId == GD_PIX_NbPixmaps) {

    requestedPixmap->id   = pixmapId;
    requestedPixmap->data = pixmapAddr;

  }
  else {

    requestedPixmap->id   = pixmapId;
    requestedPixmap->data = gdisp_pixmapAddrs[pixmapId];

  }

  /*
   * Deduce width and height from pixmap data.
   */
  if (sscanf(requestedPixmap->data[0],
	     "%d %d",
	     &requestedPixmap->width,
	     &requestedPixmap->height) != 2) {

    requestedPixmap->width  = 1;
    requestedPixmap->height = 1; /* fallback */

  }

  /*
   * Take into account the parent style for transparency purpose.
   */
  style = gtk_widget_get_style(pixmapParent);

  /*
   * Now, create the pixmap according to specified data.
   */
  requestedPixmap->pixmap =
    gdk_pixmap_create_from_xpm_d(pixmapParent->window,
				 &requestedPixmap->mask,
				 &style->bg[GTK_STATE_NORMAL],
				 requestedPixmap->data);

  /*
   * Store it into the list.
   */
  kernel->widgets.pixmapTable = g_list_prepend(kernel->widgets.pixmapTable,
					       (gpointer)requestedPixmap);

  return requestedPixmap;

}

/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


Pixmap_T*
gdisp_getPixmapById ( Kernel_T  *kernel,
		      Pixmap_ID  pixmapId,
		      GtkWidget *pixmapParent )
{

  GList     *requestedPixmapListItem =    (GList*)NULL;
  Pixmap_T  *requestedPixmap         = (Pixmap_T*)NULL;
  gchar    **pixmapAddr              =   (gchar**)NULL;


  /*
   * Retreive requested pixmap.
   */
  if (kernel->widgets.pixmapTable != (GList*)NULL) {

    requestedPixmapListItem = g_list_find_custom(kernel->widgets.pixmapTable,
						 (gpointer)&pixmapId,
						 gdisp_checkPixmapById);

  }

  if (requestedPixmapListItem == (GList*)NULL) {

    requestedPixmap = gdisp_createPixmap(kernel,
					 pixmapId,
					 pixmapAddr,
					 pixmapParent);
  }
  else {

    requestedPixmap = (Pixmap_T*)requestedPixmapListItem->data;

  }

  return requestedPixmap;

}


Pixmap_T*
gdisp_getPixmapByAddr ( Kernel_T   *kernel,
			gchar     **pixmapAddr,
			GtkWidget  *pixmapParent )
{

  GList     *requestedPixmapListItem = (GList*)NULL;
  Pixmap_T  *requestedPixmap         = (Pixmap_T*)NULL;
  Pixmap_ID  pixmapId                = GD_PIX_NbPixmaps;


  /*
   * Retreive requested pixmap.
   */
  if (kernel->widgets.pixmapTable != (GList*)NULL) {

    requestedPixmapListItem = g_list_find_custom(kernel->widgets.pixmapTable,
						 (gpointer)pixmapAddr,
						 gdisp_checkPixmapByAddr);

  }

  if (requestedPixmapListItem == (GList*)NULL) {

    requestedPixmap = gdisp_createPixmap(kernel,
					 pixmapId,
					 pixmapAddr,
					 pixmapParent);
  }
  else {

    requestedPixmap = (Pixmap_T*)requestedPixmapListItem->data;

  }

  return requestedPixmap;

}


void
gdisp_destroyPixmaps ( Kernel_T *kernel )
{

  GList    *pixmapItem =    (GList*)NULL;
  Pixmap_T *pixmap     = (Pixmap_T*)NULL;


  /*
   * Loop over all created pixmaps, destroy them.
   */
  pixmapItem = g_list_first(kernel->widgets.pixmapTable);

  while (pixmapItem != (GList*)NULL) {

    pixmap = (Pixmap_T*)pixmapItem->data;

#if defined(GD_UNREF_THINGS)
    gdk_pixmap_unref(pixmap->pixmap);
    gdk_bitmap_unref(pixmap->mask  );
#endif

    pixmapItem = g_list_next(pixmapItem);

  }

  g_list_free(kernel->widgets.pixmapTable);

}
