/*!  \file 

$Id: gdisp_providers.c,v 1.4 2004-05-11 19:47:42 esteban Exp $

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

File      : Information / Actions upon available providers.

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


static gchar*
gdisp_providerStatusToString ( ThreadStatus_T status,
			       guint          *colorId )
{

  gchar *sStatus = (gchar*)NULL;

  switch (status) {

  case GD_THREAD_STOPPED :
    sStatus  = "STOPPED";
    *colorId = _WHITE_;
    break;

  case GD_THREAD_STARTING :
    sStatus = "STARTING...";
    *colorId = _ORANGE_;
    break;

  case GD_THREAD_WARNING :
    sStatus = "WARNING";
    *colorId = _ORANGE_;
    break;

  case GD_THREAD_REQUEST_SAMPLE_ERROR :
    sStatus = "REQUEST SAMPLE";
    *colorId = _RED_;
    break;

  case GD_THREAD_SAMPLE_INIT_ERROR :
    sStatus = "SAMPLE INIT";
    *colorId = _RED_;
    break;

  case GD_THREAD_SAMPLE_DESTROY_ERROR :
    sStatus = "SAMPLE DESTROY";
    *colorId = _RED_;
    break;

  case GD_THREAD_ERROR :
    sStatus = "THREAD ERROR";
    *colorId = _RED_;
    break;

  case GD_THREAD_RUNNING :
    sStatus = "RUNNING";
    *colorId = _GREEN_;
    break;

  default :
    sStatus = "UNKNOWN";
    *colorId = _GREY_;
    break;

  }

  return sStatus;

}


/*
 *  Graphically show the status of all providers.
 */
static void
gdisp_poolProviderThreadStatus ( Kernel_T *kernel )
{

  GList      *providerItem      =      (GList*)NULL;
  Provider_T *provider          = (Provider_T*)NULL;
  guint       providerLoad      = 0;
  gchar       rowBuffer[128];
  guint       bgColorId         = _WHITE_;


  /*
   * Loop over all available providers into the list.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    gtk_clist_set_text(GTK_CLIST(provider->pCList),
		       1 /* status row */,
		       1 /* information */,
	       gdisp_providerStatusToString(provider->pSamplingThreadStatus,
					    &bgColorId));

    gtk_clist_set_background(GTK_CLIST(provider->pCList),
			     1 /* status row */,
			     &kernel->colors[bgColorId]);

    sprintf(rowBuffer,"%d",provider->pSampleList.len);
    gtk_clist_set_text(GTK_CLIST(provider->pCList),
		       4 /* sample symbol row */,
		       1 /* information       */,
		       rowBuffer);

    /* ----------------------------------------------------------- */

    providerLoad    = provider->pLoad;
    provider->pLoad = 0; /* re-init for next cycle */

    if (providerLoad > provider->pMaxLoad) {

      /*
       * This may happend because GTK timers are not very precise...
       * So a 1 second period timer does not occur exactly every second.
       * So percentage may be higher than 100 %.
       */
      providerLoad = provider->pMaxLoad;

    }

    sprintf(rowBuffer,
	    "%d Bytes/s, %d %c",
	    providerLoad,
	    (guint)(100.0 * (gfloat)providerLoad / (gfloat)provider->pMaxLoad),
	    '%');

    gtk_clist_set_text(GTK_CLIST(provider->pCList),
		       5 /* provider load row */,
		       1 /* information       */,
		       rowBuffer);

    /* ----------------------------------------------------------- */

    providerItem = g_list_next(providerItem);

  } /* while (providerItem != (GList*)NULL) */

}


/*
 * Include Provider Logos.
 */
#include "pixmaps/gdisp_stubLogo.xpm"
#include "pixmaps/gdisp_resLogo.xpm"


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * These two procedures deal with provider thread management.
 * In order to graphically show the status of all providers, a procedure
 * is registered in order to pool provider thread status.
 * The procedure is registered when the provider page is show in the databook.
 * The procedure is unregistered when other pages are shown in the databook.
 */
void
gdisp_providerTimer ( Kernel_T  *kernel,
		      gboolean   timerIsStarted )
{

#if defined(PROVIDER_DEBUG)

  fprintf(stdout,
	  "Provider Timer : %s\n",
	  timerIsStarted == TRUE ? "STARTED" : "STOPPED");
  fflush (stdout);

#endif

  switch (timerIsStarted) {

  case TRUE :

    /* refresh immediately */
    gdisp_poolProviderThreadStatus(kernel);

    (*kernel->registerAction)(kernel,
			      gdisp_poolProviderThreadStatus);

    break;

  case FALSE :

    /* stop refreshing thread status */
    (*kernel->unRegisterAction)(kernel,
				gdisp_poolProviderThreadStatus);

    break;

  }

}


/*
 * Create GDISP+ provider list.
 */
void
gdisp_createProviderList ( Kernel_T  *kernel,
			   GtkWidget *parent )
{

  GtkWidget        *frame            =  (GtkWidget*)NULL;
  GtkWidget        *vBox             =  (GtkWidget*)NULL;
  GtkWidget        *hBox             =  (GtkWidget*)NULL;
  GtkWidget        *scrolledWindow   =  (GtkWidget*)NULL;
  GtkWidget        *pixmapWidget     =  (GtkWidget*)NULL;
  GdkPixmap        *pixmap           =  (GdkPixmap*)NULL;
  GdkBitmap        *mask             =  (GdkBitmap*)NULL;
  GtkStyle         *style            =   (GtkStyle*)NULL;

  GList            *providerItem     =      (GList*)NULL;
  Provider_T       *provider         = (Provider_T*)NULL;

  gchar            *rowInfo  [  2];
  guint             rowNumber        = 0;
  gchar             rowBuffer[128];
  guint             bgColorId        = _WHITE_;


  /* ------------------------ FRAME WITH LABEL ------------------------ */

  /*
   * Create a Frame that will contain a scrolled window for providers.
   * Align the label at the left of the frame.
   * Set the style of the frame.
   */
  frame = gtk_frame_new(" Available Providers ");
  gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
  gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);

  gtk_container_add(GTK_CONTAINER(parent),frame);
  gtk_widget_show(frame);


  /* ----------- SCROLLED WINDOW FOR THE LIST OF PROVIDERS  ----------- */

  /*
   * This is the scrolled window to put the List widget inside.
   */
  scrolledWindow = gtk_scrolled_window_new(NULL /* H Adjustment */,
					   NULL /* V Adjustment */);
  gtk_container_border_width(GTK_CONTAINER(scrolledWindow),5);
  gtk_container_add(GTK_CONTAINER(frame),scrolledWindow); 
  gtk_widget_show(scrolledWindow);


  /* ----------- VERTICAL BOX FOR HANDLING ALL PROVIDERS  ----------- */

  /*
   * We need a vertical packing box for managing all providers.
   */
  vBox = gtk_vbox_new(FALSE, /* homogeneous */
		      5      /* spacing     */ );
  gtk_container_border_width(GTK_CONTAINER(vBox),10);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolledWindow),
					vBox);
  gtk_widget_show(vBox);


  /* ---------------- PER PROVIDER ------------- PER PROVIDER --------- */

  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    provider->pColor = gdisp_getProviderColor(kernel,
					      provider->pIdentity);


    /* ------------------------ FRAME WITH LABEL ------------------------ */

    /*
     * Create a Frame that will contain all provider information.
     * Align the label at the left of the frame.
     * Set the style of the frame.
     */
    sprintf(rowBuffer," %d ",provider->pIdentity + 1);
    frame = gtk_frame_new(rowBuffer);

    gtk_frame_set_label_align(GTK_FRAME(frame),0.1,0.0);
    gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_ETCHED_IN);
    gtk_box_pack_start(GTK_BOX(vBox),
		       frame,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */
    gtk_widget_show(frame);


    /* ------------------------ HORIZONTAL BOX  ------------------------ */

    /*
     * Create a horizontal packing box.
     */
    hBox = gtk_hbox_new(FALSE, /* homogeneous */
			5      /* spacing     */ );
    gtk_container_border_width(GTK_CONTAINER(hBox),10);
    gtk_container_add(GTK_CONTAINER(frame),hBox); 
    gtk_widget_show(hBox);


    /* ---------------------- PROVIDER LOGO ---------------------- */

    /*
     * Use GDK services to create provider Logo (XPM format).
     */
    style  = gtk_widget_get_style(scrolledWindow);
    pixmap = gdk_pixmap_create_from_xpm_d(scrolledWindow->window,
					  &mask,
					  &style->bg[GTK_STATE_NORMAL],
					  (gchar**)gdisp_stubLogo);

    pixmapWidget = gtk_pixmap_new(pixmap,mask);
    gtk_box_pack_start(GTK_BOX(hBox),
		       pixmapWidget,
		       FALSE, /* expand  */
		       FALSE, /* fill    */
		       0);    /* padding */
    gtk_widget_show(pixmapWidget);


    /* -------------------------- CLIST --------------------------- */

    /*
     * A CList for containing all information.
     */
    provider->pCList = gtk_clist_new(2 /* columns */);

    gtk_clist_set_shadow_type(GTK_CLIST(provider->pCList),
			      GTK_SHADOW_ETCHED_IN);

    gtk_clist_set_button_actions(GTK_CLIST(provider->pCList),
				 0, /* left button */
				 GTK_BUTTON_IGNORED);

    gtk_clist_set_column_auto_resize(GTK_CLIST(provider->pCList),
				     0, /* first column */
				     TRUE);

    /* ------------------ LABELS WITH INFORMATION ------------------- */

    rowInfo[0] = "Name";
    rowInfo[1] = provider->pName->str;

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    if (gdisp_getProviderNumber(kernel) > 1) {

      gdisp_getProviderIdPixmap(kernel,
				provider->pCList,
				provider->pIdentity,
				&pixmap,
				&mask);

      gtk_clist_set_pixtext(GTK_CLIST(provider->pCList),
			    rowNumber,
			    1, /* second column */
			    provider->pName->str,
			    5, /* spacing */
			    pixmap,
			    mask);

    }

    rowInfo[0] = "Status";
    rowInfo[1] = gdisp_providerStatusToString(provider->pSamplingThreadStatus,
					      &bgColorId);

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    rowInfo[1] = rowBuffer;

    rowInfo[0] = "Base Frequency";
    sprintf(rowInfo[1],"%3.0f",provider->pBaseFrequency);

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    rowInfo[0] = "Total Symbols";
    sprintf(rowInfo[1],"%d",provider->pSymbolNumber);

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    rowInfo[0] = "Sampled Symbols";
    sprintf(rowInfo[1],"%d",provider->pSampleList.len);

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    /* ----------------------------------------------------------- */

    rowInfo[0] = "Detected Flow";
    sprintf(rowInfo[1],"0 Bytes/s, 0 %c",'%');

    rowNumber  = gtk_clist_append(GTK_CLIST(provider->pCList),
				  rowInfo);

    /* ----------------------------------------------------------- */

    gtk_box_pack_start(GTK_BOX(hBox),
		       provider->pCList,
		       TRUE, /* expand  */
		       TRUE, /* fill    */
		       0);   /* padding */

    gtk_widget_show(provider->pCList);


    /*
     * Next provider.
     */
    providerItem = g_list_next(providerItem);

  }

}


