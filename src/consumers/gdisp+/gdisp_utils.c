/*!  \file 

$Id: gdisp_utils.c,v 1.3 2004-05-11 19:47:52 esteban Exp $

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

File      : GDISP utilities.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"
#include "gdisp_fonts.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Button with a label and a pixmap.
 */
static GtkWidget*
gdisp_createLabelPixmapBox (GtkWidget  *parent,
			    gchar     **pixmapData,
			    gchar      *labelData)
{

  GtkWidget *hBox    = (GtkWidget*)NULL;
  GtkWidget *label   = (GtkWidget*)NULL;
  GtkWidget *wPixmap = (GtkWidget*)NULL;
  GdkPixmap *pixmap  = (GdkPixmap*)NULL;
  GdkBitmap *mask    = (GdkBitmap*)NULL;
  GtkStyle  *style   =  (GtkStyle*)NULL;


  assert(pixmapData);
  assert(labelData);


  /*
   * Create horizontal box for nesting the pixmap and the label.
   */
  hBox = gtk_hbox_new(FALSE, /* homogeneous */
		      1      /* spacing     */ );
  gtk_container_set_border_width(GTK_CONTAINER(hBox),2);


  /*
   * Get the style of the button to get the background color.
   */
  style = gtk_widget_get_style(parent);


  /*
   * Now, create the pixmap according to specified data.
   */
  pixmap = gdk_pixmap_create_from_xpm_d(parent->window,
					&mask,
					&style->bg[GTK_STATE_NORMAL],
					pixmapData);
  wPixmap = gtk_pixmap_new(pixmap,mask);


  /*
   * Create a label according to specified data.
   */
  label = gtk_label_new(labelData);


  /*
   * Pack the pixmap and label into the box.
   */
  gtk_box_pack_start(GTK_BOX(hBox),
		     wPixmap,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     3     /* padding */);

  gtk_box_pack_start(GTK_BOX(hBox),
		     label,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     3     /* padding */);


  /*
   * Show all widgets.
   */
  gtk_widget_show(wPixmap);
  gtk_widget_show(label);
  gtk_widget_show(hBox);


  /*
   * Return the packing box.
   */
  return(hBox);

}


#include "pixmaps/gdisp_applyLogo.xpm"
#include "pixmaps/gdisp_doneLogo.xpm"


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * PGCD computation.
 */
guint
gdisp_computePgcd (void)
{

  guint a = 8136;
  guint b =  492;
  guint c =    1;

  while (c != 0) {

    c = a % b;

    a = b;

    b = c;

  }

  return a;

}


/*
 * Take here a time stamp expressed in 'nanoseconds'.
 */
HRTime_T
gdisp_getHRTime (void)
{
  /*
   * Returned time is expressed in 'nanoseconds'.
   */
#if defined(GDISP_SYSTEM_HAVE_GETHRTIME)

  return gethrtime();

#else

  struct timeval tp;

  gettimeofday(&tp,(void*)NULL);

  return (  (HRTime_T)tp.tv_sec  * G_GINT64_CONSTANT(1000000000)
	  + (HRTime_T)tp.tv_usec * G_GINT64_CONSTANT(      1000));

#endif
}



/*
 * Sleep for a while expressed in 'microseconds'.
 */
void
gdisp_uSleep (guint uSeconds)
{

#if defined(GDISP_SYSTEM_HAVE_NANOSLEEP)

  struct timespec ts;

  ts.tv_sec  =  uSeconds / 1000000;
  ts.tv_nsec = (uSeconds % 1000000) * 1000;

  nanosleep(&ts,(struct timespec*)NULL);

#else

#if defined(GDISP_SYSTEM_HAVE_THREADSAFE_USLEEP)

  usleep(uSeconds);

#endif /* GDISP_SYSTEM_HAVE_THREADSAFE_USLEEP */

#endif /* GDISP_SYSTEM_HAVE_NANOSLEEP */

}


/*
 * Wait 'time2wait' and no more than 'time2wait', from 'timeStamp'.
 * ---> Requested 'time2wait' expressed in nano-seconds.
 */
HRTime_T
gdisp_waitTime (HRTime_T timeStamp,
		HRTime_T time2wait)
{

  struct timespec realDelta;

  /*
   * Current time 'now' is expressed in 'nanoseconds'.
   */
  HRTime_T now           = gdisp_getHRTime();
  HRTime_T realTime2wait = time2wait - (now - timeStamp);

  if (realTime2wait >= 0) {

    realDelta.tv_sec  = realTime2wait / _ONE_SECOND_IN_NANOSECONDS_;
    realDelta.tv_nsec = realTime2wait % _ONE_SECOND_IN_NANOSECONDS_;

    nanosleep(&realDelta,(struct timespec*)NULL);

  }

  return gdisp_getHRTime();

}


/*
 * Create a button bar.
 * By now, only 'apply' and 'done' buttons are available.
 */
GtkWidget*
gdisp_createButtonBar (GtkWidget  *window,
		       GtkWidget **applyButton,
		       GtkWidget **doneButton)
{

  GtkWidget *hBox       = (GtkWidget*)NULL;
  GtkWidget *hBoxButton = (GtkWidget*)NULL;


  /*
   * Create horizontal box for nesting all buttons.
   */
  hBox = gtk_hbox_new(FALSE, /* homogeneous */
		      1      /* spacing     */ );
  gtk_container_set_border_width(GTK_CONTAINER(hBox),2);


  /*
   * 'done' button.
   */
  if (doneButton != (GtkWidget**)NULL) {

    /*
     * Create a new button.
     */
    *doneButton = gtk_button_new();
    hBoxButton  = gdisp_createLabelPixmapBox(window,
					     gdisp_doneLogo,
					     "Done");
    gtk_container_add(GTK_CONTAINER(*doneButton),
		      hBoxButton);

    gtk_box_pack_end(GTK_BOX(hBox),
		     *doneButton,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     3     /* padding */);

    gtk_widget_show(*doneButton);

  }


  /*
   * 'apply' button.
   */
  if (applyButton != (GtkWidget**)NULL) {

    /*
     * Create a new button.
     */
    *applyButton = gtk_button_new();
    hBoxButton   = gdisp_createLabelPixmapBox(window,
					      gdisp_applyLogo,
					      "Apply");
    gtk_container_add(GTK_CONTAINER(*applyButton),
		      hBoxButton);

    gtk_box_pack_end(GTK_BOX(hBox),
		     *applyButton,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     3     /* padding */);

    gtk_widget_show(*applyButton);

  }


  /*
   * finally, show the horizontal packing box.
   */
  gtk_widget_show(hBox);

  return hBox;

}


/*
 * Font loading.
 */
void
gdisp_loadFonts(GdkFont *fonts[GD_FONT_MAX_SIZE][GD_FONT_MAX_TYPE])
{

  gchar *fontNames[GD_FONT_MAX_SIZE][GD_FONT_MAX_TYPE] =
    { { GD_NormalFont_10_, GD_ItalicFont_10_, GD_FixedFont_10_ },
      { GD_NormalFont_12_, GD_ItalicFont_12_, GD_FixedFont_12_ },
      { GD_NormalFont_14_, GD_ItalicFont_14_, GD_FixedFont_14_ } };

  FontSize_T fontSize = GD_FONT_SMALL;
  FontType_T fontType = GD_FONT_NORMAL;

  /*
   * Back up to default font in case of problem.
   */
  for (fontSize=GD_FONT_SMALL; fontSize<GD_FONT_MAX_SIZE; fontSize++) {

    for (fontType=GD_FONT_NORMAL; fontType<GD_FONT_MAX_TYPE; fontType++) {

      fonts[fontSize][fontType] =
	gdk_font_load(fontNames[fontSize][fontType]);

      if (fonts[fontSize][fontType] == (GdkFont*)NULL)
	fonts[fontSize][fontType] = gdk_font_load(GD_DefaultFontName_);

    } /* loop on types */

  } /* loop on sizes */

}


/*
 * Font destruction.
 */
void
gdisp_destroyFonts(GdkFont *fonts[GD_FONT_MAX_SIZE][GD_FONT_MAX_TYPE])
{

  FontSize_T fontSize = GD_FONT_SMALL;
  FontType_T fontType = GD_FONT_NORMAL;

  /*
   * Destroy all fonts.
   */
  for (fontSize=GD_FONT_SMALL; fontSize<GD_FONT_MAX_SIZE; fontSize++) {

    for (fontType=GD_FONT_NORMAL; fontType<GD_FONT_MAX_TYPE; fontType++) {

      gdk_font_unref(fonts[fontSize][fontType]);

    } /* loop on types */

  } /* loop on sizes */

}


/*
 * Get back pixmap according to message type.
 */
#include "pixmaps/gdisp_warningLogo.xpm"
#include "pixmaps/gdisp_errorLogo.xpm"
#include "pixmaps/gdisp_infoLogo.xpm"

GtkWidget*
gdisp_getMessagePixmaps (Kernel_T  *kernel,
			 GtkWidget *parent,
			 Message_T  messageType,
			 gchar     *message)
{

  GtkWidget *pixmapWidget = (GtkWidget*)NULL;
  GdkBitmap *mask         = (GdkBitmap*)NULL;
  GtkWidget *hBox         = (GtkWidget*)NULL;
  GtkWidget *label        = (GtkWidget*)NULL;
  GtkStyle  *style        =  (GtkStyle*)NULL;


  /*
   * Start by creating a horizontal packing box.
   */
  hBox = gtk_hbox_new(FALSE, /* homogeneous */
		      3      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(hBox),0);
  gtk_widget_show(hBox);

  /*
   * Choose the correct logo according to the message type.
   * Avoid creating the pixmap each time the function is called.
   * Remember the pixmap address in the Kernel.
   */
  style = gtk_widget_get_style(parent);
  switch (messageType) {

  case GD_MESSAGE :

    if (kernel->widgets.mainBoardInfoPixmap == (GdkPixmap*)NULL) {

      kernel->widgets.mainBoardInfoPixmap =
	gdk_pixmap_create_from_xpm_d(parent->window,
				     &mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar**)gdisp_infoLogo);
      kernel->widgets.mainBoardInfoPixmapMask = mask;

    }

    pixmapWidget = gtk_pixmap_new(kernel->widgets.mainBoardInfoPixmap,
				  kernel->widgets.mainBoardInfoPixmapMask);
    gtk_widget_show(pixmapWidget);
    break;

  case GD_WARNING :

    if (kernel->widgets.mainBoardWarningPixmap == (GdkPixmap*)NULL) {

      kernel->widgets.mainBoardWarningPixmap =
	gdk_pixmap_create_from_xpm_d(parent->window,
				     &mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar**)gdisp_warningLogo);
      kernel->widgets.mainBoardWarningPixmapMask = mask;

    }

    pixmapWidget = gtk_pixmap_new(kernel->widgets.mainBoardWarningPixmap,
				  kernel->widgets.mainBoardWarningPixmapMask);
    gtk_widget_show(pixmapWidget);
    break;

  case GD_ERROR   :

    if (kernel->widgets.mainBoardErrorPixmap == (GdkPixmap*)NULL) {

      kernel->widgets.mainBoardErrorPixmap =
	gdk_pixmap_create_from_xpm_d(parent->window,
				     &mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar**)gdisp_errorLogo);
      kernel->widgets.mainBoardErrorPixmapMask = mask;

    }

    pixmapWidget = gtk_pixmap_new(kernel->widgets.mainBoardErrorPixmap,
				  kernel->widgets.mainBoardErrorPixmapMask);
    gtk_widget_show(pixmapWidget);
    break;

  default :
    break;

  }

  gtk_box_pack_start(GTK_BOX(hBox),
		     pixmapWidget,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);

  /*
   * Create the label with the message.
   */
  label = gtk_label_new(message);

  gtk_box_pack_start(GTK_BOX(hBox),
		     label,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);

  gtk_widget_show(label);

  return hBox;

}


/*
 * Get back pixmap according to provider identity.
 */
#include "pixmaps/gdisp_magentaBall.xpm"
#include "pixmaps/gdisp_cyanBall.xpm"
#include "pixmaps/gdisp_yellowBall.xpm"
#include "pixmaps/gdisp_blueBall.xpm"
#include "pixmaps/gdisp_greenBall.xpm"
#include "pixmaps/gdisp_redBall.xpm"

void
gdisp_getProviderIdPixmap ( Kernel_T   *kernel,
			    GtkWidget  *parent,
			    guint       providerIdentity,
			    GdkPixmap **identityPixmap,
			    GdkBitmap **identityPixmapMask )
{

  GtkStyle   *style  =  (GtkStyle*)NULL;
  GdkPixmap  *pixmap = (GdkPixmap*)NULL;
  GdkBitmap  *mask   = (GdkBitmap*)NULL;
  gchar     **pixmapTable[GD_MAX_PROVIDER_NUMBER] = { gdisp_magentaBall,
						      gdisp_cyanBall,
						      gdisp_yellowBall,
						      gdisp_blueBall,
						      gdisp_greenBall,
						      gdisp_redBall };

  /*
   * Get back parent widget style.
   */
  style = gtk_widget_get_style(parent);

  /*
   * Get back the correct pixmap according to provider identity.
   * Create the pixmap if it does not already exist.
   */
  pixmap = kernel->widgets.providerPixmaps[providerIdentity];
  if (pixmap == (GdkPixmap*)NULL) {

    pixmap = gdk_pixmap_create_from_xpm_d(parent->window,
					  &mask,
					  &kernel->colors[_WHITE_],
					  pixmapTable[providerIdentity]);

    kernel->widgets.providerPixmaps    [providerIdentity] = pixmap;
    kernel->widgets.providerPixmapMasks[providerIdentity] = mask;

  }

  *identityPixmap     = pixmap;
  *identityPixmapMask = mask;

}


/*
 * Convert a string list to a string table.
 * A string list is a list of coma-separated string as follows :
 *
 *           gchar *stringList      = "boat,car,bus,train";
 *
 * This string list is converted into a string table as follows :
 *
 *           gchar *stringTable[]   = { "boat", "car", "bus", "train" };
 *           guint  stringTableSize = 4;
 */
void
gdisp_getStringTableFromStringList ( gchar   *stringList,
				     gchar ***stringTable,
				     guint   *stringTableSize )
{

  gchar  *delimiter =           ",";
  gchar **token     = (gchar**)NULL;

  /*
   * Initialisation.
   */
  *stringTable     = (gchar**)NULL;
  *stringTableSize = 0;

  /*
   * Checks.
   */
  if (stringList == (gchar*)NULL || strlen(stringList) == 0) {

    return;

  }

  /*
   * Use GLib string functions to perform the work.
   */
  *stringTable = g_strsplit(stringList,
			    delimiter,
			    (gint)0 /* split all the string */);

  /*
   * Compute the number of created strings in the table.
   */
  token = *stringTable;
  while (*token != (gchar*)NULL) {

    token++;
    (*stringTableSize)++; /* Increase the content, not the pointer */

  }

}


void
gdisp_freeStringTable ( gchar ***stringTable,
			guint   *stringTableSize )
{

  /*
   * Use GLib string functions to perform the work.
   */
  g_strfreev(*stringTable);

  *stringTable     = (gchar**)NULL; 
  *stringTableSize = 0;

}


/*
 * Look into the string table and search in each string an occurence
 * of the string 'name'.
 */
gchar*
gdisp_strStr ( gchar   *name,
	       gchar  **stringTable,
	       guint    stringTableSize )
{

  gchar **stringPtr = stringTable;

  /*
   * Run over the whole string table...
   */
  while (stringPtr < stringTable + stringTableSize) {

    /*
     * ... and try to find the requested "name".
     */
    if (strstr(name,*stringPtr) != (gchar*)NULL) {

      /*
       * Got you...
       */
      return *stringPtr;

    }

    stringPtr++;

  }

  /*
   * Nothing emerged from the process...
   */
  return (gchar*)NULL;

}


/*
 * Loop over all graphic plots of graphic pages, and apply
 * the given callback to each plots.
 */
void
gdisp_loopOnGraphicPlots ( Kernel_T  *kernel,
			   void     (*callback)(Kernel_T*,
						Page_T*,
						PlotSystemData_T*,
						void*),
			   void      *userData )
{

  PlotSystemData_T *plotSystemData    = (PlotSystemData_T*)NULL;
  PlotSystemData_T *plotSystemDataEnd = (PlotSystemData_T*)NULL;
  GList            *pageItem          =            (GList*)NULL;
  Page_T           *page              =           (Page_T*)NULL;


  /* ************************************************************
   *
   * BEGIN : Loop over all graphic plots of all pages, do actions...
   *
   * ************************************************************/

  pageItem = g_list_first(kernel->pageList);
  while (pageItem != (GList*)NULL) {

    page = (Page_T*)pageItem->data;

    plotSystemData    = page->pPlotSystemData;
    plotSystemDataEnd = page->pPlotSystemData + (page->pRows * page->pColumns);

    while (plotSystemData < plotSystemDataEnd) {

      (*callback)(kernel,
		  page,
		  plotSystemData,
		  userData),

      plotSystemData++;

    }

    pageItem = g_list_next(pageItem);

  }

  /* ************************************************************
   *
   * END.
   *
   * ************************************************************/

}


