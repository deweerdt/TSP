/*!  \file 

$Id: gdisp_sampledSymbols.c,v 1.2 2004-05-11 19:47:43 esteban Exp $

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

File      : Information / Actions upon available sampled symbols.

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
 * Graphically show the status of all sampled symbols.
 */
static void
gdisp_poolSampledSymbolList ( Kernel_T *kernel )
{

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * These two procedures deal with sampled symbol list.
 * In order to graphically show all sampled symbols, a procedure
 * is registered in order to pool sampled symbol list.
 * The procedure is registered when the sampled symbol page is show
 * in the databook. The procedure is unregistered when other pages are
 * shown in the databook.
 */
void
gdisp_sampledSymbolTimer ( Kernel_T  *kernel,
			   gboolean   timerIsStarted )
{

#if defined(PROVIDER_DEBUG)

  fprintf(stdout,
	  "Sampled Symbol Timer : %s\n",
	  timerIsStarted == TRUE ? "STARTED" : "STOPPED");
  fflush (stdout);

#endif

  switch (timerIsStarted) {

  case TRUE :

    /* refresh immediately */
    gdisp_poolSampledSymbolList(kernel);

    (*kernel->registerAction)(kernel,
			      gdisp_poolSampledSymbolList);

    break;

  case FALSE :

    /* stop refreshing thread status */
    (*kernel->unRegisterAction)(kernel,
				gdisp_poolSampledSymbolList);

    break;

  }

}


/*
 * Create GDISP+ sampled symbol list.
 */
void
gdisp_createSampledSymbolList ( Kernel_T  *kernel,
				GtkWidget *parent )
{

  GtkWidget        *pFrame           =    (GtkWidget*)NULL;
  GtkWidget        *sFrame           =    (GtkWidget*)NULL;
  GtkWidget        *pVBox            =    (GtkWidget*)NULL;
  GtkWidget        *sVBox            =    (GtkWidget*)NULL;
  GtkWidget        *sHBox            =    (GtkWidget*)NULL;
  GtkWidget        *scrolledWindow   =    (GtkWidget*)NULL;
  GtkWidget        *sLabel           =    (GtkWidget*)NULL;
  GtkWidget        *sEntry           =    (GtkWidget*)NULL;

  GList            *providerItem     =        (GList*)NULL;
  Provider_T       *provider         =   (Provider_T*)NULL;
  Symbol_T         *symbol           =     (Symbol_T*)NULL;
  gint              pSampleCpt       =                   0;
  SampleList_T     *pSampleList      = (SampleList_T*)NULL;
  guint             pSampleMax       =                   0;
  gchar             rowBuffer[128];


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for sampled symbols.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  sFrame = gtk_frame_new(" Sampled Symbols ");
  gtk_frame_set_label_align(GTK_FRAME(sFrame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(sFrame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(parent),sFrame);
  gtk_widget_show(sFrame);


  /* ------- SCROLLED WINDOW FOR THE LIST OF SAMPLED SYMBOLS --------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_container_add(GTK_CONTAINER(sFrame),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /* --------- VERTICAL BOX FOR HANDLING ALL PROVIDERS --------- */

  /*
   * We need a vertical packing box for managing all providers.
   */
  pVBox = gtk_vbox_new(FALSE, /* homogeneous */
		       5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(pVBox),10);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					pVBox);
  gtk_widget_show(pVBox);


  /* ------------------------ PER PROVIDER ---------------------- */

  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /* ------------------------ FRAME WITH LABEL ------------------------ */

    /*
     * Create a Frame that will contain all provider information.
     * Align the label at the left of the frame.
     * Set the style of the frame.
     */
    sprintf(rowBuffer,
	    " %d : %s ",
	    provider->pIdentity + 1,
	    provider->pName->str);

    pFrame = gtk_frame_new(rowBuffer);

    gtk_frame_set_label_align(GTK_FRAME(pFrame),0.1,0.0);
    gtk_frame_set_shadow_type(GTK_FRAME(pFrame),GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(pVBox),
		       pFrame,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */
    gtk_widget_show(pFrame);


    /* ------------------------ VERTICAL BOX ------------------------ */

    /*
     * Create a vertical packing box for packing symbols.
     */
    sVBox = gtk_vbox_new(FALSE, /* homogeneous */
			 5      /* spacing     */ );
    gtk_container_border_width(GTK_CONTAINER(sVBox),10);
    gtk_container_add(GTK_CONTAINER(pFrame),sVBox); 
    gtk_widget_show(sVBox);


    /* --------------------- PER SAMPLED SYMBOLS ---------------- */

    /*
     * A frame per sampled symbol of the current provider.
     */
    pSampleList = &provider->pSampleList;
    pSampleMax  = pSampleList->len;

    for (pSampleCpt=0; pSampleCpt<pSampleMax; pSampleCpt++) {

      sprintf(rowBuffer," %s ",pSampleList->val[pSampleCpt].name);
      sFrame = gtk_frame_new(rowBuffer);

      gtk_frame_set_label_align(GTK_FRAME(sFrame),0.1,0.0);
      gtk_frame_set_shadow_type(GTK_FRAME(sFrame),GTK_SHADOW_ETCHED_IN);
      gtk_box_pack_start(GTK_BOX(sVBox),
			 sFrame,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      gtk_widget_show(sFrame);


      /* --------------------- HORIZONTAL BOX --------------------- */

      /*
       * Create a horizontal packing box for packing symbol information.
       */
      sHBox = gtk_hbox_new(FALSE, /* homogeneous */
			   5      /* spacing     */ );
      gtk_container_border_width(GTK_CONTAINER(sHBox),5);
      gtk_container_add(GTK_CONTAINER(sFrame),sHBox); 
      gtk_widget_show(sHBox);


      /* ------------------ LABEL FOR INFORMATION ------------------- */

      /*
       * Get in touch with the symbol through the global index.
       */
      symbol = &provider->pSymbolList[pSampleList->val[pSampleCpt].index];

      sLabel = gtk_label_new("Reference");
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sLabel,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      gtk_widget_show(sLabel);

      sEntry = gtk_entry_new();
      gtk_widget_set_usize(sEntry,25,20);
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sEntry,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      sprintf(rowBuffer,"%d",symbol->sReference);
      gtk_entry_set_max_length (GTK_ENTRY   (sEntry),        2);
      gtk_entry_set_text       (GTK_ENTRY   (sEntry),rowBuffer);
      gtk_editable_set_editable(GTK_EDITABLE(sEntry),    FALSE);
      gtk_widget_show(sEntry);

      sLabel = gtk_label_new("Period");
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sLabel,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      gtk_widget_show(sLabel);

      sEntry = gtk_entry_new();
      gtk_widget_set_usize(sEntry,25,20);
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sEntry,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      sprintf(rowBuffer,"%d",symbol->sInfo.period);
      gtk_entry_set_max_length (GTK_ENTRY   (sEntry),        2);
      gtk_entry_set_text       (GTK_ENTRY   (sEntry),rowBuffer);
      gtk_editable_set_editable(GTK_EDITABLE(sEntry),    FALSE);
      gtk_widget_show(sEntry);

      sLabel = gtk_label_new("Phase");
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sLabel,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      gtk_widget_show(sLabel);

      sEntry = gtk_entry_new();
      gtk_widget_set_usize(sEntry,25,20);
      gtk_box_pack_start(GTK_BOX(sHBox),
			 sEntry,
			 FALSE, /* expand  */
			 FALSE, /* fill    */
			 0);    /* padding */
      sprintf(rowBuffer,"%d",symbol->sInfo.phase);
      gtk_entry_set_max_length (GTK_ENTRY   (sEntry),        2);
      gtk_entry_set_text       (GTK_ENTRY   (sEntry),rowBuffer);
      gtk_editable_set_editable(GTK_EDITABLE(sEntry),    FALSE);
      gtk_widget_show(sEntry);

    }

    /*
     * Next provider.
     */
    providerItem = g_list_next(providerItem);

  }

}


