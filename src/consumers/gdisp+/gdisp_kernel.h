/*

$Id: gdisp_kernel.h,v 1.24 2006-04-07 10:37:17 morvan Exp $

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
Maintainer: tsp@astrium.eads.net
Component : Graphic Tool

-----------------------------------------------------------------------

Purpose   : Graphic Tool based on GTK+ that provide several kinds of
            plot. This tool is to be used with the generic TSP protocol.

File      : Graphic Tool Kernel Interface.
            Definition of all structures and types for the application.

-----------------------------------------------------------------------
*/

#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <gtk/gtk.h>
#include <pthread.h>


/*
 * TSP Consumer include.
 * Definitition of a dynamic array of double.
 */
#include "tsp_const_def.h"
#include "tsp_consumer.h"
#include "tsp_hash.h"
#include "gdisp_doubleArray.h"

/** 
 * @defgroup TSP_GDispPlus_Kernel Kernel API
 * @ingroup TSP_GDispPlusLib
 * @{
 */

/*
 * Define this to enable Drag And Drop debug.
 */
#undef _DND_DEBUG_

/*
 * Few defines.
 */
#undef  GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS
#define GD_UNREF_THINGS


/*
 * 10 out of 170 colors.
 * FIXME : allocate only these 10 colors, and not the entire colormap.
 */
#define _RED_       3
#define _GREEN_    10
#define _ORANGE_   14
#define _YELLOW_   17
#define _BLUE_     24
#define _MAGENTA_  31
#define _CYAN_     38
#define _BLACK_    42
#define _GREY_    155
#define _WHITE_   169


/*
 * A message may be a simple message, a warning or an information
 * reporting a error.
 */
typedef enum {

  GD_MESSAGE = 0,
  GD_WARNING,
  GD_ERROR

} Message_T;


/*
 * Time management.
 * Everything below is expressed in 'nanoseconds'.
 */
typedef gint64 HRTime_T;

#define GDISP_SYSTEM_HAVE_NANOSLEEP

#define _10_MILLISECONDS_IN_NANOSECONDS_       10000000LL
#define _100_MILLISECONDS_IN_NANOSECONDS_     100000000LL
#define _250_MILLISECONDS_IN_NANOSECONDS_     250000000LL
#define _ONE_SECOND_IN_NANOSECONDS_          1000000000LL

#define _10_MILLISECONDS_IN_MICROSECONDS_         10000LL
#define _100_MILLISECONDS_IN_MICROSECONDS_       100000LL
#define _250_MILLISECONDS_IN_MICROSECONDS_       250000LL
#define _ONE_SECOND_IN_MICROSECONDS_            1000000LL


/**
 * HOST management.
 * Few things by now... be patient...
 */
typedef struct Host_T_ {

  GString *hName; /*!< the host name */
  
} Host_T;


/**
 * TSP provider management which define
 * Available provider statuses.
 */
typedef enum {
  /** The provider has just been created. Nothing can be done with it. */
  GD_FROM_SCRATCH = 0,
  /** The provider has now its name, But 'TSP_consumer_request_*' functions can not be called yet. */ 
  GD_SESSION_CLOSED,
  /**  Ready now to request any information of the provider. */
  GD_SESSION_OPENED,
  /** The provider has been given the set of symbols to be sampled. But 'sampling' is still OFF. */
  GD_SAMPLE_REQUESTED,
  /** 'sampling' is now ON. */
  GD_SAMPLE_STARTED

} ProviderStatus_T;


/*
 * Ha Ha...
 * Here is now the description of what a symbol looks like within GDisp+.
 *  - sReference  : must be incremented each time the symbol is dropped on
 *                  a graphic plot. It is up to graphic plots to increment
 *                  the 'sReference' variable, and to decrement it each time
 *                  the symbol is no longer requested by the plot.
 *  - sConfIndex  : identity within the configuration.
 *  - sInfo       : structure coming from TSP core.
 *                  contains : name, index, period and phase.
 *  - sLastValue  : the double precision value of the symbol.
 *  - sTimeTag    : the time tag of the symbol value.
 *  - sHasChanged : has this symbol evolved since last frame ?
 */

/* FIXME : the following three definitions must be dynamic */
#define TSP_PROVIDER_FREQ     100   /* Hz                */
#define GDISP_REFRESH_FREQ     10   /* Hz                */
#define GDISP_WIN_T_DURATION   60   /* Seconds on X Axis */

typedef struct Symbol_T_ {

  guchar                           sReference;
  guint                            sPgi;
  TSP_sample_symbol_info_t         sInfo;
  guint                            sTimeTag;
  gdouble                          sLastValue;
  gboolean                         sHasChanged;

  /*
   * Graphic.
   */
  GtkCTreeNode                    *sNode;

} Symbol_T;


/*
 * When showing all available symbols into a Gtk CList,
 * try to sort these symbols with different manners.
 */
typedef enum {

  GD_SORT_BY_NAME = 0,
  GD_SORT_BY_NAME_REVERSE,
  GD_SORT_BY_PROVIDER,
  GD_SORT_BY_INDEX

} SortingMethod_T;


/*
 * When droping a set of symbols (DnD Action on graphic plot),
 * does the drop action be broadcast to all graphic plots ?
 *   - GD_DND_UNICAST   : Only one graphic plot,
 *   - GD_DND_MULTICAST : All graphic plots of the current page,
 *   - GD_DND_BROADCAST : All graphic plots of all existing graphic pages.
 */
typedef enum {

  GD_DND_UNICAST = 0,
  GD_DND_MULTICAST,
  GD_DND_BROADCAST

} DndScope_T;


/*
 * Shorter name for symbol that must be sampled.
 */
typedef TSP_sample_symbol_info_list_t SampleList_T;


/*
 * Thread Status.
 */
typedef enum {

  GD_THREAD_STOPPED = 0,
  GD_THREAD_STARTING,
  GD_THREAD_WARNING,
  GD_THREAD_REQUEST_SAMPLE_ERROR,
  GD_THREAD_SAMPLE_INIT_ERROR,
  GD_THREAD_SAMPLE_DESTROY_ERROR,
  GD_THREAD_ERROR,
  GD_THREAD_RUNNING

} ThreadStatus_T;


/*
 * A TSP provider definition.
 *
 * The number of provider is limited because I have only 6 ball pixmaps
 * to represent them. That's all. There is no other reason.
 * Theorically, there is no limit, except the number of ball pixmaps.
 * Feel free to create other ball pixmaps...
 */
#define GD_MAX_PROVIDER_NUMBER 6

typedef struct Provider_T_ {

  TSP_provider_t    pHandle;
  guint             pIdentity;

  ProviderStatus_T  pStatus;
  GString          *pOriginalUrl;
  GString          *pUrl;
  gdouble           pBaseFrequency;
  gint              pMaxPeriod;
  gint              pMaxClientNumber;
  gint              pCurrentClientNumber;

  gint              pSymbolNumber;
  Symbol_T         *pSymbolList;
  hash_t           *pSymbolHashTable;    /* name -> pointer */
  hash_t           *pSymbolHashTablePGI; /* pgi  -> pointer */

  SampleList_T      pSampleList;
  guint             pLoad;
  guint             pMaxLoad;

  /*
   * Thread management.
   */
  pthread_t         pSamplingThread;
  ThreadStatus_T    pSamplingThreadStatus;

  /*
   * Configuration temporarily data.
   */
#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)
  void             *pSymbolInConfiguration;
  guint             pNbSymbolInConfiguration;
#endif

  /*
   * Graphic information.
   */
  GtkWidget        *pCList;
  GdkColor         *pColor;
  GtkCTreeNode     *pNode;

} Provider_T;


/*
 * Plot types.
 *  - GD_PLOT_DEFAULT : when the graphic page has just been created.
 *  - GD_PLOT_TEXT    : alphanumeric evolution of symbols.
 *  - GD_PLOT_2D      : y = f(t) or y = f(x)
 *  - GD_PLOT_ORBITAL : a spacecraft orbiting around the Earth.
 *  - GD_PLOT_2D5     : z = f(x,y). (not implemented)
 *  - GD_PLOT_3D      : full colored tri-dimensional object. (not implemented)
 */
typedef enum {

  GD_PLOT_DEFAULT = 0,
  GD_PLOT_TEXT,
  GD_PLOT_2D,
  GD_PLOT_ORBITAL,
  GD_MAX_PLOT /* this last one defines the limit */

} PlotType_T;


/*
 * Font management.
 */
typedef enum {

  GD_FONT_NORMAL = 0,
  GD_FONT_ITALIC,
  GD_FONT_FIXED,
  GD_FONT_MAX_TYPE

} FontType_T;

typedef enum {

  GD_FONT_SMALL = 0,
  GD_FONT_MEDIUM,
  GD_FONT_BIG,
  GD_FONT_MAX_SIZE

} FontSize_T;


/*
 * Kernel alias.
 */
typedef struct Kernel_T_ *Kernel_T_Ptr;


/*
 * Definition of the operating system of a given typed plot.
 */
typedef void*        aFunction_T;
typedef aFunction_T *FunctionTable_T;

typedef struct PlotSystemInfo_T_ {

  gchar  *psName;
  gchar  *psFormula;
  gchar  *psDescription;
  gchar **psLogo;

} PlotSystemInfo_T;

typedef struct PlotSystemZone_T_ {

#define GD_ZONE_MAX_POINTS 5
  guchar     pszId; /* different from 0 in order to be valid */
  gdouble    pszX[GD_ZONE_MAX_POINTS];
  gdouble    pszY[GD_ZONE_MAX_POINTS];
  guint      pszPointNb;
  gchar    **pszIcon;
  gboolean   pszAcceptDrops;
  gchar     *pszComment;

} PlotSystemZone_T;

typedef struct PlotSystem_T_ {

  /* DO NOT PUT ANYTHING BEFORE THE FOLLOWING FUNCTIONS     */
  /* See 'gdisp_kernel.c' when checking plot system support */

  /*
   * All that is needed to manage a plot system.
   * Two types of functions :
   *  - The first ones MUST not rely on a 'void*' argument that may be
   *    the opaque structure of a plot. Only the kernel is given.
   *  - The other ones can rely on the kernel AND the opaque structure
   *    of the plot.
   */

  /* FIRST TYPE : Kernel only.                  */
  void       (*psGetInformation)     (Kernel_T_Ptr,PlotSystemInfo_T*     );
  void      *(*psCreate )            (Kernel_T_Ptr                       );
  GArray    *(*psGetDropZones)       (Kernel_T_Ptr                       );

  /* SECOND TYPE : Kernel and opaque structure. */
  void       (*psDestroy)            (Kernel_T_Ptr,void*                 );
  void       (*psSetParent)          (Kernel_T_Ptr,void*,GtkWidget*      );
  GtkWidget *(*psGetTopLevelWidget)  (Kernel_T_Ptr,void*                 );
  void       (*psShow)               (Kernel_T_Ptr,void*                 );
  PlotType_T (*psGetType)            (Kernel_T_Ptr,void*                 );
  void       (*psSetDimensions)      (Kernel_T_Ptr,void*,guint,guint     );
  void       (*psAddSymbols)         (Kernel_T_Ptr,void*,GList*,guchar   );
  GList     *(*psGetSymbols)         (Kernel_T_Ptr,void*,gchar           );
  void       (*psGetSymbolAttributes)(Kernel_T_Ptr,void*,Symbol_T*,GList*);
  void       (*psSetSymbolAttributes)(Kernel_T_Ptr,void*,Symbol_T*,GList*);
  gboolean   (*psStartStep)          (Kernel_T_Ptr,void*                 );
  void       (*psStep)               (Kernel_T_Ptr,void*                 );
  void       (*psStopStep)           (Kernel_T_Ptr,void*                 );
  void       (*psTreatSymbolValues)  (Kernel_T_Ptr,void*                 );
  guint      (*psGetPeriod)          (Kernel_T_Ptr,void*                 );

  /*
   * Kernel verification.
   */
  gboolean     psIsSupported;

} PlotSystem_T;


/*
 * Data associated to each graphic plot on a graphic page.
 */
typedef struct PlotSystemData_T_ {

  /*
   * Core.
   */
  PlotSystem_T *plotSystem;
  void         *plotData;
  guint         plotCycle;

  /*
   * Position within graphic page.
   */
  guint         plotRow;
  guint         plotNbRows;
  guint         plotColumn;
  guint         plotNbColumns;
  
} PlotSystemData_T;


/*
 * A Graphic Page.
 */
typedef struct Page_T_ {

  /*
   * Dimensions and name.
   */
  guchar            pRows;
  guchar            pColumns;
  GString          *pName;

  /*
   * Plot system data (size is 'pRows' x 'pColumns').
   */
  PlotSystemData_T *pPlotSystemData;
  guint             pCurrentNbPlotSystems;

  /*
   * Graphic widgets.
   */
  GtkWidget        *pWindow;
  GtkWidget        *pTable;

} Page_T;


/*
 * A structure that hols all GtkWidgets that must be recorded in order
 * to manage all top-level callbacks.
 */
#include "gdisp_pixmaps.h"

typedef struct KernelWidget_T_ {

  /* -------------------- MAIN BOARD WIDGETS -------------------- */

  GtkWidget         *mainBoardWindow;
  gint               mainBoardWindowXPosition;
  gint               mainBoardWindowYPosition;
  GtkWidget         *mainBoardOkButton;
  GtkWidget         *mainBoardStopButton;
  GtkWidget         *mainBoardOutputList;
  GtkWidget         *mainBoardOutputScrolledWindow;
  guint              mainBoardOutputListSize;
  GtkWidget         *mainBoardLogoArea;
  GdkGC             *mainBoardLogoContext;
#define GD_ANIMATED_LOGO_NB 11
  Pixmap_T          *mainBoardLogoTable[GD_ANIMATED_LOGO_NB];
  Pixmap_T         **mainBoardCurrentLogo;
  Pixmap_T          *mainBoardLargeLogo;
  gint               mainBoardLargeLogoX;
  gboolean           mainBoardLargeLogoGoesLeft;
  GtkWidget         *mainBoardMenuBar;

  /* -------------------- PILOT BOARD WIDGETS -------------------- */

  Pixmap_T          *pilotBoardDigitPixmap;
  GtkWidget         *pilotBoardTimeArea;
  GdkGC             *pilotBoardTimeContext;
  GtkWidget         *pilotBoardElapsedTimeArea;
  GdkGC             *pilotBoardElapsedTimeContext;

  /* -------------------- DATA BOOK WIDGETS ---------------------- */

  GtkWidget         *dataBookWindow;
  gint               dataBookWindowXPosition;
  gint               dataBookWindowYPosition;
  GtkWidget         *dataBookWidget;
  GtkWidget         *dataBookApplyButton;

  /* ----------------- WIDGETS IN DATA BOOK PAGES ----------------- */

#define _SYMBOL_CLIST_COLUMNS_NB_ 3
  GtkWidget         *symbolCList;
  GtkWidget         *symbolFrame;
  GtkWidget         *providerVBox;
  GtkWidget         *pRadioButton;
  GtkWidget         *naRadioButton;
  GtkWidget         *ndRadioButton;
  GtkWidget         *uRadioButton;
  GtkWidget         *spRadioButton;
  GtkWidget         *apRadioButton;
  GtkWidget         *filterEntry;
  GtkStyle          *selectedNodeStyle;
  GtkStyle          *unselectedNodeStyle;
  Pixmap_T          *expandedNodePixmap;
  Pixmap_T          *collapsedNodePixmap;
  GtkWidget         *sampledSymbolHTree;
  GtkWidget         *sampledSymbolScrolledWindow;
  GtkWidget         *fileSelector;

  /* ------------------------ PIXMAP TABLE ------------------------ */

  GList             *pixmapTable;

} KernelWidget_T;


/*
 * Kernel structure.
 */
typedef struct Kernel_T_ {

  /*
   * GDISP+ Kernel Information.
   */
  gint               argCounter;
  gchar            **argTable;

  /*
   * GTK timer identity and period in milli-seconds.
   */
  gint               logoTimerIdentity;
  gint               logoHighSpeedTimerIdentity;
  gint               stepTimerIdentity;
#define GD_TIMER_MIN_PERIOD 100
  guint              stepTimerPeriod;
  guint              stepGlobalCycle;
  time_t             startSamplingTime;
  time_t             stopSamplingTime;
  gint               kernelTimerIdentity;
  GPtrArray         *kernelRegisteredActions;

  /*
   * Definition of the multi-threaded environment.
   */
  gboolean           isThreadSafe;
  void            *(*mutexNew)    (void);
  void             (*mutexLock)   (Kernel_T_Ptr,void*);
  gboolean         (*mutexTrylock)(Kernel_T_Ptr,void*);
  void             (*mutexUnlock) (Kernel_T_Ptr,void*);
  void             (*mutexFree)   (Kernel_T_Ptr,void*);

  /*
   * Sampling thread and attributes.
   */
  gboolean           samplingThreadMustExit;
  pthread_t          garbageCollectorThread;

  /*
   * Useful functions available in the kernel.
   */
  void             (*outputFunc)              (Kernel_T_Ptr,
					       GString*,
					       Message_T);
  void             (*registerAction)          (Kernel_T_Ptr,
					       void(*action)(Kernel_T_Ptr));
  void             (*unRegisterAction)        (Kernel_T_Ptr,
					       void(*action)(Kernel_T_Ptr));
  void             (*assignSymbolsToProviders)(Kernel_T_Ptr);
  PlotType_T       (*getPlotTypeFromPlotName) (Kernel_T_Ptr,gchar*);
  void             (*startSamplingProcess)    (Kernel_T_Ptr);
  void             (*stopSamplingProcess)     (Kernel_T_Ptr);

  /*
   * Host/URL management.
   */
  GList             *hostList;
  GList             *urlList;

  /*
   * Provider management.
   */
  GList             *providerList;
  SortingMethod_T    sortingMethod;
  guint              providerId;

  /*
   * IO management.
   */
  gchar             *ioFilename;
  gboolean         (*factoryAction)(Kernel_T_Ptr);
  guint              factoryActionId;
  gboolean           retreiveAllSymbols;

  /*
   * -------------------- Graphic part --------------------
   */

  /*
   * Colormap and color management.
   */
  GdkColormap       *colormap;
  GdkVisual         *visual;
  GdkColor          *colors;
  gint               colorNumber;
  GdkFont           *fonts[GD_FONT_MAX_SIZE][GD_FONT_MAX_TYPE];

  /*
   * Definition of all available plot systems.
   */
  PlotSystem_T       plotSystems[GD_MAX_PLOT];
  PlotType_T         currentPlotType;

  /*
   * Graphic pages.
   */
  GList             *pageList;

  /*
   * Drag & Dop process.
   */
  GList             *dndSelection;
  DndScope_T         dndScope;
  GdkWindow         *dndIconWindow;
  GdkWindow         *dndIconWindowParent;
  GdkGC             *dndIconWindowGc;
  GdkPixmap         *dndIconPixmap;
  GdkBitmap         *dndIconPixmapMask;

  /*
   * Widget management.
   */
  KernelWidget_T     widgets;

} Kernel_T;

/* doxygen end tag do not remove the line below */
/** @} */

#endif /* __KERNEL_H__ */
