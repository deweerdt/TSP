/*!  \file 

$Id: gdisp_dataBook.c,v 1.1 2004-03-26 21:09:17 esteban Exp $

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

File      : Notebook gathering symbols, providers, plots.

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
gdispManageDeleteEventFromWM (GtkWidget *symbolWindow,
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
gdispDestroySignalHandler (GtkWidget *dataBookWindow,
			   gpointer   data)
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * Stop provider GTK timer.
   */
  gdisp_providerTimer     (kernel,FALSE /* timerIsStoped */);
  gdisp_sampledSymbolTimer(kernel,FALSE /* timerIsStoped */);

  /*
   * Close and destroy data book.
   */
  gtk_widget_destroy(dataBookWindow);
  kernel->widgets.dataBookWindow      = (GtkWidget*)NULL;

  /*
   * Reset all widgets.
   */
  kernel->widgets.dataBookWidget      = (GtkWidget*)NULL;
  kernel->widgets.dataBookApplyButton = (GtkWidget*)NULL;
  kernel->widgets.symbolCList         = (GtkWidget*)NULL;
  kernel->widgets.symbolFrame         = (GtkWidget*)NULL;
  kernel->widgets.pRadioButton        = (GtkWidget*)NULL;
  kernel->widgets.naRadioButton       = (GtkWidget*)NULL;
  kernel->widgets.ndRadioButton       = (GtkWidget*)NULL;
  kernel->widgets.uRadioButton        = (GtkWidget*)NULL;
  kernel->widgets.spRadioButton       = (GtkWidget*)NULL;
  kernel->widgets.apRadioButton       = (GtkWidget*)NULL;
  kernel->widgets.filterEntry         = (GtkWidget*)NULL;

}


/*
 * Apply button callback.
 */
static void
gdisp_applyChangesCallback (GtkWidget *applyButtonWidget,
			    gpointer   data )
{

  /*
   * These definitions must match the order of page creation.
   */
#define GD_ALL_SYMBOL_PAGE     0
#define GD_SAMPLED_SYMBOL_PAGE 1
#define GD_PROVIDER_PAGE       2
#define GD_GRAPHIC_PLOT_PAGE   3

  Kernel_T *kernel     = (Kernel_T*)data;
  gint      pageNumber = 0;

  /*
   * Activate the "apply" callback according to the selected page.
   */
  pageNumber =
  gtk_notebook_get_current_page(GTK_NOTEBOOK(kernel->widgets.dataBookWidget));

  switch (pageNumber) {

  case GD_ALL_SYMBOL_PAGE     :
    gdisp_symbolApplyCallback(kernel->widgets.dataBookApplyButton,
			      (gpointer)kernel);
    break;

  case GD_SAMPLED_SYMBOL_PAGE :
  case GD_PROVIDER_PAGE       :
  case GD_GRAPHIC_PLOT_PAGE   :
  default                     :
    break;

  }

}


/*
 * The current page of the data book has changed,
 * OR a new page has been inserted into the databook.
 */
static void
gdisp_switchDataBookPageCallback ( GtkNotebook     *notebook,
				   GtkNotebookPage *page,
				   gint             pageNumber,
				   gpointer         data )
{

  Kernel_T *kernel = (Kernel_T*)data;

  /*
   * This callback is activated whenever a new page is inserted
   * into the notebook widget.
   * Thus, the apply button is not already created.
   */
  if (kernel->widgets.dataBookApplyButton == (GtkWidget*)NULL) {

    return;

  }

  /*
   * Apply button is active when the symbol list is shown.
   */
  switch (pageNumber) {

  case GD_ALL_SYMBOL_PAGE     :
    gtk_widget_set_sensitive(kernel->widgets.dataBookApplyButton,
			     TRUE); /* active */
    break;

  case GD_SAMPLED_SYMBOL_PAGE :
  case GD_PROVIDER_PAGE       :
  case GD_GRAPHIC_PLOT_PAGE   :
  default                     :
    gtk_widget_set_sensitive(kernel->widgets.dataBookApplyButton,
			     FALSE); /* inactive */
    break;

  }

  /*
   * Registered provider status polling when provider page is shown.
   */
  gdisp_providerTimer(kernel,
		      pageNumber == GD_PROVIDER_PAGE ?
		      TRUE : FALSE);

  /*
   * Registered sampled symbol polling when sampled symbol page is shown.
   */
  gdisp_sampledSymbolTimer(kernel,
			   pageNumber == GD_SAMPLED_SYMBOL_PAGE ?
			   TRUE : FALSE);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Create GDISP+ data book.
 */
void
gdisp_showDataBook (gpointer factoryData,
		    guint    itemData)
{

  Kernel_T  *kernel      = (Kernel_T*)factoryData;

  GtkWidget *notebook    = (GtkWidget*)NULL;
  GtkWidget *mainVBox    = (GtkWidget*)NULL;
  GtkWidget *mainHBox    = (GtkWidget*)NULL;
  GtkWidget *mainChild   = (GtkWidget*)NULL;
  GtkWidget *label       = (GtkWidget*)NULL;
  GtkWidget *closeButton = (GtkWidget*)NULL;
  GtkWidget *applyButton = (GtkWidget*)NULL;

  assert(kernel);


  /*
   * If provider list is already on the screen, just raise its window.
   */
  if (kernel->widgets.dataBookWindow != (GtkWidget*)NULL) {

    gdk_window_raise(GTK_WIDGET(kernel->widgets.dataBookWindow)->window);

    return;

  }


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * The GTK_WINDOW_TOPLEVEL argument specifies that we want the window
   * to undergo window manager decoration and placement.
   */
  kernel->widgets.dataBookWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  assert(kernel->widgets.dataBookWindow);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.dataBookWindow),
		     "delete_event",
		     GTK_SIGNAL_FUNC(gdispManageDeleteEventFromWM),
		     (gpointer)kernel);

  gtk_signal_connect(GTK_OBJECT(kernel->widgets.dataBookWindow),
		     "destroy",
		     GTK_SIGNAL_FUNC(gdispDestroySignalHandler),
		     (gpointer)kernel);

  /*
   * Set up window title and border width.
   */
  gtk_window_set_title(GTK_WINDOW(kernel->widgets.dataBookWindow),
		       "GDISP+ Symbols / Providers / Graphic Plots");

  gtk_container_set_border_width(GTK_CONTAINER(kernel->widgets.dataBookWindow),
				 1);


  /* ------------------------ PACKING BOX ------------------------ */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  mainVBox = gtk_vbox_new(FALSE, /* homogeneous */
			  5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainVBox),3);
  gtk_container_add(GTK_CONTAINER(kernel->widgets.dataBookWindow),mainVBox);
  gtk_widget_show(mainVBox);


  /* ------------------------ MAIN WINDOW ------------------------ */

  /*
   * Map top-level window.
   */
  gtk_widget_show(kernel->widgets.dataBookWindow);


  /* ------------------------ DATA BOOK ------------------------ */

  /*
   * Create a notebook widget, place the position of the tabs.
   */
  notebook = gtk_notebook_new ();
  gtk_notebook_set_tab_pos(GTK_NOTEBOOK(notebook),GTK_POS_TOP);
  gtk_box_pack_start(GTK_BOX(mainVBox),
		     notebook,
		     TRUE  /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);

  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));

  gtk_widget_show(notebook);

  gtk_signal_connect(GTK_OBJECT(notebook),
		     "switch-page",
		     GTK_SIGNAL_FUNC(gdisp_switchDataBookPageCallback),
		     (gpointer)kernel);

  kernel->widgets.dataBookWidget = notebook;


  /* -------------- PACKING BOX CHILD FOR SYMBOL LIST -------------- */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  label     = gtk_label_new("All Symbols");
  mainChild = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainChild),3);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			   mainChild,
			   label);
  gtk_widget_show(mainChild);

  /*
   * Insert the symbol list into the databook.
   */
  gdisp_createSymbolList(kernel,mainChild);


  /* -------------- PACKING BOX CHILD FOR SAMPLED SYMBOLS -------------- */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  label     = gtk_label_new("Sampled Symbols");
  mainChild = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainChild),3);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			   mainChild,
			   label);
  gtk_widget_show(mainChild);

  /*
   * Insert the sampled symbol list into the databook.
   */
  gdisp_createSampledSymbolList(kernel,mainChild);


  /* -------------- PACKING BOX CHILD FOR PROVIDER LIST -------------- */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  label     = gtk_label_new("Providers");
  mainChild = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainChild),3);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			   mainChild,
			   label);
  gtk_widget_show(mainChild);

  /*
   * Insert the symbol list into the databook.
   */
  gdisp_createProviderList(kernel,mainChild);


  /* -------------- PACKING BOX CHILD FOR GRAPHIC PLOTS -------------- */

  /*
   * We need a vertical packing box for managing all widgets.
   */
  label     = gtk_label_new("Graphic Plots");
  mainChild = gtk_vbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(mainChild),3);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
			   mainChild,
			   label);
  gtk_widget_show(mainChild);

  /*
   * Insert the symbol list into the databook.
   */
  gdisp_createGraphicList(kernel,mainChild);


  /* ------------------------ PACKING BOX ------------------------ */

  mainHBox = gdisp_createButtonBar(kernel->widgets.dataBookWindow,
				   &applyButton,
				   &closeButton);

  gtk_box_pack_start(GTK_BOX(mainVBox),
		     mainHBox,
		     FALSE /* expand  */,
		     TRUE  /* fill    */,
		     0     /* padding */);

  gtk_signal_connect(GTK_OBJECT(applyButton),
		     "clicked",
		     GTK_SIGNAL_FUNC(gdisp_applyChangesCallback),
		     (gpointer)kernel);

  gtk_signal_connect_object(GTK_OBJECT(closeButton),
			    "clicked",
			    (GtkSignalFunc)gtk_widget_destroy,
			    GTK_OBJECT(kernel->widgets.dataBookWindow));

  /*
   * This grabs this button to be the default button.
   * Simply hitting the "Enter" key will cause this button to activate.
   */
  GTK_WIDGET_SET_FLAGS(applyButton,GTK_CAN_DEFAULT);
  GTK_WIDGET_SET_FLAGS(closeButton,GTK_CAN_DEFAULT);
  gtk_widget_grab_default(closeButton);

  /*
   * Remember apply button identity.
   */
  kernel->widgets.dataBookApplyButton = applyButton;

  /*
   * Temporary.
   */
  gtk_widget_set_sensitive(gtk_notebook_get_nth_page(GTK_NOTEBOOK(notebook),
						     1),
			   FALSE);

}

