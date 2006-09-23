/*

$Id: gdisp_hosts.c,v 1.10 2006-09-23 20:35:01 esteban Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : HOSTS Management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <assert.h>
#include <string.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"
#include "gdisp_popupMenu.h"


/*
 --------------------------------------------------------------------
                             STATIC DEFINITIONS
 --------------------------------------------------------------------
*/

#define GD_ACTIVATE_ALL 1
#define GD_FREEZE_ALL   2
#define GD_INVERT_ALL   3

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Remember host window window position.
 */
static void
gdisp_getHostWindowPosition (Kernel_T *kernel)
{

  /*
   * Remember window position.
   */
  gdk_window_get_position(GTK_WIDGET(kernel->widgets.hostWindow)->window,
			  &kernel->widgets.hostWindowXPosition,
			  &kernel->widgets.hostWindowYPosition);

  if (kernel->widgets.hostWindowXPosition < 0) {
    kernel->widgets.hostWindowXPosition = 0;
  }

  if (kernel->widgets.hostWindowYPosition < 0) {
    kernel->widgets.hostWindowYPosition = 0;
  }

}


/*
 * The "delete_event" occurs when the window manager sens this event
 * to the application, usually by the "close" option, or on the titlebar.
 * Returning TRUE means that we do not want to have the "destroy" event 
 * emitted, keeping GDISP+ running. Returning FALSE, we ask that "destroy"
 * be emitted, which in turn will call the "destroy" signal handler.
 */
static gint
gdisp_manageDeleteEventFromWM (GtkWidget *symbolWindow,
			       GdkEvent  *event,
			       gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Remember window position.
   */
  gdisp_getHostWindowPosition(kernel);

  return FALSE;

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdisp_destroySignalHandler (GtkWidget *hostWindow,
			    gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Reset all widgets.
   */
  kernel->widgets.hostWindow = (GtkWidget*)NULL;

}


/*
 * Find a host by its name.
 */
static Host_T*
gdisp_findHostByName ( Kernel_T *kernel,
		       gchar    *hostName )
{

  Host_T *host     = (Host_T*)NULL;
  GList  *hostItem =  (GList*)NULL;

  /*
   * Loop over the kernel host list.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    if (strcmp(host->hName->str,hostName) == 0) {
      return host;
    }

    hostItem = g_list_next(hostItem);

  }

  return (Host_T*)NULL;

}


/*
 * host button callback.
 */
static void
gdisp_changeHostStateCallback (GtkWidget *hostButton,
			       gpointer   data )
{

  Kernel_T *kernel   = (Kernel_T*)data;
  Host_T   *host     = (Host_T*)NULL;
  GList    *hostItem =  (GList*)NULL;

  /*
   * Find the correct host by comparing the button identity.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    if (host->hPressButton == hostButton) {
      break; /* out of while */
    }

    hostItem = g_list_next(hostItem);

  }

  if (hostItem != (GList*)NULL) {

    host->hIsActive = GD_TOGGLE_BOOLEAN(host->hIsActive);

    gtk_widget_set_style(host->hPressButton,
			 host->hIsActive == TRUE ?
			 kernel->widgets.hostActiveStyle :
			 kernel->widgets.hostInactiveStyle);

  }

}


/*
 * Close button callback.
 */
static void
gdisp_closeCallback (GtkWidget *closeButtonWidget,
		     gpointer   data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Remember window position.
   */
  gdisp_getHostWindowPosition(kernel);

  /*
   * Destroy host active style.
   */
  gtk_style_unref(kernel->widgets.hostActiveStyle);
  kernel->widgets.hostActiveStyle = (GtkStyle*)NULL;
  gtk_style_unref(kernel->widgets.hostInactiveStyle);
  kernel->widgets.hostInactiveStyle = (GtkStyle*)NULL;

  /*
   * Destroy popup menu.
   */
  gdisp_destroyMenu((PopupMenu_T*)kernel->widgets.hostPopupMenu);

  /*
   * Tells GTK+ that it has to exit from the GTK+ main processing loop.
   */
  gtk_widget_destroy(kernel->widgets.hostWindow);

}


/*
 * Build the host and url list according to user specifications.
 */
static void
gdisp_addHostOrUrl ( Kernel_T   *kernel,
		     gchar      *hostName,
		     HostType_T  hostType)
{

  Host_T *host = (Host_T*)NULL;

  /*
   * Check.
   */
  if (hostName == (gchar*)NULL) {
    return;
  }

  /*
   * Add a new host into the host list, if this host does not already exist.
   */
  if (gdisp_findHostByName(kernel,hostName) == (Host_T*)NULL) {

    /*
     * Allocate a host structure.
     */
    host = g_malloc0(sizeof(Host_T));
    assert(host);

    host->hName     = g_string_new(hostName);
    host->hIsActive = TRUE;
    host->hType     = hostType;

    /*
     * Insert this new host into the host list.
     */
    kernel->hostList = g_list_append(kernel->hostList,
				     (gpointer)host);

  }

}


/*
 * Popup Menu Handler.
 * Change the activation of the hosts and URLs.
 */
static void
gdisp_hostPopupMenuHandler ( Kernel_T    *kernel,
			     PopupMenu_T *menu,
			     gpointer     menuData,
			     gpointer     itemData )
{

  guint     action       = GPOINTER_TO_UINT(itemData);
  Host_T   *host         = (Host_T*)NULL;
  GList    *hostItem     = (GList*)NULL;
  gboolean  hostIsActive = TRUE;

  /*
   * Treat action.
   */
  switch (action) {

  case GD_ACTIVATE_ALL :
    hostIsActive = TRUE;
    break;

  case GD_FREEZE_ALL :
    hostIsActive = FALSE;
    break;

  case GD_INVERT_ALL :
  default :
    /* never here */
    break;

  }

  /*
   * Loop over all hosts and apply the requested action.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    if (action == GD_INVERT_ALL) {
      host->hIsActive = GD_TOGGLE_BOOLEAN(host->hIsActive);
    }
    else {
      host->hIsActive = hostIsActive;
    }

    gtk_widget_set_style(host->hPressButton,
			 host->hIsActive == TRUE ?
			 kernel->widgets.hostActiveStyle :
			 kernel->widgets.hostInactiveStyle);

    hostItem = g_list_next(hostItem);

  }

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Build the host list according to user specifications.
 */
void
gdisp_addHost ( Kernel_T *kernel,
		gchar    *hostName )
{

  gdisp_addHostOrUrl(kernel,
		     hostName,
		     GD_HOST);

}


/*
 * Build the URL list according to user specifications.
 */
void
gdisp_addUrl ( Kernel_T *kernel,
	       gchar    *urlName )
{

  gdisp_addHostOrUrl(kernel,
		     urlName,
		     GD_URL);

}


/*
 * Destroy host list.
 */
void
gdisp_destroyHosts ( Kernel_T *kernel )
{

  GList  *hostItem =  (GList*)NULL;
  Host_T *host     = (Host_T*)NULL;


  /*
   * Release all hosts.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    g_string_free(host->hName,TRUE);

    g_free(host);

    hostItem = g_list_next(hostItem);

  }

  g_list_free(kernel->hostList);
  kernel->hostList = (GList*)NULL;

}


/*
 * Activate / Freeze host activity.
 * This enables TARGA to open sessions with authorized hosts.
 */
void
gdisp_manageHosts ( gpointer factoryData,
		    guint    itemData )
{

  Kernel_T  *kernel         = (Kernel_T*)factoryData;

  GtkWidget *frame          = (GtkWidget*)NULL;
  GtkWidget *scrolledWindow = (GtkWidget*)NULL;
  GtkWidget *firstVBox      = (GtkWidget*)NULL;
  GtkWidget *mainVBox       = (GtkWidget*)NULL;
  GtkWidget *secondVBox     = (GtkWidget*)NULL;
  GtkWidget *mainHBox       = (GtkWidget*)NULL;
  GtkWidget *pixmapWidget   = (GtkWidget*)NULL;
  Pixmap_T  *pixmap         = (Pixmap_T*)NULL;
  GtkWidget *closeButton    = (GtkWidget*)NULL;
  GtkStyle  *greenStyle     = (GtkStyle*)NULL;
  GtkStyle  *grayStyle      = (GtkStyle*)NULL;

  Host_T    *host           = (Host_T*)NULL;
  GList     *hostItem       = (GList*)NULL;


  /*
   * If host window is already on the screen, just raise its window.
   */
  if (kernel->widgets.hostWindow != (GtkWidget*)NULL) {

    gdk_window_raise(GTK_WIDGET(kernel->widgets.hostWindow)->window);

    return;

  }


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  kernel->widgets.hostWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  assert(kernel->widgets.hostWindow);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.hostWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdisp_manageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.hostWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdisp_destroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window title and border width.
   */
  gtk_window_set_title(GTK_WINDOW(kernel->widgets.hostWindow),
		       "Host Management");

  gtk_container_set_border_width(GTK_CONTAINER(kernel->widgets.hostWindow),
				 1);


  /* ----------- VERTICAL BOX FOR HANDLING ALL WIDGETS  ----------- */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  firstVBox = gtk_vbox_new(FALSE, /* homogeneous */
			   0      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(firstVBox),2);

  gtk_container_add(GTK_CONTAINER(kernel->widgets.hostWindow),
		    firstVBox);

  gtk_widget_show(firstVBox);


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for hosts.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new(" Available Hosts & URLs ");
  gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_box_pack_start(GTK_BOX(firstVBox),
		     frame,
		     FALSE, /* expand  */
		     FALSE, /* fill    */
		     0);    /* padding */

  gtk_widget_show(frame);


  /* ----------- SCROLLED WINDOW FOR THE LIST OF HOSTS  ----------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_widget_set_usize(scrolledWindow,395,300);
  gtk_container_add(GTK_CONTAINER(frame),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /* ----------- VERTICAL BOX FOR HANDLING ALL HOSTS  ----------- */

  /*
   * We need a vertical packing box for managing all hosts.
   */
  mainVBox = gtk_vbox_new(FALSE, /* homogeneous */
			  0      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainVBox),10);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					mainVBox);
  gtk_widget_show(mainVBox);


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Map top-level window.
   */
  gtk_widget_show(kernel->widgets.hostWindow);
  gdk_window_move(GTK_WIDGET(kernel->widgets.hostWindow)->window,
		  kernel->widgets.hostWindowXPosition,
		  kernel->widgets.hostWindowYPosition);


  /* --------------------- INSERT ALL HOSTS --------------------- */

  /*
   * GTK Style from default style.
   */
  greenStyle = gtk_style_copy(gtk_widget_get_default_style());
  greenStyle = gtk_style_ref(greenStyle);

  greenStyle->bg  [GTK_STATE_NORMAL  ] = kernel->colors[_PALE_GREEN_];
  greenStyle->bg  [GTK_STATE_ACTIVE  ] = kernel->colors[_PALE_GREEN_];
  greenStyle->bg  [GTK_STATE_SELECTED] = kernel->colors[_PALE_GREEN_];
  greenStyle->bg  [GTK_STATE_PRELIGHT] = kernel->colors[_PALE_GREEN_];

  kernel->widgets.hostActiveStyle = greenStyle;

  grayStyle = gtk_style_copy(gtk_widget_get_default_style());
  grayStyle = gtk_style_ref(grayStyle);

  grayStyle->bg  [GTK_STATE_NORMAL  ] = kernel->colors[_DARK_GREY_];
  grayStyle->bg  [GTK_STATE_ACTIVE  ] = kernel->colors[_DARK_GREY_];
  grayStyle->bg  [GTK_STATE_SELECTED] = kernel->colors[_DARK_GREY_];
  grayStyle->bg  [GTK_STATE_PRELIGHT] = kernel->colors[_DARK_GREY_];

  kernel->widgets.hostInactiveStyle = grayStyle;

  /* --------------------- INSERT ALL HOSTS --------------------- */

  /*
   * Loop over the kernel host list.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;


    /* ------------------------ HORIZONTAL BOX  ------------------------ */

    /*
     * Create a horizontal packing box.
     */
    mainHBox = gtk_hbox_new(FALSE, /* homogeneous */
			    5      /* spacing     */ );
    gtk_container_border_width(GTK_CONTAINER(mainHBox),0);

    gtk_box_pack_start(GTK_BOX(mainVBox),
		       mainHBox,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */

    gtk_widget_show(mainHBox);


    /* ---------------------- HOST LOGO ---------------------- */

    /*
     * Use GDK services to create provider Logo (XPM format).
     */
    pixmap = gdisp_getPixmapById(kernel,
				 host->hType == GD_HOST ?
				 GD_PIX_stubProvider : GD_PIX_urlLogo,
				 kernel->widgets.hostWindow);

    pixmapWidget = gtk_pixmap_new(pixmap->pixmap,
				  pixmap->mask);

    gtk_box_pack_start(GTK_BOX(mainHBox),
		       pixmapWidget,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */

    gtk_widget_show(pixmapWidget);


    /* ----------------- ALTERNATE VERTICAL BOX ---------------- */

    secondVBox = gtk_vbox_new(TRUE, /* homogeneous */
			      0      /* spacing     */ );
    gtk_container_border_width(GTK_CONTAINER(secondVBox),0);

    gtk_box_pack_start(GTK_BOX(mainHBox),
		       secondVBox,
		       TRUE, /* expand  */
		       TRUE, /* fill    */
		       0);    /* padding */

    gtk_widget_show(secondVBox);


    /* -------------------- ACTIVATION BUTTON ------------------ */

    host->hPressButton = gtk_button_new_with_label(host->hName->str);

    gtk_box_pack_start(GTK_BOX(secondVBox),
		       host->hPressButton,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */

    gtk_widget_set_style(host->hPressButton,
			 host->hIsActive == TRUE ? greenStyle : grayStyle);

    gtk_widget_show(host->hPressButton);

    gtk_signal_connect(GTK_OBJECT(host->hPressButton),
		       "clicked",
		       GTK_SIGNAL_FUNC(gdisp_changeHostStateCallback),
		       (gpointer)kernel);

    /* ----------------------- NEXT HOST ----------------------- */

    hostItem = g_list_next(hostItem);

  }


  /* ------------------------ PACKING BOX ------------------------ */

  mainHBox = gdisp_createButtonBar(kernel,
				   kernel->widgets.hostWindow,
				   (GtkWidget**)NULL,
				   &closeButton);

  gtk_container_border_width(GTK_CONTAINER(mainHBox),5);

  gtk_box_pack_start(GTK_BOX(firstVBox),
		     mainHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);

  gtk_signal_connect(GTK_OBJECT(closeButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_closeCallback),
		     (gpointer)kernel);

  /*
   * This grabs this button to be the default button.
   * Simply hitting the "Enter" key will cause this button to activate.
   */
  GTK_WIDGET_SET_FLAGS(closeButton,GTK_CAN_DEFAULT);
  gtk_widget_grab_default(closeButton);


  /* ------------------------ POPUP MENU ------------------------ */

  /*
   * Create the dynamic menu.
   * Menu cannot be created before because the parent list is not shown yet.
   */
  kernel->widgets.hostPopupMenu =
    (void*)gdisp_createMenu(kernel,
			    scrolledWindow,
			    gdisp_hostPopupMenuHandler,
			    (gpointer)kernel);

  gdisp_addMenuItem((PopupMenu_T*)kernel->widgets.hostPopupMenu,
		    GD_POPUP_TITLE,
		    "Hosts & URLs",
		    (gpointer)NULL);

  gdisp_addMenuItem((PopupMenu_T*)kernel->widgets.hostPopupMenu,
		    GD_POPUP_SEPARATOR,
		    (gchar*)NULL,
		    (gpointer)NULL);

  gdisp_addMenuItem((PopupMenu_T*)kernel->widgets.hostPopupMenu,
		    GD_POPUP_ITEM,
		    "Activate All",
		    (gpointer)GUINT_TO_POINTER(GD_ACTIVATE_ALL));

  gdisp_addMenuItem((PopupMenu_T*)kernel->widgets.hostPopupMenu,
		    GD_POPUP_ITEM,
		    "Freeze All",
		    (gpointer)GUINT_TO_POINTER(GD_FREEZE_ALL));

  gdisp_addMenuItem((PopupMenu_T*)kernel->widgets.hostPopupMenu,
		    GD_POPUP_ITEM,
		    "Invert All",
		    (gpointer)GUINT_TO_POINTER(GD_INVERT_ALL));

}

