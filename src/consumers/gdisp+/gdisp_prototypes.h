/*!  \file 

$Id: gdisp_prototypes.h,v 1.1 2004-02-04 20:32:10 esteban Exp $

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

File      : Definition of all prototypes.

-----------------------------------------------------------------------
*/

#ifndef __PROTOTYPES_H__
#define __PROTOTYPES_H__


/*
 * From gdisp_kernel.c
 */
Kernel_T  *gdisp_createKernel    (int argc, char **argv);
void       gdisp_destroyKernel   (Kernel_T *kernel);


/*
 * From gdisp_sampling.c
 */
void       gdisp_startSamplingProcess (Kernel_T *kernel);
void       gdisp_stopSamplingProcess  (Kernel_T *kernel);


/*
 * From gdisp_mainBoard.c
 */
void       gdisp_createMainBoard         (Kernel_T *kernel);
void       gdisp_writeInitialInformation (Kernel_T *kernel);


/*
 * From gdisp_providers.c
 */
void       gdisp_showProviderList (gpointer factoryData,
				   guint    itemData);


/*
 * From gdisp_colormap.c
 */
void       gdisp_createColormap  (Kernel_T *kernel);
void       gdisp_destroyColormap (Kernel_T *kernel);
GdkColor  *gdisp_getProviderColor(Kernel_T *kernel,
				  gint      providerId);

/*
 * From gdisp_consumers.c
 */
void       gdisp_consumingInit     (Kernel_T *kernel);
guint      gdisp_getProviderNumber (Kernel_T *kernel);
void       gdisp_consumingEnd      (Kernel_T *kernel);

/*
 * From gdisp_symbols.c
 */
void       gdisp_showSymbolList (gpointer factoryData,
				 guint    itemData);

/*
 * From gdisp_pages.c
 */
void       gdisp_createGraphicPage  (gpointer  factoryData,
				     guint     pageDimension);
GList*     gdisp_getSymbolsInPages (Kernel_T *kernel);

/*
 * From gdisp_defaultPlot.c
 */
void       gdisp_initDefaultPlotSystem (Kernel_T     *kernel,
					PlotSystem_T *plotSystem);

/*
 * From gdisp_plot2D.c
 */
void       gdisp_initPlot2DSystem (Kernel_T     *kernel,
				   PlotSystem_T *plotSystem);

/*
 * From gdisp_utils.c
 */
HRTime_T   gdisp_getHRTime         (void);
void       gdisp_uSleep            (guint       uSeconds);
HRTime_T   gdisp_waitTime          (HRTime_T    timeStamp,
				    HRTime_T    time2wait);
GtkWidget *gdisp_createButtonBar   (GtkWidget  *window,
				    GtkWidget **aButton,
				    GtkWidget **dButton);
void       gdisp_loadFonts         (GdkFont    *fonts[GD_FONT_MAX_SIZE]
                                                     [GD_FONT_MAX_TYPE]);
void       gdisp_destroyFonts      (GdkFont    *fonts[GD_FONT_MAX_SIZE]
				                     [GD_FONT_MAX_TYPE]);
GtkWidget *gdisp_getMessagePixmaps (Kernel_T   *kernel,
				    GtkWidget  *window,
				    Message_T   messageType,
				    gchar      *message);

void       gdisp_getStringTableFromStringList ( gchar   *stringList,
						gchar ***stringTable,
						guint   *stringTableSize );
void       gdisp_freeStringTable              ( gchar ***stringTable,
				                guint   *stringTableSize );
gchar     *gdisp_strStr                       ( gchar   *name,
				                gchar  **stringTable,
				                guint    stringTableSize );


				    
#endif /* __PROTOTYPES_H__ */
