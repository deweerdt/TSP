/*!  \file 

$Id: gdisp_providers.c,v 1.1 2004-02-04 20:32:10 esteban Exp $

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

File      : Information / Actions upon available providers.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * The "delete_event" occurs when the window manager sens this event
 * to the application, usually by the "close" option, or on the titlebar.
 * Returning TRUE means that we do not want to have the "destroy" event 
 * emitted, keeping GDISP+ running. Returning FALSE, we ask that "destroy"
 * be emitted, which in turn will call the "destroy" signal handler.
 */
static gint
gdispManageDeleteEventFromWM (GtkWidget *providerWindow,
			      GdkEvent  *event,
			      gpointer   data)
{

  return FALSE;

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdispDestroySignalHandler (GtkWidget *providerWindow,
			   gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Close and destroy provider list window.
   */
  gtk_widget_destroy(providerWindow);
  kernel->widgets.providerWindow = (GtkWidget*)NULL;

}


/*
 * This callback is called whenever a provider button" is pressed.
 * The argument "data" is the kernel itself.
 * The only way to determine which button has been pressed (ie the
 * provider the information of which is requested), is to compare
 * the argument "widget" (the target button) to all button pointers
 * stored into the list of providers.
 */
static void
gdisp_providerInformationCallback (GtkWidget *buttonWidget,
				   gpointer   data )
{

  Kernel_T   *kernel            =   (Kernel_T*)data;
  GList      *providerItem      =      (GList*)NULL;
  Provider_T *provider          = (Provider_T*)NULL;
  GString    *informationString =    (GString*)NULL;

#define _MAX_INFORMATION_   6
  gint        informationCpt    = 0;
  gchar      *information[_MAX_INFORMATION_][2] =
                                   { { "Name"                 , "" },
				     { "Base frequency"       , "" },
				     { "Maximum period"       , "" },
				     { "Maximum client number", "" },
				     { "Current client number", "" },
				     { "Available symbols"    , "" } };

  /*
   * If no button is specified (buttonWidget == NULL), just
   * insert the name of the information.
   */
  if (buttonWidget == (GtkWidget*)NULL) {

    for (informationCpt=0;
	 informationCpt<_MAX_INFORMATION_; informationCpt++) {

      gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
		       information[informationCpt]);

    }

  }
  else {

    /*
     * Loop over all available providers into the list.
     */
    providerItem = g_list_first(kernel->providerList);
    while (providerItem != (GList*)NULL) {

      provider = (Provider_T*)providerItem->data;
      if (provider->pButton == buttonWidget) {

	/*
	 * We have found here the target provider.
	 * Put its information into the CList.
	 *
	 * There are two convenience functions that should be used
	 * when a lot of changes have to be made to the list.
	 * This is to prevent the list flickering while being repeatedly
	 * updated, which may be highly annoying to the user. So instead
	 * it is a good idea to freeze the list, do the updates to it,
	 * and finally thaw it which causes the list to be updated on the
	 * screen.
	 */
	gtk_clist_freeze(GTK_CLIST(kernel->widgets.providerCList));
	gtk_clist_clear (GTK_CLIST(kernel->widgets.providerCList));

	informationString = g_string_new((gchar*)NULL);

	information[0][1] = provider->pName->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[0]);

	g_string_sprintf(informationString,"%f",provider->pBaseFrequency);
	information[1][1] = informationString->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[1]);

	g_string_sprintf(informationString,"%d",provider->pMaxPeriod);
	information[2][1] = informationString->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[2]);

	g_string_sprintf(informationString,"%d",provider->pMaxClientNumber);
	information[3][1] = informationString->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[3]);

	g_string_sprintf(informationString,"%d",
			                    provider->pCurrentClientNumber);
	information[4][1] = informationString->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[4]);

	g_string_sprintf(informationString,"%d",
                                           provider->pSymbolNumber);
	information[5][1] = informationString->str;
	gtk_clist_append(GTK_CLIST(kernel->widgets.providerCList),
			 information[5]);

	g_string_free(informationString,TRUE);

	gtk_clist_thaw  (GTK_CLIST(kernel->widgets.providerCList));


      }

      providerItem = g_list_next(providerItem);

    } /* while (providerItem != (GList*)NULL) */

  } /* else */


  /*
   * This CList is not selectable.
   */
  for (informationCpt=0;
       informationCpt<_MAX_INFORMATION_; informationCpt++) {

    gtk_clist_set_selectable(GTK_CLIST(kernel->widgets.providerCList),
			     informationCpt, /* row */
			     FALSE /* not selectable */);

  }

}


/*
 * Include Provider Logos.
 */
#include "pixmaps/gdisp_stubLogo.xpm"
#include "pixmaps/gdisp_resLogo.xpm"


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ provider list.
 */
void
gdisp_showProviderList (gpointer factoryData, guint itemData)
{

  Kernel_T         *kernel           =   (Kernel_T*)factoryData;

  GtkWidget        *mainVBox         =  (GtkWidget*)NULL;
  GtkWidget        *secondVBox       =  (GtkWidget*)NULL;
  GtkWidget        *mainHBox         =  (GtkWidget*)NULL;
  GtkWidget        *frame            =  (GtkWidget*)NULL;
  GtkWidget        *scrolledWindow   =  (GtkWidget*)NULL;
  GtkWidget        *logoHBox         =  (GtkWidget*)NULL;
  GtkWidget        *closeButton      =  (GtkWidget*)NULL;

#define _CLIST_COLUMNS_NB_ 2
  GtkWidget        *cList            =  (GtkWidget*)NULL;
  gchar            *cListTitles[_CLIST_COLUMNS_NB_] = { "Provider", "" };

  GtkWidget        *vBoxWidget       =  (GtkWidget*)NULL;
  GtkWidget        *pixmapWidget     =  (GtkWidget*)NULL;
  GdkPixmap        *pixmap           =  (GdkPixmap*)NULL;
  GdkBitmap        *mask             =  (GdkBitmap*)NULL;
  GtkStyle         *style            =   (GtkStyle*)NULL;

  GList            *providerItem     =      (GList*)NULL;
  Provider_T       *provider         = (Provider_T*)NULL;
  gint              providerCount    = 0;


  /*
   * Few checking...
   */
  assert(kernel);
  assert(kernel->colors);


  /*
   * If provider list is already on the screen, just raise its window.
   */
  if (kernel->widgets.providerWindow != (GtkWidget*)NULL) {

    gdk_window_raise(GTK_WIDGET(kernel->widgets.providerWindow)->window);

    return;

  }


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  kernel->widgets.providerWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  assert(kernel->widgets.providerWindow);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.providerWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdispManageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.providerWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdispDestroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window size, title and border width.
   */
#if defined(WINDOW_SIZE)
  gtk_widget_set_usize(GTK_WIDGET(kernel->widgets.providerWindow),
		       400 /* width  */,
		       200 /* height */);
#endif

  gtk_window_set_title(GTK_WINDOW(kernel->widgets.providerWindow),
		       "GDISP+ Providers.");

  gtk_container_set_border_width(GTK_CONTAINER(kernel->widgets.providerWindow),
				 1);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  mainVBox = gtk_vbox_new(FALSE, /* homogeneous */
			  5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainVBox),3);
  gtk_container_add(GTK_CONTAINER(kernel->widgets.providerWindow),mainVBox);
  gtk_widget_show(mainVBox);


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Map top-level window.
   */
  gtk_widget_show(kernel->widgets.providerWindow);
  gdk_window_set_colormap(GTK_WIDGET(kernel->widgets.providerWindow)->window,
			  kernel->colormap);


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for provider logos.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new(" Available Providers ");
  gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(mainVBox),frame);
  gtk_widget_show(frame);


  /* ------------ VERTICAL PACKING BOX INTO THE FRAME ------------ */

  /*
   * We need a vertical packing box for managing the two scrolled windows.
   */
  secondVBox = gtk_vbox_new(FALSE, /* homogeneous */
			    5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(secondVBox),3);
  gtk_container_add(GTK_CONTAINER(frame),secondVBox);
  gtk_widget_show(secondVBox);


  /* ----------- SCROLLED WINDOW FOR THE LIST OF PROVIDERS  ----------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_widget_set_usize(scrolledWindow,350,100);
  gtk_container_add(GTK_CONTAINER(secondVBox),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /*
   * We need a horizontal packing box for managing all logos.
   */
  logoHBox = gtk_hbox_new(TRUE,  /* homogeneous */
			  5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(logoHBox),2);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					logoHBox);
  gtk_widget_show(logoHBox);


  /*
   * Use GDK services to create TSP Logo (XPM format).
   */
  style  = gtk_widget_get_style(kernel->widgets.providerWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(kernel->widgets.providerWindow->window,
					&mask,
					&style->bg[GTK_STATE_NORMAL],
					(gchar**)gdisp_resLogo);


  /* -------------- CLIST FOR PROVIDER INFORMATION --------------- */

  /*
   * This is the scrolled window to put the CList widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_widget_set_usize(scrolledWindow,350,150);
  gtk_box_pack_start(GTK_BOX(secondVBox),
		     scrolledWindow,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(scrolledWindow);

  /*
   * Create the CList. For our needs, we use 2 columns.
   */
  cList = gtk_clist_new_with_titles(_CLIST_COLUMNS_NB_,cListTitles);
  gtk_clist_column_titles_show(GTK_CLIST(cList));
  kernel->widgets.providerCList = cList;

  /*
   * It isn't necessary to shadow the border, but it looks nice.
   */
  gtk_clist_set_shadow_type(GTK_CLIST(cList),GTK_SHADOW_OUT);

  /*
   * What however is important, is that we set the column widths as
   * they will never be right otherwise.
   * Note that the columns are numbered from 0 and up (to 1 in our case).
   */
  gtk_clist_set_column_width(GTK_CLIST(cList),0,150);

  /*
   * Add the CList widget to the vertical box and show it.
   */
  gtk_container_add(GTK_CONTAINER(scrolledWindow),cList);
  gtk_widget_show(cList);

  /*
   * Insert information names into the CList.
   */
  gdisp_providerInformationCallback((GtkWidget*)NULL,(gpointer)kernel);


  /* ------------------------ PACKING BOX ------------------------ */

  mainHBox = gdisp_createButtonBar(kernel->widgets.providerWindow,
				   (GtkWidget**)NULL /* no 'apply' button */,
				   &closeButton);

  gtk_box_pack_start(GTK_BOX(mainVBox),
		     mainHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);

  gtk_signal_connect_object(GTK_OBJECT(closeButton),
			    "clicked",
			    (GtkSignalFunc)gtk_widget_destroy,
			    GTK_OBJECT(kernel->widgets.providerWindow));

  /*
   * This grabs this button to be the default button.
   * Simply hitting the "Enter" key will cause this button to activate.
   */
  GTK_WIDGET_SET_FLAGS(closeButton,GTK_CAN_DEFAULT);
  gtk_widget_grab_default(closeButton);


  /* ------------------------ LIST OF PROVIDERS  ------------------------ */

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    vBoxWidget = gtk_vbox_new(FALSE, /* homogeneous */
			      2      /* spacing     */ );
    gtk_container_border_width(GTK_CONTAINER(vBoxWidget),2);
    gtk_box_pack_start(GTK_BOX(logoHBox),
		       vBoxWidget,
		       FALSE /* expand  */,
		       TRUE  /* fill    */,
		       0     /* padding */);
    gtk_widget_show(vBoxWidget);

    pixmapWidget = gtk_pixmap_new(pixmap,mask);
    gtk_box_pack_start(GTK_BOX(vBoxWidget),
		       pixmapWidget,
		       FALSE /* expand  */,
		       TRUE  /* fill    */,
		       0     /* padding */);
    gtk_widget_show(pixmapWidget);

    provider = (Provider_T*)providerItem->data;
    if (provider->pStatus == GD_FROM_SCRATCH) {

      provider->pButton = gtk_button_new_with_label("Unknown");

    }
    else {

      provider->pButton = gtk_button_new_with_label(provider->pName->str);

    }


    /*
     * Set up the color affected to that provider.
     */
    provider->pColor = gdisp_getProviderColor(kernel,
					      providerCount++);


    /*
     * Connect the "clicked" signal of the button to our callback.
     */
    gtk_signal_connect(GTK_OBJECT(provider->pButton),
		       "clicked",
		       GTK_SIGNAL_FUNC(gdisp_providerInformationCallback),
		       (gpointer)kernel);

    gtk_box_pack_start(GTK_BOX(vBoxWidget),
		       provider->pButton,
		       FALSE /* expand  */,
		       TRUE  /* fill    */,
		       0     /* padding */);
    gtk_widget_show(provider->pButton);

    providerItem = g_list_next(providerItem);

  }

}

