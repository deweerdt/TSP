/*

$Id: gdisp_plotText.h,v 1.8 2006-11-08 21:31:12 esteban Exp $

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

File      : Definition of 'text plot' private structures.

-----------------------------------------------------------------------
*/

#ifndef __TEXT_PLOT_H__
#define __TEXT_PLOT_H__


/*
 * Private structure of a 'text plot'.
 */
enum {

  GD_SYMBOL_NAME_COLUMN  = 0,
  GD_SYMBOL_VALUE_COLUMN = 1,
  GD_SYMBOL_WRITE_COLUMN = 2,
  GD_SYMBOL_MAX_COLUMNS  = 3

};


/*
 * Characteristics of a row.
 */
typedef struct PlotTextRowData_T_ {

  Symbol_T *symbol;
  Format_T  format;

} PlotTextRowData_T;


/*
 * Main plot structure.
 */
typedef struct PlotText_T_ {

  /*
   * Attributes.
   */

  /*
   * List of pointer on TSP_Symbol_T.
   */
  GList               *pttSymbolList;

  /*
   * Graphic widget.
   */
  GtkStyle            *pttStyle;
  GtkWidget           *pttCList;
  guint                pttCListWidth;
  guint                pttCListHeight;
  gfloat               pttColumnRatio;
  gboolean             pttShowWriteColumn;
  guchar               pttIsSizeAllocating;
  gint                 pttSelectedRow;
  gint                 pttSelectedColumn;
  gboolean             pttShowUnits;

  /*
   * Popup menus.
   */
  PopupMenu_T         *pttMainMenu;
  PopupMenu_T         *pttHexadecimalMenu;
  PopupMenu_T         *pttFloatingFixedMenu;
  void                *pttRemoveSymbolsItem;

  /*
   * Parent widget.
   */
  GtkWidget           *pttParent;

} PlotText_T;


#endif /* __TEXT_PLOT_H__ */
