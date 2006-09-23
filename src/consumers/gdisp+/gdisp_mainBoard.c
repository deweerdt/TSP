/*

$Id: gdisp_mainBoard.c,v 1.16 2006-09-23 20:35:02 esteban Exp $

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
 * Remember main board window position.
 */
static void
gdisp_getMainBoardWindowPosition (Kernel_T *kernel)
{

  /*
   * Remember window position.
   */
  gdk_window_get_position(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window,
			  &kernel->widgets.mainBoardWindowXPosition,
			  &kernel->widgets.mainBoardWindowYPosition);

  if (kernel->widgets.mainBoardWindowXPosition < 0) {
    kernel->widgets.mainBoardWindowXPosition = 0;
  }

  if (kernel->widgets.mainBoardWindowYPosition < 0) {
    kernel->widgets.mainBoardWindowYPosition = 0;
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
gdisp_manageDeleteEventFromWM (GtkWidget *mainBoardWindow,
			       GdkEvent  *event,
			       gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Remember window position.
   */
  gdisp_getMainBoardWindowPosition(kernel);

  /*
   * Allow the window manager to close graphic page windows
   * only if sampling is off.
   */
  return (kernel->samplingThreadMustExit == FALSE ? TRUE : FALSE);

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdisp_destroySignalHandler (GtkWidget *mainBoardWindow,
			    gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Close all other top level window.
   */
  gdisp_closeDataBookWindow(kernel);
  gdisp_closeConfiguration (kernel);

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
gdisp_quitItemHandler (gpointer factoryData,
		       guint    itemData)
{

  Kernel_T *kernel = (Kernel_T*)factoryData;

  /*
   * Update whole GUI.
   */
  gdisp_updateWholeGui();

  /*
   * Remember window position.
   */
  gdisp_getMainBoardWindowPosition(kernel);

  /*
   * Tells GTK+ that it has to exit from the GTK+ main processing loop.
   */
  gtk_widget_destroy(kernel->widgets.mainBoardWindow);

}


/*
 * Add a line into the main board output list.
 * !!! WARNING !!!
 * 'string' parameter is released after being used.
 */
static void
gdisp_outputWrite(Kernel_T  *kernel,
		  GString   *string,
		  Message_T  messageType)
{

#define _TIME_STRING_MAX_LEN_ 80

  gchar          timeString[_TIME_STRING_MAX_LEN_];
  GtkWidget     *listItem       = (GtkWidget*)NULL;
  GtkWidget     *hBox           = (GtkWidget*)NULL;
  GtkWidget     *scrolledWindow = (GtkWidget*)NULL;
  GtkAdjustment *vAdj           = (GtkAdjustment*)NULL;
  time_t         nowTime;

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

  /*
   * Makes the scrollbar go down.
   */
  scrolledWindow = kernel->widgets.mainBoardOutputScrolledWindow;
  vAdj =
    gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(scrolledWindow));

  gtk_adjustment_set_value(vAdj,vAdj->upper /* maximum, thus bottom */);
  /* gtk_adjustment_value_changed(vAdj); */

  /*
   * Increment output list size.
   */
  kernel->widgets.mainBoardOutputListSize++;

  g_string_free(string,TRUE);


  /*
   * Raise main-board window in order to see the incoming message.
   */
  gdk_window_raise(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window);

  /*
   * Allow graphic updates.
   */
  gdisp_updateWholeGui();

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
 * Write an information in the status window.
 */
static void
gdisp_statusWrite ( Kernel_T *kernel,
		    GString  *message )
{

  /*
   * Pop previous message.
   */
  if (message != (GString*)NULL) {

    gtk_statusbar_pop(GTK_STATUSBAR(kernel->widgets.mainBoardStatusBar),
		      kernel->widgets.mainBoardStatusContextID);

    gtk_statusbar_push(GTK_STATUSBAR(kernel->widgets.mainBoardStatusBar),
		       kernel->widgets.mainBoardStatusContextID,
		       message->str);

    g_string_free(message,TRUE);

  }

}


/*
 * Manage (set/get) file name through the standart Gtk+ file selection widget.
 */
#define GD_NO_ACTION      0
#define GD_NEW_CONF       1
#define GD_OPEN_CONF      2
#define GD_SAVE_CONF      3
#define GD_SAVE_AS_CONF   4
#define GD_CLOSE_CONF     5

#define GD_NEED_FILE    256

static void
gdisp_launchAction ( GtkObject *siblingButton,
		     gpointer   userData )
{

  Kernel_T         *kernel         = (Kernel_T*)userData;
  gboolean          hasCompleted   = FALSE;
  GtkFileSelection *fileSelection  = (GtkFileSelection*)NULL;
  GString          *messageString  = (GString*)NULL;
  gchar            *messageHeader  = (gchar*)NULL;
  gchar            *messageFooter  = (gchar*)NULL;
  Message_T         messageType    = GD_ERROR;
  gboolean          freeIoFilename = FALSE;

  /*
   * Update GUI.
   */
  gdisp_updateWholeGui();

  /*
   * Get back selected filename.
   */
  if (kernel->widgets.fileSelector != (GtkWidget*)NULL) {

    fileSelection = GTK_FILE_SELECTION(kernel->widgets.fileSelector);

    if (kernel->ioFilename != (gchar*)NULL) {
      g_free(kernel->ioFilename);
    }

    kernel->ioFilename =
      gdisp_strDup(gtk_file_selection_get_filename(fileSelection));

  }

  /*
   * Launch action.
   */
  hasCompleted = (*kernel->factoryAction)(kernel);

  /*
   * Keep in mind that the GString is released into the
   * 'gdispOutputWrite' function.
   * Do not 'g_string_free' it.
   */
  switch (kernel->factoryActionId) {

  case GD_NEW_CONF :
    messageHeader  = "New configuration";
    freeIoFilename = TRUE;
    break;

  case GD_OPEN_CONF :
    messageHeader = "Open configuration";
    break;

  case GD_SAVE_CONF :
  case GD_SAVE_AS_CONF :
    messageHeader = "Save configuration";
    break;

  case GD_CLOSE_CONF :
    messageHeader  = "Close configuration";
    freeIoFilename = TRUE;
    break;

  default :
    messageHeader = "Invalid action";
    break;

  }

  messageType   = hasCompleted == TRUE ? GD_MESSAGE : GD_ERROR;
  messageFooter = hasCompleted == TRUE ?
    "correctly performed." : "has raised an error.";

  if (kernel->ioFilename != (gchar*)NULL) {

    /*
     * Output window.
     */
    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "IO file [ %s ].",
		     kernel->ioFilename);
    kernel->outputFunc(kernel,messageString,messageType);

    /*
     * Status window.
     */
    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,
		     "Configuration is '%s'.",
		     kernel->ioFilename);
    kernel->statusFunc(kernel,messageString);

  }

  if (freeIoFilename == TRUE) {

    messageString = g_string_new(" TARGA is ready.");
    (*kernel->statusFunc)(kernel,messageString);

  }

  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "%s %s",
		   messageHeader,
		   messageFooter);
  kernel->outputFunc(kernel,messageString,messageType);

  /*
   * Reset file selector address.
   */
  kernel->widgets.fileSelector = (GtkWidget*)NULL;
  kernel->factoryAction        = (gboolean(*)(Kernel_T*))NULL;
  kernel->factoryActionId      = GD_NO_ACTION;
  if (freeIoFilename == TRUE) {
    if (kernel->ioFilename != (gchar*)NULL) {
      g_free(kernel->ioFilename);
      kernel->ioFilename = (gchar*)NULL;
    }
  }

}

static void
gdisp_chooseFilename ( Kernel_T *kernel )
{

  GtkFileSelection *fileSelection = (GtkFileSelection*)NULL;


  /*
   * Create the file selector.
   */
  kernel->widgets.fileSelector =
    gtk_file_selection_new("Please select a configuration file.");

  fileSelection = GTK_FILE_SELECTION(kernel->widgets.fileSelector);

  gtk_signal_connect(GTK_OBJECT(fileSelection->ok_button),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_launchAction),
		     (gpointer)kernel);

  /*
   * Interesting files are those that have the ".gdpc" extension.
   */
#if defined(GD_FILTER_ON_EXTENSION)
  gtk_file_selection_complete(fileSelection,
			      ".xml");
#endif

  /*
   * Ensure that the dialog box is destroyed when the user clicks a button.
   */
  gtk_signal_connect_object(GTK_OBJECT(fileSelection->ok_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    (gpointer)kernel->widgets.fileSelector);

  gtk_signal_connect_object(GTK_OBJECT(fileSelection->cancel_button),
			    "clicked",
			    GTK_SIGNAL_FUNC(gtk_widget_destroy),
			    (gpointer)kernel->widgets.fileSelector);

  /*
   * Display that dialog box.
   */
  gtk_widget_show(kernel->widgets.fileSelector);

}


static void
gdisp_manageAction ( gpointer factoryData,
		     guint    factoryActionId )
{

  Kernel_T *kernel       = (Kernel_T*)factoryData;
  gboolean  needFilename = FALSE;

  /*
   * The action needs a filename in order to run.
   */
  if (factoryActionId & GD_NEED_FILE) {
    needFilename = TRUE;
  }
  factoryActionId &= ~GD_NEED_FILE;

  /*
   * Reset IO information.
   */
  kernel->factoryAction   = (gboolean(*)(Kernel_T*))NULL;
  kernel->factoryActionId = factoryActionId;

  /*
   * Look at the action to be performed.
   */
  switch (factoryActionId) {

  case GD_NEW_CONF :
    kernel->factoryAction = gdisp_newConfiguration;
    break;

  case GD_OPEN_CONF :
    kernel->factoryAction = gdisp_openConfigurationFile;
    break;

  case GD_SAVE_CONF :
    kernel->factoryAction = gdisp_saveConfigurationFile;
    if (kernel->ioFilename != (gchar*)NULL) {
      needFilename = FALSE;
    }
    break;

  case GD_SAVE_AS_CONF :
    kernel->factoryAction = gdisp_saveConfigurationFile;
    break;

  case GD_CLOSE_CONF :
    kernel->factoryAction = gdisp_closeConfiguration;
    break;

  default :
    return; /* action not implemented */
    break;

  }

  /*
   * Proceed.
   */
  if (needFilename == TRUE) {
    gdisp_chooseFilename(kernel);
  }
  else {
    kernel->widgets.fileSelector = (GtkWidget*)NULL;
    gdisp_launchAction((GtkObject*)NULL,
		       (gpointer)kernel);
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

 { "/_Session",                 NULL,
   NULL,                        0,                           "<Branch>"      },
 { "/_Session/_New",            "<control>N",
   gdisp_manageAction,          GD_NEW_CONF,                 NULL            },
 { "/_Session/_Open",           "<control>O",
   gdisp_manageAction,          GD_OPEN_CONF | GD_NEED_FILE, NULL            },
 { "/_Session/_Save",           "<control>S",
   gdisp_manageAction,          GD_SAVE_CONF | GD_NEED_FILE, NULL            },
 { "/_Session/Save _As",        "<control>A",
   gdisp_manageAction,          GD_SAVE_AS_CONF | GD_NEED_FILE, NULL         },
 { "/_Session/_Close",          "<control>C",
   gdisp_manageAction,          GD_CLOSE_CONF,               NULL            },
 { "/_Session/Sep1",            NULL,
   NULL,                        0,                           "<Separator>"   },
 { "/_Session/_Hosts & URL",    "<control>H",
   gdisp_manageHosts,           0,                           NULL            },
 { "/_Session/Sep2",            NULL,
   NULL,                        0,                           "<Separator>"   },
 { "/_Session/_Quit",           "<control>Q",
   gdisp_quitItemHandler,       0,                           NULL            },
 { "/_Data",                    NULL,
   NULL,                        0,                           "<Branch>"      },
 { "/_Data/_All Data",          "<control>A",
   gdisp_showDataBook,          0,                           NULL            },
 { "/_Plots",                   NULL,
   NULL,                        0,                           "<Branch>"      },
 { "/_Plots/New page",          NULL,
   NULL,                        0,                           "<Branch>"      },
 { "/_Plots/New page/_Custom",  "<control>C",
   gdisp_createGraphicPage,     0,                           NULL            },
 { "/_Plots/New page/Sep3",     NULL,
   NULL,                        0,                           "<Separator>"   },
 { "/_Plots/New page/_1 x 1",   "<control>1",
   gdisp_createGraphicPage,     1,                           NULL            },
 { "/_Plots/New page/_2 x 2",   "<control>2",
   gdisp_createGraphicPage,     2,                           NULL            },
 { "/_Plots/New page/_3 x 3",   "<control>3",
   gdisp_createGraphicPage,     3,                           NULL            },
 { "/_Plots/New page/_4 x 4",   "<control>4",
   gdisp_createGraphicPage,     4,                           NULL            },
 { "/_Plots/New page/_5 x 5",   "<control>5",
   gdisp_createGraphicPage,     5,                           NULL            },
 { "/_Help",                    NULL,
   NULL,                        0,                           "<LastBranch>"  },
 { "/_Help/About",              NULL,
   NULL,                        0,                           NULL            },

};

static GtkItemFactoryEntry
gdispMainBoardMenuShortDefinitions[] = {

 { "/_Session",                 NULL,
   NULL,                        0,                           "<Branch>"      },
 { "/_Session/_Open",           "<control>O",
   gdisp_manageAction,          GD_OPEN_CONF | GD_NEED_FILE, NULL            },
 { "/_Session/Sep1",            NULL,
   NULL,                        0,                           "<Separator>"   },
 { "/_Session/_Quit",           "<control>Q",
   gdisp_quitItemHandler,       0,                           NULL            },
 { "/_Help",                    NULL,
   NULL,                        0,                           "<LastBranch>"  },
 { "/_Help/About",              NULL,
   NULL,                        0,                           NULL            },

};


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

  GtkWidget      *mainVBox        = (GtkWidget*)NULL;
  GtkWidget      *mainHBox        = (GtkWidget*)NULL;
  GtkWidget      *pilotBox        = (GtkWidget*)NULL;
  GtkWidget      *logoFrame       = (GtkWidget*)NULL;
  GtkWidget      *menuBar         = (GtkWidget*)NULL;
  GtkWidget      *scrolledWindow  = (GtkWidget*)NULL;
  GtkWidget      *hSeparator      = (GtkWidget*)NULL;
  GtkWidget      *vSeparator      = (GtkWidget*)NULL;
  GtkWidget      *outputMenu      = (GtkWidget*)NULL;
  GtkWidget      *outputClearItem = (GtkWidget*)NULL;
  GtkWidget      *statusBar       = (GtkWidget*)NULL;
  GtkItemFactory *itemFactory     = (GtkItemFactory*)NULL;
  GtkAccelGroup  *accelGroup      = (GtkAccelGroup*)NULL;
  gint            menuItemNb      = 0;

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
		     GTK_SIGNAL_FUNC(gdisp_manageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.mainBoardWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdisp_destroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window title and border width.
   */
  gtk_widget_set_usize(GTK_WIDGET(kernel->widgets.mainBoardWindow),
		       600 /* width  */,
		       170 /* height */);

  gtk_window_set_title(GTK_WINDOW(kernel->widgets.mainBoardWindow),
		       "TARGA Copyright (c) 2006.");

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
  if (kernel->editionIsAllowed == TRUE) {

    menuItemNb = sizeof(gdispMainBoardMenuDefinitions   ) /
                 sizeof(gdispMainBoardMenuDefinitions[0]);

  }
  else {

    menuItemNb = sizeof(gdispMainBoardMenuShortDefinitions   ) /
                 sizeof(gdispMainBoardMenuShortDefinitions[0]);

  }

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
  if (kernel->editionIsAllowed == TRUE) {

    gtk_item_factory_create_items(itemFactory,
				  menuItemNb,
				  gdispMainBoardMenuDefinitions,
				  (gpointer)kernel);

  }
  else {

    gtk_item_factory_create_items(itemFactory,
				  menuItemNb,
				  gdispMainBoardMenuShortDefinitions,
				  (gpointer)kernel);

  }

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
  kernel->widgets.mainBoardMenuBar = menuBar;

  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Map top-level window.
   */
  gtk_widget_show(kernel->widgets.mainBoardWindow);
  gdk_window_move(GTK_WIDGET(kernel->widgets.mainBoardWindow)->window,
		  kernel->widgets.mainBoardWindowXPosition,
		  kernel->widgets.mainBoardWindowYPosition);


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

  kernel->widgets.mainBoardOutputScrolledWindow = scrolledWindow;

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


  /* ----------------------- PILOT BOARD ----------------------- */

  /*
   * Create the pilot board (not shown after creation).
   */
  pilotBox = gdisp_createPilotBoard(kernel);

  gtk_box_pack_start(GTK_BOX(mainHBox),
		     pilotBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(pilotBox);


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
   * Create a drawing area that will receive the animated logo.
   */
  logoFrame = gdisp_createAnimatedLogo(kernel);

  gtk_box_pack_start(GTK_BOX(mainHBox),
		     logoFrame,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);

  gtk_widget_show(logoFrame);

  /*
   * Manage logo animation.
   */
  gdisp_stopLogoAnimation(kernel,FALSE /* stop all */);


  /* ------------------------ SEPARATOR ------------------------ */

  /*
   * Separate output window and status bar with a horizontal separator.
   */
  hSeparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(mainVBox),
		     hSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(hSeparator);


  /* -------------------------- STATUS BAR -------------------------- */

  statusBar = gtk_statusbar_new();

  gtk_box_pack_start(GTK_BOX(mainVBox),
		     statusBar,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(statusBar);

  kernel->widgets.mainBoardStatusBar       = statusBar;
  kernel->widgets.mainBoardStatusContextID = 
    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusBar),
				 "kernel");


  /* ------------------------ END MAIN BOARD  ------------------------ */

  /*
   * Tell the kernel that the output list has been created.
   * It is now ready to be used.
   */
  kernel->outputFunc = gdisp_outputWrite;
  kernel->statusFunc = gdisp_statusWrite;

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
  messageString = g_string_new("TARGA Graphic Tool, Version 1.0");
  (*kernel->outputFunc)(kernel,messageString,GD_MESSAGE);

  /*
   * Report whether we are in a thread-safe environment.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "Implementation is %sthread-safe.",
		   kernel->isThreadSafe == TRUE ? "" : "NOT ");
  (*kernel->outputFunc)(kernel,messageString,GD_WARNING);

  /*
   * Application is ready.
   */
  messageString = g_string_new(" TARGA is ready.");
  (*kernel->statusFunc)(kernel,messageString);

}

