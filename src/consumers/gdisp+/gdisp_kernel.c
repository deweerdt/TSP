/*

$Id: gdisp_kernel.c,v 1.16 2006-05-13 20:55:02 esteban Exp $

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

File      : Graphic Tool Kernel Implementation.
            Kernel allocation.

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


/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

static void*
gdisp_mutexNew (void)
{

  guint            status = 0;
  pthread_mutex_t *mutex  = (pthread_mutex_t*)NULL;

  mutex = (pthread_mutex_t*)g_malloc0(sizeof(pthread_mutex_t));
  assert(mutex);

  status = pthread_mutex_init(mutex,(pthread_mutexattr_t*)NULL);

  return (void*)mutex;

}


static void
gdisp_mutexLock (Kernel_T *kernel,
		 void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_lock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  case EDEADLK :
    error = g_string_new("The mutex is already locked by the calling thread.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  default :
    break;

  }

}


static gboolean
gdisp_mutexTrylock (Kernel_T *kernel,
		    void     *mutex)
{

  guint     status  = 0;
  gboolean  gStatus = TRUE;
  GString  *error   = (GString*)NULL;

  status = pthread_mutex_trylock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    gStatus = FALSE;
    break;

  case EBUSY  :
    error = g_string_new("The mutex could not be acquired"
			 "because it was currently locked.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    gStatus = FALSE;
    break;

  default :
    gStatus = TRUE;
    break;

  }

  return gStatus;

}


static void
gdisp_mutexUnlock (Kernel_T *kernel,
		   void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_unlock((pthread_mutex_t*)mutex);
  switch (status) {

  case EINVAL :
    error = g_string_new("The mutex has not been properly initialized.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  case EPERM :
    error = g_string_new("The calling thread does not own the mutex");
    kernel->outputFunc(kernel,error,GD_ERROR);
    break;

  default :
    break;

  }

}


static void
gdisp_mutexFree (Kernel_T *kernel,
		 void     *mutex)
{

  guint    status = 0;
  GString *error  = (GString*)NULL;

  status = pthread_mutex_destroy((pthread_mutex_t*)mutex);
  switch (status) {

  case EBUSY :
    error = g_string_new("The mutex is currently locked.");
    kernel->outputFunc(kernel,error,GD_ERROR);
    return;
    break;

  default :
    break;

  }

  memset(mutex,0,sizeof(pthread_mutex_t));
  g_free(mutex);

}

/* --------------------------------------------------------------- */

static void
gdisp_registerAction ( Kernel_T  *kernel,
		       void     (*action)(Kernel_T*) )
{

  g_ptr_array_add(kernel->kernelRegisteredActions,
		  (gpointer)action);

}


static void
gdisp_unRegisterAction ( Kernel_T  *kernel,
			 void     (*action)(Kernel_T*) )
{

  g_ptr_array_remove_fast(kernel->kernelRegisteredActions,
			  (gpointer)action);

}


static gint
gdisp_activateRegisteredActions ( void *data )
{

  Kernel_T  *kernel             = (Kernel_T*)data;
  void     (*action)(Kernel_T*) = (void(*)(Kernel_T*))NULL;
  guint      cptAction          = 0;
  guint      nbActions          = 0;

  /*
   * Loop over all registered actions and activate them.
   */
  nbActions = kernel->kernelRegisteredActions->len;
  for (cptAction=0; cptAction<nbActions; cptAction++) {

    action = (void(*)(Kernel_T*))
             g_ptr_array_index(kernel->kernelRegisteredActions,cptAction);

    (*action)(kernel);

  }

  return TRUE; /* keep on running */

}

/* --------------------------------------------------------------- */

static PlotType_T
gdisp_getPlotTypeFromPlotName ( Kernel_T *kernel,
				gchar    *plotName )
{

  PlotSystemInfo_T  plotInformation;
  PlotSystem_T     *plotSystem = (PlotSystem_T*)NULL;
  PlotType_T        plotType   = GD_PLOT_DEFAULT;

  for (plotType=GD_PLOT_DEFAULT; plotType<GD_MAX_PLOT; plotType++) {

    plotSystem = &kernel->plotSystems[plotType];

    if (plotSystem->psIsSupported == TRUE) {

      (*plotSystem->psGetInformation)(kernel,
				      &plotInformation);

      if (plotInformation.psName != (gchar*)NULL &&
	  strcmp(plotInformation.psName,plotName) == 0) {
	return plotType; /* got you... */
      }

    }

  } /* loop */

  return GD_PLOT_DEFAULT;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Allocate memory for GDISP+ Kernel.
 */
Kernel_T*
gdisp_createKernel (gint    argc,
		    gchar **argv)
{

  Kernel_T        *kernel          =       (Kernel_T*)NULL;
  PlotSystem_T    *plotSystem      =   (PlotSystem_T*)NULL;
  PlotType_T       plotType        =       GD_PLOT_DEFAULT;
  guint            functionSetSize =                     0;
  guint            functionCpt     =                     0;
  guint            functionNb      =                     0;
  FunctionTable_T  functionTable   = (FunctionTable_T)NULL;

  /*
   * Memory allocation.
   */
  kernel = (Kernel_T*)g_malloc0(sizeof(Kernel_T));
  assert(kernel);

  /*
   * Defaults.
   */
  kernel->asyncWriteIsAllowed    = FALSE;
  kernel->isThreadSafe           = FALSE;
  kernel->sortingMethod          = GD_SORT_BY_PROVIDER;
  kernel->dndScope               = GD_DND_UNICAST;
  kernel->argCounter             = argc;
  kernel->argTable               = argv;
  kernel->stepTimerPeriod        = GD_TIMER_MIN_PERIOD; /* milli-seconds */
  kernel->samplingThreadMustExit = TRUE;
  kernel->retreiveAllSymbols     = FALSE;

  /*
   * Try to know whether a multi-threaded environment is available ?
   */

  /* ... later ... */

  /*
   * We must provide our own thread-safe system.
   */
  kernel->mutexNew     = gdisp_mutexNew;
  kernel->mutexLock    = gdisp_mutexLock;
  kernel->mutexTrylock = gdisp_mutexTrylock;
  kernel->mutexUnlock  = gdisp_mutexUnlock;
  kernel->mutexFree    = gdisp_mutexFree;

  /*
   * Use this system to create a mutex.
   */
  kernel->isThreadSafe = TRUE;

  /*
   * Initialise all fonts.
   */
  gdisp_loadFonts(kernel->fonts);


  /*
   * Initialise all plot systems.
   * Each plot system that is supported may provide several functions.
   */
  kernel->currentPlotType = GD_PLOT_TEXT;

  /* Remove size of 'psIsSupported' */
  functionSetSize = sizeof(PlotSystem_T) - sizeof(gboolean);
  functionNb      = functionSetSize / sizeof(aFunction_T);

  for (plotType=GD_PLOT_DEFAULT; plotType<GD_MAX_PLOT; plotType++) {

    plotSystem = &kernel->plotSystems[plotType];
    switch (plotType) {

    case GD_PLOT_DEFAULT :
      gdisp_initDefaultPlotSystem(kernel,plotSystem);
      break;

    case GD_PLOT_2D :
      gdisp_initPlot2DSystem(kernel,plotSystem);
      break;

    case GD_PLOT_TEXT :
      gdisp_initPlotTextSystem(kernel,plotSystem);
      break;

    case GD_PLOT_ORBITAL :
#if _USE_OPENGL
      gdisp_initOrbitalPlotSystem(kernel,plotSystem);
#endif
      break;

    default :
      break;

    }

    /*
     * We must check out, at kernel level, that all functions have been
     * initialised by each plot system.
     */

    /* By default, the plot system is supported */
    plotSystem->psIsSupported = TRUE;
    functionTable             = (FunctionTable_T)plotSystem;

    for (functionCpt=0; functionCpt<functionNb; functionCpt++) {

      if (functionTable[functionCpt] == (aFunction_T)NULL) {

	plotSystem->psIsSupported = FALSE;

      } /* if */

    } /* for 'functionCpt' */

  } /* for 'plotType' */

  /*
   * Remember how to register periodic actions.
   */
  kernel->kernelRegisteredActions = g_ptr_array_new();

  kernel->registerAction          = gdisp_registerAction;
  kernel->unRegisterAction        = gdisp_unRegisterAction;

  kernel->kernelTimerIdentity =
    gtk_timeout_add(1000, /* milli-seconds */
		    gdisp_activateRegisteredActions,
		    (void*)kernel);


  /*
   * Remember :
   *  -  how to assign symbols to providers for sampling purpose.
   *  -  how to get plot type from plot name.
   *  -  how to perform an asynchronous write operation of a symbol.
   */
  kernel->assignSymbolsToProviders = gdisp_affectRequestedSymbolsToProvider;
  kernel->getPlotTypeFromPlotName  = gdisp_getPlotTypeFromPlotName;
  kernel->asyncWriteSymbol         = gdisp_asyncWriteSymbol;

  /*
   * Read user preference file.
   */
  gdisp_loadPreferenceFile(kernel);

  /*
   * Return the kernel itself.
   */
  return kernel;

}


/*
 * Free memory used by GDISP+ Kernel.
 */
void
gdisp_destroyKernel (Kernel_T *kernel)
{

  assert(kernel);

  /*
   * Try to save preference file.
   */
  gdisp_savePreferenceFile(kernel);

  /*
   * Stop all timers.
   */
  gdisp_stopLogoAnimation(kernel,TRUE /* stop all */);
  gtk_timeout_remove(kernel->kernelTimerIdentity);

  /*
   * Destroy all fonts and pixmaps.
   */
  gdisp_destroyFonts  (kernel->fonts);
  gdisp_destroyPixmaps(kernel);

  /*
   * Free Kernel.
   */
  if (kernel->ioFilename != (gchar*)NULL) {
    g_free(kernel->ioFilename);
  }

  g_ptr_array_free(kernel->kernelRegisteredActions,FALSE);

  memset(kernel,0,sizeof(Kernel_T));
  g_free(kernel);

}


