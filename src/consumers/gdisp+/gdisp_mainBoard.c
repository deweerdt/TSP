/*!  \file 

$Id: gdisp_mainBoard.c,v 1.1 2004-02-04 20:32:09 esteban Exp $

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

File      : Graphic Tool Main Board.

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
gdispManageDeleteEventFromWM (GtkWidget *mainBoardWindow,
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
gdispDestroySignalHandler (GtkWidget *mainBoardWindow,
			   gpointer   data)
{

  /*
   * Tells GTK+ that it has to exit from the GTK+ main processing loop.
   */
  gtk_main_quit();

}


/*
 * 'Quit' item from 'File' menu of the main menu bar.
 * Quit handler : tell GTK+ we have to leave...
 */
static void
gdispQuitItemHandler (gpointer factoryData,
		      guint    itemData)
{

#if defined(NOT_TO_FORGET)
  Kernel_T *kernel = (Kernel_T*)factoryData;
#endif

  /*
   * Tells GTK+ that it has to exit from the GTK+ main processing loop.
   */
  gtk_main_quit();

}


/*
 * Add a line into the main board output list.
 * !!! WARNING !!!
 * 'string' parameter is released after being used.
 */
static void
gdispOutputWrite(Kernel_T  *kernel,
		 GString   *string,
		 Message_T  messageType)
{

#define _TIME_STRING_MAX_LEN_ 80

  gchar      timeString[_TIME_STRING_MAX_LEN_];
  GtkWidget *listItem = (GtkWidget*)NULL;
  GtkWidget *hBox     = (GtkWidget*)NULL;
  time_t     nowTime;

  assert(kernel);
  assert(kernel->widgets.mainBoardWindow);
  assert(kernel->widgets.mainBoardOutputList);


  /*
   * Format time message : hours:minutes.
   */
  nowTime = time((time_t*)NULL);
  strftime(timeString,
	   _TIME_STRING_MAX_LEN_,
	   "%H:%M : ",
	   localtime(&nowTime));
  g_string_prepend(string,timeString);


  /*
   * Create a 'list item' containing a 'label' and 'pixmap'.
   * Add this 'list item' into the main board output list.
   */
  hBox = gdisp_getMessagePixmaps (kernel,
				  kernel->widgets.mainBoardWindow,
				  messageType,
				  string->str);

  listItem = gtk_list_item_new();
  gtk_container_add(GTK_CONTAINER(listItem),hBox);  

  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardOutputList),
		    listItem);

  gtk_widget_show(listItem);

  gtk_list_scroll_vertical(GTK_LIST(kernel->widgets.mainBoardOutputList),
			   GTK_SCROLL_JUMP,
			   (gfloat)1 /* bottom of the list */);

  kernel->widgets.mainBoardOutputListSize++;

  g_string_free(string,TRUE);


  /*
   * Raise main-board window in order to see the incoming message.
   */
  gdk_window_raise(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);

}


static void
gdisp_clearOutputList (gpointer data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  assert(kernel);
  assert(kernel->widgets.mainBoardOutputList);

  /*
   * Use output list size to remove all list items.
   */
  if (kernel->widgets.mainBoardOutputListSize != 0) {

    gtk_list_clear_items(GTK_LIST(kernel->widgets.mainBoardOutputList),
			 0 /* start index */,
			 kernel->widgets.mainBoardOutputListSize /* stop */);

    kernel->widgets.mainBoardOutputListSize = 0;

  }

}


/*
 * Event handler for the main board output list.
 * Show the popup-menu given as an argument when right-clicking
 * onto the output list.
 */
static gint
gdisp_outputListEvent (GtkWidget *widget,
		       GdkEvent  *event )
{

  if (event->type == GDK_BUTTON_PRESS) {

    GdkEventButton *bEvent = (GdkEventButton*)event;

    if (bEvent->button == 3 /* right-click */) {

      gtk_menu_popup(GTK_MENU(widget),
		     NULL /* parent menu shell      */,
		     NULL /* parent menu item       */,
		     NULL /* user position function */,
		     NULL /* user private data      */,
		     bEvent->button,
		     bEvent->time);

      /*
       * Tell calling code that we have handled this event.
       * The process stops here.
       */
      return TRUE;

    }

  }

  /*
   * Tell calling code that we have not handled this event.
   * Pass it on.
   */
  return FALSE;

}


/*
 * This callback is called whenever play / stop buttons are pressed.
 * The argument "data" is the kernel itself.
 * The only way to determine which button has been pressed, is to
 * compare the argument "widget" (the target button) to both existent
 * button pointers stored into the kernel.
 */
static void
gdisp_togglePlayModeCallback (GtkWidget *buttonWidget,
			      gpointer   data )
{

  Kernel_T     *kernel     =     (Kernel_T*)data;

  if (buttonWidget == kernel->widgets.mainBoardOkButton) {

    gtk_widget_hide(kernel->widgets.mainBoardOkButton  );
    gtk_widget_show(kernel->widgets.mainBoardStopButton);

    gdisp_startSamplingProcess(kernel);

  }
  else if (buttonWidget == kernel->widgets.mainBoardStopButton) {

    gdisp_stopSamplingProcess(kernel);

    gtk_widget_hide(kernel->widgets.mainBoardStopButton);
    gtk_widget_show(kernel->widgets.mainBoardOkButton  );

  }

}


/*
 * This is the 'GtkItemFactoryEntry' structure used to generate new menus.
 *
 * Item 1 : The menu path.
 *          The letter after the underscore indicates an accelerator key 
 *          once the menu is open.
 * Item 2 : The accelerator key for the entry.
 * Item 3 : The callback function.
 * Item 4 : The callback parameter.
 *          This changes the parameters with which the function is called.
 *          The default is NULL.
 * Item 5 : The item type, used to define what kind of an item it is.
 *          Here are the possible values :
 *
 *          NULL            -> same as "<Item>".
 *          ""              -> same as "<Item>".
 *          "<Title>"       -> create a title item.
 *          "<Item>"        -> create a simple item.
 *          "<CheckItem>"   -> create a check item.
 *          "<ToggleItem>"  -> create a toggle item.
 *          "<RadioItem>"   -> create a radio item.
 *          <path>          -> path of a radio item to link against.
 *          "<Separator>"   -> create a separator.
 *          "<Branch>"      -> create an item to hold sub items (optional).
 *          "<LastBranch>"  -> create a right justified branch.
 */

static GtkItemFactoryEntry
gdispMainBoardMenuDefinitions[] = {

 { "/_File",                    NULL,
                                NULL,                   0, "<Branch>"     },
 { "/_File/_New",               "<control>N",
                                NULL,                   0, NULL           },
 { "/_File/_Open",              "<control>O",
                                NULL,                   0, NULL           },
 { "/_File/_Save",              "<control>S",
                                NULL,                   0, NULL           },
 { "/_File/Save _As",           NULL,
                                NULL,                   0, NULL           },
 { "/_File/Sep1",               NULL,
                                NULL,                   0, "<Separator>"  },
 { "/_File/Quit",               "<control>Q",
                                gdispQuitItemHandler,   0, NULL           },
 { "/_Data",                    NULL,
                                NULL,                   0, "<Branch>"     },
 { "/_Data/_Providers",         "<control>P",
                                gdisp_showProviderList, 0, NULL           },
 { "/_Data/S_ymbols",           "<control>Y",
                                gdisp_showSymbolList,   0, NULL           },
 { "/_Plots",                   NULL,
                                NULL,                   0, "<Branch>"     },
 { "/_Plots/New page",          NULL,
                                NULL,                   0, "<Branch>"     },
 { "/_Plots/New page/_Custom",  "<control>C",
                                gdisp_createGraphicPage,0, NULL           },
 { "/_Plots/New page/Sep2",     NULL,
                                NULL,                   0, "<Separator>"  },
 { "/_Plots/New page/_1 x 1",   "<control>1",
                                gdisp_createGraphicPage,1, NULL           },
 { "/_Plots/New page/_2 x 2",   "<control>2",
                                gdisp_createGraphicPage,2, NULL           },
 { "/_Plots/New page/_3 x 3",   "<control>3",
                                gdisp_createGraphicPage,3, NULL           },
 { "/_Plots/New page/_4 x 4",   "<control>4",
                                gdisp_createGraphicPage,4, NULL           },
 { "/_Plots/New page/_5 x 5",   "<control>5",
                                gdisp_createGraphicPage,5, NULL           },
 { "/_Help",                    NULL,
                                NULL,                   0, "<LastBranch>" },
 { "/_Help/About",              NULL,
                                NULL,                   0, NULL           },

};


/*
 * Include for TSP Logo.
 */
#include "pixmaps/gdisp_gdispLogo.xpm"
#include "pixmaps/gdisp_stopButton.xpm"
#include "pixmaps/gdisp_okButton.xpm"


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ main board.
 */
void
gdisp_createMainBoard (Kernel_T *kernel)
{

  GtkWidget      *mainVBox        =      (GtkWidget*)NULL;
  GtkWidget      *mainHBox        =      (GtkWidget*)NULL;
  GtkWidget      *menuBar         =      (GtkWidget*)NULL;
  GtkWidget      *scrolledWindow  =      (GtkWidget*)NULL;
  GtkWidget      *hSeparator      =      (GtkWidget*)NULL;
  GtkWidget      *vSeparator      =      (GtkWidget*)NULL;
  GtkWidget      *outputMenu      =      (GtkWidget*)NULL;
  GtkWidget      *outputClearItem =      (GtkWidget*)NULL;
  GtkItemFactory *itemFactory     = (GtkItemFactory*)NULL;
  GtkAccelGroup  *accelGroup      =  (GtkAccelGroup*)NULL;
  gint            menuItemNb      =                  0;
  GtkWidget      *pixmapWidget    =      (GtkWidget*)NULL;
  GdkPixmap      *pixmap          =      (GdkPixmap*)NULL;
  GdkBitmap      *mask            =      (GdkBitmap*)NULL;
  GtkStyle       *style           =       (GtkStyle*)NULL;

  assert(kernel);


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  kernel->widgets.mainBoardWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  assert(kernel->widgets.mainBoardWindow);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdispManageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdispDestroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window size, title and border width.
   */
  gtk_widget_set_usize(GTK_WIDGET(kernel->widgets.mainBoardWindow),
		       450 /* width  */,
		       150 /* height */);

  gtk_window_set_title(GTK_WINDOW(kernel->widgets.mainBoardWindow),
		       "GDISP+ Copyright (c) 2003.");

  gtk_container_set_border_width(
			   GTK_CONTAINER(kernel->widgets.mainBoardWindow),
			   3);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  mainVBox = gtk_vbox_new(FALSE, /* homogeneous */
			  3      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainVBox),2);
  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardWindow),mainVBox);
  gtk_widget_show(mainVBox);


  /* ------------------------ MENU BAR  ------------------------ */

  /*
   * Create all popup-menus with 'itemFactory' process.
   */
  menuItemNb = sizeof(gdispMainBoardMenuDefinitions   ) /
               sizeof(gdispMainBoardMenuDefinitions[0]);

  accelGroup = gtk_accel_group_new();

  /*
   * This function initializes the item factory.
   *  Param 1 : The type of menu.
   *   Can be GTK_TYPE_MENU_BAR, GTK_TYPE_MENU or GTK_TYPE_OPTION_MENU.
   *  Param 2 : The path of the menu.
   *  Param 3 : A pointer to a 'gtk_accel_group'.
   *   The item factory sets up the accelerator table while generating menus.
   */
  itemFactory = gtk_item_factory_new(GTK_TYPE_MENU_BAR,
				     "<main>", 
				     accelGroup);

  /*
   * This function generates the menu items.
   * Pass the item factory, the number of items in the array,
   * the array itself, and any callback data for the the menu items.
   */
  gtk_item_factory_create_items(itemFactory,
				menuItemNb,
				gdispMainBoardMenuDefinitions,
				(gpointer)kernel);

  /*
   * Attach the new accelerator group to the window.
   */
  gtk_window_add_accel_group(GTK_WINDOW(kernel->widgets.mainBoardWindow),
			     accelGroup);

  /*
   * Finally, get back item factory widget and pack it.
   */ 
  menuBar = gtk_item_factory_get_widget(itemFactory,"<main>");
  gtk_box_pack_start(GTK_BOX(mainVBox),
		     menuBar,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(menuBar);


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Map top-level window.
   */
  gtk_widget_show(kernel->widgets.mainBoardWindow);


  /* ------------------------ SEPARATOR ------------------------ */

  /*
   * Separate menu-bar and output window with a horizontal separator.
   */
  hSeparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(mainVBox),
		     hSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(hSeparator);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a horizontal packing box for managing the logo and
   * the output window.
   */
  mainHBox = gtk_hbox_new(FALSE, /* homogeneous */
			  3      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainHBox),0);
  gtk_box_pack_start(GTK_BOX(mainVBox),
		     mainHBox,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(mainHBox);


  /* ------------ MENU WHEN CLICKING ON THE LIST ------------ */

  /*
   * Init the menu-widget, and remember
   * -- never 'gtk_show_widget()' the menu widget !! --
   * This is the menu that holds the menu items, the one that will pop
   * up when you click on the output list.
   */
  outputMenu = gtk_menu_new ();

  /*
   * Create a menu-item with the name 'Clear'...
   * ... and add it to the output menu.
   * Connect a signal for clearing the output list when "activated".
   */
  outputClearItem = gtk_menu_item_new_with_label("Clear");
  gtk_menu_append(GTK_MENU(outputMenu),outputClearItem);

  gtk_signal_connect_object(GTK_OBJECT(outputClearItem),
			    "activate",
			    GTK_SIGNAL_FUNC(gdisp_clearOutputList),
			    (gpointer)kernel);

  gtk_widget_show(outputClearItem);


  /* ------------------------ OUTPUT WINDOW  ------------------------ */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     scrolledWindow,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(scrolledWindow);

  /*
   * Create the List widget.
   */
  kernel->widgets.mainBoardOutputList = gtk_list_new();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					kernel->widgets.mainBoardOutputList);
  gtk_widget_show(kernel->widgets.mainBoardOutputList);

  /*
   * Connect signals for clearing the list.
   */
  gtk_signal_connect_object(GTK_OBJECT(kernel->widgets.mainBoardOutputList),
			    "event",
			    GTK_SIGNAL_FUNC(gdisp_outputListEvent),
			    GTK_OBJECT(outputMenu));


  /* ------------------------ SEPARATOR ------------------------ */

  /*
   * Separate scrolled-window and play button with a vertical separator.
   */
  vSeparator = gtk_vseparator_new();
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     vSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(vSeparator);


  /* ----------------------- PLAY BUTTON ----------------------- */

  /*
   * OK button.
   */
  style  = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_okButton);

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  pixmapWidget = gtk_pixmap_new(pixmap,mask);
  gtk_widget_show(pixmapWidget);

  /*
   * Create the button that contains the pixmap.
   */
  kernel->widgets.mainBoardOkButton = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardOkButton),
		    pixmapWidget);
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     kernel->widgets.mainBoardOkButton,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.mainBoardOkButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardOkButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_togglePlayModeCallback),
		     (gpointer)kernel);

  /*
   * STOP button.
   */
  style  = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_stopButton);

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  pixmapWidget = gtk_pixmap_new(pixmap,mask);
  gtk_widget_show(pixmapWidget);

  /*
   * Create the button that contains the pixmap.
   */
  kernel->widgets.mainBoardStopButton = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(kernel->widgets.mainBoardStopButton),
		    pixmapWidget);
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     kernel->widgets.mainBoardStopButton,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  /* gtk_widget_show(kernel->widgets.mainBoardStopButton); */

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardStopButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_togglePlayModeCallback),
		     (gpointer)kernel);


  /* ------------------------ SEPARATOR ------------------------ */

  /*
   * Separate play button and logo with a vertical separator.
   */
  vSeparator = gtk_vseparator_new();
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     vSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(vSeparator);


  /* ------------------------ GDISP+ LOGO  ------------------------ */

  /*
   * Use GDK services to create GDISP+ Logo (XPM format).
   */
  style  = gtk_widget_get_style(kernel->widgets.mainBoardWindow);
  pixmap = gdk_pixmap_create_from_xpm_d(
                               kernel->widgets.mainBoardWindow->window,
			       &mask,
			       &style->bg[GTK_STATE_NORMAL],
			       (gchar**)gdisp_gdispLogo);

  /*
   * Create a pixmap widget to contain the pixmap.
   */
  pixmapWidget = gtk_pixmap_new(pixmap,mask);
  gtk_box_pack_start(GTK_BOX(mainHBox),
		     pixmapWidget,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(pixmapWidget);


  /* ------------------------ END MAIN BOARD  ------------------------ */

  /*
   * Tell the kernel that the output list has been created.
   * It is now ready to be used.
   */
  kernel->outputFunc = gdispOutputWrite;

}


void
gdisp_writeInitialInformation (Kernel_T *kernel)
{

  GString *messageString = (GString*)NULL;


  /*
   * Keep in mind that the GString is released into the
   * 'gdispOutputWrite' function.
   * Do not 'g_string_free' it.
   */

  /*
   * Add into the list GDISP+ Version identification.
   */
  messageString = g_string_new("GDISP+ Graphic Tool, Version 1.0");
  kernel->outputFunc(kernel,messageString,GD_MESSAGE);


  /*
   * Report whether we are in a thread-safe environment.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "Implementation is %sthread-safe.",
		   kernel->isThreadSafe == TRUE ? "" : "NOT ");
  kernel->outputFunc(kernel,messageString,GD_WARNING);

}

