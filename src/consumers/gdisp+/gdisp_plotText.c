/*!  \file 

$Id: gdisp_plotText.c,v 1.8 2006-02-02 21:03:32 esteban Exp $

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

File      : Text plot system.

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

#include "gdisp_format.h"
#include "gdisp_popupMenu.h"
#include "gdisp_plotText.h"


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#undef GD_DEBUG_TEXT


/*
 * Function in order to sort symbols alphabetically' when inserting
 * them into the double-linked list.
 */
static gint
gdisp_sortSymbolByName(gconstpointer data1,
		       gconstpointer data2)
{

  Symbol_T *symbol1 = (Symbol_T*)data1,
           *symbol2 = (Symbol_T*)data2;

  return (strcmp(symbol1->sInfo.name,symbol2->sInfo.name));

}


/*
 * Callback when a row is selected into the list.
 */
static void
gdisp_selectPlotTextRow (GtkCList       *cList,
			 gint            row,
			 gint            column,
			 GdkEventButton *event,
			 gpointer        userData /* kernel */)
{

  /*
   * Info : Kernel_T *kernel = (Kernel_T*)userData;
   */
  PlotText_T *plot = (PlotText_T*)NULL;
  
  /*
   * Get back the plot instance.
   */
  plot = gtk_object_get_data(GTK_OBJECT(cList),
			     "plotPointer");

  /*
   * Only line number is interesting.
   */
   plot->pttSelectedRow = row;

}


/*
 * Callback when a row is deselected in the list.
 */
static void
gdisp_unselectPlotTextRow ( GtkCList       *cList,
			    gint            row,
			    gint            column,
			    GdkEventButton *event,
			    gpointer        userData)
{

  /*
   * Info : Kernel_T *kernel = (Kernel_T*)userData;
   */

  /* nothing by now */

}


/*
 * Callback when a row moves in the list.
 */
static void
gdisp_movePlotTextRow ( GtkCList *clist,
			gint      sourcePosition,
			gint      destinationPosition,
			gpointer  userData)
{

  /*
   * Info : Kernel_T *kernel = (Kernel_T*)userData;
   */

  /* nothing by now */

}


/*
 * Size allocate management.
 */
static void
gdisp_sizeAllocate (GtkWidget     *widget,
		    GtkAllocation *allocation,
		    gpointer       userData /* kernel */)
{

  /*
   * For information :
   * Kernel_T   *kernel   = (Kernel_T*)userData;
   */
  PlotText_T *plot     = (PlotText_T*)NULL;
  gfloat      newWidth = (gfloat)0;

  /*
   * Get back the plot instance.
   */
  plot = gtk_object_get_data(GTK_OBJECT(widget),
			     "plotPointer");

  /*
   * Check for recursive calls to that handler.
   */
  if (plot->pttIsSizeAllocating == 1) {
    plot->pttIsSizeAllocating = 0;
    return;
  }

  /*
   * Remember my initial width and height.
   */
  plot->pttCListWidth  = allocation->width;
  plot->pttCListHeight = allocation->height;

  if (plot->pttCList != (GtkWidget*)NULL) {

    /*
     * What however is important, is that we set the column widths as
     * they will never be right otherwise.
     * Note that the columns are numbered from 0 and up (to 2 in our case).
     */
    newWidth = plot->pttColumnRatio * allocation->width;

    gtk_clist_set_column_width(GTK_CLIST(plot->pttCList),
			       GD_SYMBOL_NAME_COLUMN, /* first column */
			       (gint)newWidth);

    /*
     * In order to avoid recursive calls to that handler.
     */
    plot->pttIsSizeAllocating = 1;

  }

}


/*
 * Resize column handler.
 */
static void
gdisp_resizeColumn (GtkCList *cList,
		    gint      column,
		    gint      width,
		    gpointer  userData)
{

  /*
   * For information :
   * Kernel_T   *kernel = (Kernel_T*)userData;
   */
  PlotText_T *plot   = (PlotText_T*)NULL;

  /*
   * Get back the plot instance.
   */
  plot = gtk_object_get_data(GTK_OBJECT(cList),
			     "plotPointer");

  /*
   * Check for internal GTK initialisations.
   */
  if (plot->pttCList->allocation.width == 1) {
    return;
  }

  /*
   * Handle only first column.
   */
  if (column != GD_SYMBOL_NAME_COLUMN) {
    return;
  }

  /*
   * Compute first column width ratio.
   */
  plot->pttColumnRatio = (gfloat)width /
                         (gfloat)plot->pttCList->allocation.width;

}


/*
 * Create a 'text plot' by providing an opaque structure to the
 * caller. This opaque structure will be given as an argument to all
 * plot function. These functions remain generic.
 */
static void*
gdisp_createPlotText (Kernel_T *kernel)
{

  PlotText_T *plot = (PlotText_T*)NULL;

  /*
   * Dynamic allocation.
   */
  plot = g_malloc0(sizeof(PlotText_T));
  assert(plot);


  /*
   * Few initialisations.
   */
  plot->pttType        = GD_PLOT_TEXT;
  plot->pttColumnRatio = (gfloat)0.66;

  /*
   * Create a CList with 2 columns.
   */
  plot->pttCList = gtk_clist_new(GD_SYMBOL_MAX_COLUMNS);

  gtk_clist_set_shadow_type(GTK_CLIST(plot->pttCList),
			    GTK_SHADOW_IN);

  gtk_clist_set_column_title(GTK_CLIST(plot->pttCList),
			     GD_SYMBOL_NAME_COLUMN, /* first column */
			     "Symbols");

  gtk_clist_set_column_title(GTK_CLIST(plot->pttCList),
			     GD_SYMBOL_VALUE_COLUMN, /* second column */
			     "Values");

  gtk_clist_set_sort_column(GTK_CLIST(plot->pttCList),
			    GD_SYMBOL_NAME_COLUMN); /* first column */

  gtk_clist_set_sort_type(GTK_CLIST(plot->pttCList),
			  GTK_SORT_ASCENDING);

  gtk_clist_set_auto_sort(GTK_CLIST(plot->pttCList),
			  TRUE); /* Auto-sort is allowed */

  gtk_clist_set_button_actions(GTK_CLIST(plot->pttCList),
			       0, /* left button */
			       GTK_BUTTON_SELECTS);

  gtk_clist_set_selection_mode(GTK_CLIST(plot->pttCList),
			       GTK_SELECTION_EXTENDED);

  gtk_clist_column_titles_show(GTK_CLIST(plot->pttCList));


  /*
   * Connect signals.
   */
  gtk_signal_connect(GTK_OBJECT(plot->pttCList),
		     "select-row",
		     gdisp_selectPlotTextRow,
		     (gpointer)kernel);


  gtk_signal_connect(GTK_OBJECT(plot->pttCList),
		     "unselect-row",
		     gdisp_unselectPlotTextRow,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->pttCList),
		     "row-move",
		     gdisp_movePlotTextRow,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->pttCList),
		     "size-allocate",
		     gdisp_sizeAllocate,
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(plot->pttCList),
		     "resize-column",
		     gdisp_resizeColumn,
		     (gpointer)kernel);

  gtk_object_set_data(GTK_OBJECT(plot->pttCList),
		      "plotPointer",
		      (gpointer)plot);


  /*
   * GTK Style from default style.
   */
  plot->pttStyle = gtk_style_copy(gtk_widget_get_default_style());
  plot->pttStyle = gtk_style_ref(plot->pttStyle);

  plot->pttStyle->base[GTK_STATE_NORMAL]   = kernel->colors[_BLACK_ ];
  plot->pttStyle->fg  [GTK_STATE_NORMAL]   = kernel->colors[_WHITE_];

  plot->pttStyle->base[GTK_STATE_ACTIVE]   = kernel->colors[_BLACK_ ];
  plot->pttStyle->fg  [GTK_STATE_ACTIVE]   = kernel->colors[_WHITE_];

  plot->pttStyle->base[GTK_STATE_SELECTED] = kernel->colors[_WHITE_];
  plot->pttStyle->fg  [GTK_STATE_SELECTED] = kernel->colors[_BLACK_ ];
  plot->pttStyle->bg  [GTK_STATE_SELECTED] = kernel->colors[_WHITE_];

  gtk_widget_set_style(plot->pttCList,
		       plot->pttStyle);


  /*
   * Return the opaque structure.
   */
  return (void*)plot;

}


/*
 * Destroy a 'text' plot opaque structure.
 */
static void
gdisp_destroyPlotText(Kernel_T *kernel,
		      void     *data)
{

  PlotText_T        *plot    = (PlotText_T*)data;
  PlotTextRowData_T *rowData = (PlotTextRowData_T*)NULL;
  guint              row     = 0;


  /*
   * Free symbol list.
   */
  gdisp_dereferenceSymbolList(plot->pttSymbolList);


  /*
   * Now destroy everything.
   */
  if (plot->pttStyle != (GtkStyle*)NULL) {
#if defined(GD_UNREF_THINGS)
    gtk_style_unref(plot->pttStyle);
#endif
  }

  /*
   * Loop over all rows.
   */
  for (row=0; row<GTK_CLIST(plot->pttCList)->rows; row++) {

    rowData = gtk_clist_get_row_data(GTK_CLIST(plot->pttCList),
				     row);

    if (rowData != (PlotTextRowData_T*)NULL) {
      g_free(rowData);
    }

  }

  /*
   * Destroy the list.
   */
  gtk_widget_destroy(plot->pttCList);


  /*
   * Destroy Menu.
   */
  gdisp_destroyMenu(plot->pttMenu);


  /*
   * Free opaque structure.
   */
  memset(plot,0,sizeof(PlotText_T));
  g_free(plot);

}


/*
 * Record the parent widget.
 */
static void
gdisp_setPlotTextParent (Kernel_T  *kernel,
			 void      *data,
			 GtkWidget *parent)
{

  PlotText_T *plot = (PlotText_T*)data;

  /*
   * Remember my parent.
   */
  plot->pttParent = parent;

}


/*
 * Record initial dimensions provided by the calling process.
 */
static void
gdisp_setPlotTextInitialDimensions (Kernel_T *kernel,
				    void     *data,
				    guint     width,
				    guint     height)
{

  PlotText_T *plot = (PlotText_T*)data;

  /*
   * Remember my initial width and height.
   */
  plot->pttCListWidth  = width;
  plot->pttCListHeight = height;

  if (plot->pttCList != (GtkWidget*)NULL) {

    /*
     * What however is important, is that we set the column widths as
     * they will never be right otherwise.
     * Note that the columns are numbered from 0 and up (to 2 in our case).
     */
    gtk_clist_set_column_width(GTK_CLIST(plot->pttCList),
			       GD_SYMBOL_NAME_COLUMN, /* first column */
			       2 * width / 3);

    gtk_clist_set_column_width(GTK_CLIST(plot->pttCList),
			       GD_SYMBOL_VALUE_COLUMN, /* second column */
			       1 * width / 3);

  }

}


/*
 * Give back to the calling process the top level widget
 * in order to be inserted in a possible container for further
 * dynamic X management.
 */
static GtkWidget*
gdisp_getPlotTextTopLevelWidget (Kernel_T  *kernel,
				 void      *data)
{

  PlotText_T *plot = (PlotText_T*)data;

#if defined(GD_DEBUG_TEXT)
  fprintf(stdout,"Getting back text plot top level widget.\n");
  fflush (stdout);
#endif

  return (GtkWidget*)plot->pttCList;

}


/*
 * Popup Menu Handler.
 * Change the format of the selected symbol.
 */
static void
gdisp_popupMenuHandler ( Kernel_T    *kernel,
			 PopupMenu_T *menu,
			 gpointer     menuData,
			 gpointer     itemData )
{

  PlotText_T        *plot    = (PlotText_T*)menuData;
  Format_T           format  = (Format_T)itemData;
  PlotTextRowData_T *rowData = (PlotTextRowData_T*)NULL;

  /*
   * Look for the current selected row and change its display format.
   */
  rowData =
    (PlotTextRowData_T*)gtk_clist_get_row_data(GTK_CLIST(plot->pttCList),
					       plot->pttSelectedRow);

  if (rowData != (PlotTextRowData_T*)NULL) {
    rowData->format = format;
  }

}


/*
 * By now, the 'text plot' widgets are created, but not shown yet.
 * Show them here.
 */
static void
gdisp_showPlotText (Kernel_T  *kernel,
		    void      *data)
{

  PlotText_T *plot = (PlotText_T*)data;

#if defined(GD_DEBUG_TEXT)
  fprintf(stdout,"Showing text plot.\n");
  fflush (stdout);
#endif

  /*
   * Now show everything.
   */
  gtk_widget_show(plot->pttCList);

  /*
   * Create the dynamic menu.
   * Menu cannot be created before because the parent list is not shown yet.
   */
  plot->pttMenu = gdisp_createMenu(kernel,
				   plot->pttCList,
				   "          Format" /* title */,
				   gdisp_popupMenuHandler,
				   (gpointer)plot);

  /*
   * Choose useful formats among all available.
   */
  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_DEFAULT_FORMAT),
		    (gpointer)GUINT_TO_POINTER(GD_DEFAULT_FORMAT));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_HEXADECIMAL_1),
		    (gpointer)GUINT_TO_POINTER(GD_HEXADECIMAL_1));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_HEXADECIMAL_2),
		    (gpointer)GUINT_TO_POINTER(GD_HEXADECIMAL_2));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_HEXADECIMAL_4),
		    (gpointer)GUINT_TO_POINTER(GD_HEXADECIMAL_4));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_HEXADECIMAL_8),
		    (gpointer)GUINT_TO_POINTER(GD_HEXADECIMAL_8));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_BINARY),
		    (gpointer)GUINT_TO_POINTER(GD_BINARY));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_FLOATING_FIXED),
		    (gpointer)GUINT_TO_POINTER(GD_FLOATING_FIXED));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_SCIENTIFIC),
		    (gpointer)GUINT_TO_POINTER(GD_SCIENTIFIC));

  gdisp_addMenuItem(plot->pttMenu,
		    gdisp_getFormatLabel(GD_ASCII),
		    (gpointer)GUINT_TO_POINTER(GD_ASCII));

}


/*
 * Return to calling process what king of plot we are.
 */
static PlotType_T
gdisp_getPlotTextType (Kernel_T *kernel,
		       void     *data)
{

  PlotText_T *plot = (PlotText_T*)data;

#if defined(GD_DEBUG_TEXT)
  fprintf(stdout,"Getting back text plot type.\n");
#endif

  /*
   * Must be GD_PLOT_TEXT. See 'create' routine.
   */
  return plot->pttType;

}


/*
 * Record any incoming symbols.
 */
static void
gdisp_addSymbolsToPlotText (Kernel_T *kernel,
			    void     *data,
			    GList    *symbolList,
			    guchar    zoneId)
{

  PlotText_T        *plot       = (PlotText_T*)data;
  GList             *symbolItem = (GList*)NULL;
  Symbol_T          *symbol     = (Symbol_T*)NULL;
  PlotTextRowData_T *rowData    = (PlotTextRowData_T*)NULL;
  gint               rowNumber  = 0;
  gchar              sValue [256];
  gchar             *rowInfo[GD_SYMBOL_MAX_COLUMNS];

#if defined(GD_DEBUG_TEXT)
  fprintf(stdout,"Adding symbols to text plot.\n");
  fflush (stdout);
#endif

  /*
   * Incoming symbols are to be attached to the list.
   */
  rowInfo[1] = (gchar*)sValue;
  gtk_clist_freeze(GTK_CLIST(plot->pttCList));

  /*
   * Loop over all incoming symbols and store only those that
   * are not already in the final list.
   */
  symbolItem = g_list_first(symbolList);
  while (symbolItem != (GList*)NULL) {

    if (g_list_find(plot->pttSymbolList,symbolItem->data) == (GList*)NULL) {

      plot->pttSymbolList = g_list_append(plot->pttSymbolList,
					  symbolItem->data);

      /*
       * Do not forget to increment the reference of the Y symbol.
       */
      symbol = (Symbol_T*)symbolItem->data;
      symbol->sReference++;

      /*
       * Insert symbol into the graphic list.
       */
      rowInfo[0] = symbol->sInfo.name;

      gdisp_formatDoubleValue(symbol->sLastValue,
			      GD_DEFAULT_FORMAT,
			      rowInfo[1]);

      rowNumber = gtk_clist_append(GTK_CLIST(plot->pttCList),
				   rowInfo);

      /*
       * Attach characteristics to the row.
       */
      rowData = (PlotTextRowData_T*)g_malloc0(sizeof(PlotTextRowData_T));

      if (rowData != (PlotTextRowData_T*)NULL) {

	rowData->symbol = symbol;
	rowData->format = GD_DEFAULT_FORMAT;

	gtk_clist_set_row_data(GTK_CLIST(plot->pttCList),
			       rowNumber,
			       (gpointer)rowData);

      }

    }

    symbolItem = g_list_next(symbolItem);

  }

  /*
   * Sort symbols by name.
   */
  plot->pttSymbolList = g_list_sort(plot->pttSymbolList,
				    gdisp_sortSymbolByName);

  gtk_clist_thaw(GTK_CLIST(plot->pttCList));

}


/*
 * Broadcast all symbols.
 */
static GList*
gdisp_getSymbolsFromPlotText (Kernel_T *kernel,
			      void     *data,
			      gchar     axis)
{

  PlotText_T *plot = (PlotText_T*)data;

#if defined(GD_DEBUG_TEXT)
  fprintf(stdout,"Give back the list of symbols handled by the text plot.\n");
  fflush (stdout);
#endif

  return plot->pttSymbolList;

}


/*
 * Real time Starting Step Action.
 */
static gboolean
gdisp_startStepOnPlotText (Kernel_T *kernel,
			   void     *data)
{

  /*
   * Nothing to be done on text plot, except that we must
   * return TRUE to the calling procedure in order to allow the general
   * step management to proceed.
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
gdisp_stepOnPlotText (Kernel_T *kernel,
		      void     *data)
{

  PlotText_T        *plot       = (PlotText_T*)data;
  PlotTextRowData_T *rowData    = (PlotTextRowData_T*)NULL;
  GList             *symbolItem =      (GList*)NULL;
  Symbol_T          *symbol     =   (Symbol_T*)NULL;
  gint               rowNumber  = 0;
  gchar              sValue [256];


  /*
   * Loop over all symbols.
   */
  symbolItem = g_list_first(plot->pttSymbolList);
  while (symbolItem != (GList*)NULL) {

    symbol = (Symbol_T*)symbolItem->data;

    /*
     * Get back requested format.
     */
    rowData =
      (PlotTextRowData_T*)gtk_clist_get_row_data(GTK_CLIST(plot->pttCList),
						 rowNumber);

    if (rowData != (PlotTextRowData_T*)NULL) {

      gdisp_formatDoubleValue(symbol->sLastValue,
			      rowData->format,
			      sValue);

    }
    else {

      gdisp_formatDoubleValue(symbol->sLastValue,
			      GD_DEFAULT_FORMAT,
			      sValue);

    }

    /*
     * Update symbol into the graphic list.
     */
    gtk_clist_set_text(GTK_CLIST(plot->pttCList),
		       rowNumber,
		       GD_SYMBOL_VALUE_COLUMN,
		       sValue);

#if defined(TRY_PIXMAP)

    gtk_clist_set_pixtext(GTK_CLIST(plot->pttCList),
			  rowNumber,
			  GD_SYMBOL_NAME_COLUMN,
			  symbol->sInfo.name,
			  10, /* spacing */
			  pixmap,
			  mask);

#endif

    rowNumber++;

    symbolItem = g_list_next(symbolItem);

  }

}


/*
 * Real time Starting Step Action.
 */
static void
gdisp_stopStepOnPlotText (Kernel_T *kernel,
			  void     *data)
{

  /*
   * Nothing to be done on default plot.
   */

}


/*
 * Get back to the calling procedure my information.
 */
static void
gdisp_getPlotTextInformation (Kernel_T         *kernel,
			      PlotSystemInfo_T *information)
{

#include "pixmaps/gdisp_textLogo.xpm"

  /*
   *   - Name,
   *   - Formula,
   *   - Descripton for tooltip purpose.
   */
  information->psName        = "Text Plot";
  information->psFormula     = "Value = F ( Time )";
  information->psDescription = "A typical textual plot that shows the "
    "evolution of several symbols by printing the last known value.";
  information->psLogo        = gdisp_textLogo;

}


/*
 * Get back to the calling procedure my period, expressed in milliseconds.
 * CAUTION : The period must be an exact multiple of 10.
 *           Should not be lower than 100.
 */
static guint
gdisp_getPlotTextPeriod (Kernel_T *kernel,
			 void     *data)
{

  /*
   * My period is 1000 milli-seconds.
   * FIXME : This value is hardcoded. It should be easily modifiable.
   * But how ?
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
gdisp_treatPlotTextSymbolValues (Kernel_T *kernel,
				 void     *data)
{

  /*
   * Nothing to be done here for text plot, because the last
   * value of all symbols is retrieved in the "step" procedure.
   */

}


/*
 * Get back the zones that have been defined on that plot.
 */
static GArray*
gdisp_getPlotTextDropZones (Kernel_T *kernel)
{

  /*
   * No zones on text plots.
   */
  return (GArray*)NULL;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


void
gdisp_initPlotTextSystem (Kernel_T     *kernel,
			  PlotSystem_T *plotSystem)
{

  /*
   * We must here provide all 'PlotText' private functions
   * that remain 'static' here, but accessible from everywhere
   * via the kernel.
   */
  plotSystem->psCreate            = gdisp_createPlotText;
  plotSystem->psDestroy           = gdisp_destroyPlotText;
  plotSystem->psSetParent         = gdisp_setPlotTextParent;
  plotSystem->psGetTopLevelWidget = gdisp_getPlotTextTopLevelWidget;
  plotSystem->psShow              = gdisp_showPlotText;
  plotSystem->psGetType           = gdisp_getPlotTextType;
  plotSystem->psAddSymbols        = gdisp_addSymbolsToPlotText;
  plotSystem->psGetSymbols        = gdisp_getSymbolsFromPlotText;
  plotSystem->psSetDimensions     = gdisp_setPlotTextInitialDimensions;
  plotSystem->psStartStep         = gdisp_startStepOnPlotText;
  plotSystem->psStep              = gdisp_stepOnPlotText;
  plotSystem->psStopStep          = gdisp_stopStepOnPlotText;
  plotSystem->psGetInformation    = gdisp_getPlotTextInformation;
  plotSystem->psTreatSymbolValues = gdisp_treatPlotTextSymbolValues;
  plotSystem->psGetPeriod         = gdisp_getPlotTextPeriod;
  plotSystem->psGetDropZones      = gdisp_getPlotTextDropZones;

}


