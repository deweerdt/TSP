/*!  \file 

$Id: gdisp_symbols.c,v 1.4 2004-06-17 20:03:02 esteban Exp $

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


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Insert all available symbols into the Gtk CList.
 * Take care of requested sorting method.
 */
static gint
gdispInsertAndSortSymbols (Kernel_T  *kernel,
			   gchar    **stringFilter,
			   guint      stringFilterSize)
{

#define GD_MAKE_DIFFERENCE_WITH_PIXMAP

  GList      *providerItem   =      (GList*)NULL;
  Symbol_T   *symbolPtr      =   (Symbol_T*)NULL;
  Provider_T *provider       = (Provider_T*)NULL;
  gint        symbolCpt      =                 0,
              rowNumber      =                 0;
  gchar      *symbolInfos[_SYMBOL_CLIST_COLUMNS_NB_] =
                                                    { (gchar*)NULL,
						      "n/a",
						      "undefined" };
#if defined(GD_MAKE_DIFFERENCE_WITH_PIXMAP)
  GdkPixmap  *idPixmap       = (GdkPixmap*)NULL;
  GdkBitmap  *idPixmapMask   = (GdkBitmap*)NULL;
#endif

  assert(kernel);

  /* ------------------------ LIST OF SYMBOLS ------------------------ */

  gtk_clist_freeze(GTK_CLIST(kernel->widgets.symbolCList));
  gtk_clist_clear (GTK_CLIST(kernel->widgets.symbolCList));


  /*
   * When sorting method is GD_SORT_BY_PROVIDER or GD_SORT_BY_NAME,
   * loop over all provider, then loop over all provider symbols.
   */
  if (kernel->sortingMethod == GD_SORT_BY_PROVIDER     ||
      kernel->sortingMethod == GD_SORT_BY_NAME         ||
      kernel->sortingMethod == GD_SORT_BY_NAME_REVERSE    ) {
 
    /*
     * Insert all symbols into the cList.
     */
    providerItem = g_list_first(kernel->providerList);
    while (providerItem != (GList*)NULL) {

      provider = (Provider_T*)providerItem->data;
      if (provider->pStatus == GD_SESSION_OPENED) {

	symbolPtr = provider->pSymbolList;
	for (symbolCpt=0; symbolCpt<provider->pSymbolNumber; symbolCpt++) {

	  if (stringFilterSize == 0 /* insert all symbols */ ||
	      gdisp_strStr(symbolPtr->sInfo.name,
			   stringFilter,
			   stringFilterSize) != (gchar*)NULL ) {

	    symbolInfos[0] = symbolPtr->sInfo.name;

	    rowNumber =
	      gtk_clist_append(GTK_CLIST(kernel->widgets.symbolCList),
			       symbolInfos);

	    /*
	     * If there are several providers, show symbol appartenance.
	     */
	    if (gdisp_getProviderNumber(kernel) > 1) {

#if defined(GD_MAKE_DIFFERENCE_WITH_PIXMAP)

	      gdisp_getProviderIdPixmap(kernel,
					kernel->widgets.symbolCList,
					provider->pIdentity,
					&idPixmap,
					&idPixmapMask);

	      gtk_clist_set_pixtext(GTK_CLIST(kernel->widgets.symbolCList),
				    rowNumber,
				    0, /* first column */
				    symbolPtr->sInfo.name,
				    5, /* spacing */
				    idPixmap,
				    idPixmapMask);

#else

	      /*
	       * Provider color may have not been calculated before.
	       * Provider color still may be null if the maximum number
	       * of providers is excedeed. Cf. << gdisp_colormap.c >>
	       */
	      if (provider->pColor == (GdkColor*)NULL)
		provider->pColor = gdisp_getProviderColor(kernel,
							  provider->pIdentity);

	      gtk_clist_set_background(GTK_CLIST(kernel->widgets.symbolCList),
				       rowNumber, /* row */
				       provider->pColor);

#endif

	    }

	    /*
	     * Remember here the symbol the row refers to.
	     */
	    gtk_clist_set_row_data(GTK_CLIST(kernel->widgets.symbolCList),
				   rowNumber, /* row */
				   (gpointer)symbolPtr);

	  } /* filter */

	  symbolPtr++;

	} /* symbolCpt */

      } /* provider status */

      providerItem = g_list_next(providerItem);

    }

  }


  /*
   * If sorting method is GD_SORT_BY_NAME, keep on
   * sorting all symbols into the CList alphabetically.
   */
  if (kernel->sortingMethod == GD_SORT_BY_NAME         ||
      kernel->sortingMethod == GD_SORT_BY_NAME_REVERSE    ) {

    /*
     * Specify sorting method.
     */
    gtk_clist_set_sort_column(GTK_CLIST(kernel->widgets.symbolCList),
			      0 /* first column */ );

    gtk_clist_set_sort_type(GTK_CLIST(kernel->widgets.symbolCList),
			    kernel->sortingMethod == GD_SORT_BY_NAME ?
			    GTK_SORT_ASCENDING : GTK_SORT_DESCENDING);

    gtk_clist_sort(GTK_CLIST(kernel->widgets.symbolCList));

  }


  /*
   * Allow graphic refreshes.
   */
  gtk_clist_thaw(GTK_CLIST(kernel->widgets.symbolCList));

  return (rowNumber + 1);

}


/*
 * This callback is called whenever a sorting-method button is pressed.
 * The argument "data" is the kernel itself.
 * The only way to determine which button has been pressed (ie the
 * sorting method  which is requested), is to compare the argument
 * "widget" (the target button) to all radio button pointers stored
 * into the kernel.
 */
static void
gdisp_changeSortingMethodCallback (GtkWidget *radioButtonWidget,
				   gpointer   data )
{

  Kernel_T *kernel = (Kernel_T*)data;


  /*
   * Compare 'radioButtonWidget' with radio buttons in the kernel.
   */
  if (radioButtonWidget == kernel->widgets.pRadioButton) {

    kernel->sortingMethod = GD_SORT_BY_PROVIDER;

  }
  else if (radioButtonWidget == kernel->widgets.naRadioButton) {

    kernel->sortingMethod = GD_SORT_BY_NAME;

  }
  else if (radioButtonWidget == kernel->widgets.ndRadioButton) {

    kernel->sortingMethod = GD_SORT_BY_NAME_REVERSE;

  }
  else {

    /* no check because this should never happend...  */

  }

}


/*
 * This callback is called whenever a DnD-scope button is pressed.
 * The argument "data" is the kernel itself.
 * The only way to determine which button has been pressed (ie the
 * DnD-scope which is requested), is to compare the argument
 * "widget" (the target button) to all radio button pointers stored
 * into the kernel.
 */
static void
gdisp_changeDnDScopeCallback (GtkWidget *radioButtonWidget,
			      gpointer   data )
{

  Kernel_T *kernel = (Kernel_T*)data;


  /*
   * Compare 'radioButtonWidget' with radio buttons in the kernel.
   */
  if (radioButtonWidget == kernel->widgets.uRadioButton) {

    kernel->dndScope = GD_DND_UNICAST;

  }
  else if (radioButtonWidget == kernel->widgets.spRadioButton) {

    kernel->dndScope = GD_DND_MULTICAST;

  }
  else if (radioButtonWidget == kernel->widgets.apRadioButton) {

    kernel->dndScope = GD_DND_BROADCAST;

  }
  else {

    /* no check because this should never happend...  */

  }

}


/*
 * DND "drag_begin" handler : this is called whenever a drag starts.
 */
static void
gdisp_beginDNDCallback (GtkWidget      *widget /* symbol cList */,
			GdkDragContext *dragContext,
			gpointer        data)
{

  Kernel_T *kernel       = (Kernel_T*)data;
  GList    *selectedRows =    (GList*)NULL;
  guint     row          =               0;


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
   * Retreive symbols that are selected and put them into the
   * kernel DND selection list.
   */
  assert(kernel->dndSelection == (GList*)NULL);

  selectedRows =
    g_list_first(GTK_CLIST(kernel->widgets.symbolCList)->selection);

  while (selectedRows != (GList*)NULL) {

    row = (guint)selectedRows->data;

    kernel->dndSelection =
      g_list_append(
           kernel->dndSelection,
	   gtk_clist_get_row_data(GTK_CLIST(kernel->widgets.symbolCList),
				  row));

    selectedRows = g_list_next(selectedRows);

  }
  
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
  if (strlen(filter) == 0)
    filter = (gchar*)NULL;

  gdisp_getStringTableFromStringList(filter,
				     &stringTable,
				     &stringTableSize);

  /*
   * Apply changes, with a possible filter.
   */
  symbolCount = gdispInsertAndSortSymbols(kernel,
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

  GtkWidget      *otherVBox         =   (GtkWidget*)NULL;
  GtkWidget      *otherHBox         =   (GtkWidget*)NULL;
  GtkWidget      *frame             =   (GtkWidget*)NULL;
  GtkWidget      *scrolledWindow    =   (GtkWidget*)NULL;
  GtkWidget      *hSeparator        =   (GtkWidget*)NULL;
  GtkWidget      *label             =   (GtkWidget*)NULL;
  GtkTooltips    *toolTipGroup      = (GtkTooltips*)NULL;

  GtkTargetEntry  targetEntry;

  GSList         *radioButtonGroup  = (GSList*)NULL;
  gboolean        radioStatuses[]   = { TRUE, FALSE, FALSE };

  GString        *messageString     = (GString*)NULL;

  gint            symbolCount       = 0;
  GtkWidget      *cList             = (GtkWidget*)NULL;
  gchar          *cListTitles[_SYMBOL_CLIST_COLUMNS_NB_] =
                                                      { "Name",
							"Unit",
							"Comment" };

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
  gtk_widget_set_usize(scrolledWindow,350,300);
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     scrolledWindow,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(scrolledWindow);


  /* -------------- CLIST FOR SYMBOL INFORMATION --------------- */

  /*
   * Create the CList. For our needs, we use 2 columns.
   */
  cList = gtk_clist_new_with_titles(_SYMBOL_CLIST_COLUMNS_NB_,cListTitles);
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
			       GTK_SELECTION_MULTIPLE);

  /*
   * What however is important, is that we set the column widths as
   * they will never be right otherwise.
   * Note that the columns are numbered from 0 and up (to 2 in our case).
   */
  gtk_clist_set_column_width(GTK_CLIST(cList),0,200);
  gtk_clist_set_column_width(GTK_CLIST(cList),1,050);

  gtk_clist_set_button_actions(GTK_CLIST(cList),
			       0, /* left button performs selection & DND */
			       GTK_BUTTON_SELECTS | GTK_BUTTON_DRAGS);
  gtk_clist_set_use_drag_icons(GTK_CLIST(cList),
                               TRUE /* use icons */);

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
		     kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_end",
		     GTK_SIGNAL_FUNC(gdisp_endDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_get",
		     GTK_SIGNAL_FUNC(gdisp_dataRequestDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_received",
		     GTK_SIGNAL_FUNC(gdisp_dataReceivedDNDCallback),
		     kernel);

  gtk_signal_connect(GTK_OBJECT(cList),
		     "drag_data_delete",
		     GTK_SIGNAL_FUNC(gdisp_dataDestroyedDNDCallback),
		     kernel);


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


  /* ------------------------ ANOTHER V BOX ----------------------- */

  /*
   * We need a vertical packing box.
   */
  otherVBox = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherVBox),3);
  gtk_container_add(GTK_CONTAINER(frame),otherVBox);
  gtk_widget_show(otherVBox);


  /* ------------- HORIZONTAL PACKING BOX INTO THE FRAME ------------- */

  /*
   * We need a horizontal packing box for managing the radio buttons.
   */
  otherHBox = gtk_hbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherHBox),3);
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     otherHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(otherHBox);


  /* ---------------- RADIO BUTTON FOR SORTING METHODS --------------- */

  /*
   * Just a label to better understand.
   */
  label = gtk_label_new("Sort By   ");
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     label,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(label);

  /*
   * Sort by provider (default).
   */
  kernel->widgets.pRadioButton = gtk_radio_button_new_with_label((GSList*)NULL,
								 "Provider");
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.pRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.pRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.pRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeSortingMethodCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.pRadioButton,
		       "Sort all symbols by provider",
		       "");

  /*
   * This radio button must be set to "no sensitive" when only one
   * provider is available.
   * Then choose correct sorting method.
   */
  if (gdisp_getProviderNumber(kernel) == 1) {

    gtk_widget_set_sensitive(kernel->widgets.pRadioButton,
			     FALSE /* not sensitive */);
    if (kernel->sortingMethod == GD_SORT_BY_PROVIDER)
      kernel->sortingMethod = GD_SORT_BY_NAME;

  }

  /*
   * Sort by name (ascending).
   */
  radioButtonGroup =
    gtk_radio_button_group(GTK_RADIO_BUTTON(kernel->widgets.pRadioButton));

  kernel->widgets.naRadioButton =
    gtk_radio_button_new_with_label(radioButtonGroup,
				    "Name Ascending");

  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.naRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.naRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.naRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeSortingMethodCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.naRadioButton,
		       "Sort all symbols alphabetically",
		       "");

  /*
   * Sort by name (descending).
   */
  radioButtonGroup =
    gtk_radio_button_group(GTK_RADIO_BUTTON(kernel->widgets.naRadioButton));

  kernel->widgets.ndRadioButton =
    gtk_radio_button_new_with_label(radioButtonGroup,
				    "Name Descending");

  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.ndRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.ndRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.ndRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeSortingMethodCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.ndRadioButton,
		       "Sort all symbols reverse-alphabetically",
		       "");

  /*
   * Take care of current sorting method in the kernel.
   */
  switch (kernel->sortingMethod) {

  case GD_SORT_BY_PROVIDER :
    radioStatuses[0 /* sort by provider        */] = TRUE;
    radioStatuses[1 /* sort by name ascending  */] = FALSE;
    radioStatuses[2 /* sort by name descending */] = FALSE;
    break;

  case GD_SORT_BY_NAME         :
    radioStatuses[0 /* sort by provider        */] = FALSE;
    radioStatuses[1 /* sort by name ascending  */] = TRUE;
    radioStatuses[2 /* sort by name descending */] = FALSE;
    break;

  case GD_SORT_BY_NAME_REVERSE :
    radioStatuses[0 /* sort by provider        */] = FALSE;
    radioStatuses[1 /* sort by name ascending  */] = FALSE;
    radioStatuses[2 /* sort by name descending */] = TRUE;
    break;

  default :
    break;

  }

  gtk_toggle_button_set_active(
                  GTK_TOGGLE_BUTTON(kernel->widgets.pRadioButton),
		  radioStatuses[0 /* sort by provider        */]);

  gtk_toggle_button_set_active(
                  GTK_TOGGLE_BUTTON(kernel->widgets.naRadioButton),
		  radioStatuses[1 /* sort by name ascending  */]);

  gtk_toggle_button_set_active(
                  GTK_TOGGLE_BUTTON(kernel->widgets.ndRadioButton),
		  radioStatuses[2 /* sort by name descending */]);


  /* -------------------- HORIZONTAL SEPARATOR --------------------- */

  hSeparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     hSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(hSeparator);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a horizontal packing box for managing the filter and the label.
   */
  otherHBox = gtk_hbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherHBox),5);
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     otherHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
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


  /* -------------------- HORIZONTAL SEPARATOR --------------------- */

  hSeparator = gtk_hseparator_new();
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     hSeparator,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(hSeparator);


  /* ------------- HORIZONTAL PACKING BOX INTO THE FRAME ------------- */

  /*
   * We need a horizontal packing box for managing the radio buttons.
   */
  otherHBox = gtk_hbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(otherHBox),3);
  gtk_box_pack_start(GTK_BOX(otherVBox),
		     otherHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(otherHBox);


  /* ---------------- RADIO BUTTON FOR DnD SCOPE --------------- */

  /*
   * Just a label to better understand.
   */
  label = gtk_label_new("Drag & Drop Scope       ");
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     label,
		     FALSE  /* expand  */,
		     FALSE /* fill    */,
		     0     /* padding */);
  gtk_widget_show(label);

  /*
   * Drag & Drop Scope is "Unique" (the target is a unique graphic plot).
   */
  kernel->widgets.uRadioButton = gtk_radio_button_new_with_label((GSList*)NULL,
								 "Unique");
  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.uRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.uRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.uRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeDnDScopeCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.uRadioButton,
		       "Only the graphic plot on which the drop action "
		       "occured will receive the new symbols",
		       "");

  /*
   * Drag & Drop Scope is "Single Page".
   * All graphic plots inside the page that received the drop action will
   * be given the symbols.
   */
  radioButtonGroup =
    gtk_radio_button_group(GTK_RADIO_BUTTON(kernel->widgets.uRadioButton));

  kernel->widgets.spRadioButton =
    gtk_radio_button_new_with_label(radioButtonGroup,
				    "Single Page");

  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.spRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.spRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.spRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeDnDScopeCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.spRadioButton,
		       "All graphic plots belonging to the page on which "
		       "the drop action occured will receive the new symbols",
		       "");


  /*
   * Drag & Drop Scope is "All Pages".
   * All graphic plots of all existing pages will be given the symbols.
   */
  radioButtonGroup =
    gtk_radio_button_group(GTK_RADIO_BUTTON(kernel->widgets.spRadioButton));

  kernel->widgets.apRadioButton =
    gtk_radio_button_new_with_label(radioButtonGroup,
				    "All Pages");

  gtk_box_pack_start(GTK_BOX(otherHBox),
		     kernel->widgets.apRadioButton,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);
  gtk_widget_show(kernel->widgets.apRadioButton);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.apRadioButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_changeDnDScopeCallback),
		     (gpointer)kernel);

  gtk_tooltips_set_tip(GTK_TOOLTIPS(toolTipGroup),
		       kernel->widgets.apRadioButton,
		       "All graphic plots of all graphic pages "
		       "will receive the new symbols",
		       "");


  /* ------------------------ LIST OF SYMBOLS  ------------------------ */

  /*
   * Insert symbols into the corresponding CList.
   * ...according to the default sorting method in the kernel...
   */
  symbolCount = gdispInsertAndSortSymbols(kernel,
					  (gchar**)NULL /* no filter */,
					  (guint)0      /* no filter */);


  /*
   * Keep in mind that the GString is released into the
   * 'gdispOutputWrite' function.
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
 * Destroy GDISP+ symbol list.
 */
void
gdisp_destroySymbolList ( Kernel_T *kernel )
{

  /*
   * Nothing by now.
   */

}
