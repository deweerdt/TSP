/*!  \file 

$Id: gdisp_sampling.c,v 1.1 2004-02-04 20:32:10 esteban Exp $

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

File      : Graphic Tool Sampling Core Process.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>


/*
 * GDISP+ includes.
 */
#include "gdisp_sampling.h"

#undef THREAD_DEBUG
#undef SAMPLING_DEBUG

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Encapsulation of thread creation.
 */
static gint
gdisp_createThread ( Kernel_T               *kernel,
		     gchar                  *threadName,
		     pthread_t              *threadIdentity,
		     const pthread_attr_t   *threadAttributes,
		     void                 *(*threadProcedure)(void*),
		     void                   *threadArgument,
		     gboolean                threadIsDetached,
		     gboolean                threadCreationIsVerbose )
{

  ThreadStatus_T  threadStatus  = GD_THREAD_STOPPED;
  GString        *messageString =    (GString*)NULL;
  gint            status        =                 0;


  /*
   * Really create the thread.
   */
  *threadIdentity = (pthread_t)NULL;

  status = pthread_create(threadIdentity,
			  threadAttributes,
			  threadProcedure,
			  threadArgument);

  if (status != 0) {

    *threadIdentity = (pthread_t)NULL;
    threadStatus    = GD_THREAD_ERROR;

    if (threadCreationIsVerbose == TRUE) {

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s thread creation failed.",
		       threadName);
      kernel->outputFunc(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s.",
		       strerror(errno));
      kernel->outputFunc(kernel,messageString,GD_ERROR);

    } /* verbose */

  }
  else if (threadIsDetached == TRUE) {

    status = pthread_detach(*threadIdentity);

    if (status != 0 && threadCreationIsVerbose == TRUE) {

      threadStatus = GD_THREAD_WARNING;

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "Cannot detach %s thread.",
		       threadName);
      kernel->outputFunc(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "%s.",
		       strerror(errno));
      kernel->outputFunc(kernel,messageString,GD_ERROR);

    }

  }

  return threadStatus;

}


/*
 * All symbols that must be plotted do not belong to the same
 * provider. So give back requested symbols to the provider each
 * of them belongs to.
 */
static void
gdisp_affectRequestedSymbolsToProvider ( Kernel_T *kernel )
{

  GArray     *requestedSymbolArray =     (GArray*)NULL;
  GList      *providerItem         =      (GList*)NULL;
  Provider_T *provider             = (Provider_T*)NULL;
  GList      *symbolList           =      (GList*)NULL;
  GList      *symbolItem           =      (GList*)NULL;
  Symbol_T   *symbol               =   (Symbol_T*)NULL;
  guint       elementSize          =                 0;

  /*
   * Get back requested symbols from graphic pages.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);


  /*
   * Loop over all providers, cancel any previous sampling
   * configuration, attach new symbols if any.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pStatus == GD_SESSION_OPENED) {

      /*
       * Cancel any previous sampling configuration.
       */
      provider->pSampleList.len = 0;
      if (provider->pSampleList.val != (TSP_consumer_symbol_requested_t*)NULL)
	free(provider->pSampleList.val);
      provider->pSampleList.val = (TSP_consumer_symbol_requested_t*)NULL;


      /*
       * Temporary ressource.
       */
      elementSize = sizeof(TSP_consumer_symbol_requested_t);
      requestedSymbolArray = g_array_new(FALSE, /* zero_terminated */
					 TRUE,  /* clear           */
					 (guint)elementSize);


      /*
       * Loop on every symbol that must be sampled.
       */
      symbolItem = g_list_first(symbolList);
      while (symbolItem != (GList*)NULL) {

	symbol = (Symbol_T*)symbolItem->data;

	/*
	 * The symbol belongs to the current provider because its
	 * address is in the good address interval.
	 */
	if (provider->pSymbolList <= symbol &&
	    symbol < provider->pSymbolList + provider->pSymbolNumber) {

	  g_array_append_val(requestedSymbolArray,symbol->sInfo);

	}

	symbolItem = g_list_next(symbolItem);

      }


      /*
       * Transfer information to provider.
       */
      provider->pSampleList.len = requestedSymbolArray->len;
      provider->pSampleList.val =
	(TSP_consumer_symbol_requested_t*)requestedSymbolArray->data;


      /*
       * Free temporary ressource.
       * Free the table, not the content.
       */
      g_array_free(requestedSymbolArray,FALSE);

    } /* provider->pStatus == GD_SESSION_OPENED */

    providerItem = g_list_next(providerItem);

  }


  /*
   * Release ressource.
   */
  g_list_free(symbolList);

}


/*
 * Main routine to perform steps on the graphic pages.
 * The prototype of this routine ensures that it may be used
 * as a callback given to the 'gtk_timeout_add' action.
 */
#undef BENCHMARK

static gint
gdisp_stepsOnGraphicPlots ( void *data )
{

  Kernel_T        *kernel        =      (Kernel_T*)data;

  PlotSystem_T   **plotSystem    = (PlotSystem_T**)NULL;
  PlotSystem_T   **plotSystemEnd = (PlotSystem_T**)NULL;
  void           **plotData      =         (void**)NULL;
  GList           *pageItem      =         (GList*)NULL;
  Page_T          *page          =        (Page_T*)NULL;

#if defined(BENCHMARK)

  HRTime_T         startMark     = (HRTime_T)0;
  HRTime_T         stopMark      = (HRTime_T)0;
  HRTime_T         deltaMark     = (HRTime_T)0;
  static HRTime_T  lastMark      = (HRTime_T)0;

#endif

  /*
   * Check GTK timer.
   */
#if defined(BENCHMARK)

  if (lastMark > 0) {

    deltaMark = gdisp_getHRTime() - lastMark;

    fprintf(stdout,
	    "GTK Timer : %d nanoseconds\n",
	    (int)(deltaMark & 0xFFFFFFFF));

  }

#endif

  /*
   * Do the plot step.
   */
#if defined(BENCHMARK)

  startMark = gdisp_getHRTime();

#endif

  /* ************************************************************
   *
   * BEGIN : Loop over all graphic plots of all pages, do steps...
   *
   * ************************************************************/

  pageItem = g_list_first(kernel->pageList);
  while (pageItem != (GList*)NULL) {

    page = (Page_T*)pageItem->data;

    plotSystem    = page->pPlotSystems;
    plotData      = page->pPlotData;
    plotSystemEnd = page->pPlotSystems + (page->pRows * page->pColumns);

    while (plotSystem < plotSystemEnd) {

      (*(*plotSystem)->psStep)(kernel,
			       *plotData);

      plotSystem++;
      plotData  ++;

    }

    pageItem = g_list_next(pageItem);

  }

  /* ************************************************************
   *
   * END.
   *
   * ************************************************************/

#if defined(BENCHMARK)

  stopMark = gdisp_getHRTime();

  deltaMark = stopMark - startMark;

  fprintf(stdout,
	  "Step on all plots : %d nanoseconds\n",
          (int)(deltaMark & 0xFFFFFFFF));

  lastMark = gdisp_getHRTime();

#endif

  return TRUE;

}


/*
 * Memory allocation for sampling symbols.
 */
static void
gdisp_freeSymbolsForSampling (Kernel_T *kernel)
{

  GList    *symbolList =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop on every symbol that must be sampled.
   * Free memory that has been used for storing the evolution of the symbol.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);
  symbolList = g_list_first(symbolList);

  while (symbolList != (GList*)NULL) {

    symbol = (Symbol_T*)symbolList->data;

    /*
     * Release everything necessary here.
     */

    symbolList = g_list_next(symbolList);

  }

  g_list_free(symbolList);

}


/*
 * Memory allocation for sampling symbols.
 */
static gboolean
gdisp_allocateSymbolsForSampling (Kernel_T *kernel)
{

  GList    *symbolList =    (GList*)NULL;
  Symbol_T *symbol     = (Symbol_T*)NULL;

  /*
   * Loop on every symbol that must be sampled.
   * Allocate memory for storing the evolution of the symbol.
   */
  symbolList = gdisp_getSymbolsInPages(kernel);
  symbolList = g_list_first(symbolList);

  while (symbolList != (GList*)NULL) {

    symbol = (Symbol_T*)symbolList->data;

    /*
     * Do allocation here if necessary.
     */

    symbolList = g_list_next(symbolList);

  }

  g_list_free(symbolList);

  return TRUE; /* no error */

}


/*
 * Garbage collector on symbols.
 * Loop over all symbols and release memory if necessary.
 */
static void*
gdisp_garbageCollectorThread ( void *data )
{

  Kernel_T   *kernel       =   (Kernel_T*)data;
  GList      *providerItem =      (GList*)NULL;
  Symbol_T   *symbolPtr    =   (Symbol_T*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;
  gint        symbolCpt    =                 0;

#if defined(THREAD_DEBUG)
  fprintf(stdout,"Beginning of garbage collector thread.\n");
  fflush (stdout);
#endif

  /*
   * Wait for the thread creation to finalise...
   */
  while (kernel->garbageCollectorThread != pthread_self()) {

    gdisp_uSleep(_100_MILLISECONDS_IN_MICROSECONDS_);

  }

  /*
   * Garbage collector on symbols.
   * Loop over all symbols and release memory if necessary.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider  = (Provider_T*)providerItem->data;
    symbolPtr = provider->pSymbolList;

    for (symbolCpt=0; symbolCpt<provider->pSymbolNumber; symbolCpt++) {

      /*
       * Release memory if no more graphic plots make use of this symbol.
       */
      if (symbolPtr->sReference == 0) {

	/*
	 * Nothing by now.
	 */

      }

      /*
       * Every 100 symbols, wait for a while by leaving CPU.
       * This enables other threads to keep on running...
       * FIXME : confirm it is useful...
       */
      if (symbolCpt % 100 == 0) {

	gdisp_uSleep(0);

      }

      symbolPtr++;

    } /* symbolCpt */

    providerItem = g_list_next(providerItem);

  }

  /*
   * Tell the kernel the garbage thread is about to finish.
   */
  kernel->garbageCollectorThread = (pthread_t)NULL;

#if defined(THREAD_DEBUG)
  fprintf(stdout,"End of garbage collector thread.\n");
  fflush (stdout);
#endif

  pthread_exit((void*)TRUE /* OK */);

}


/*
 * Main Sampling Thread.
 */
static void*
gdisp_samplingThread (void *data )
{

  Kernel_T     *kernel           =   (Kernel_T*)data;
  Provider_T   *provider         = (Provider_T*)NULL;
  Symbol_T     *symbol           =   (Symbol_T*)NULL;
  GList        *providerItem     =      (GList*)NULL;
  gboolean      providerIsFound  =             FALSE;
  guchar        watchDog         =                10;
  gint          requestStatus    =                 0;
  gboolean      sampleHasArrived =             FALSE;
  TSP_sample_t  sampleValue;

#if defined(THREAD_DEBUG)
  fprintf(stdout,"Beginning of provider sampling thread.\n");
  fflush (stdout);
#endif

  /*
   * Wait for the thread creation to finalise...
   * Let's search for the provider we are talking about...
   * because only the kernel has been given to the thread.
   */
  do {

    providerItem = g_list_first(kernel->providerList);
    while (providerItem != (GList*)NULL && providerIsFound == FALSE) {

      provider = (Provider_T*)providerItem->data;

      if (provider->pSamplingThread == pthread_self()) {

	providerIsFound = TRUE;

      }

      providerItem = g_list_next(providerItem);

    } /* loop on providers */

    if (providerIsFound == FALSE) {

      watchDog--;
      gdisp_uSleep(_100_MILLISECONDS_IN_MICROSECONDS_);

    }

  } while (providerIsFound == FALSE && watchDog > 0);


  /*
   * Do I know who I am ?
   */
  if (providerIsFound == FALSE) {

    pthread_exit((void*)FALSE);

  }


  /*
   * Give requested symbols to TSP core.
   */
  requestStatus = TSP_consumer_request_sample(provider->pHandle,
					      &provider->pSampleList);

  if (requestStatus == 0) {

    provider->pSamplingThreadStatus = GD_THREAD_REQUEST_SAMPLE_ERROR;
    pthread_exit((void*)FALSE);

  }


  /*
   * Tell TSP core to start sampling process.
   * Wait for a while so that TSP core has time to fill in FIFOs.
   */
  requestStatus =
    TSP_consumer_request_sample_init(provider->pHandle,
				     (TSP_sample_callback_t)NULL,
				     (void*)NULL);

  if (requestStatus == 0) {

    provider->pSamplingThreadStatus = GD_THREAD_SAMPLE_INIT_ERROR;
    pthread_exit((void*)FALSE);

  }
  else {

    gdisp_uSleep(_ONE_SECOND_IN_MICROSECONDS_ / 2);

  }


  /*
   * Sample... Do it...
   */
  provider->pSamplingThreadStatus = GD_THREAD_RUNNING;

  while (kernel->samplingThreadMustExit == FALSE) {

    requestStatus = TSP_consumer_read_sample(provider->pHandle,
					     &sampleValue,
					     &sampleHasArrived);

    if (requestStatus == FALSE) {

      /*
       * Out of "while" loop.
       */
      break;

    }

    if (sampleHasArrived == TRUE) {

#if defined(SAMPLING_DEBUG)
      printf("Time [%d] - Index [%d] - Name [%s] - Value [%f]\n",
	 sampleValue.time,
	 sampleValue.provider_global_index,
	 provider->pSymbolList[sampleValue.provider_global_index].sInfo.name,
	 (float)sampleValue.user_value);
#endif

      symbol = &provider->pSymbolList[sampleValue.provider_global_index];

      symbol->sTimeTag = (guint)sampleValue.time;
      symbol->sValue   = sampleValue.user_value;

    } /* sampleHasArrived == TRUE */

    else {

      /*
       * Used to give time to other TSP threads for filling internal
       * FIFOs with received samples.
       */
      gdisp_uSleep(_ONE_SECOND_IN_MICROSECONDS_ / provider->pBaseFrequency);

    }

  }

  provider->pSamplingThreadStatus = GD_THREAD_STOPPED;
  

  /*
   * Tell TSP core to stop sampling process.
   */
  requestStatus = TSP_consumer_request_sample_destroy(provider->pHandle);
  if (requestStatus == 0) {

    provider->pSamplingThreadStatus = GD_THREAD_SAMPLE_DESTROY_ERROR;

  }


  /*
   * Bye bye.
   */
  provider->pSamplingThread = (pthread_t)NULL;

#if defined(THREAD_DEBUG)
  fprintf(stdout,"End of provider sampling thread.\n");
  fflush (stdout);
#endif

  pthread_exit((void*)TRUE);

}


/*
 * Pre-Sampling Thread.
 */
static void*
gdisp_preSamplingThread (void *data )
{

  Kernel_T       *kernel            =   (Kernel_T*)data;
  Provider_T     *provider          = (Provider_T*)NULL;
  GList          *providerItem      =      (GList*)NULL;
  gboolean        memoryIsAllocated =             FALSE;
  ThreadStatus_T  threadStatus      =   GD_THREAD_ERROR;


#if defined(THREAD_DEBUG)
  fprintf(stdout,"Beginning of pre-sampling thread.\n");
  fflush (stdout);
#endif

  /*
   * Memory allocation for sampling symbols.
   */
  memoryIsAllocated = gdisp_allocateSymbolsForSampling(kernel);
  if (memoryIsAllocated == FALSE) {

    /*
     * No matter the returned value, because 'preSamplingThread' is detached.
     */
    pthread_exit((void*)FALSE);

  }


  /*
   * Compute provider sampling configurations.
   */
  gdisp_affectRequestedSymbolsToProvider(kernel);


  /*
   * Loop on each provider, and launch the sampling thread.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /*
     * Create the thread is only there something to be sampled...
     */
    if (provider->pSampleList.len != 0) {

      threadStatus = gdisp_createThread(kernel,
					provider->pName->str,
					&provider->pSamplingThread,
					(const pthread_attr_t*)NULL,
					gdisp_samplingThread,
					(void*)kernel,
					TRUE  /* thread is detached */,
					FALSE /* no verbose         */);

      provider->pSamplingThreadStatus =
	threadStatus == GD_THREAD_ERROR ? threadStatus : GD_THREAD_WARNING;

    } /* something to be sampled... */

    providerItem = g_list_next(providerItem);

  } /* loop on providers */


  /*
   * Launch a timer.
   * The period must be defined in milli-seconds.
   */
  kernel->timerIdentity  = gtk_timeout_add(kernel->timerPeriod,
					   gdisp_stepsOnGraphicPlots,
					   (gpointer)kernel);


  /*
   * No matter the returned value, because 'preSamplingThread' is detached.
   */
#if defined(THREAD_DEBUG)
  fprintf(stdout,"End of pre-sampling thread.\n");
  fflush (stdout);
#endif

  pthread_exit((void*)TRUE);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Start everything that deals with sampling process.
 */
void
gdisp_startSamplingProcess (Kernel_T *kernel)
{

  pthread_t      preSamplingThread = (pthread_t)NULL;
  ThreadStatus_T threadStatus      = GD_THREAD_ERROR;


  /*
   * All provider threads must keep on running...
   */
  kernel->samplingThreadMustExit = FALSE;


  /*
   * Create the thread that will handle TSP symbol pre-sampling,
   * according to symbols that have been dropped on plots.
   * The pre-sampling phase consists in allocating memory for symbols
   * that must be sampled, assigning symbols to providers, creating
   * a sampling thread for each provider, and launching a GTK timer
   * for managing steps on graphic plots.
   */
  threadStatus = gdisp_createThread(kernel,
				    "Pre-sampling",
				    &preSamplingThread,
				    (const pthread_attr_t*)NULL,
				    gdisp_preSamplingThread,
				    (void*)kernel,
				    TRUE /* thread is detached */,
				    TRUE /* verbose            */);

  if (threadStatus == GD_THREAD_ERROR) {

    return;

  }


  /*
   * Create the garbage collector thread that will release
   * memory of symbols that are no longer in use.
   * CAUTION : this thread must be detached in order to release
   * ressouces, because no other thread will wait for it (pthread_join).
   */
  if (kernel->garbageCollectorThread == (pthread_t)NULL) {

    threadStatus = gdisp_createThread(kernel,
				      "Garbage collector",
				      &kernel->garbageCollectorThread,
				      (const pthread_attr_t*)NULL,
				      gdisp_garbageCollectorThread,
				      (void*)kernel,
				      TRUE /* thread is detached */,
				      TRUE /* verbose            */);

  } /* test whether garbage collector thread already exists */

}


/*
 * Stop everything that deals with sampling process.
 */
void
gdisp_stopSamplingProcess (Kernel_T *kernel)
{

  /*
   * Stop the timer that performs steps on graphic plots.
   */
  if (kernel->timerIdentity > 0) {

    gtk_timeout_remove(kernel->timerIdentity);
    kernel->timerIdentity = 0;

  }


  /*
   * Stop sampling threads.
   * Do not wait (pthread_join) for those provider threads,
   * because they are detached.
   */
  kernel->samplingThreadMustExit = TRUE;

}
