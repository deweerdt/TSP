/*!  \file 

$Id: gdisp_consumers.c,v 1.9 2004-10-22 20:17:34 esteban Exp $

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

File      : GDISP consumer management.

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
#include <sys/time.h>
#include <unistd.h>


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
 * Function in order to sort providers alphabetically' when inserting
 * them into the double-linked list in the kernel.
 */
static gint
gdisp_sortProviderByName(gconstpointer data1,
			 gconstpointer data2)
{

  Provider_T *provider1 = (Provider_T*)data1,
             *provider2 = (Provider_T*)data2;

  return (strcmp(provider1->pUrl->str,provider2->pUrl->str));

}


/*
 * Manage a new provider -> read information and add symbols.
 */
static int
gdisp_insertProvider ( Kernel_T *kernel,
		       gchar    *url )
{

  GString        *messageString    = (GString*)NULL;

  static guint     providerIdentity = 0;
  TSP_provider_t  *provider        = NULL;
  gint            symbolCpt        = 0;

  Provider_T     *newProvider      = (Provider_T*)NULL;
  gint            requestStatus    = 0;

  const TSP_consumer_information_t *providerInfo =
                                  (const TSP_consumer_information_t*)NULL;

  /*
   * Try to connect to the URL.
   */
  provider = TSP_consumer_connect_url(url);

  if (provider != (TSP_provider_t*)NULL) {

    /*
     * Store all available information for this provider.
     */
    
    /*
     * Allocate memory for this new provider.
     * Set up its status to 'FROM_SCRATCH'.
     */
    newProvider = (Provider_T*)g_malloc0(sizeof(Provider_T));
    assert(newProvider);
    
    newProvider->pStatus = GD_FROM_SCRATCH;
    
    
    /*
     * Now store the handle, get back the name.
     * Set up its status to 'SESSION_CLOSED'.
     * Insert it into the kernel provider list.
     */
    newProvider->pHandle   = provider;
    newProvider->pIdentity = providerIdentity++;
    newProvider->pUrl      =
      g_string_new(TSP_consumer_get_connected_name(newProvider->pHandle));
    assert(newProvider->pUrl);
    
    newProvider->pStatus = GD_SESSION_CLOSED;
    
    kernel->providerList = g_list_insert_sorted(kernel->providerList,
						(gpointer)newProvider,
						gdisp_sortProviderByName);
    assert(kernel->providerList);
    
    
    /*
     * Ask the provider for a new consumer session.
     * Set up its status to 'SESSION_OPENED'.
     */
    requestStatus = TSP_consumer_request_open(newProvider->pHandle,
					      (gint)NULL,
					      (gchar**)NULL);
    if (requestStatus == TRUE) {
      
      /*
       * Now the session is opened, get back all available information.
       * Keep the current status, since symbols are not requested here.
       */
      requestStatus = TSP_consumer_request_information(newProvider->pHandle);
      if (requestStatus == TRUE) {
	
	/* Do not free 'providerInfo' structure */
	providerInfo = TSP_consumer_get_information(newProvider->pHandle);
	assert(providerInfo);

	newProvider->pBaseFrequency       = providerInfo->base_frequency;
	newProvider->pMaxPeriod           = providerInfo->max_period;
	newProvider->pMaxClientNumber     = providerInfo->max_client_number;
	newProvider->pCurrentClientNumber =
	                                providerInfo->current_client_number;
	newProvider->pSymbolNumber        = providerInfo->symbols.len;

	if (newProvider->pSymbolNumber > 0) {

	  newProvider->pSymbolList = (Symbol_T*)
	    g_malloc0(newProvider->pSymbolNumber * sizeof(Symbol_T));
	  assert(newProvider->pSymbolList);

	}

	for (symbolCpt=0; symbolCpt<newProvider->pSymbolNumber; symbolCpt++) {

	  /*
	   * I do not duplicate the name, because I want to save up memory.
	   * So what I do is good if I do not call
	   * 'TSP_consumer_request_information' again, because this routine
	   * releases (via "free") the "name" variable.
	   */
	  newProvider->pSymbolList[symbolCpt].sInfo.name   =
                                  providerInfo->symbols.val[symbolCpt].name;
	  newProvider->pSymbolList[symbolCpt].sInfo.index  =
                                  providerInfo->symbols.val[symbolCpt].index;

	  /* Default is : at maximum frequency without offset */
	  newProvider->pSymbolList[symbolCpt].sInfo.period = 1;
	  newProvider->pSymbolList[symbolCpt].sInfo.phase  = 0;
	  newProvider->pSymbolList[symbolCpt].sHasChanged  = FALSE;

	} /* symbolCpt */

	/*
	 * Session is now opened towards provider.
	 */
	newProvider->pStatus = GD_SESSION_OPENED;

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,
			 "Session opened on <%s>",
			 newProvider->pUrl->str);
	kernel->outputFunc(kernel,messageString,GD_MESSAGE);


      } /* requestStatus == TRUE (TSP_consumer_request_information) */

      else {

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,"Cannot get back information from");
	kernel->outputFunc(kernel,messageString,GD_ERROR);

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,
			 "<%s> provider. Session is closed.",
			 newProvider->pUrl->str);
	kernel->outputFunc(kernel,messageString,GD_ERROR);

      }

    } /* requestStatus == TRUE (TSP_consumer_request_open) */

    else {

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,"Cannot open a session towards");
      kernel->outputFunc(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "<%s> provider. Aborting.",
		       newProvider->pUrl->str);
      kernel->outputFunc(kernel,messageString,GD_ERROR);

    }
    
  } /* End if available provider */


  return (provider == NULL ? -1 : 0);

}


/*
 * Insert possible providers on a host.
 */
static void
gdisp_insertHostProviders ( Kernel_T *kernel,
			    Host_T   *host )
{
  GString        *messageString    = (GString*)NULL;
  gchar          *hostUrl          = (gchar*) NULL;
  gint            providerCpt      = 0;
  gint            providersFound   = 0;

  /*
   * Look for and insert providers on the given host.
   */
  hostUrl = g_malloc0(strlen(host->hName->str) + 10);
  for (providerCpt=0; providerCpt<TSP_MAX_SERVER_NUMBER; providerCpt++) {

    sprintf(hostUrl, "//%s/:%d", host->hName->str, providerCpt);

    if (gdisp_insertProvider(kernel,hostUrl) == 0) {
      providersFound++;
    }

  }
  g_free(hostUrl);

  /*
   * Report the number of providers that have been found.
   */
  messageString = g_string_new((gchar*)NULL);
  if (providersFound == 0) {

    g_string_sprintf(messageString,
		     "No TSP provider found on host %s.",
		     host->hName->str);

  }
  else {

    g_string_sprintf(messageString,
		     "%d TSP provider(s) found on host %s.",
		     providersFound,
		     host->hName->str);

  }

  kernel->outputFunc(kernel,messageString,GD_WARNING);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * GDISP+ is a TSP consumer.
 * Initialize here the consummation management.
 *  - retreive all available providers on a given URLs/hosts.
 *  - ...
 */
void
gdisp_consumingInit (Kernel_T *kernel)
{

#define _HOST_NAME_MAX_LEN_ 256
  gchar           localHostName[_HOST_NAME_MAX_LEN_];
  gint            hostStatus       = 0;
  GList          *hostList         = (GList*)NULL;
  GList          *urlList          = (GList*)NULL;
  gint            urlsFound        = 0;
  GString        *messageString    = (GString*)NULL;

  /*
   * Few checking...
   */
  assert(kernel);



  /* --------------------- TSP INITIALISATION ---------------------- */

  /*
   * Initialisation for TSP library.
   * This function removes the arguments it knows from the argument list,
   * leaving anything it does not recognize for GDISP application to parse
   * or ignore. 
   */
  hostStatus = TSP_consumer_init(&kernel->argCounter,
				 &kernel->argTable);

  if (hostStatus == FALSE) {

    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,"TSP Initialisation failed.");
    kernel->outputFunc(kernel,messageString,GD_ERROR);

    return;

  }


  /* ------------------------ URL LIST ------------------------- */

  /*
   * Insert all URLs.
   */
  urlList = g_list_first(kernel->urlList);
  while (urlList != (GList*)NULL) {

    if (gdisp_insertProvider(kernel,
			     (gchar*)urlList->data) == 0) {
      urlsFound++;
    }

    urlList = g_list_next(urlList);

  }

  /* ------------------------ HOST LIST ------------------------- */

  /*
   * Get back local host and insert it if no URL found.
   */
  if (urlsFound == 0) {

    hostStatus    = gethostname(localHostName,_HOST_NAME_MAX_LEN_);
    messageString = g_string_new((gchar*)NULL);
    
    if (hostStatus == -1) {
      
      g_string_sprintf(messageString,"Local host is UNKNOWN.");
      kernel->outputFunc(kernel,messageString,GD_ERROR);
      
    }
    else {
      
      g_string_sprintf(messageString,
		       "Local host is '%s'.",
		       localHostName);
      kernel->outputFunc(kernel,messageString,GD_MESSAGE);
      
      gdisp_addHost(kernel,localHostName);

    }
  }

  /*
   * Insert all hosts.
   */
  hostList = g_list_first(kernel->hostList);
  while (hostList != (GList*)NULL) {

    gdisp_insertHostProviders (kernel,
			       (Host_T*)hostList->data);

    hostList = g_list_next(hostList);

  }
}


/*
 * Return the number of available providers in the provider list.
 */
guint
gdisp_getProviderNumber (Kernel_T *kernel)
{

  assert(kernel);

  if (kernel->providerList != (GList*)NULL) {

    return g_list_length(kernel->providerList);

  }
  else {

    return 0;

  }

}


/*
 * GDISP+ is a TSP consumer.
 * Close everything related to consummation.
 */
void
gdisp_consumingEnd (Kernel_T *kernel)
{

  GList      *providerItem =      (GList*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;

  assert(kernel);


  /*
   * Release all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    g_free(provider->pSymbolList);

    if (provider->pSampleList.len != 0)
      free(provider->pSampleList.val);

    g_free(provider);

    providerItem = g_list_next(providerItem);

  }

  g_list_free(kernel->providerList);
  kernel->providerList = (GList*)NULL;


  /*
   * Destroy all hosts & URLs.
   */
  gdisp_destroyHosts(kernel);
  gdisp_destroyUrls (kernel);


  /*
   * Leave consumming services.
   */
  TSP_consumer_end();

}

