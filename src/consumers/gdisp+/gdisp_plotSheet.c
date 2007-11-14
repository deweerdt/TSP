/*

$Id: gdisp_plotSheet.c,v 1.1 2007-11-14 21:53:19 esteban Exp $

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

File      : gdisp_plotSheet.c
            SpreadSheet plot.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"

#include "gdisp_plotSheet.h"


/*
 --------------------------------------------------------------------
                         STATIC GLOBAL VARIABLES
 --------------------------------------------------------------------
*/

#define SHEET_VERBOSE

#define GD_DEFAULT_MAX_ROWS    100
#define GD_DEFAULT_MAX_COLUMNS  26

/*
 * Global Variables.
 */
Sheet_T               *SheetCurrentSheet   = (Sheet_T*)NULL;
SheetGeneralToolbar_T *SheetGeneralToolbar = (SheetGeneralToolbar_T*)NULL;

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static void gdisp_createSheetGeneralToolBar ( Kernel_T *kernel );

/*
 * Rename columns and rows.
 */
static void
SheetRenameColumnsAndRows ( Kernel_T *kernel,
			    Sheet_T  *sheet )
{

  guint column = 0;
  guint row    = 0;
  gchar columnTitle[2];
  gchar rowTitle   [8];

  /*
   * Column titles are letters instead of numbers.
   */
  columnTitle[0] = 'A';
  columnTitle[1] = '\0';
  for (column=0; column<=GTK_SHEET(sheet->sheet)->maxcol; column++) {

    gtk_sheet_column_button_add_label(GTK_SHEET(sheet->sheet),
				      column,
				      columnTitle);

    gtk_sheet_set_column_title(GTK_SHEET(sheet->sheet),
			       column,
			       columnTitle);

    columnTitle[0]++;

  }

  /*
   * Row titles are numbers.
   */
  for (row=0; row<=GTK_SHEET(sheet->sheet)->maxrow; row++) {

    sprintf(rowTitle,"%d",row+1);

    gtk_sheet_row_button_add_label(GTK_SHEET(sheet->sheet),
				   row,
				   rowTitle);

    gtk_sheet_set_row_title(GTK_SHEET(sheet->sheet),
			    row,
			    rowTitle);

  }

}

/*
 * Popup Menu handler.
 */
static void
SheetPopupMenuHandler ( Kernel_T    *kernel,
			PopupMenu_T *menu,
			gpointer     menuData,
			gpointer     itemData )
{

  Sheet_T  *sheet         = (Sheet_T*)menuData;
  guint     item          = GPOINTER_TO_UINT(itemData);
  gboolean  gridIsVisible = TRUE;

  gtk_sheet_freeze(GTK_SHEET(sheet->sheet));

  switch (item) {

  case GD_SHOW_HIDE_GRID :

    gridIsVisible = gtk_sheet_grid_visible(GTK_SHEET(sheet->sheet));
    gtk_sheet_show_grid(GTK_SHEET(sheet->sheet),
			GD_TOGGLE_BOOLEAN(gridIsVisible));

    break;

  case GD_SHOW_HIDE_TOOLBAR :

    gdisp_createSheetGeneralToolBar(kernel);
    break;

  case GD_SHOW_HIDE_ROW_TITLES :

    if (GTK_SHEET_ROW_TITLES_VISIBLE(GTK_SHEET(sheet->sheet))) {
      gtk_sheet_hide_row_titles(GTK_SHEET(sheet->sheet));
    }
    else {
      gtk_sheet_show_row_titles(GTK_SHEET(sheet->sheet));
    }
    break;

  case GD_SHOW_HIDE_COLUMN_TITLES :

    if (GTK_SHEET_COL_TITLES_VISIBLE(GTK_SHEET(sheet->sheet))) {
      gtk_sheet_hide_column_titles(GTK_SHEET(sheet->sheet));
    }
    else {
      gtk_sheet_show_column_titles(GTK_SHEET(sheet->sheet));
    }
    break;

  case GD_INSERT_ROWS :

    if (GTK_SHEET(sheet->sheet)->state == GTK_SHEET_ROW_SELECTED) {

      gtk_sheet_insert_rows(GTK_SHEET(sheet->sheet),
			    GTK_SHEET(sheet->sheet)->range.row0,
			    GTK_SHEET(sheet->sheet)->range.rowi -
			    GTK_SHEET(sheet->sheet)->range.row0 + 1);

    }
    break;

  case GD_INSERT_COLUMNS :

    if (GTK_SHEET(sheet->sheet)->state == GTK_SHEET_COLUMN_SELECTED) {

      gtk_sheet_insert_columns(GTK_SHEET(sheet->sheet),
			    GTK_SHEET(sheet->sheet)->range.col0,
			    GTK_SHEET(sheet->sheet)->range.coli -
			    GTK_SHEET(sheet->sheet)->range.col0 + 1);

    } 
    break;

  case GD_DELETE_COLUMNS :

    if (GTK_SHEET(sheet->sheet)->state == GTK_SHEET_COLUMN_SELECTED) {

      gtk_sheet_delete_columns(GTK_SHEET(sheet->sheet),
			       GTK_SHEET(sheet->sheet)->range.col0,
			       GTK_SHEET(sheet->sheet)->range.coli -
			       GTK_SHEET(sheet->sheet)->range.col0 + 1);

    } 
    break;

  case GD_DELETE_ROWS :

    if (GTK_SHEET(sheet->sheet)->state == GTK_SHEET_ROW_SELECTED) {

      gtk_sheet_delete_rows(GTK_SHEET(sheet->sheet),
			    GTK_SHEET(sheet->sheet)->range.row0,
			    GTK_SHEET(sheet->sheet)->range.rowi -
			    GTK_SHEET(sheet->sheet)->range.row0 + 1);

    }

    break;

  case GD_CLEAR_CELLS :

    if (GTK_SHEET(sheet->sheet)->state != GTK_SHEET_NORMAL) {

      gtk_sheet_range_clear(GTK_SHEET(sheet->sheet),
			    &GTK_SHEET(sheet->sheet)->range);

    } 
    break;

  default :
    break;

  }

  if (item == GD_INSERT_ROWS    ||
      item == GD_INSERT_COLUMNS ||
      item == GD_DELETE_COLUMNS ||
      item == GD_DELETE_ROWS       ) {

    SheetRenameColumnsAndRows(kernel,sheet);

  }

  gtk_sheet_thaw(GTK_SHEET(sheet->sheet));

}

/*
 * Create popup menu on sheet.
 */
static void
SheetCreatePopupMenu ( Kernel_T  *kernel,
		       Sheet_T   *sheet,
		       GtkWidget *parent )
{

  guint itemData = 0;

  /*
   * Create the dynamic menu.
   */
  sheet->popupMenu = gdisp_createMenu((void*)kernel,
				      parent,
				      SheetPopupMenuHandler,
				      (gpointer)sheet);

  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_TITLE,
		    "Main Options",
		    (gpointer)NULL);

  itemData = GD_SHOW_HIDE_GRID;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Show / Hide grid",
		    (gpointer)GUINT_TO_POINTER(itemData));

  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_SEPARATOR,
		    (gchar*)NULL,
		    (gpointer)NULL);

  itemData = GD_SHOW_HIDE_TOOLBAR;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Show / Hide toolbar",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_SHOW_HIDE_ROW_TITLES;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Show / Hide row titles",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_SHOW_HIDE_COLUMN_TITLES;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Show / Hide column titles",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_INSERT_COLUMNS;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Insert column(s)",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_INSERT_ROWS;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Insert row(s)",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_DELETE_COLUMNS;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Delete column(s)",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_DELETE_ROWS;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Delete row(s)",
		    (gpointer)GUINT_TO_POINTER(itemData));

  itemData = GD_CLEAR_CELLS;
  gdisp_addMenuItem(sheet->popupMenu,
		    GD_POPUP_ITEM,
		    "Clear cells",
		    (gpointer)GUINT_TO_POINTER(itemData));

}

/*
 * =============================================================
                    GENERAL TOOL BAR HANDLERS
 * =============================================================
 */

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

  /* Kernel_T *kernel = (Kernel_T*)data; */

  return FALSE;

}


/*
 * The "destroy" event occurs when we call "gtk_widget_destroy" on
 * the top-level window, of if we return FALSE in the "delete_event"
 * callback (see above).
 */
static void
gdisp_destroySignalHandler (GtkWidget *dataBookWindow,
			    gpointer   data)
{

  /* Kernel_T *kernel = (Kernel_T*)data; */

  /*
   * Reset global address.
   */
  g_free(SheetGeneralToolbar);
  SheetGeneralToolbar = (SheetGeneralToolbar_T*)NULL;

}

/*
 * Cell Font Handler.
 */
static void
SheetChangeFont ( GtkWidget *widget,
		  gpointer   data )
{

  /* Kernel_T               *kernel         = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet          = (Sheet_T**)NULL;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");

  /*
   * Change the font of the current range.
   */
  gtk_sheet_range_set_font(GTK_SHEET((*sheet)->sheet),
			   &GTK_SHEET((*sheet)->sheet)->range,
			   GTK_FONT_COMBO((*generalToolbar)->fontComboBox)->font);

}

/*
 * Cell Left Justify Handler.
 */
static void
SheetLeftJustify ( GtkWidget *widget,
		   gpointer   data )
{

  /* Kernel_T               *kernel         = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet          = (Sheet_T**)NULL;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");

  /*
   * Update button aspect.
   */
  if (*generalToolbar != (SheetGeneralToolbar_T*)NULL) {

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->leftJustifiedButton),
				 GTK_STATE_ACTIVE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->rightJustifiedButton),
				 GTK_STATE_NORMAL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->centeredButton),
				 GTK_STATE_NORMAL);
  }

  gtk_sheet_range_set_justification(GTK_SHEET((*sheet)->sheet),
				    &GTK_SHEET((*sheet)->sheet)->range,
				    GTK_JUSTIFY_LEFT);

}

/*
 * Cell Centered Justify Handler.
 */
static void
SheetCenteredJustify ( GtkWidget *widget,
		       gpointer   data )
{

  /* Kernel_T               *kernel         = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet          = (Sheet_T**)NULL;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");

  /*
   * Update button aspect.
   */
  if (*generalToolbar != (SheetGeneralToolbar_T*)NULL) {

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->leftJustifiedButton),
				 GTK_STATE_NORMAL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->rightJustifiedButton),
				 GTK_STATE_NORMAL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->centeredButton),
				 GTK_STATE_ACTIVE);

  }

  gtk_sheet_range_set_justification(GTK_SHEET((*sheet)->sheet),
				    &GTK_SHEET((*sheet)->sheet)->range,
				    GTK_JUSTIFY_CENTER);

}

/*
 * Cell Right Justify Handler.
 */
static void
SheetRightJustify ( GtkWidget *widget,
		    gpointer   data )
{

  /* Kernel_T               *kernel         = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet          = (Sheet_T**)NULL;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");

  /*
   * Update button aspect.
   */
  if (*generalToolbar != (SheetGeneralToolbar_T*)NULL) {

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->leftJustifiedButton),
				 GTK_STATE_NORMAL);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->rightJustifiedButton),
				 GTK_STATE_ACTIVE);

    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON((*generalToolbar)->centeredButton),
				 GTK_STATE_NORMAL);

  }

  gtk_sheet_range_set_justification(GTK_SHEET((*sheet)->sheet),
				    &GTK_SHEET((*sheet)->sheet)->range,
				    GTK_JUSTIFY_RIGHT);

}

/*
 * General Entry Changed Handler.
 */
static void
SheetGeneralEntryChanged ( GtkWidget *widget,
			   gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  GtkEntry               *sheetEntry      = (GtkEntry*)NULL;
  char                   *text            = (gchar*)NULL;


  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  if (*generalToolbar == (SheetGeneralToolbar_T*)NULL) {
    return;
  }

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


  if (!GTK_WIDGET_HAS_FOCUS(widget)) {
    return;
  }

#if defined(SHEET_VERBOSE)
  printf("SheetGeneralEntryChanged\n");
#endif

  text = gtk_entry_get_text(GTK_ENTRY((*generalToolbar)->generalEntry));

  if (text != (gchar*)NULL) {

    sheetEntry = GTK_ENTRY(gtk_sheet_get_entry(GTK_SHEET((*sheet)->sheet)));

    gtk_entry_set_text(sheetEntry,text);

  }

}

/*
 * General entry Activated Handler.
 */
static void 
SheetGeneralEntryActivated ( GtkWidget *widget,
			     gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  GtkEntry               *sheetEntry      = (GtkEntry*)NULL;
  gint                    row             = 0;
  gint                    column          = 0;
  gint                    justification   = GTK_JUSTIFY_LEFT;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


#if defined(SHEET_VERBOSE)
  printf("SheetGeneralEntryActivated\n");
#endif
  
  row    = GTK_SHEET((*sheet)->sheet)->active_cell.row;
  column = GTK_SHEET((*sheet)->sheet)->active_cell.col;

  sheetEntry = GTK_ENTRY(gtk_sheet_get_entry(GTK_SHEET((*sheet)->sheet)));

  if (GTK_IS_ITEM_ENTRY(sheetEntry)) {
    justification = GTK_ITEM_ENTRY(sheetEntry)->justification;
  }

  gtk_sheet_set_cell(GTK_SHEET((*sheet)->sheet),
		     row,
		     column,
		     justification,
		     gtk_entry_get_text(sheetEntry));

}

/*
 * Cell Foreground Handler.
 */
static void 
SheetChangeForegroundColor ( GtkWidget *widget,
			     gint       i,
			     gchar     *colorName,
			     gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  GdkGC                  *tmpGc           = (GdkGC*)NULL;
  GdkColor                color;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


  /*
   * Allocate color.
   */
  gdk_color_parse(colorName,&color);
  gdk_color_alloc(gtk_widget_get_colormap(widget),&color);

  /*
   * Modify background.
   */
  gtk_sheet_range_set_foreground(GTK_SHEET((*sheet)->sheet),
				 &GTK_SHEET((*sheet)->sheet)->range,
				 &color);

  /*
   * Update button color in toolbar.
   */
  tmpGc = gdk_gc_new((*sheet)->sheet->window);
  gdk_gc_set_foreground(tmpGc,&color);
  gdk_draw_rectangle(GTK_PIXMAP((*generalToolbar)->fgPixmap)->pixmap,
		     tmpGc,
		     TRUE, /* filled */
		     5,    /* X      */
		     20,   /* Y      */
		     16,   /* width  */
		     4);   /* height */
  gtk_widget_draw((*generalToolbar)->bgPixmap,(GdkRectangle*)NULL);
  gdk_gc_unref(tmpGc);

}

/*
 * Cell Background Handler.
 */
static void 
SheetChangeBackgroundColor ( GtkWidget *widget,
			     gint       i,
			     gchar     *colorName,
			     gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  GdkGC                  *tmpGc           = (GdkGC*)NULL;
  GdkColor                color;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


  /*
   * Allocate color.
   */
  gdk_color_parse(colorName,&color);
  gdk_color_alloc(gtk_widget_get_colormap(widget),&color);

  /*
   * Modify background.
   */
  gtk_sheet_range_set_background(GTK_SHEET((*sheet)->sheet),
				 &GTK_SHEET((*sheet)->sheet)->range,
				 &color);

  /*
   * Update button color in toolbar.
   */
  tmpGc = gdk_gc_new((*sheet)->sheet->window);
  gdk_gc_set_foreground(tmpGc,&color);
  gdk_draw_rectangle(GTK_PIXMAP((*generalToolbar)->bgPixmap)->pixmap,
		     tmpGc,
		     TRUE, /* filled */
		     4,    /* X      */
		     20,   /* Y      */
		     18,   /* width  */
		     4);   /* height */
  gtk_widget_draw((*generalToolbar)->bgPixmap,(GdkRectangle*)NULL); 
  gdk_gc_unref(tmpGc);

}

/*
 * Cell Border Color Handler.
 */
static void 
SheetChangeBorderColor ( GtkWidget *widget,
			 gint       i,
			 gchar     *colorName,
			 gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  GdkGC                  *tmpGc           = (GdkGC*)NULL;
  GdkColor                color;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


  /*
   * Allocate color.
   */
  gdk_color_parse(colorName,&color);
  gdk_color_alloc(gtk_widget_get_colormap(widget),&color);

  /*
   * Modify border color.
   */
  gtk_sheet_range_set_border_color(GTK_SHEET((*sheet)->sheet),
				   &GTK_SHEET((*sheet)->sheet)->range,
				   &color);

  /*
   * Update button color in toolbar.
   */
  tmpGc = gdk_gc_new((*sheet)->sheet->window);
  gdk_gc_set_foreground(tmpGc,&color);
  gdk_draw_rectangle(GTK_PIXMAP((*generalToolbar)->bdPixmap)->pixmap,
		     tmpGc,
		     TRUE, /* filled */
		     4,    /* X      */
		     20,   /* Y      */
		     18,   /* width  */
		     4);   /* height */
  gtk_widget_draw((*generalToolbar)->bdPixmap,(GdkRectangle*)NULL); 
  gdk_gc_unref(tmpGc);

}

/*
 * Cell Border Handler.
 */
static void
SheetChangeBorder ( GtkWidget *widget,
		    gint       requestedBorder,
		    gpointer   data )
{

  /* Kernel_T               *kernel          = (Kernel_T*)data; */
  SheetGeneralToolbar_T **generalToolbar  = (SheetGeneralToolbar_T**)NULL;
  Sheet_T               **sheet           = (Sheet_T**)NULL;
  gint                    borderMask      = 0;
  gint                    allMask         = 0;
  gint                    borderLineWidth = 2;
  /* Line style is : GDK_LINE_SOLID, GDK_LINE_ON_OFF_DASH, GDK_LINE_DOUBLE_DASH */
  gint                    borderLineStyle = GDK_LINE_SOLID;
  gint                    auxCol          = 0;
  gint                    auxRow          = 0;
  gint                    i               = 0;
  gint                    j               = 0;
  GtkSheetRange           range;
  GtkSheetRange           auxRange;

  /*
   * Get back the general toolbar instance.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

  /*
   * Get back the current sheet.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "currentSheet");


  range = GTK_SHEET((*sheet)->sheet)->range;
  gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			     &range,
			     0,  /* border mask       */
			     0,  /* border line width */
			     0); /* border line style */

  switch (requestedBorder) {

  case 0:
    break;

  case 1:
    borderMask = GTK_SHEET_TOP_BORDER;
    range.rowi = range.row0;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 2:
    borderMask = GTK_SHEET_BOTTOM_BORDER;
    range.row0 = range.rowi;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 3:
    borderMask = GTK_SHEET_RIGHT_BORDER;
    range.col0 = range.coli;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 4:
    borderMask = GTK_SHEET_LEFT_BORDER;
    range.coli = range.col0;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 5:
    if (range.col0 == range.coli) {

      borderMask = GTK_SHEET_LEFT_BORDER | GTK_SHEET_RIGHT_BORDER;
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

    }
    else {

      borderMask = GTK_SHEET_LEFT_BORDER;
      auxCol     = range.coli;
      range.coli = range.col0;
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

      borderMask = GTK_SHEET_RIGHT_BORDER;
      range.col0 = range.coli = auxCol; 
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

    }
    break;

  case 6:

    if (range.row0 == range.rowi) {

      borderMask = GTK_SHEET_TOP_BORDER | GTK_SHEET_BOTTOM_BORDER;
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

    }
    else {

      borderMask = GTK_SHEET_TOP_BORDER;
      auxRow     = range.rowi;
      range.rowi = range.row0;
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

      borderMask = GTK_SHEET_BOTTOM_BORDER;
      range.row0 = range.rowi =auxRow; 
      gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				 &range,
				 borderMask,
				 borderLineWidth,
				 borderLineStyle);

    }
    break;

  case 7:

    borderMask = GTK_SHEET_RIGHT_BORDER | GTK_SHEET_LEFT_BORDER;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 8:

    borderMask = GTK_SHEET_BOTTOM_BORDER | GTK_SHEET_TOP_BORDER;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  case 9:

    allMask    = GTK_SHEET_BOTTOM_BORDER | GTK_SHEET_TOP_BORDER |
                 GTK_SHEET_LEFT_BORDER   | GTK_SHEET_RIGHT_BORDER;

    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       allMask,
			       borderLineWidth,
			       borderLineStyle);

    for (i=range.row0; i<=range.rowi; i++)
      for (j=range.col0; j<=range.coli; j++) {

	borderMask    = allMask;
	auxRange.row0 = auxRange.rowi = i;
	auxRange.col0 = auxRange.coli = j;
	if (i == range.rowi) {
	  borderMask ^= GTK_SHEET_BOTTOM_BORDER;
	}
	if (i == range.row0) {
	  borderMask ^= GTK_SHEET_TOP_BORDER;
	}
	if (j == range.coli) {
	  borderMask ^= GTK_SHEET_RIGHT_BORDER;
	}
	if (j == range.col0) {
	  borderMask ^= GTK_SHEET_LEFT_BORDER;
	}
	if (borderMask != allMask) {
	  gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				     &auxRange,
				     borderMask, 
				     borderLineWidth,
				     borderLineStyle);
	}

      }
    break;

  case 10:

    for (i=range.row0; i<=range.rowi; i++)
      for (j=range.col0; j<=range.coli; j++) {

	borderMask    = 0;
	auxRange.row0 = auxRange.rowi = i;
	auxRange.col0 = auxRange.coli = j;

	if (i == range.rowi) {
	  borderMask |= GTK_SHEET_BOTTOM_BORDER;
	}
	if (i == range.row0) {
	  borderMask |= GTK_SHEET_TOP_BORDER;
	}
	if (j == range.coli) {
	  borderMask |= GTK_SHEET_RIGHT_BORDER;
	}
	if (j == range.col0) {
	  borderMask |= GTK_SHEET_LEFT_BORDER;
	}
	if (borderMask != 0) {
	  gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
				     &auxRange,
				     borderMask, 
				     borderLineWidth,
				     borderLineStyle);
	}

      }
    break;

  case 11:

    borderMask = GTK_SHEET_BOTTOM_BORDER | GTK_SHEET_TOP_BORDER |
                 GTK_SHEET_LEFT_BORDER   | GTK_SHEET_RIGHT_BORDER;
    gtk_sheet_range_set_border(GTK_SHEET((*sheet)->sheet),
			       &range,
			       borderMask,
			       borderLineWidth,
			       borderLineStyle);
    break;

  }

}

/*
 * =============================================================
                         MAIN SHEET HANDLERS
 * =============================================================
 */

/*
 * Cell Entry Changed Handler.
 */
static void 
SheetEntryChanged ( GtkWidget *widget,
		    gpointer   data)
{

  /* Kernel_T               *kernel         = (Kernel_T*)data; */
  Sheet_T                *sheet          = (Sheet_T*)NULL;
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  gchar                  *text           = (gchar*)NULL; 
  gint                    row            = 0;
  gint                    column         = 0;
  GtkWidget              *sheetEntry     = (GtkWidget*)NULL;

  if (!GTK_WIDGET_HAS_FOCUS(widget)) {
    return;
  }

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			      "plotPointer");

  row    = GTK_SHEET(sheet->sheet)->active_cell.row;
  column = GTK_SHEET(sheet->sheet)->active_cell.col;

#if defined(SHEET_VERBOSE_)
  printf("sheet_entry_changed signal : (%d,%d) changed.\n",row,column);
#endif

  sheetEntry = gtk_sheet_get_entry(GTK_SHEET(sheet->sheet));

  text = gtk_entry_get_text(GTK_ENTRY(sheetEntry));

  if (text != (gchar*)NULL) {

    generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
					 "generalToolbar");

    if ((*generalToolbar) != (SheetGeneralToolbar_T*)NULL &&
	(*generalToolbar)->generalEntry != (GtkWidget*)NULL) {

      gtk_entry_set_text(GTK_ENTRY((*generalToolbar)->generalEntry),text);

    }

  }

}

/*
 * Cell Changed Handler.
 */
static gboolean
SheetChanged ( GtkWidget *widget,
	       gint       row,
	       gint       column,
	       gpointer   data ) 
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("changed signal : (%d,%d) changed (%s).\n",
	 row,
	 column,
	 gtk_sheet_cell_get_text(GTK_SHEET(sheet->sheet),
				 row,
				 column));
#endif

  return TRUE;

}

/*
 * Cell Activated Handler.
 */
static gboolean
ActivateSheetCell ( GtkWidget *widget,
		    gint       row,
		    gint       column,
		    gpointer   data ) 
{

  Kernel_T               *kernel         = (Kernel_T*)data;
  Sheet_T                *sheet          = (Sheet_T*)NULL;
  SheetGeneralToolbar_T **generalToolbar = (SheetGeneralToolbar_T**)NULL;
  GtkWidget              *sheetEntry     = (GtkWidget*)NULL;
  gchar                  *text           = (gchar*)NULL;
  gchar                   string[100];
  GtkSheetCellAttr        attributes;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

  /*
   * Get back general toolbar address.
   */
  generalToolbar = gtk_object_get_data(GTK_OBJECT(widget),
				       "generalToolbar");

#if defined(SHEET_VERBOSE)
  printf("activate signal : (%d,%d) activated (%s).\n",
	 row,
	 column,
	 gtk_sheet_cell_get_text(GTK_SHEET(sheet->sheet),
				 row,
				 column));
#endif

  /*
   * Write the right location.
   */

  if (GTK_SHEET(widget)->column[column].name != (gchar*)NULL) {

    sprintf(string,
	    "  %s:%d  ",
	    GTK_SHEET(widget)->column[column].name,
	    row);

  }
  else {

    sprintf(string,
	    " Row %d, Column %d ",
	    row,
	    column);

  }

  if ((*generalToolbar) != (SheetGeneralToolbar_T*)NULL &&
      (*generalToolbar)->location != (GtkWidget*)NULL) {

    gtk_label_set(GTK_LABEL((*generalToolbar)->location),
		  string);

  }

  /*
   * Update general entry if exists.
   */
  sheetEntry = gtk_sheet_get_entry(GTK_SHEET(sheet->sheet));
  text       = gtk_entry_get_text(GTK_ENTRY(sheetEntry));

  if ((*generalToolbar) != (SheetGeneralToolbar_T*)NULL &&
      (*generalToolbar)->generalEntry != (GtkWidget*)NULL) {

    if (text != (gchar*)NULL) {
      gtk_entry_set_text(GTK_ENTRY((*generalToolbar)->generalEntry), text);
    }
    else {
      gtk_entry_set_text(GTK_ENTRY((*generalToolbar)->generalEntry), "");
    }

  }

  /*
   * Get back current cell attributes.
   */
  gtk_sheet_get_attributes(GTK_SHEET(sheet->sheet),
			   GTK_SHEET(sheet->sheet)->active_cell.row, /* row    */
			   GTK_SHEET(sheet->sheet)->active_cell.col, /* column */
			   &attributes);

  /*
   * Update general entry sensitivity.
   */
  if ((*generalToolbar) != (SheetGeneralToolbar_T*)NULL &&
      (*generalToolbar)->generalEntry != (GtkWidget*)NULL) {

    gtk_entry_set_editable(GTK_ENTRY((*generalToolbar)->generalEntry),
			   attributes.is_editable);

  }

  /*
   * Update justification widgets.
   */
  switch (attributes.justification){

  case GTK_JUSTIFY_CENTER:
    SheetCenteredJustify(GTK_WIDGET(sheet->sheet),
			 (gpointer)kernel);
    break;

  case GTK_JUSTIFY_RIGHT:
    SheetRightJustify(GTK_WIDGET(sheet->sheet),
		      (gpointer)kernel);
    break;

  case GTK_JUSTIFY_LEFT:
  default:
    SheetLeftJustify(GTK_WIDGET(sheet->sheet),
		     (gpointer)kernel);
    break;

  }

  return TRUE;

}

/*
 * Cell De-Activated Handler.
 */
static gboolean
DeActivateSheetCell ( GtkWidget *widget,
		      gint       row,
		      gint       column,
		      gpointer   data ) 
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("deactivate signal : (%d,%d) de-activated (%s).\n",
	 row,
	 column,
	 gtk_sheet_cell_get_text(GTK_SHEET(sheet->sheet),
				 row,
				 column));
#endif

  return TRUE;

}

/*
 * Cell Set Handler.
 */
static void
SetSheetCell ( GtkWidget *widget,
	       gint       row,
	       gint       column,
	       gpointer   data ) 
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("set-cell signal : (%d,%d) cell set (%s).\n",
	 row,
	 column,
	 gtk_sheet_cell_get_text(GTK_SHEET(sheet->sheet),
				 row,
				 column));
#endif

}

/*
 * Cell Clear Handler.
 */
static void
ClearSheetCell ( GtkWidget *widget,
		 gint       row,
		 gint       column,
		 gpointer   data ) 
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("clear-cell signal : (%d,%d) cell cleared.\n",row,column);
#endif

}

/*
 * Range Traverse Handler.
 */
static gboolean
TraverseSheetCells ( GtkWidget *widget,
		     gint       row,
		     gint       column,
		     gint      *newRow,
		     gint      *newColumn,
		     gpointer   data ) 
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("traverse signal : from (%d,%d) to (%d,%d).\n",
	 row,
	 column,
	 *newRow,
	 *newColumn);
#endif

  return TRUE;

}

/*
 * Paste Operation.
 */
static void
SheetPasteRange ( Sheet_T *plot )
{

  GtkSheet *sheet       = GTK_SHEET(plot->sheet);
  gint      startRow    = 0;
  gint      startColumn = 0;
  gint      row         = 0;
  gint      column      = 0;
  gint      nbRows      = 0;
  gint      nbColumns   = 0;

  /*
   * Get back active starting cell.
   */
  startRow    = sheet->active_cell.row;
  startColumn = sheet->active_cell.col;

  nbRows    = sheet->clip_range.rowi - sheet->clip_range.row0;
  nbColumns = sheet->clip_range.coli - sheet->clip_range.col0;

  for (row=0; row<=nbRows; row++) {

    for (column=0; column<=nbColumns; column++) {

      gtk_sheet_set_cell_text(sheet,
			      startRow    + row,
			      startColumn + column,
			      gtk_sheet_cell_get_text(sheet,
						      sheet->clip_range.row0 + row,
						      sheet->clip_range.col0 + column));

    }

  }

}

/*
 * CLip Board Handler.
 */
static void
ClipBoardHandler ( GtkWidget   *widget,
		   GdkEventKey *key,
		   gpointer     data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;
  GtkSheet *_sheet = (GtkSheet*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

  _sheet = GTK_SHEET(sheet->sheet);

  if (key->state   & GDK_CONTROL_MASK ||
      key->keyval == GDK_Control_L    ||
      key->keyval == GDK_Control_R        ) {

    if ((key->keyval == 'c' || key->keyval == 'C') &&
	_sheet->state != GTK_STATE_NORMAL) {

      if (GTK_SHEET_IN_CLIP(_sheet)) {
	gtk_sheet_unclip_range(_sheet);
      }

      gtk_sheet_clip_range(_sheet,&_sheet->range);
      /* gtk_sheet_unselect_range(_sheet); */

#if defined(SHEET_VERBOSE)
      printf("control-C\n");
#endif

    }

    if (key->keyval =='x' || key->keyval == 'X') {

      gtk_sheet_unclip_range(_sheet);    

#if defined(SHEET_VERBOSE)
      printf("control-X\n");
#endif

    }

    if (key->keyval =='v' || key->keyval == 'V') {

      SheetPasteRange(sheet);    

#if defined(SHEET_VERBOSE)
      printf("control-V\n");
#endif

    }

  }
  else {

#if defined(SHEET_VERBOSE)
    printf("key-pressed : %d\n",key->keyval);
#endif

    switch (key->keyval) {

    case GDK_Delete :
      /*
       * Delete current range of cells.
       */
      gtk_sheet_range_clear(GTK_SHEET(sheet->sheet),
			    &GTK_SHEET(sheet->sheet)->range);
      break;

    default :
      break;

    }

  }

}

/*
 * Range Resize Handler.
 */
static void
ResizeSheetRange ( GtkWidget     *widget,
		   GtkSheetRange *oldRange,
		   GtkSheetRange *newRange,
		   gpointer       data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("resize_range signal : from (%d,%d:%d,%d) to (%d,%d:%d,%d)\n",
	 oldRange->row0,
	 oldRange->col0,
	 oldRange->rowi,
	 oldRange->coli,
	 newRange->row0,
	 newRange->col0,
	 newRange->rowi,
	 newRange->coli);
#endif

}

/*
 * Move Range Handler.
 */
static void
MoveSheetRange ( GtkWidget     *widget,
		 GtkSheetRange *oldRange,
		 GtkSheetRange *newRange,
		 gpointer       data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("move_range signal : from (%d,%d:%d,%d) to (%d,%d:%d,%d)\n",
	 oldRange->row0,
	 oldRange->col0,
	 oldRange->rowi,
	 oldRange->coli,
	 newRange->row0,
	 newRange->col0,
	 newRange->rowi,
	 newRange->coli);
#endif

}

/*
 * Clip Range Handler.
 */
static void
ClipSheetRange ( GtkWidget     *widget,
		 GtkSheetRange *clipRange,
		 gpointer       data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("clip_range signal : from (%d,%d) to (%d,%d)\n",
	 clipRange->row0,
	 clipRange->col0,
	 clipRange->rowi,
	 clipRange->coli);
#endif

}

/*
 * Select Range Handler.
 */
static void
SelectSheetRange ( GtkWidget     *widget,
		   GtkSheetRange *range,
		   gpointer       data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("select_range signal : from (%d,%d) to (%d,%d)\n",
	 range->row0,
	 range->col0,
	 range->rowi,
	 range->coli);
#endif

}

/*
 * Select Row Handler.
 */
static void
SelectSheetRow ( GtkWidget *widget,
		 gint       row,
		 gpointer   data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("select_row signal : row %d\n",
	 row);
#endif

}

/*
 * Select Column Handler.
 */
static void
SelectSheetColumn ( GtkWidget *widget,
		    gint       column,
		    gpointer   data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("select_column signal : column %d\n",
	 column);
#endif

}

/*
 * New Column Width Handler.
 */
static void
SheetNewColumnWidth ( GtkWidget *widget,
		      gint       column,
		      guint      width,
		      gpointer   data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("new_column_width signal : column %d is now %d\n",
	 column,
	 width);
#endif

}

/*
 * New Row Height Handler.
 */
static void
SheetNewRowHeight ( GtkWidget *widget,
		    gint       row,
		    guint      height,
		    gpointer   data )
{

  /* Kernel_T *kernel = (Kernel_T*)data; */
  Sheet_T  *sheet  = (Sheet_T*)NULL;

  /*
   * Get back the plot instance.
   */
  sheet = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

#if defined(SHEET_VERBOSE)
  printf("new_row_height signal : row %d is now %d\n",
	 row,
	 height);
#endif

}

/*
 * Create the general toolbar.
 */
static void
gdisp_createSheetGeneralToolBar ( Kernel_T *kernel )
{

  SheetGeneralToolbar_T *generalToolbar = (SheetGeneralToolbar_T*)NULL;
  GtkWidget             *localHBox      = (GtkWidget*)NULL;
  GtkWidget             *localVBox      = (GtkWidget*)NULL;
  GdkPixmap             *pixmap         = (GdkPixmap*)NULL;
  GdkBitmap             *mask           = (GdkBitmap*)NULL;
  GtkWidget             *bPixmap        = (GtkWidget*)NULL;
  GtkRequisition         request; 


  /* -------------------- ALREADY EXISTS ? --------------------- */

  if (SheetGeneralToolbar != (SheetGeneralToolbar_T*)NULL) {

    gtk_widget_destroy(SheetGeneralToolbar->topLevelWindow);
    return;

  }

  /* --------------------- MEMORY ALLOCATION ------------------- */

  generalToolbar = g_malloc0(sizeof(SheetGeneralToolbar_T));
  assert(generalToolbar);

  SheetGeneralToolbar = generalToolbar;

  /* ---------------------- TOP LEVEL WINDOW --------------------*/

  /*
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  generalToolbar->topLevelWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->topLevelWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdisp_manageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->topLevelWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdisp_destroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window title and border width.
   */
  gtk_window_set_title(GTK_WINDOW(generalToolbar->topLevelWindow),
		       "Spreadsheet General Toolbar");

  gtk_container_set_border_width(GTK_CONTAINER(generalToolbar->topLevelWindow),
				 1);

  /* ------------------------- VERTICAL BOX -----------------------*/

  /*
   * We need a vertical packing box for managing all widgets.
   */
  localVBox = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(localVBox),3);
  gtk_container_add(GTK_CONTAINER(generalToolbar->topLevelWindow),localVBox);
  gtk_widget_show(localVBox);

  /* ---------------------- GENERAL TOOLBAR ---------------------*/

  generalToolbar->toolBar = gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
					    GTK_TOOLBAR_ICONS);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* FONT management */

  generalToolbar->fontComboBox = gtk_font_combo_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->fontComboBox,
			    "Change the font of the selected cell(s).",
			    "");

  gtk_widget_set_usize(GTK_FONT_COMBO(generalToolbar->fontComboBox)->italic_button,
		       32,  /* width  */
		       32); /* height */

  gtk_widget_set_usize(GTK_FONT_COMBO(generalToolbar->fontComboBox)->bold_button,
		       32,  /* width  */
		       32); /* height */

  gtk_widget_show(generalToolbar->fontComboBox);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->fontComboBox),
		     "changed",
		     (GtkSignalFunc)SheetChangeFont,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->fontComboBox),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->fontComboBox),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* LEFT Justified Button */

  generalToolbar->leftJustifiedButton = gtk_toggle_button_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->leftJustifiedButton,
			    "Selected cell text is left justified.",
			    "");

  gtk_widget_show(generalToolbar->leftJustifiedButton);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetLeftJustifiedPixmap);
  bPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(generalToolbar->leftJustifiedButton),
		    bPixmap);
  gtk_widget_show(bPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->leftJustifiedButton),
		     "released",
		     (GtkSignalFunc)SheetLeftJustify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->leftJustifiedButton),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->leftJustifiedButton),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  /* CENTERED Button */

  generalToolbar->centeredButton = gtk_toggle_button_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->centeredButton,
			    "Selected cell text is centered.",
			    "");

  gtk_widget_show(generalToolbar->centeredButton);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetCenterJustifiedPixmap);
  bPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(generalToolbar->centeredButton),
		    bPixmap);
  gtk_widget_show(bPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->centeredButton),
		     "released",
		     (GtkSignalFunc)SheetCenteredJustify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->centeredButton),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->centeredButton),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  /* RIGHT Justified Button */

  generalToolbar->rightJustifiedButton = gtk_toggle_button_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->rightJustifiedButton,
			    "Selected cell text is right justified.",
			    "");

  gtk_widget_show(generalToolbar->rightJustifiedButton);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetRightJustifiedPixmap);
  bPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(generalToolbar->rightJustifiedButton),
		    bPixmap);
  gtk_widget_show(bPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->rightJustifiedButton),
		     "released",
		     (GtkSignalFunc)SheetRightJustify,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->rightJustifiedButton),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->rightJustifiedButton),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* BORDER Management */

  generalToolbar->borderComboBox = gtk_border_combo_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->borderComboBox,
			    "Change the border of the selected cell(s).",
			    "");

  gtk_widget_set_usize(GTK_COMBO_BOX(generalToolbar->borderComboBox)->button,
		       32,  /* width  */
		       32); /* height */

  gtk_widget_show(generalToolbar->borderComboBox);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->borderComboBox),
		     "changed",
		     (GtkSignalFunc)SheetChangeBorder,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->borderComboBox),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->borderComboBox),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* BORDER COLOR Management */

  generalToolbar->bdColorComboBox = gtk_color_combo_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->bdColorComboBox,
			    "Change the border color of the selected cell(s).",
			    "");

  gtk_widget_show(generalToolbar->bdColorComboBox);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetLinePixmap);
  generalToolbar->bdPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO_BOX(generalToolbar->bdColorComboBox)->button),
		    generalToolbar->bdPixmap);
  gtk_widget_show(generalToolbar->bdPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->bdColorComboBox),
		     "changed",
		     (GtkSignalFunc)SheetChangeBorderColor,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->bdColorComboBox),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->bdColorComboBox),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* BG COLOR Management */

  generalToolbar->bgColorComboBox = gtk_color_combo_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->bgColorComboBox,
			    "Change the background color of the selected cell(s).",
			    "");

  gtk_widget_show(generalToolbar->bgColorComboBox);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetPaintPixmap);
  generalToolbar->bgPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO_BOX(generalToolbar->bgColorComboBox)->button),
		    generalToolbar->bgPixmap);
  gtk_widget_show(generalToolbar->bgPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->bgColorComboBox),
		     "changed",
		     (GtkSignalFunc)SheetChangeBackgroundColor,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->bgColorComboBox),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->bgColorComboBox),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  /* FG COLOR Management */

  generalToolbar->fgColorComboBox = gtk_color_combo_new();

  gtk_toolbar_append_widget(GTK_TOOLBAR(generalToolbar->toolBar),
			    generalToolbar->fgColorComboBox,
			    "Change the font of the selected cell(s).",
			    "");

  gtk_widget_show(generalToolbar->fgColorComboBox);

  /* ------------------------------------------------------- */
  pixmap = gdk_pixmap_colormap_create_from_xpm_d((GdkWindow*)NULL,
						 kernel->colormap,
						 &mask,
						 (GdkColor*)NULL,
						 SheetFontPixmap);
  generalToolbar->fgPixmap = gtk_pixmap_new(pixmap,mask);
  gtk_container_add(GTK_CONTAINER(GTK_COMBO_BOX(generalToolbar->fgColorComboBox)->button),
		    generalToolbar->fgPixmap);
  gtk_widget_show(generalToolbar->fgPixmap);
  /* ------------------------------------------------------- */

  gtk_signal_connect(GTK_OBJECT(generalToolbar->fgColorComboBox),
		     "changed",
		     (GtkSignalFunc)SheetChangeForegroundColor,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->fgColorComboBox),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->fgColorComboBox),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_toolbar_append_space(GTK_TOOLBAR(generalToolbar->toolBar));

  gtk_box_pack_start(GTK_BOX(localVBox),
		     generalToolbar->toolBar,FALSE,TRUE,1);

  gtk_widget_show(generalToolbar->toolBar);

  /* ----------------------- FIRST HORIZONTAL BOX ------------------------*/

  localHBox = gtk_hbox_new(FALSE, 1);
  gtk_container_set_border_width(GTK_CONTAINER(localHBox),0);
  gtk_box_pack_start(GTK_BOX(localVBox),
		     localHBox,FALSE,TRUE,1);
  gtk_widget_show(localHBox);

  /* ------------------- LABEL FOR CURRENT CELL -------------------- */

  generalToolbar->location = gtk_label_new(""); 
  gtk_widget_size_request(generalToolbar->location,&request); 
  gtk_widget_set_usize(generalToolbar->location,160,request.height);
  gtk_box_pack_start(GTK_BOX(localHBox),generalToolbar->location,FALSE,TRUE,0);
  gtk_widget_show(generalToolbar->location);

  /* ------------------------- MAIN ENTRY --------------------------- */

  generalToolbar->generalEntry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(localHBox),generalToolbar->generalEntry,TRUE,TRUE,0);
  gtk_widget_show(generalToolbar->generalEntry);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->generalEntry),
		      "changed",
		     (GtkSignalFunc)SheetGeneralEntryChanged,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(generalToolbar->generalEntry),
		     "activate",
		     (GtkSignalFunc)SheetGeneralEntryActivated,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->generalEntry),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(generalToolbar->generalEntry),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  /*
   * Show general toolbar.
   */
  gtk_widget_show(generalToolbar->topLevelWindow);

}

/*
 * Create a plot by providing an opaque structure to the caller.
 * This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_createSheet (Kernel_T *kernel)
{

  Sheet_T *plot = (Sheet_T*)NULL;

  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(Sheet_T));
  assert(plot);

  /*
   * ========
   */
  SheetCurrentSheet = plot; /*** FIXME ***/
  /*
   * ========
   */

  /* ----------------------- MAIN VERTICAL BOX ------------------------*/

  plot->topLevelWidget = gtk_vbox_new(FALSE,1);
  gtk_container_set_border_width(GTK_CONTAINER(plot->topLevelWidget),1);

  /* ------------------------ MAIN SHEET OBJECT ------------------------ */

  plot->scrolledWindow = gtk_scrolled_window_new(NULL,NULL);
  gtk_box_pack_start(GTK_BOX(plot->topLevelWidget),
		     plot->scrolledWindow,
		     TRUE,
		     TRUE,
		     0);
  gtk_widget_show(plot->scrolledWindow);

  plot->sheet = gtk_sheet_new(GD_DEFAULT_MAX_ROWS,
			      GD_DEFAULT_MAX_COLUMNS,
			      "Title");

  gtk_container_add(GTK_CONTAINER(plot->scrolledWindow),plot->sheet);
  gtk_widget_show(plot->sheet);

  gtk_signal_connect(GTK_OBJECT(gtk_sheet_get_entry(GTK_SHEET(plot->sheet))),
		     "changed",
		     (GtkSignalFunc)SheetEntryChanged,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(gtk_sheet_get_entry(GTK_SHEET(plot->sheet))),
		      "plotPointer",
		      (gpointer)plot);

  gtk_object_set_data(GTK_OBJECT(gtk_sheet_get_entry(GTK_SHEET(plot->sheet))),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(gtk_sheet_get_entry(GTK_SHEET(plot->sheet))),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "changed",
		     (GtkSignalFunc)SheetChanged,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "set_cell",
		     (GtkSignalFunc)SetSheetCell,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "clear_cell",
		     (GtkSignalFunc)ClearSheetCell,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "activate",
		     (GtkSignalFunc)ActivateSheetCell,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "deactivate",
		     (GtkSignalFunc)DeActivateSheetCell,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "select_row",
		     (GtkSignalFunc)SelectSheetRow, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "select_column",
		     (GtkSignalFunc)SelectSheetColumn,
		     (gpointer)kernel);

  if (0) { /* DO NOT NEED THESE SIGNALS BY NOW */

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "traverse",
		     (GtkSignalFunc)TraverseSheetCells, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "select_range",
		     (GtkSignalFunc)SelectSheetRange, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "resize_range",
		     (GtkSignalFunc)ResizeSheetRange, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "move_range",
		     (GtkSignalFunc)MoveSheetRange, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "clip_range",
		     (GtkSignalFunc)ClipSheetRange, 
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "new_column_width",
		     (GtkSignalFunc)SheetNewColumnWidth,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "new_row_height",
		     (GtkSignalFunc)SheetNewRowHeight,
		     (gpointer)kernel);

  } /* if (0) */

  gtk_signal_connect(GTK_OBJECT(plot->sheet),
		     "key_press_event",
		     (GtkSignalFunc)ClipBoardHandler,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(plot->sheet),
		      "plotPointer",
		      (gpointer)plot);

  gtk_object_set_data(GTK_OBJECT(plot->sheet),
		      "generalToolbar",
		      (gpointer)&SheetGeneralToolbar);

  gtk_object_set_data(GTK_OBJECT(plot->sheet),
		      "currentSheet",
		      (gpointer)&SheetCurrentSheet);

  /*
   * Rename columns and rows.
   */
  SheetRenameColumnsAndRows(kernel,plot);

  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a plot opaque structure.
 */
static void
gdisp_destroySheet(Kernel_T *kernel,
		   void     *data)
{

  Sheet_T *plot = (Sheet_T*)data;

  /*
   * Now destroy everything.
   */
  gtk_widget_destroy(plot->topLevelWidget);


  /*
   * Destroy Menu.
   */
  gdisp_destroyMenu(plot->popupMenu);


  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(Sheet_T));
  g_free(plot);

}


/*
 * Record parent widget.
 */
static void
gdisp_setSheetParentWidget (Kernel_T  *kernel,
			    void      *data,
			    GtkWidget *parent)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Store parent widget.
   */

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setSheetInitialDimensions (Kernel_T *kernel,
				 void     *data,
				 guint     width,
				 guint     height)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Remeber here initial dimensions of the viewport.
   */

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_getSheetTopLevelWidget (Kernel_T  *kernel,
			      void      *data)
{

  Sheet_T *plot = (Sheet_T*)data;

  return (GtkWidget*)plot->topLevelWidget;

}


/*
 * By now, the plot widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showSheet (Kernel_T  *kernel,
		 void      *data)
{

  Sheet_T *plot = (Sheet_T*)data;

  /*
   * Now show everything.
   */
  gtk_widget_show(plot->topLevelWidget);

  /* ------------------- RIGHT CLICK POPUP MENU -------------------- */

  SheetCreatePopupMenu(kernel,plot,plot->sheet);

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getSheetType (Kernel_T *kernel)
{

  /*
   * Return the type of the plot.
   */
  return GD_PLOT_SHEET;

}


/*
 * Record any incoming symbols.
 */
static void
gdisp_addSymbolsToSheet (Kernel_T *kernel,
			 void     *data,
			 GList    *symbolList,
			 guchar    zoneId)
{

  Sheet_T         *plot       = (Sheet_T*)data;
  gint             xPos       = 0;
  gint             yPos       = 0;
  gint             row        = 0;
  gint             column     = 0;
  GdkModifierType  modifier   = 0;
  GList           *symbolItem = (GList*)NULL;
  Symbol_T        *symbol     = (Symbol_T*)NULL;

  /*
   * Record here all incoming symbols.
   */
  gdk_window_get_pointer(GTK_SHEET(plot->sheet)->sheet_window,
			 &xPos,
			 &yPos,
			 &modifier);

  gtk_sheet_get_pixel_info(GTK_SHEET(plot->sheet),
			   xPos,
			   yPos,
			   &row,
			   &column);

  /*
   * Only one symbol per cell.
   * Take the first symbol of the incoming list. DO NOT LOOP.
   */
  symbolItem = g_list_first(symbolList);
  if (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;
    symbol->sReference++;

    gtk_sheet_set_cell_text(GTK_SHEET(plot->sheet),
			    row,
			    column,
			    symbol->sInfo.name);

    gtk_sheet_set_active_cell(GTK_SHEET(plot->sheet),
			      row,
			      column);

  }


}


/*
 * Broadcast all recorded symbols.
 */
static GList*
gdisp_getSymbolsFromSheet (Kernel_T *kernel,
			   void     *data,
			   gchar     axis)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Return the list of symbols.
   */
  return (GList*)NULL;

}


/*
 * Get plot attributes in order to be saved into the configuration.
 */
#if defined(GDISP_SHEET_RESERVED_FOR_THE_FUTURE)

static void
gdisp_getPlotAttributesSheet (Kernel_T *kernel,
			      void     *data,
			      GList    *attributeList)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Nothing by now.
   */

}

#endif

/*
 * Set plot attributes from the configuration.
 */
static void
gdisp_setPlotAttributesSheet (Kernel_T *kernel,
			      void     *data,
			      GList    *attributeList)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Nothing by now.
   */

}


/*
 * Get symbol attributes in order to be saved into the configuration.
 */
static void
gdisp_getSymbolAttributesSheet (Kernel_T *kernel,
				void     *data,
				Symbol_T *symbol,
				GList    *attributeList)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Return all attributes of a symbol.
   */

}

/*
 * Set symbol attributes from the configuration.
 */
static void
gdisp_setSymbolAttributesSheet (Kernel_T *kernel,
				void     *data,
				Symbol_T *symbol,
				GList    *attributeList)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Store all attributes of a symbol.
   */

}

/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOnSheet (Kernel_T *kernel,
			void     *data)
{

  /*
   * Do anything you want before starting steps.
   *
   * BUT we must return TRUE to the calling procedure in order to allow
   * the general step management to proceed.
   *
   * Returning FALSE means that our plot is not enabled to perform its
   * step operations, because of this or that...
   */
  return TRUE;

}


/*
 * Real time Step Action.
 */
static void
gdisp_stepOnSheet (Kernel_T *kernel,
		   void     *data)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Do anything you want to perform steps.
   */
}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOnSheet (Kernel_T *kernel,
		       void     *data)
{

  /* Sheet_T *plot = (Sheet_T*)data; */

  /*
   * Do anything you want when steps are stopped.
   */

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getSheetInformation (Kernel_T         *kernel,
			   PlotSystemInfo_T *information)
{

#include "pixmaps/gdisp_sheetLogo.xpm"

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose,
   *   - Logo.
   */
  information->psName        = "Spreadsheet";
  information->psFormula     = "Matrix of cells";
  information->psDescription = "Fully operational spreadsheet";
  information->psLogo        = gdisp_sheetLogo;

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_getSheetPeriod (Kernel_T         *kernel,
		      void             *data)
{

  /*
   * My period is 1000 milli-seconds.
   */
  return 1000;

}


/*
 * This procedure is called whenever all symbols have been time-tagged
 * by the corresponding provider sampling thread.
 * The last value of all symbols can now be retreived by the graphic plot.
 *
 * CAUTION : This procedure is called in another thread, compared to all
 * other procedures of the graphic plot that are called by GTK main thread.
 */
static void
gdisp_treatSheetSymbolValues (Kernel_T *kernel,
			      void     *data)
{

  /*
   * Take into account all last values.
   */

}


/*
 * Get back the zones that have been defined on that plot.
 */
static GArray*
gdisp_getSheetDropZones (Kernel_T *kernel)
{

  /*
   * No zones on that plot.
   */
  return (GArray*)NULL;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


#if defined(GD_DYNAMIC_GRAPHIC_MODULES)

void
gdisp_initGraphicSystem (Kernel_T     *kernel,
			 PlotSystem_T *plotSystem)

#else

void
gdisp_initPlotSheetSystem (Kernel_T     *kernel,
			   PlotSystem_T *plotSystem)

#endif

{

  /*
   * We must here provide all plot private functions that remain
   * 'static' here, but accessible from everywhere via the kernel.
   */
  plotSystem->psCreate              = gdisp_createSheet;
  plotSystem->psDestroy             = gdisp_destroySheet;
  plotSystem->psSetParent           = gdisp_setSheetParentWidget;
  plotSystem->psGetTopLevelWidget   = gdisp_getSheetTopLevelWidget;
  plotSystem->psSetDimensions       = gdisp_setSheetInitialDimensions;
  plotSystem->psShow                = gdisp_showSheet;
  plotSystem->psGetType             = gdisp_getSheetType;
  plotSystem->psAddSymbols          = gdisp_addSymbolsToSheet;
  plotSystem->psGetSymbols          = gdisp_getSymbolsFromSheet;
  plotSystem->psGetSymbolAttributes = gdisp_getSymbolAttributesSheet;
  plotSystem->psSetSymbolAttributes = gdisp_setSymbolAttributesSheet;
  plotSystem->psStartStep           = gdisp_startStepOnSheet;
  plotSystem->psStep                = gdisp_stepOnSheet;
  plotSystem->psStopStep            = gdisp_stopStepOnSheet;
  plotSystem->psGetInformation      = gdisp_getSheetInformation;
  plotSystem->psTreatSymbolValues   = gdisp_treatSheetSymbolValues;
  plotSystem->psGetPeriod           = gdisp_getSheetPeriod;
  plotSystem->psGetDropZones        = gdisp_getSheetDropZones;
  plotSystem->psSetPlotAttributes   = gdisp_setPlotAttributesSheet;
  /* plotSystem->psGetPlotAttributes   = gdisp_getPlotAttributesSheet; */
 
}
