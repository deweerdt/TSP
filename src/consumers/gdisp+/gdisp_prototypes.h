/*

$Id: gdisp_prototypes.h,v 1.24 2007-11-14 21:53:20 esteban Exp $

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
gboolean   gdisp_startSamplingProcess             (Kernel_T           *kernel);
void       gdisp_stopSamplingProcess              (Kernel_T           *kernel);
void       gdisp_affectRequestedSymbolsToProvider (Kernel_T           *kernel);
gboolean   gdisp_buildProviderPgiHashTable        (Provider_T         *provider,
						   const SampleList_T *sampleList);
gboolean   gdisp_asyncWriteSymbol                 (Kernel_T           *kernel,
						   Symbol_T           *symbol,
						   gchar              *valueAsString);


/*
 * From gdisp_hosts.c
 */
void       gdisp_addHost      (Kernel_T *kernel,
			       gchar    *hostName);
void       gdisp_destroyHosts (Kernel_T *kernel);
void       gdisp_addUrl       (Kernel_T *kernel,
			       gchar    *urlName);
void       gdisp_manageHosts  (gpointer factoryData,
			       guint    itemData);


/*
 * From gdisp_mainBoard.c
 */
void       gdisp_createMainBoard         (Kernel_T *kernel);
void       gdisp_writeInitialInformation (Kernel_T *kernel);


/*
 * From gdisp_pilotBoard.c
 */
GtkWidget* gdisp_createPilotBoard (Kernel_T *kernel);


/*
 * From gdisp_databook.c
 */
void       gdisp_showDataBook          (gpointer factoryData,
					guint    itemData);
void       gdisp_closeDataBookWindow   (Kernel_T *kernel);
void       gdisp_refreshDataBookWindow (Kernel_T *kernel);

/*
 * From gdisp_providers.c
 */
void       gdisp_createProviderList       (Kernel_T        *kernel,
					   GtkWidget       *parent);
void       gdisp_providerTimer            (Kernel_T        *kernel,
					   gboolean         timerIsStarted);
void       gdisp_destroyProviderList      (Kernel_T        *kernel);
void       gdisp_sortProviderSymbols      (Kernel_T        *kernel,
					   SortingMethod_T  sortingMethod);
void       gdisp_refreshProviderList      (Kernel_T        *kernel);

/*
 * From gdisp_sampledSymbols.c
 */
void       gdisp_createSampledSymbolList  (Kernel_T  *kernel,
					   GtkWidget *parent);
void       gdisp_sampledSymbolTimer       (Kernel_T  *kernel,
					   gboolean   timerIsStarted);
void       gdisp_destroySampledSymbolList (Kernel_T  *kernel);
void       gdisp_refreshSampledSymbolList (Kernel_T  *kernel);


/*
 * From gdisp_graphics.c
 */
void       gdisp_createGraphicList (Kernel_T  *kernel,
				    GtkWidget *parent);
void       gdisp_destroyGraphicList(Kernel_T  *kernel);


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
void        gdisp_getSymbolExtendedInformation(Provider_T *provider,
					       gboolean    forgetPreviousExtInfo);
void        gdisp_consumingInit               (Kernel_T *kernel);
guint       gdisp_getProviderNumber           (Kernel_T *kernel);
Provider_T *gdisp_getProviderByOriginalUrl    (Kernel_T *kernel,
					       gchar    *originalUrl);
void        gdisp_consumingEnd                (Kernel_T *kernel);


/*
 * From gdisp_symbols.c
 */
void       gdisp_createSymbolList    (Kernel_T  *kernel,
				      GtkWidget *parent);
void       gdisp_symbolApplyCallback (GtkWidget *applyButtonWidget,
				      gpointer   data);
void       gdisp_destroySymbolList   (Kernel_T  *kernel);
void       gdisp_refreshSymbolList   (Kernel_T  *kernel);


/*
 * From gdisp_pages.c
 */
Page_T           *gdisp_allocateGraphicPage         (Kernel_T   *kernel,
						     gchar      *pageTitle,
						     guint       pageRows,
						     guint       pageColumns);
PlotSystemData_T *gdisp_addPlotToGraphicPage        (Kernel_T   *kernel,
						     Page_T     *page,
						     PlotType_T  plotType,
						     guint       plotRow,
						     guint       plotNbRows,
						     guint       plotColumn,
						     guint     plotNbColumns);
void              gdisp_finalizeGraphicPageCreation (Kernel_T *kernel,
						     Page_T   *newPage);
GList            *gdisp_getSymbolsInPages           (Kernel_T *kernel);
void              gdisp_createGraphicPage           (gpointer  factoryData,
						     guint     pageDimension);
void              gdisp_destroyAllGraphicPages      (Kernel_T *kernel);


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
 * From gdisp_plotEarth.c
 */
void       gdisp_initPlotEarthSystem (Kernel_T     *kernel,
				      PlotSystem_T *plotSystem);

/*
 * From gdisp_plotText.c
 */
void       gdisp_initPlotTextSystem (Kernel_T     *kernel,
				     PlotSystem_T *plotSystem);

/*
 * From gdisp_plotSheet.c
 */
void       gdisp_initPlotSheetSystem (Kernel_T     *kernel,
				      PlotSystem_T *plotSystem);

/*
 * From gdisp_plotOrbital.c
 */
void       gdisp_initOrbitalPlotSystem (Kernel_T     *kernel,
					PlotSystem_T *plotSystem);

/*
 * From gdisp_pixmaps.c
 */
Pixmap_T *gdisp_getPixmapById   ( Kernel_T    *kernel,
				  Pixmap_ID   pixmapId,
				  GtkWidget  *pixmapParent );

Pixmap_T *gdisp_getPixmapByAddr ( Kernel_T   *kernel,
				  gchar     **pixmapAddr,
				  GtkWidget  *pixmapParent );

void      gdisp_destroyPixmaps  ( Kernel_T   *kernel       );


/*
 * From gdisp_utils.c
 */
HRTime_T   gdisp_getHRTime         (void);
void       gdisp_uSleep            (guint       uSeconds);
HRTime_T   gdisp_waitTime          (HRTime_T    timeStamp,
				    HRTime_T    time2wait);
GtkWidget *gdisp_createButtonBar   (Kernel_T   *kernel,
				    GtkWidget  *window,
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

Pixmap_T  *gdisp_getProviderIdPixmap (Kernel_T   *kernel,
				      GtkWidget  *parent,
				      guint       providerIdentity);

void       gdisp_getStringTableFromStringList (gchar   *stringList,
					       gchar ***stringTable,
					       guint   *stringTableSize);
void       gdisp_freeStringTable              (gchar ***stringTable,
					       guint   *stringTableSize);
gchar     *gdisp_strStr                       (gchar   *name,
					       gchar  **stringTable,
					       guint    stringTableSize);

void       gdisp_loopOnGraphicPlots (Kernel_T  *kernel,
				     void     (*callback)(Kernel_T*,
							  Page_T*,
							  PlotSystemData_T*,
							  void*),
				     void      *userData);

gboolean   gdisp_positionIsInsideZone (PlotSystemZone_T *zone,
				       gdouble           x,
				       gdouble           y);

guint      gdisp_computePgcd          (guint a,
				       guint b);
				    
void       gdisp_dereferenceSymbolList (GList *symbolList);

gchar     *gdisp_strDup (gchar *string);
gchar     *gdisp_uIntToStr (guint value, gchar *ptr);
void       gdisp_updateWholeGui (void);

void       gdisp_showInputWindow (Kernel_T *kernel,
				  gchar    *title,
				  gchar    *subTitle,
				  Symbol_T *symbol,
				  gpointer  userData,
				  void    (*userHandler)(Kernel_T*,
							 Symbol_T*,
							 gchar*,
							 gpointer),
				  gboolean  onlyNumbers);
gint       gdisp_atoi (gchar *string,
		       gint   defaultValue);
GdkColor  *gdisp_getDistantColor (Kernel_T *kernel,
				  guint     colorId);


/*
 * From gdisp_preferences.c
 */
void       gdisp_loadPreferenceFile (Kernel_T *kernel);
void       gdisp_savePreferenceFile (Kernel_T *kernel);


/*
 * From gdisp_toString.c
 */
gchar*         gdisp_dndScopeToString          (Kernel_T *kernel);
gchar*         gdisp_sortingMethodToString     (Kernel_T *kernel);
gchar*         gdisp_sortingDirectionToString  (Kernel_T *kernel);
void           gdisp_setUpPreferenceFromString (Kernel_T *kernel,
						gchar    *preference,
						gchar    *value);
gchar*         gdisp_getTypeAsString           (Symbol_T *symbol);
TSP_datatype_t gdisp_getTypeFromString         (gchar    *typeAsString);


/*
 * From gdisp_configuration.c
 */
gboolean   gdisp_newConfiguration      (Kernel_T *kernel);
gboolean   gdisp_closeConfiguration    (Kernel_T *kernel);
gboolean   gdisp_saveConfigurationFile (Kernel_T *kernel);
gboolean   gdisp_openConfigurationFile (Kernel_T *kernel);


/*
 * From gdisp_animatedLogo.c
 */
GtkWidget *gdisp_createAnimatedLogo (Kernel_T *kernel);
void       gdisp_startLogoAnimation (Kernel_T *kernel);
void       gdisp_stopLogoAnimation  (Kernel_T *kernel, gboolean stopAll);


/*
 * From gdisp_splashScreen.c
 */
void       gdisp_showSplashScreen (void);


/*
 * From gdisp_modules.c
 */
void       gdisp_loadGraphicModules (Kernel_T *kernel);

/*
 * From gdisp_dataBox.c
 */
gpointer   gdisp_createDataBox        (Kernel_T  *kernel,
				       GtkWidget *dbParent);
void       gdisp_setDataBoxTitle      (gpointer   dataBoxVoid,
				       gchar     *dataBoxTitle);
void       gdisp_setDataBoxDimensions (gpointer   dataBoxVoid,
				       guint      dataBoxWidth,
				       guint      dataBoxHeight);
void       gdisp_destroyDataBox       (gpointer   dataBoxVoid);
void       gdisp_setDataBoxNbValues   (gpointer   dataBoxVoid,
				       guint      nbValues);
void       gdisp_setDataBoxXData      (gpointer   dataBoxVoid,
				       gchar     *xName,
				       gfloat    *xData);
guint      gdisp_addDataBoxYData      (gpointer   dataBoxVoid,
				       gchar     *yName,
				       gfloat    *yData,
				       GdkColor  *yColor);
void       gdisp_rescaleDataBox       (gpointer   dataBoxVoid);

#endif /* __PROTOTYPES_H__ */
