/*

$Id: gdisp_symbols.c,v 1.11 2006-07-30 20:25:58 esteban Exp $

-----------------------------------------------------------------------

GDISP+ - Graphic Tool based on GTK+,
         for being used upon the generic Transport Sampling Protocol.

Copyright (c) 2004 - Euskadi.

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

File      : Information / Actions upon available symbols.

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
#include "gdisp_dragAndDrop.h"
#include "gdisp_popupMenu.h"


/*
 * Type definition for column management.
 */
typedef enum {

  GD_COL_Type = 0,
  GD_COL_Name,
  GD_COL_Dim,
  GD_COL_ExtInfo,
  GD_COL_Provider,
  GD_COL_LAST

} ColumnDef_T;

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Sort symbol list according to sorting method.
 */
static void
gdisp_sortSymbolList (Kernel_T *kernel)
{

  ColumnDef_T sortingColumn = GD_COL_Name;

  /*
   * Check out the column to be used for sorting action.
   */
  if (kernel->sortingMethod == GD_SORT_BY_TYPE) {
    sortingColumn = GD_COL_Type;
  }
  else if (kernel->sortingMethod == GD_SORT_BY_NAME) {
    sortingColumn = GD_COL_Name;
  }
  else if (kernel->sortingMethod == GD_SORT_BY_DIM) {
    sortingColumn = GD_COL_Dim;
  }
  else if (kernel->sortingMethod == GD_SORT_BY_EXTINFO) {
    sortingColumn = GD_COL_ExtInfo;
  }
  else if (kernel->sortingMethod == GD_SORT_BY_PROVIDER) {
    sortingColumn = GD_COL_Provider;
  }
  else {
    sortingColumn = GD_COL_Name;
  }

  gtk_clist_set_sort_column(GTK_CLIST(kernel->widgets.symbolCList),
			    sortingColumn);

  gtk_clist_set_sort_type(GTK_CLIST(kernel->widgets.symbolCList),
			  kernel->sortingDirection == GD_SORT_ASCENDING ?
			  GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);

  gtk_clist_sort(GTK_CLIST(kernel->widgets.symbolCList));

}


/*
 * Insert all available symbols into the Gtk CList.
 * Take care of requested sorting method.
 */
static gint
gdisp_insertAndSortSymbols (Kernel_T  *kernel,
			    gchar    **stringFilter,
			    guint      stringFilterSize)
{

  GList      *providerItem   =      (GList*)NULL;
  Symbol_T   *symbolPtr      =   (Symbol_T*)NULL;
  Provider_T *provider       = (Provider_T*)NULL;
  gint        symbolCpt      =                 0,
              nbSymbols      =                 0,
              rowNumber      =                 0;
  gchar       rowBuffer[128];
  gchar      *symbolInfos[GD_COL_LAST] = { (gchar*)NULL,
					   (gchar*)NULL,
					   (gchar*)NULL,
					   (gchar*)NULL,
					   "" };
  Pixmap_T   *idPixmap       = (Pixmap_T*)NULL;

  assert(kernel);

  /* ------------------------ LIST OF SYMBOLS ------------------------ */

  gtk_clist_freeze(GTK_CLIST(kernel->widgets.symbolCList));
  gtk_clist_clear (GTK_CLIST(kernel->widgets.symbolCList));

  /*
   * Loop over all provider, then loop over all provider symbols.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pStatus == GD_SESSION_OPENED) {

      symbolPtr = provider->pSymbolList;

      for (symbolCpt=0; symbolCpt<provider->pSymbolNumber; symbolCpt++) {

	/*
	 * Take into account string filter.
	 */
	if (stringFilterSize == 0 /* insert all symbols */ ||
	    gdisp_strStr(symbolPtr->sInfo.name,
			 stringFilter,
			 stringFilterSize) != (gchar*)NULL ) {

	  symbolInfos[0] = gdisp_getTypeAsString(symbolPtr);
	  symbolInfos[1] = symbolPtr->sInfo.name;

	  /*
	   * show the dimension is the symbol is a table.
	   */
	  if (symbolPtr->sInfo.dimension > 1) {

	    symbolInfos[2] = rowBuffer;
	    sprintf(symbolInfos[2],"%d",symbolPtr->sInfo.dimension);

	  }
	  else {

	    symbolInfos[2] = "";

	  }

	  /*
	   * Insert the symbol into the list.
	   */
	  rowNumber =
	    gtk_clist_append(GTK_CLIST(kernel->widgets.symbolCList),
			     symbolInfos);

	  nbSymbols++;

	  /*
	   * If symbol is not available, use special color.
	   */
	  if (symbolPtr->sInfo.provider_global_index < 0) {

	    gtk_clist_set_foreground(GTK_CLIST(kernel->widgets.symbolCList),
				     rowNumber, /* row */
				     &kernel->colors[_GREY_]);

	    gtk_clist_set_selectable(GTK_CLIST(kernel->widgets.symbolCList),
				     rowNumber, /* row */
				     FALSE);

	  }

	  /*
	   * If symbol has extended information, insert a information pixmap.
	   */
	  if (symbolPtr->sExtInfoList != (GList*)NULL) {

	    idPixmap = gdisp_getPixmapById(kernel,
					   GD_PIX_info,
					   kernel->widgets.symbolCList);

	    gtk_clist_set_pixmap(GTK_CLIST(kernel->widgets.symbolCList),
				 rowNumber,
				 GD_COL_ExtInfo, /* column */
				 idPixmap->pixmap,
				 idPixmap->mask);
	  }
	  else {

	    gtk_clist_set_text(GTK_CLIST(kernel->widgets.symbolCList),
			       rowNumber,
			       GD_COL_ExtInfo, /* column */
			       "");

	  }

	  /*
	   * If there are several providers, show symbol appartenance.
	   */
	  if (gdisp_getProviderNumber(kernel) > 1) {

	    idPixmap = gdisp_getProviderIdPixmap(kernel,
						 kernel->widgets.symbolCList,
						 provider->pIdentity);

	    gtk_clist_set_pixmap(GTK_CLIST(kernel->widgets.symbolCList),
				 rowNumber,
				 GD_COL_Provider, /* column */
				 idPixmap->pixmap,
				 idPixmap->mask);

	  } /* gdisp_getProviderNumber(kernel) > 1 */

	  /*
	   * Remember here the symbol the row refers to.
	   */
	  gtk_clist_set_row_data(GTK_CLIST(kernel->widgets.symbolCList),
				 rowNumber, /* row */
				 (gpointer)symbolPtr);

	} /* filter */

	symbolPtr++;

      } /* loop over all symbols */

    } /* provider status */

    providerItem = g_list_next(providerItem);

  } /* loop over all providers */

  /*
   * Take into account sorting method.
   */
  gdisp_sortSymbolList(kernel);

  /*
   * Allow graphic refreshes.
   */
  gtk_clist_thaw(GTK_CLIST(kernel->widgets.symbolCList));

  return nbSymbols;

}


/*
 * List Selection callback.
 */
static void
gdisp_listSelectionCallback ( GtkWidget      *widget /* symbol cList */,
			      gint            row, 
			      gint            column,
			      GdkEventButton *event,
			      gpointer        data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Add the selected symbol into the DND selection list.
   */
  if (g_list_find(kernel->dndSelection,
		  gtk_clist_get_row_data(GTK_CLIST(widget),
					 row)) != (GList*)NULL) {

    /* do not add twice */
    return;

  }

  kernel->dndSelection =
    g_list_append(kernel->dndSelection,
		  gtk_clist_get_row_data(GTK_CLIST(widget),
					 row));

}


/*
 * List Deselection callback.
 */
static void
gdisp_listDeselectionCallback ( GtkWidget      *widget /* symbol cList */,
				gint            row, 
				gint            column,
				GdkEventButton *event,
				gpointer        data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Remove the selected symbol from the DND selection list.
   */
  kernel->dndSelection =
    g_list_remove(kernel->dndSelection,
		  gtk_clist_get_row_data(GTK_CLIST(widget),
					 row));

}


/*
 * DND "drag_begin" handler : this is called whenever a drag starts.
 */
static void
gdisp_beginDNDCallback (GtkWidget      *widget /* symbol cList */,
			GdkDragContext *dragContext,
			gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed drag begin setup code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"begin DND Callback for symbol list.\n");
  fflush (stdout);
#endif

  /*
   * Noting to be done.
   */

}


/*
 * DND "drag_end" handler : this is called when a drag and drop has
 * completed. So this function is the last one to be called in
 * any given DND operation.
 */
static void
gdisp_endDNDCallback (GtkWidget      *widget /* symbol cList */,
		      GdkDragContext *dragContext,
		      gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed drag end cleanup code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"end DND Callback for symbol list.\n");
  fflush (stdout);
#endif

  /*
   * Clear selection on the symbol list, so that the user has to
   * choose other symbols before beginning another DND operation.
   */
  gtk_clist_unselect_all(GTK_CLIST(kernel->widgets.symbolCList));

  /*
   * Free kernel DND selection list.
   */
  if (kernel->dndSelection != (GList*)NULL) {

    g_list_free(kernel->dndSelection);
    kernel->dndSelection = (GList*)NULL;

  }

  /*
   * Now all is done, what about removing icon window ?
   */
  if (kernel->dndIconWindow != (GdkWindow*)NULL) {

    gdk_window_destroy(kernel->dndIconWindow);
    kernel->dndIconWindow       = (GdkWindow*)NULL;
    kernel->dndIconWindowParent = (GdkWindow*)NULL;
    kernel->dndIconPixmap       = (GdkPixmap*)NULL;
    kernel->dndIconPixmapMask   = (GdkBitmap*)NULL;

    gdk_gc_destroy(kernel->dndIconWindowGc);
    kernel->dndIconWindowGc = (GdkGC*)NULL;

  }

}


/*
 * DND "drag_data_get" handler, for handling requests for DND
 * data on the specified widget. This function is called when
 * there is need for DND data on the source, so this function is
 * responsable for setting up the dynamic data exchange buffer
 * (DDE as sometimes it is called) and sending it out.
 */
static void
gdisp_dataRequestDNDCallback (GtkWidget        *widget /* symbol cList */,
			      GdkDragContext   *dragContext,
			      GtkSelectionData *selectionData,
			      guint             entryInfo,
			      guint             time,
			      gpointer          data)
{

  Kernel_T *kernel = (Kernel_T*)data;
  gchar    *action = GD_DND_SYMBOL_LIST_EXCHANGE;

  /*
   * Put any needed data request code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Request DND Callback for symbol list.\n");
  fflush (stdout);
#endif

  /*
   * Send out the data using the selection system,
   * this is also used for cut and paste but
   * GTK uses it for drag and drop as well. When
   * sending a string, GTK will ensure that a null
   * terminating byte is added to the end so we
   * do not need to add it. GTK also coppies the
   * data so the original will never be modified.
   */
  gtk_selection_data_set(selectionData,
			 GDK_SELECTION_TYPE_STRING,
			 8,	/* 8 bits per character */
			 action,
			 strlen(action));

}


/*
 * DND "drag_data_received" handler. When 'gdisp_dataRequestDNDCallback'
 * calls 'gtk_selection_data_set()' to send out the data, this function
 * receives it and is responsible for handling it.
 *
 * This is also the only DND callback function where the given
 * inputs may reflect those of the drop target, so we need to check
 * if this is the same structure or not.
 *
 * NOT THE CASE HERE, just for checking DND process.
 */
static void
gdisp_dataReceivedDNDCallback (GtkWidget        *widget /* symbol cList */,
			       GdkDragContext   *dragContext,
			       gint              x,
			       gint              y,
			       GtkSelectionData *selectionData,
			       guint             entryInfo,
			       guint             time,
			       gpointer          data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed data received code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Receive DND Callback for symbol list.\n");
  fflush (stdout);
#endif

  /*
   * This procedure does nothing because there is nothing to be done here.
   * The symbol list will never receive any data.
   */

}


/*
 * DND "drag_data_delete" handler, this function is called when
 * the data on the source `should' be deleted (ie if the DND was
 * a move).
 *
 * NOT THE CASE HERE, just for checking DND process.
 */
static void
gdisp_dataDestroyedDNDCallback (GtkWidget      *widget /* symbol cList */,
				GdkDragContext *dragContext,
				gpointer        data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Put any needed data destroyed code here.
   */
  assert(kernel);
  assert(dragContext);

#if defined(_DND_DEBUG_)
  fprintf(stdout,"data Destroy DND Callback for symbol list.\n");
  fflush (stdout);
#endif

  /*
   * This procedure does nothing because there is nothing to be done here.
   * All symbols are duplicated (DND is a COPY action).
   * So nothing to be destroyed.
   */

}


/*
 * Button-press handler on a column title in order to change sorting
 * method and direction.
 */
static void
gdisp_clickColumnCallback (GtkCList *cList,
			   gint      column,
			   gpointer  userData)
{

  Kernel_T *kernel = (Kernel_T*)userData;

  /*
   * If the button has been pressed on the column which is
   * already the sorting column, then change sorting direction.
   */
  if ((column == GD_COL_Type     &&
       kernel->sortingMethod == GD_SORT_BY_TYPE     ) ||
      (column == GD_COL_Name     &&
       kernel->sortingMethod == GD_SORT_BY_NAME     ) ||
      (column == GD_COL_Dim      &&
       kernel->sortingMethod == GD_SORT_BY_DIM      ) ||
      (column == GD_COL_ExtInfo  &&
       kernel->sortingMethod == GD_SORT_BY_EXTINFO  ) ||
      (column == GD_COL_Provider &&
       kernel->sortingMethod == GD_SORT_BY_PROVIDER )) {

    kernel->sortingDirection = GD_TOGGLE_VALUE(kernel->sortingDirection,
					       GD_SORT_ASCENDING,
					       GD_SORT_DESCENDING);

  }
  else {

    kernel->sortingDirection = GD_SORT_ASCENDING;

    if (column == GD_COL_Type) {
      kernel->sortingMethod = GD_SORT_BY_TYPE;
    }
    else if (column == GD_COL_Name) {
      kernel->sortingMethod = GD_SORT_BY_NAME;
    }
    else if (column == GD_COL_Dim) {
      kernel->sortingMethod = GD_SORT_BY_DIM;
    }
    else if (column == GD_COL_ExtInfo) {
      kernel->sortingMethod = GD_SORT_BY_EXTINFO;
    }
    else if (column == GD_COL_Provider) {
      kernel->sortingMethod = GD_SORT_BY_PROVIDER;
    }

  }

  /*
   * Freeze redrawings while sorting.
   */
  gtk_clist_freeze(GTK_CLIST(kernel->widgets.symbolCList));

  gdisp_sortSymbolList(kernel);

  gtk_clist_thaw(GTK_CLIST(kernel->widgets.symbolCList));

}


/*
 * Popup Menu Handler.
 * Change the scope of the DnD operation.
 */
static void
gdisp_popupMenuHandler ( Kernel_T    *kernel,
			 PopupMenu_T *menu,
			 gpointer     menuData, /* null */
			 gpointer     itemData )
{

  /*
   * Update drag and drop scope.
   */
  kernel->dndScope = (DndScope_T)itemData;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Apply callback (activated from databook widget).
 */
void
gdisp_symbolApplyCallback (GtkWidget *applyButtonWidget,
			   gpointer   data )
{

  Kernel_T  *kernel          = (Kernel_T*)data;
  gint       symbolCount     =               0;
  gchar     *filter          =    (gchar*)NULL;
  GString   *messageString   =  (GString*)NULL;

  gchar    **stringTable     =   (gchar**)NULL;
  guint      stringTableSize =               0;


  /*
   * Get back filter.
   * Turn the filter (a coma-separated string list) into a string table.
   */
  filter = gtk_entry_get_text(GTK_ENTRY(kernel->widgets.filterEntry));
  if (strlen(filter) == 0) {
    filter = (gchar*)NULL;
  }

  gdisp_getStringTableFromStringList(filter,
				     &stringTable,
				     &stringTableSize);

  /*
   * Apply changes, with a possible filter.
   */
  symbolCount = gdisp_insertAndSortSymbols(kernel,
					   stringTable,
					   stringTableSize);


  /*
   * Free allocated memory for string table.
   */
  gdisp_freeStringTable(&stringTable,
			&stringTableSize);


  /*
   * Change symbol list frame label to reflect the exact number
   * of listed symbols.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   " Available Symbols : %d ",
		   symbolCount);

  gtk_frame_set_label(GTK_FRAME(kernel->widgets.symbolFrame),
		      messageString->str);

  g_string_free(messageString,TRUE);

}


/*
 * Create GDISP+ symbol list.
 */
void
gdisp_createSymbolList ( Kernel_T  *kernel,
			 GtkWidget *parent )
{

  GtkTargetEntry  targetEntry;
  PopupMenu_T    *popupMenu         = (PopupMenu_T*)NULL;
  DndScope_T      dndScope          = GD_DND_UNICAST;
  GtkWidget      *otherVBox         = (GtkWidget*)NULL;
  GtkWidget      *otherHBox         = (GtkWidget*)NULL;
  GtkWidget      *frame             = (GtkWidget*)NULL;
  GtkWidget      *scrolledWindow    = (GtkWidget*)NULL;
  GtkWidget      *label             = (GtkWidget*)NULL;
  GtkStyle       *cListStyle        = (GtkStyle*)NULL;
  GtkTooltips    *toolTipGroup      = (GtkTooltips*)NULL;
  Pixmap_T       *infoPixmap        = (Pixmap_T*)NULL;
  GString        *messageString     = (GString*)NULL;
  gint            symbolCount       = 0;
  GtkWidget      *cList             = (GtkWidget*)NULL;
  gchar          *cListTitles[GD_COL_LAST] = { "Type",
					       "Name",
					       "Dim",
					       (gchar*)NULL,
					       "P" };

  /* ------------------------ TOOLTIP GROUP ------------------------ */

  /*
   * Create the group of tooltips.
   * Look downwards for the association of tooltips and widgets.
   */
  toolTipGroup = gtk_tooltips_new();


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for symbols.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new(" Available Symbols ");
  gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(parent),frame);
  gtk_widget_show(frame);

  kernel->widgets.symbolFrame = frame; /* in order to change its text */


  /* ------------ VERTICAL PACKING BOX INTO THE FRAME ------------ */

  /*
   * We need a vertical packing box for managing the scrolled window.
   */
  otherVBox = gtk_vbox_new(FALSE, /* homogeneous */
			    5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherVBox),3);
  gtk_container_add(GTK_CONTAINER(frame),otherVBox);
  gtk_widget_show(otherVBox);


  /* ----------- SCROLLED WINDOW FOR THE LIST OF SYMBOLS  ----------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_widget_set_usize(scrolledWindow,395,300);
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     scrolledWindow,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(scrolledWindow);


  /* -------------- CLIST FOR SYMBOL INFORMATION --------------- */

  /*
   * Create the CList. For our needs, we use 4 columns.
   */
  cList = gtk_clist_new_with_titles(GD_COL_LAST,cListTitles);
  gtk_clist_column_titles_show((GtkCList*)cList);
  kernel->widgets.symbolCList = cList;

  /*
   * It isn't necessary to shadow the border, but it looks nice.
   */
  gtk_clist_set_shadow_type(GTK_CLIST(cList),GTK_SHADOW_OUT);

  /*
   * Multiple selection is possible.
   */
  gtk_clist_set_selection_mode(GTK_CLIST(cList),
			       GTK_SELECTION_EXTENDED);

  /*
   * What however is important, is that we set the column widths as
   * they will never be right otherwise.
   * Note that the columns are numbered from 0 and up (to 5 in our case).
   */
  gtk_clist_set_column_width(GTK_CLIST(cList),GD_COL_Type    ,050);
  gtk_clist_set_column_width(GTK_CLIST(cList),GD_COL_Name    ,210);
  gtk_clist_set_column_width(GTK_CLIST(cList),GD_COL_Dim     ,040);
  gtk_clist_set_column_width(GTK_CLIST(cList),GD_COL_ExtInfo ,030);
  gtk_clist_set_column_width(GTK_CLIST(cList),GD_COL_Provider,030);

  /*
   * Extended information column header is a pixmap.
   */
  infoPixmap = gdisp_getPixmapById(kernel,
				   GD_PIX_info,
				   kernel->widgets.symbolCList);

  gtk_clist_set_column_widget(GTK_CLIST(kernel->widgets.symbolCList),
			      GD_COL_ExtInfo, /* column */
			      gtk_pixmap_new(infoPixmap->pixmap,
					     infoPixmap->mask));

  /*
   * Set up button actions.
   */
  gtk_clist_set_button_actions(GTK_CLIST(cList),
			       0, /* left button performs selection & DND */
			       GTK_BUTTON_SELECTS | GTK_BUTTON_DRAGS);

  gtk_clist_set_use_drag_icons(GTK_CLIST(cList),
                               FALSE /* use icons */);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "select_row",
		     gdisp_listSelectionCallback,
		     kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "unselect_row",
		     gdisp_listDeselectionCallback,
		     kernel);

  /*
   * GTK Style from default style.
   */
  cListStyle       = gtk_style_copy(gtk_widget_get_default_style());
  cListStyle       = gtk_style_ref(cListStyle);
  cListStyle->font = kernel->fonts[GD_FONT_MEDIUM][GD_FONT_FIXED];

  gtk_widget_set_style(cList,
		       cListStyle);

  /*
   * Add the CList widget to the vertical box and show it.
   */
  gtk_container_add(GTK_CONTAINER(scrolledWindow),cList);
  gtk_widget_show(cList);

  /*
   * Drag And Drop settings.
   * Set up the 'cList' as a potential DND source.
   * First we set up 'targetEntry' which is a structure which specifies
   * the kinds (which we define) of drags accepted on this widget.
   */
  targetEntry.target = GD_DND_TARGET_NAME;
  targetEntry.flags  = 0;
  targetEntry.info   = GD_DND_TARGET_INFO;

  gtk_drag_source_set(cList,
		      GDK_BUTTON1_MASK,
		      &targetEntry,
		      sizeof(targetEntry) / sizeof(GtkTargetEntry),
		      GDK_ACTION_COPY);

  /*
   * Set DND signals on 'cList'.
   */
  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_begin",
		     GTK_SIGNAL_FUNC(gdisp_beginDNDCallback),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_end",
		     GTK_SIGNAL_FUNC(gdisp_endDNDCallback),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_get",
		     GTK_SIGNAL_FUNC(gdisp_dataRequestDNDCallback),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_received",
		     GTK_SIGNAL_FUNC(gdisp_dataReceivedDNDCallback),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_delete",
		     GTK_SIGNAL_FUNC(gdisp_dataDestroyedDNDCallback),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "click-column",
		     GTK_SIGNAL_FUNC(gdisp_clickColumnCallback),
		     (gpointer)kernel);


  /* ----------------------- DYNAMIC POPUP MENU ---------------------- */

  /*
   * Create the dynamic menu.
   */
  popupMenu = gdisp_createMenu(kernel,
			       cList,
			       gdisp_popupMenuHandler,
			       (gpointer)NULL);

  gdisp_addMenuItem(popupMenu,
		    GD_POPUP_TITLE,
		    "Drag & Drop Scope",
		    (gpointer)NULL);

  dndScope = GD_DND_UNICAST;
  gdisp_addMenuItem(popupMenu,
		    GD_POPUP_ITEM,
		    "Unique",
		    (gpointer)GUINT_TO_POINTER(dndScope));

  dndScope = GD_DND_MULTICAST;
  gdisp_addMenuItem(popupMenu,
		    GD_POPUP_ITEM,
		    "Single Page",
		    (gpointer)GUINT_TO_POINTER(dndScope));

  dndScope = GD_DND_BROADCAST;
  gdisp_addMenuItem(popupMenu,
		    GD_POPUP_ITEM,
		    "All Pages",
		    (gpointer)GUINT_TO_POINTER(dndScope));

  kernel->widgets.cListPopupMenu = (gpointer)popupMenu;


  /* ------------------------ FRAME WITH LABEL ----------------------- */

  /*
   * Create a Frame that will contain every sorting methods.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new((gchar*)NULL);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_box_pack_start(GTK_BOX(parent),
		     frame,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(frame);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a horizontal packing box for managing the filter and the label.
   */
  otherHBox = gtk_hbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherHBox),5);
  gtk_container_add(GTK_CONTAINER(frame),otherHBox);
  gtk_widget_show(otherHBox);


  /* ---------------------- LABEL + FILTER --------------------- */

  /*
   * Just a label to better understand.
   */
  label = gtk_label_new("Filter     ");
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     label,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(label);

  /*
   * Create the entry to enter any filter information.
   */
  kernel->widgets.filterEntry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.filterEntry,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.filterEntry);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.filterEntry,
		       "Add coma-separated strings to reduce the size "
		       "of the list (case sensitive)",
		       "");


  /* ------------------------ LIST OF SYMBOLS  ------------------------ */

  /*
   * Insert symbols into the corresponding CList.
   * ...according to the default sorting method in the kernel...
   */
  symbolCount = gdisp_insertAndSortSymbols(kernel,
					   (gchar**)NULL /* no filter */,
					   (guint)0      /* no filter */);


  /*
   * Keep in mind that the GString is released into the
   * 'gdisp_outputWrite' function.
   * Do not 'g_string_free' it.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   "A total of %d symbols have been processed.",
		   symbolCount);
  kernel->outputFunc(kernel,messageString,
		     symbolCount == 0 ? GD_ERROR : GD_MESSAGE);


  /*
   * Change frame label.
   */
  messageString = g_string_new((gchar*)NULL);
  g_string_sprintf(messageString,
		   " Available Symbols : %d ",
		   symbolCount);
  gtk_frame_set_label(GTK_FRAME(kernel->widgets.symbolFrame),
		      messageString->str);
  g_string_free(messageString,TRUE);

}


/*
 * Refresh symbol list.
 */
void
gdisp_refreshSymbolList ( Kernel_T *kernel )
{

  /*
   * Just simulate a mouse-click on 'apply' button.
   */
  if (kernel->widgets.dataBookApplyButton != (GtkWidget*)NULL) {

    gdisp_symbolApplyCallback(kernel->widgets.dataBookApplyButton,
			      (gpointer)kernel);

  }

}


/*
 * Destroy GDISP+ symbol list.
 */
void
gdisp_destroySymbolList ( Kernel_T *kernel )
{

  /*
   * Destroy popup menu.
   */
  gdisp_destroyMenu((PopupMenu_T*)kernel->widgets.cListPopupMenu);
  kernel->widgets.cListPopupMenu = (gpointer)NULL;

}
