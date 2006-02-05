/*!  \file 

$Id: gdisp_popupMenu.c,v 1.2 2006-02-05 18:02:36 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi

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

File      : Dynamic popup menu management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "gdisp_popupMenu.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


static void
gdisp_menuHandler ( GtkMenuItem *menuItem,
		    gpointer     userData )
{

  PopupMenu_T *menu            = (PopupMenu_T*)userData;
  gpointer     menuItemData    = (gpointer)NULL;
  gboolean     itemHasAsubMenu = FALSE;

  /*
   * Do nothing if the item has a sub-menu.
   */
  menuItemData = gtk_object_get_data(GTK_OBJECT(menuItem),
				     "itemHasAsubMenu");

  itemHasAsubMenu = (gboolean)GPOINTER_TO_UINT(menuItemData);

  if (itemHasAsubMenu == FALSE) {

    /*
     * Activate user handler is exists.
     */
    if (menu->userHandler != (PopupMenuHandler_T)NULL) {

      menuItemData = gtk_object_get_data(GTK_OBJECT(menuItem),
					 "menuItemData");

      (*menu->userHandler)(menu->kernel,
			   menu,
			   menu->userData,
			   menuItemData);

    } /* user handler is defined */

  } /* itemHasAsubMenu == FALSE */

}


/*
 * Event handler for the dynamic menu.
 * Show the popup-menu given as an argument when right-clicking.
 */
static gboolean
gdisp_menuGeneralHandler ( GtkWidget *widget,
			   GdkEvent  *event ,
			   gpointer   userData )
{

  PopupMenu_T *menu = (PopupMenu_T*)userData;

  /*
   * Look for a 'button-press' event.
   */
  if (event->type == GDK_BUTTON_PRESS) {

    GdkEventButton *bEvent = (GdkEventButton*)event;

    /*
     * Click with the third button.
     */
    if (bEvent->button == 3 /* right-click */) {

      gtk_menu_popup(GTK_MENU(menu->menu),
		     NULL           /* parent menu shell      */,
		     NULL           /* parent menu item       */,
		     NULL           /* user position function */,
		     NULL           /* user private data      */,
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
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Add an item to a popup menu.
 */
void*
gdisp_addMenuItem ( PopupMenu_T *menu,
		    gchar       *itemLabel,
		    gpointer     itemData )
{

  GtkWidget *menuItem = (GtkWidget*)NULL;

  /*
   * Create a menu-item... and add it to the popup menu.
   * Connect a signal for handling the menu when "activated".
   */
  menuItem = gtk_menu_item_new_with_label(itemLabel);

  gtk_menu_append(GTK_MENU(menu->menu),
		  menuItem);

  gtk_signal_connect(GTK_OBJECT(menuItem),
		     "activate",
		     GTK_SIGNAL_FUNC(gdisp_menuHandler),
		     (gpointer)menu);

  gtk_object_set_data(GTK_OBJECT(menuItem),
		      "menuItemData",
		      itemData);

  gtk_widget_show(menuItem);

  return (void*)menuItem;

}


/*
 * Destroy a popup menu and all its content.
 */
void
gdisp_destroyMenu ( PopupMenu_T *menu )
{

  if (menu != (PopupMenu_T*)NULL) {

#if defined(GD_UNREF_THINGS)
    gtk_widget_unref(menu->menu);
#endif
    g_free(menu);

  }

}


/*
 * Create a popup menu.
 */
PopupMenu_T*
gdisp_createMenu ( Kernel_T           *kernel,
		   GtkWidget          *parentWidget,
		   gchar              *title,
		   PopupMenuHandler_T  userHandler,
		   gpointer            userData )
{

  PopupMenu_T *menu     = (PopupMenu_T*)NULL;
  GtkWidget   *menuItem = (GtkWidget*)NULL;

  /* ------------------- ALLOCATE MEMORY ------------------- */

  menu = (PopupMenu_T*)g_malloc0(sizeof(PopupMenu_T));

  if (menu == (PopupMenu_T*)NULL) {
    return (PopupMenu_T*)NULL;
  }

  menu->kernel      = kernel;
  menu->parent      = parentWidget;
  menu->userHandler = userHandler;
  menu->userData    = userData;

  /* ------------ MENU WHEN CLICKING ON THE LIST ------------ */

  /*
   * Init the menu-widget, and remember
   * -- never 'gtk_show_widget()' the menu widget !! --
   * This is the menu that holds the menu items, the one that will pop
   * up when you click on the output list.
   */
  menu->menu = gtk_menu_new ();

  /*
   * Create a menu-item with as a title... and add it to the popup menu.
   */
  if (title != (gchar*)NULL) {

    menuItem = gtk_menu_item_new_with_label(title);
    gtk_menu_append(GTK_MENU(menu->menu),
		    menuItem);

    gtk_widget_show(menuItem);
    gtk_widget_set_sensitive(menuItem,FALSE);

  }

  /*
   * If parent widget is a menu item, connect the new menu
   * as a sub-menu of the given item.
   */
  if (GTK_IS_MENU_ITEM(menu->parent)) {

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu->parent),
			      menu->menu);

    gtk_menu_item_configure(GTK_MENU_ITEM(menu->parent),
			    0 /* show toggle indicator */,
			    1 /* show submenu indicator */ );

    gtk_object_set_data(GTK_OBJECT(menu->parent),
			"itemHasAsubMenu",
			(gpointer)GUINT_TO_POINTER(TRUE));

  }

  /*
   * Connect signals for handling dynamic menu.
   * This makes the dynamic menu appear when the user right clicks on the
   * parent widget.
   */
  else {

    gtk_signal_connect(GTK_OBJECT(menu->parent),
		       "event",
		       GTK_SIGNAL_FUNC(gdisp_menuGeneralHandler),
		       (gpointer)menu);

  }

  return menu;

}




