/*!  \file 

$Id: gdisp_popupMenu.h,v 1.1 2006-02-02 21:03:32 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2003 - Euskadi

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

#ifndef __POPUP_MENU_H__
#define __POPUP_MENU_H__

#include "gdisp_kernel.h"

/*
 * Popup Menu Handler.
 */
typedef struct PopupMenu_T_ *PopupMenu_T_Ptr;

typedef void (*PopupMenuHandler_T)(Kernel_T*,
				   PopupMenu_T_Ptr,
				   gpointer,
				   gpointer);

/*
 * Dynamic PopupMenu.
 */
typedef struct PopupMenu_T_ {

  /*
   * Kernel.
   */
  Kernel_T  *kernel;

  /*
   * Popup menu widget, parent widget.
   */
  GtkWidget *menu;
  GtkWidget *parent;

  /*
   * User handler and data.
   */
  PopupMenuHandler_T userHandler;
  gpointer           userData;

} PopupMenu_T;


/*
 * Prototypes.
 */
void         gdisp_addMenuItem ( PopupMenu_T        *menu,
				 gchar              *itemLabel,
				 gpointer            itemData );

void         gdisp_destroyMenu ( PopupMenu_T        *menu );

PopupMenu_T *gdisp_createMenu  ( Kernel_T           *kernel,
				 GtkWidget          *parentWidget,
				 gchar              *title,
				 PopupMenuHandler_T  userHandler,
				 gpointer            userData );

#endif /* __POPUP_MENU_H__ */
