/*

$Id: gdisp_plotSheet.h,v 1.1 2007-11-14 21:53:20 esteban Exp $

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

File      : Definition of 'spreadsheet plot' private structures.

-----------------------------------------------------------------------
*/

#ifndef __SHEET_PLOT_H__
#define __SHEET_PLOT_H__

#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>

#include "gtksheet.h"
#include "gtkitementry.h"
#include "gtkcombobox.h"
#include "gtkbordercombo.h"
#include "gtkcolorcombo.h"
#include "gtkfontcombo.h"

#include "gdisp_popupMenu.h"

/* -------------------- */

/* XPM */
static char *SheetCenterJustifiedPixmap[] =
{
  "28 26 2 1",
  ".      c #None",
  "X      c #000000000000",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "        XXXXXXXXXXXX        ",
  "        XXXXXXXXXXXX        ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "        XXXXXXXXXXXX        ",
  "        XXXXXXXXXXXX        ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "        XXXXXXXXXXXX        ",
  "        XXXXXXXXXXXX        ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
};
/* XPM */
static char *SheetFontPixmap[] = {
"26 26 3 1",
" 	c #None",
".	c #000000000000",
"X	c #000000000000",
"                          ",
"                          ",
"                          ",
"            .             ",
"           ...            ",
"           ...            ",
"          .....           ",
"          .....           ",
"         .. ....          ",
"         .. ....          ",
"        ..   ....         ",
"        .........         ",
"       ...........        ",
"       ..     ....        ",
"      ..       ....       ",
"      ..       ....       ",
"    .....     .......     ",
"                          ",
"                          ",
"                          ",
"     XXXXXXXXXXXXXXXX     ",
"     XXXXXXXXXXXXXXXX     ",
"     XXXXXXXXXXXXXXXX     ",
"     XXXXXXXXXXXXXXXX     ",
"                          ",
"                          ",
"                          "};
/* XPM */
static char *SheetLeftJustifiedPixmap[] =
{
  "28 26 2 1",
  ".      c #None",
  "X      c #000000000000",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "     XXXXXXXXXXXXX          ",
  "     XXXXXXXXXXXXX          ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "     XXXXXXXXXXXXX          ",
  "     XXXXXXXXXXXXX          ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "     XXXXXXXXXXXXX          ",
  "     XXXXXXXXXXXXX          ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
};
/* XPM */
static char *SheetPaintPixmap[] =
{
  "26 26 6 1",
  ".      c #None",
  "a      c #000000000000",
  "e      c #929292929292",
  "g      c #DBDBDBDBDBDB",
  "h      c #FFFFFFFFFFFF",
  "X      c #FFFFFFFFFFFF",
  "..........................",
  "...........ee.............",
  "..........eeee............",
  ".........eeggee...........",
  ".........eegaee...........",
  ".........eeahee...........",
  ".........aahheeaa.........",
  ".........ahhgeegaaa.......",
  "........ahhghaeggaaa......",
  ".......ahhghagaggeaaa.....",
  "......ahhghggaggeeaaae....",
  ".....ahhghgggggeeaaaae....",
  ".....ahghgggggeeaeaaae....",
  "......ahgggggeeaeeaaae....",
  ".......ahgggeeaee.aaae....",
  "........aggeeaee..aaee....",
  ".........aeeaee...aee.....",
  "..........aaee.....e......",
  "...........ee.............",
  "..........................",
  "....XXXXXXXXXXXXXXXXXX....",
  "....XXXXXXXXXXXXXXXXXX....",
  "....XXXXXXXXXXXXXXXXXX....",
  "....XXXXXXXXXXXXXXXXXX....",
  "..........................",
  "..........................",
};
/* XPM */
static char *SheetLinePixmap[] =
{
  "26 26 3 1",
  "       c #None",
  "a      c #000000000000",
  "X      c #FFFF00000000",
  "                          ",
  "                          ",
  "                          ",
  "      XXXXXXXXXXXXX       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ", 
  "      XXXXXXXXXXXXX       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      X     X     X       ",
  "      XXXXXXXXXXXXX       ",
  "                          ",
  "                          ",
  "                          ",
  "                          ",
  "    aaaaaaaaaaaaaaaaaa    ",
  "    aaaaaaaaaaaaaaaaaa    ",
  "    aaaaaaaaaaaaaaaaaa    ",
  "    aaaaaaaaaaaaaaaaaa    ",
  "                          ",
  "                          ",
};
/* XPM */
static char *SheetRightJustifiedPixmap[] =
{
  "28 26 2 1",
  ".      c #None",
  "X      c #000000000000",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "          XXXXXXXXXXXXX     ",
  "          XXXXXXXXXXXXX     ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "          XXXXXXXXXXXXX     ",
  "          XXXXXXXXXXXXX     ",
  "                            ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "     XXXXXXXXXXXXXXXXXX     ",
  "                            ",
  "          XXXXXXXXXXXXX     ",
  "          XXXXXXXXXXXXX     ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
  "                            ",
};

/* -------------------- */

/*
 * Private structure of the general toolbar.
 */
typedef struct SheetGeneralToolbar_ *SheetGeneralToolbarPtr;
typedef struct SheetGeneralToolbar_ {

  GtkWidget   *topLevelWindow;
  GtkWidget   *generalEntry;
  GtkWidget   *location;
  GtkWidget   *toolBar;
  GtkWidget   *fontComboBox;
  GtkWidget   *leftJustifiedButton;
  GtkWidget   *centeredButton;
  GtkWidget   *rightJustifiedButton;
  GtkWidget   *fgColorComboBox;
  GtkWidget   *fgPixmap;
  GtkWidget   *bgColorComboBox;
  GtkWidget   *bgPixmap;
  GtkWidget   *bdColorComboBox;
  GtkWidget   *bdPixmap;
  GtkWidget   *borderComboBox;

} SheetGeneralToolbar_T;


/*
 * Private structure of a 'sheet plot'.
 */
typedef enum {
  GD_SHOW_HIDE_GRID = 0,
  GD_SHOW_HIDE_TOOLBAR,
  GD_SHOW_HIDE_COLUMN_TITLES,
  GD_SHOW_HIDE_ROW_TITLES,
  GD_INSERT_COLUMNS,
  GD_INSERT_ROWS,
  GD_DELETE_COLUMNS,
  GD_DELETE_ROWS,
  GD_CLEAR_CELLS,
  GD_TBD
} SheetAction_T;

typedef struct Sheet_T_ *SheetPtr;
typedef struct Sheet_T_ {

  GtkWidget   *topLevelWidget;
  GtkWidget   *scrolledWindow;
  GtkWidget   *sheet;

  PopupMenu_T *popupMenu;

} Sheet_T;

#endif /* __SHEET_PLOT_H__ */
