/*

$Id: gdisp_utils.c,v 1.12 2006-07-30 20:25:58 esteban Exp $

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
gdisp_createLabelPixmapBox (Kernel_T  *kernel,
			    GtkWidget *parent,
			    Pixmap_ID  pixmapId,
			    gchar     *labelData)
{

  GtkWidget *hBox    = (GtkWidget*)NULL;
  GtkWidget *label   = (GtkWidget*)NULL;
  GtkWidget *wPixmap = (GtkWidget*)NULL;
  Pixmap_T  *pixmap  =  (Pixmap_T*)NULL;

  /*
   * Create horizontal box for nesting the pixmap and the label.
   */
  hBox = gtk_hbox_new(FALSE, /* homogeneous */
		      1      /* spacing     */ );
  gtk_container_set_border_width(GTK_CONTAINER(hBox),2);


  /*
   * Now, create the pixmap according to specified data.
   */
  pixmap = gdisp_getPixmapById(kernel,
			       pixmapId,
			       parent);

  wPixmap = gtk_pixmap_new(pixmap->pixmap,
			   pixmap->mask);


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


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * PGCD computation.
 */
guint
gdisp_computePgcd ( guint a,
		    guint b )
{

  guint pgcd    = MAX(a,b);
  guint divisor = MIN(a,b);
  guint remain  = 1;

  while (remain != 0) {

    remain  = pgcd % divisor;
    pgcd    = divisor;
    divisor = remain;

  }

  return pgcd;

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
gdisp_createButtonBar (Kernel_T   *kernel,
		       GtkWidget  *window,
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
    hBoxButton  = gdisp_createLabelPixmapBox(kernel,
					     window,
					     GD_PIX_doneButton,
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
    hBoxButton   = gdisp_createLabelPixmapBox(kernel,
					      window,
					      GD_PIX_applyButton,
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
GtkWidget*
gdisp_getMessagePixmaps (Kernel_T  *kernel,
			 GtkWidget *parent,
			 Message_T  messageType,
			 gchar     *message)
{

  GtkWidget *pixmapWidget = (GtkWidget*)NULL;
  GtkWidget *hBox         = (GtkWidget*)NULL;
  GtkWidget *label        = (GtkWidget*)NULL;
  Pixmap_T  *pixmap       =  (Pixmap_T*)NULL;


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
  switch (messageType) {

  case GD_MESSAGE :

    pixmap = gdisp_getPixmapById(kernel,
				 GD_PIX_info,
				 parent);
    break;

  case GD_WARNING :

    pixmap = gdisp_getPixmapById(kernel,
				 GD_PIX_warning,
				 parent);
    break;

  case GD_ERROR   :
  default         :

    pixmap = gdisp_getPixmapById(kernel,
				 GD_PIX_error,
				 parent);
    break;

  }

  pixmapWidget = gtk_pixmap_new(pixmap->pixmap,
				pixmap->mask);

  gtk_widget_show(pixmapWidget);

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
Pixmap_T*
gdisp_getProviderIdPixmap ( Kernel_T   *kernel,
			    GtkWidget  *parent,
			    guint       providerIdentity)
{

  Pixmap_T  *pixmap = (Pixmap_T*)NULL;
  Pixmap_ID  pixmapTable[GD_MAX_PROVIDER_NUMBER] = { GD_PIX_magentaBall,
						     GD_PIX_cyanBall,
						     GD_PIX_yellowBall,
						     GD_PIX_blueBall,
						     GD_PIX_greenBall,
						     GD_PIX_redBall };

  /*
   * Get back the correct pixmap according to provider identity.
   * Create the pixmap if it does not already exist.
   */
  pixmap = gdisp_getPixmapById(kernel,
			       pixmapTable[providerIdentity],
			       parent);

  return pixmap;

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


/*
 * Determine whether a point is inside a closed polygon.
 */
gboolean
gdisp_positionIsInsideZone ( PlotSystemZone_T *zone,
			     gdouble           x,
			     gdouble           y)
{

  guint    i        = 0;
  guint    j        = 0;
  gboolean isInside = FALSE;

  for (i=0; i<zone->pszPointNb; i++) {

    j++;

    if (j == zone->pszPointNb) {
      j=0;
    }

    if ((zone->pszY[i] < y && zone->pszY[j] >= y) ||
	(zone->pszY[j] < y && zone->pszY[i] >= y)    ) {

      if (zone->pszX[i] + (y - zone->pszY[i]) /
	  (zone->pszY[j] - zone->pszY[i]) *
	  (zone->pszX[j] - zone->pszX[i]) < x) {

        isInside = (isInside == FALSE ? TRUE : FALSE);

      }

    }

  }

  return isInside;

}


/*
 * Loop over all elements of the list.
 * Each element is a TSP symbol the 'sReference' parameter of
 * which must be decremented.
 */
void
gdisp_dereferenceSymbolList ( GList *symbolList )
{

  GList    *symbolItem =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop over all elements of the list.
   * Do not forget to decrement the 'sReference' of each symbol.
   */
  if (symbolList != (GList*)NULL) {

    symbolItem = g_list_first(symbolList);

    while (symbolItem != (GList*)NULL) {

      symbol = (Symbol_T*)symbolItem->data;

      symbol->sReference--;

      symbolItem = g_list_next(symbolItem);

    }

    g_list_free(symbolList);

  }

}

/*
 * String duplication.
 */
gchar*
gdisp_strDup ( gchar *string )
{

  if (string == (gchar*)NULL) {

    return (gchar*)NULL;

  }
  else {

    return g_memdup(string,strlen(string)+1);

  }

}

/*
 * CDDL HEADER START
 *
 * The contents of this file are subject to the terms of the
 * Common Development and Distribution License, Version 1.0 only
 * (the "License").  You may not use this file except in compliance
 * with the License.
 *
 * You can obtain a copy of the license at usr/src/OPENSOLARIS.LICENSE
 * or http://www.opensolaris.org/os/licensing.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 *
 * When distributing Covered Code, include this CDDL HEADER in each
 * file and include the License file at usr/src/OPENSOLARIS.LICENSE.
 * If applicable, add the following below this CDDL HEADER, with the
 * fields enclosed by brackets "[]" replaced with your own identifying
 * information: Portions Copyright [yyyy] [name of copyright owner]
 *
 * CDDL HEADER END
 */
gchar*
gdisp_uIntToStr ( guint value, gchar *ptr )
{

  guint t = 0;
 
  do {
    *--ptr = (gchar)('0' + value - 10 * (t = value / 10));
  } while ((value = t) != 0);
 
  return (ptr);

}


/*
 * Update the whole Gdisp+ GUI.
 */
void
gdisp_updateWholeGui ( void )
{

  /*
   * If X events are pending, treat them.
   */
  while (gtk_events_pending()) {
    gtk_main_iteration();
  }

}

/********************** INPUT WINDOW MANAGEMENT ***********************/

/*
 * Type definition.
 */
typedef struct InputWindowData_T_ {

  Kernel_T   *kernel;
  Symbol_T   *symbol;
  GtkWidget  *fieldWindow;
  GtkWidget  *fieldEntry;
  GtkWidget  *applyButton;
  gpointer    userData;
  void      (*userHandler)(Kernel_T*,Symbol_T*,gchar*,gpointer);

} InputWindowData_T;

/*
 * The "delete_event" occurs when the window manager sens this event
 * to the application, usually by the "close" option, or on the titlebar.
 * Returning TRUE means that we do not want to have the "destroy" event 
 * emitted, keeping GDISP+ running. Returning FALSE, we ask that "destroy"
 * be emitted, which in turn will call the "destroy" signal handler.
 */
static gint
gdisp_inputWindowManageDeleteEventFromWM (GtkWidget *fieldWindow,
					  GdkEvent  *event,
					  gpointer   data)
{

  /*
   * Emit "destroy" event.
   */
  return FALSE;

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdisp_inputWindowDestroySignalHandler (GtkWidget *fieldWindow,
				       gpointer   data)
{

  InputWindowData_T *iwData = (InputWindowData_T*)data;

  /*
   * Release memory.
   */
  g_free(iwData);

}

/*
 * Input Window : Close button callback.
 */
static void
gdisp_inputWindowCloseCallback (GtkWidget *closeButtonWidget,
				gpointer   data )
{

  InputWindowData_T *iwData = (InputWindowData_T*)data;

  /*
   * Tells GTK+ that it has to exit from the IWindow main processing loop.
   */
  gtk_widget_destroy(iwData->fieldWindow);

}


/*
 * Input Window : Apply button callback.
 */
static void
gdisp_inputWindowApplyCallback (GtkWidget *applyButtonWidget,
				gpointer   data )
{

  InputWindowData_T *iwData        = (InputWindowData_T*)data;
  gchar             *valueAsString = (gchar*)NULL;

  /*
   * Get back input value.
   */
  valueAsString = gtk_entry_get_text(GTK_ENTRY(iwData->fieldEntry));
  if (strlen(valueAsString) == 0) {
    return;
  }

  if (iwData->userHandler !=
      (void(*)(Kernel_T*,Symbol_T*,gchar*,gpointer))NULL) {

    (*iwData->userHandler)(iwData->kernel,
			   iwData->symbol,
			   valueAsString,
			   iwData->userData);

  }

}


/*
 * Input Window : Handler whenever the field content changes.
 */
static void
gdisp_fieldContentChangedCallback (GtkEditable *editableField,
				   gpointer     userData)
{

  InputWindowData_T *iwData        = (InputWindowData_T*)userData;
  gchar             *valueAsString = (gchar*)NULL;

  /*
   * Get back input value.
   */
  valueAsString = gtk_entry_get_text(GTK_ENTRY(iwData->fieldEntry));
  if (strlen(valueAsString) == 0) {

    gtk_widget_set_sensitive(iwData->applyButton,
			     FALSE); /* cannot apply */

    return;

  }

  /*
   * Check that content is a correct number.
   * Allowed are '0' to '9', '.', '+', '-', 'e', 'E'
   */
  while (*valueAsString != '\0') {

    if ((*valueAsString < '0' || *valueAsString > '9') &&
	*valueAsString != '.'                          &&
	*valueAsString != '+'                          &&
	*valueAsString != '-'                          &&
	*valueAsString != 'e'                          &&
	*valueAsString != 'E') {

      gtk_widget_set_sensitive(iwData->applyButton,
			       FALSE); /* cannot apply */

      return;

    }

    valueAsString++;

  }

  /*
   * Fallback : 'apply button' is sensitive.
   */
  gtk_widget_set_sensitive(iwData->applyButton,
			   TRUE);

}


/*
 * Input label standalone window.
 */
void
gdisp_showInputWindow (Kernel_T *kernel,
		       gchar    *title,
		       gchar    *subTitle,
		       Symbol_T *symbol,
		       gpointer  userData,
		       void    (*userHandler)(Kernel_T*,
					      Symbol_T*,
					      gchar*,
					      gpointer),
		       gboolean  onlyNumbers)
{

  GtkWidget         *fieldWindow = (GtkWidget*)NULL;
  GtkWidget         *fieldFrame  = (GtkWidget*)NULL;
  GtkWidget         *fieldEntry  = (GtkWidget*)NULL;
  GtkWidget         *fieldHbox   = (GtkWidget*)NULL;
  GtkWidget         *applyButton = (GtkWidget*)NULL;
  GtkWidget         *doneButton  = (GtkWidget*)NULL;
  InputWindowData_T *iwData      = (InputWindowData_T*)NULL;

  /*
   * Allocate memory for callbacks.
   */
  iwData = (InputWindowData_T*)g_malloc0(sizeof(InputWindowData_T));

  if (iwData == (InputWindowData_T*)NULL) {
    return; /* should never happen ... */
  }

  /*
   * Create a top-level window.
   * Its name is the symbol name.
   */
  fieldWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_window_set_title(GTK_WINDOW(fieldWindow),
		       title != (gchar*)NULL ? title : " Input Window ");

  gtk_window_set_position(GTK_WINDOW(fieldWindow),
			  GTK_WIN_POS_MOUSE);

  gtk_container_set_border_width(GTK_CONTAINER(fieldWindow),
				 5);

  gtk_signal_connect(GTK_OBJECT(fieldWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdisp_inputWindowManageDeleteEventFromWM),
		     (gpointer)NULL);

  gtk_signal_connect(GTK_OBJECT(fieldWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdisp_inputWindowDestroySignalHandler),
		     (gpointer)iwData);

  /*
   * Create a frame and add it to the top-level window.
   */
  fieldFrame = gtk_frame_new(subTitle != (gchar*)NULL ?
			     subTitle : " Enter new value ");

  gtk_frame_set_label_align(GTK_FRAME(fieldFrame),0.1,0.0);

  gtk_frame_set_shadow_type(GTK_FRAME(fieldFrame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(fieldWindow),fieldFrame);

  /*
   * Recursively show all widgets.
   */
  gtk_widget_show_all(fieldWindow);

  /*
   * Create the two 'apply' and 'done' buttons.
   */
  fieldHbox = gdisp_createButtonBar(kernel,
				    fieldWindow,
				    &applyButton,
				    &doneButton);

  gtk_container_add(GTK_CONTAINER(fieldFrame),
		    fieldHbox);

  gtk_signal_connect(GTK_OBJECT(applyButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_inputWindowApplyCallback),
		     (gpointer)iwData);

  if (onlyNumbers == TRUE) {
    gtk_widget_set_sensitive(applyButton,
			     FALSE);
  }

  gtk_signal_connect(GTK_OBJECT(doneButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_inputWindowCloseCallback),
		     (gpointer)iwData);

  /*
   * Create a field entry in order to get back symbol new value.
   * Add this field to the top-level window.
   */
  fieldEntry = gtk_entry_new();

  gtk_box_pack_start(GTK_BOX(fieldHbox),
		     fieldEntry,
		     FALSE /* expand  */,
		     FALSE /* fill    */,
		     3     /* padding */);

  if (onlyNumbers == TRUE) {

    gtk_signal_connect(GTK_OBJECT(fieldEntry),
		       "changed",
		       GTK_SIGNAL_FUNC(gdisp_fieldContentChangedCallback),
		       (gpointer)iwData);

  }

  /*
   * Show field entry.
   */
  gtk_widget_show(fieldEntry);

  /*
   * Store information within structure.
   */
  iwData->kernel      = kernel;
  iwData->symbol      = symbol;
  iwData->fieldWindow = fieldWindow;
  iwData->fieldEntry  = fieldEntry;
  iwData->applyButton = applyButton;
  iwData->userData    = userData;
  iwData->userHandler = userHandler;

}

/********************* INPUT WINDOW MANAGEMENT END **********************/

/*
 * Ascii to Integer with default value.
 */
gint
gdisp_atoi (gchar *string,
	    gint   defaultValue)
{

  gint integerValue = atoi(string);

  return (integerValue != 0 ? integerValue : defaultValue);

}


/*
 * Get back distant colors (not too close).
 */
GdkColor*
gdisp_getDistantColor (Kernel_T *kernel,
		       guint     colorId)
{

  guint nbDistantColors  = 0;
  guint distantColorId[] = { _RED_,
			     _GREEN_,
			     _ORANGE_,
			     _YELLOW_,
			     _BLUE_,
			     _MAGENTA_,
			     _CYAN_,
			     _BLACK_,
			     _GREY_,
			     _WHITE_ };

  nbDistantColors = sizeof(distantColorId) / sizeof(guint);

  return &kernel->colors[distantColorId[colorId % nbDistantColors]];

}
