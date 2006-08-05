/*

$Id: gdisp_consumers.c,v 1.19 2006-08-05 20:50:30 esteban Exp $

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
gdisp_sortProviderByUrl(gconstpointer data1,
			gconstpointer data2)
{

  Provider_T *provider1 = (Provider_T*)data1,
             *provider2 = (Provider_T*)data2;

  return (strcmp(provider1->pUrl->str,provider2->pUrl->str));

}


/*
 * Destroy symbol extended information.
 */
static void
gdisp_destroySymbolExtendedInformation (Provider_T *provider)
{

  Symbol_T  *symbol      = (Symbol_T*)NULL;
  GList     *extInfoList = (GList*)NULL;
  gchar    **extInfo     = (gchar**)NULL;
  guint      symbolCpt   = 0;

  /*
   * Loop over all provider symbols.
   */
  symbol = provider->pSymbolList;
  for (symbolCpt=0;
       symbolCpt<provider->pSymbolNumber;
       symbolCpt++, symbol++) {

    if (symbol->sExtInfoList != (GList*)NULL) {

      extInfoList = g_list_first(symbol->sExtInfoList);
      while (extInfoList != (GList*)NULL) {

	extInfo = (gchar**)extInfoList->data;

	g_free(extInfo[0]);
	g_free(extInfo[1]);
	g_free(extInfo);

	extInfoList = g_list_next(extInfoList);

      }

      g_list_free(symbol->sExtInfoList);
      symbol->sExtInfoList = (GList*)NULL;

    }

  } /* loop over all symbols */

}


/*
 * Get symbol extended information.
 */
static void
gdisp_getSymbolExtendedInformation (Provider_T *provider)
{

  const
  TSP_sample_symbol_extended_info_list_t *symbolExtInfoList = NULL;
  TSP_sample_symbol_extended_info_t      *symbolExtInfo     = NULL;
  TSP_extended_info_t                    *extInfo           = NULL;
  gchar    **extInfoElem = (gchar**)NULL;
  guint      pgiTableLen = 0;
  gint      *pgiTable    = (gint*)NULL;
  Symbol_T  *symbol      = (Symbol_T*)NULL;
  gint       symbolCpt   = 0;
  gint       extInfoCpt  = 0;
  gint       errorCode   = TSP_STATUS_OK;


  /*
   * Destroy any previous information.
   */
  gdisp_destroySymbolExtendedInformation(provider);

  /*
   * Memory allocation.
   */
  pgiTable = (gint*)g_malloc0(provider->pSymbolNumber * sizeof(gint));
  if (pgiTable == (gint*)NULL) {
    return;
  }

  /*
   * Build the table with all available PGIs.
   */
  symbol = provider->pSymbolList;
  for (symbolCpt=0;
       symbolCpt<provider->pSymbolNumber;
       symbolCpt++, symbol++) {

    if (symbol->sPgi != -1) {
      pgiTable[pgiTableLen++] = symbol->sPgi;
    }

  }

  /*
   * Request for TSP extended information.
   */
  errorCode = TSP_consumer_request_extended_information(provider->pHandle,
							pgiTable,
							pgiTableLen);
  if (errorCode != TSP_STATUS_OK) {
    g_free(pgiTable);
    return;
  }

  /*
   * Get TSP extended information.
   */
  symbolExtInfoList = TSP_consumer_get_extended_information(provider->pHandle);

  if (symbolExtInfoList == (TSP_sample_symbol_extended_info_list_t*)NULL) {
    g_free(pgiTable);
    return;
  }

  /*
   * Check the coherence of the response.
   */
  if (pgiTableLen !=
      symbolExtInfoList->TSP_sample_symbol_extended_info_list_t_len) {
    g_free(pgiTable);
    return;
  }

  /*
   * Loop over all available extended information.
   */
  symbol = provider->pSymbolList;

  symbolExtInfo =
    symbolExtInfoList->TSP_sample_symbol_extended_info_list_t_val;

  for (symbolCpt=0;
       symbolCpt<provider->pSymbolNumber;
       symbolCpt++, symbol++) {

    if (symbol->sPgi != -1) {

      /*
       * PGI must be the same.
       * FIXME : According to Erk, it is not true.
       */
      if (symbol->sPgi != symbolExtInfo->provider_global_index) {
	fprintf(stdout,
		"[TARGA] : Extended information -> incoherent PGI (%d <> %d).\n",
		symbol->sPgi,
		symbolExtInfo->provider_global_index);
      }

      if (symbolExtInfo->info.TSP_extended_info_list_t_len > 0) {

	extInfo = symbolExtInfo->info.TSP_extended_info_list_t_val;

	for (extInfoCpt=0;
	     extInfoCpt<symbolExtInfo->info.TSP_extended_info_list_t_len;
	     extInfoCpt++, extInfo++) {

	  extInfoElem = (gchar**)g_malloc0(2*sizeof(gchar*));
	  assert(extInfoElem);

	  extInfoElem[0] = gdisp_strDup(extInfo->key);
	  extInfoElem[1] = gdisp_strDup(extInfo->value);

	  symbol->sExtInfoList = g_list_append(symbol->sExtInfoList,
					       (gpointer)extInfoElem);

	} /* loop over all information */

      } /* has extended information */

      symbolExtInfo++;

    } /* correct PGI */

  } /* loop over all PGIs */


  /*
   * Free memory.
   */
  g_free(pgiTable);

}


/*
 * Manage a new provider -> read information and add symbols.
 */
static int
gdisp_insertProvider ( Kernel_T *kernel,
		       gchar    *url )
{

  TSP_provider_t            *provider      = NULL;
  Provider_T                *newProvider   = (Provider_T*)NULL;
  Symbol_T                  *symbolList    = (Symbol_T*)NULL;
  GString                   *messageString = (GString*)NULL;
  gint                       symbolCpt     = 0;
  gint                       requestStatus = 0;
  TSP_sample_symbol_info_t  *symbolInfo    = (TSP_sample_symbol_info_t*)NULL;
  const TSP_answer_sample_t *providerInfo  = (const TSP_answer_sample_t*)NULL;

  /*
   * Check the original url is not known yet.
   */
  if (gdisp_getProviderByOriginalUrl(kernel,url) != (Provider_T*)NULL) {
    return -1;
  }

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
    newProvider->pOriginalUrl = g_string_new(url);
    newProvider->pHandle      = provider;
    newProvider->pIdentity    = kernel->providerId++ % GD_MAX_PROVIDER_NUMBER;
    newProvider->pUrl         =
      g_string_new(TSP_consumer_get_connected_name(newProvider->pHandle));
    assert(newProvider->pUrl);

    newProvider->pStatus = GD_SESSION_CLOSED;
    
    kernel->providerList = g_list_insert_sorted(kernel->providerList,
						(gpointer)newProvider,
						gdisp_sortProviderByUrl);
    assert(kernel->providerList);
    
    
    /*
     * Ask the provider for a new consumer session.
     * Set up its status to 'SESSION_OPENED'.
     */
    requestStatus = TSP_consumer_request_open(newProvider->pHandle,
					      (gint)NULL,
					      (gchar**)NULL);

    if (TSP_STATUS_OK == requestStatus) {
      
      /*
       * Now the session is opened, get back all available information.
       * Keep the current status, since symbols are not requested here.
       */
      if (kernel->retreiveAllSymbols == TRUE) {
	requestStatus =
	  TSP_consumer_request_information(newProvider->pHandle);
      }
      else {
	requestStatus =
	  TSP_consumer_request_filtered_information(newProvider->pHandle,
						    TSP_FILTER_MINIMAL,
						    MINIMAL_STRING);
      }

      if (TSP_STATUS_OK == requestStatus) {
	
	/* Do not free 'providerInfo' structure */
	providerInfo = TSP_consumer_get_information(newProvider->pHandle);
	assert(providerInfo);

	newProvider->pVersionId           = providerInfo->version_id;
	newProvider->pChannelId           = providerInfo->channel_id;
	newProvider->pTimeOut             = providerInfo->provider_timeout;
	newProvider->pGroupNumber         = providerInfo->provider_group_number;
	newProvider->pBaseFrequency       = providerInfo->base_frequency;
	newProvider->pMaxPeriod           = providerInfo->max_period;
	newProvider->pMaxClientNumber     = providerInfo->max_client_number;
	newProvider->pCurrentClientNumber = providerInfo->current_client_number;
	newProvider->pSymbolNumber        = providerInfo->symbols.TSP_sample_symbol_info_list_t_len;

	/*
	 * Shall I retreive all symbols ?? It depends on the action.
	 */
	if (kernel->retreiveAllSymbols == TRUE) {

	  if (newProvider->pSymbolNumber > 0) {

	    /*
	     * Allocate symbol table.
	     */
	    newProvider->pSymbolList = (Symbol_T*)
	      g_malloc0(newProvider->pSymbolNumber * sizeof(Symbol_T));
	    assert(newProvider->pSymbolList);

	    /*
	     * Allocate symbol hash table for extra-boosted search.
	     */
	    newProvider->pSymbolHashTable = hash_open('.','z');
	    assert(newProvider->pSymbolHashTable);

	  }

	  /*
	   * Loop over all symbols.
	   */
	  symbolInfo = providerInfo->symbols.TSP_sample_symbol_info_list_t_val;
	  symbolList = newProvider->pSymbolList;

	  for (symbolCpt=0;
	       symbolCpt<newProvider->pSymbolNumber;
	       symbolCpt++, symbolList++, symbolInfo++) {

	    /* copy the whole structure... */
	    symbolList->sInfo = *symbolInfo;
	    symbolList->sPgi  =  symbolInfo->provider_global_index;

	    /* ... and do not forget to duplicate pointers */
	    symbolList->sInfo.name = gdisp_strDup(symbolInfo->name);

	    /* Default is : at maximum frequency without offset */
	    symbolList->sInfo.period = 1;
	    symbolList->sInfo.phase  = 0;

	    /* do not treat table */
	    if (symbolList->sInfo.dimension > 1) {
	      symbolList->sInfo.provider_global_index = -1;
	      symbolList->sPgi = -1;
	    }

	    /* insert symbol into hash table */
	    hash_append(newProvider->pSymbolHashTable,
			symbolList->sInfo.name,
			(void*)symbolList);

	  } /* symbolCpt */

	  /*
	   * Get symbol extended information.
	   */
	  gdisp_getSymbolExtendedInformation(newProvider);

	} /* retreiveAllSymbols == TRUE */

	/*
	 * Session is now opened towards provider.
	 */
	newProvider->pStatus = GD_SESSION_OPENED;

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,
			 "Session opened on <%s>",
			 newProvider->pUrl->str);
	(*kernel->outputFunc)(kernel,messageString,GD_MESSAGE);


      } /* requestStatus == TRUE (TSP_consumer_request_information) */

      else {

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,"Cannot get back information from");
	(*kernel->outputFunc)(kernel,messageString,GD_ERROR);

	messageString = g_string_new((gchar*)NULL);
	g_string_sprintf(messageString,
			 "<%s> provider. Session is closed.",
			 newProvider->pUrl->str);
	(*kernel->outputFunc)(kernel,messageString,GD_ERROR);

      }

    } /* requestStatus == TRUE (TSP_consumer_request_open) */

    else {

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,"Cannot open a session towards");
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

      messageString = g_string_new((gchar*)NULL);
      g_string_sprintf(messageString,
		       "<%s> provider. Aborting.",
		       newProvider->pUrl->str);
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

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

  (*kernel->outputFunc)(kernel,messageString,GD_WARNING);

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
  gint            hostStatus         = 0;
  GList          *hostList           = (GList*)NULL;
  GList          *urlList            = (GList*)NULL;
  gint            urlsFound          = 0;
  GString        *messageString      = (GString*)NULL;

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

  if (TSP_STATUS_OK != hostStatus) {

    messageString = g_string_new((gchar*)NULL);
    g_string_sprintf(messageString,"TSP Initialisation failed.");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);

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
      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);
      
    }
    else {
      
      g_string_sprintf(messageString,
		       "Local host is '%s'.",
		       localHostName);
      (*kernel->outputFunc)(kernel,messageString,GD_MESSAGE);
      
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
 * Get back the provider from an original url.
 */
Provider_T*
gdisp_getProviderByOriginalUrl ( Kernel_T *kernel,
				 gchar    *originalUrl )
{

  GList      *providerItem =      (GList*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;

  /*
   * Loop upon all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (strcmp(provider->pOriginalUrl->str,originalUrl) == 0) {
      return provider;
    }

    providerItem = g_list_next(providerItem);

  }

  return (Provider_T*)NULL;

}


/*
 * GDISP+ is a TSP consumer.
 * Close everything related to consummation.
 */
void
gdisp_consumingEnd (Kernel_T *kernel)
{

  GList      *providerItem = (GList*)NULL;
  Provider_T *provider     = (Provider_T*)NULL;
  guint       symbolCpt    = 0;

  assert(kernel);

  /*
   * Release all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /*
     * Close TSP connection.
     */
    TSP_consumer_request_close(provider->pHandle);

    for (symbolCpt=0; symbolCpt<provider->pSymbolNumber; symbolCpt++) {
      g_free(provider->pSymbolList[symbolCpt].sInfo.name);
      provider->pSymbolList[symbolCpt].sInfo.name = (gchar*)NULL;
    }

    gdisp_destroySymbolExtendedInformation(provider);

    if (provider->pSymbolList != (Symbol_T*)NULL) {
      g_free(provider->pSymbolList);
      provider->pSymbolList = (Symbol_T*)NULL;
    }

    if (provider->pSymbolHashTable != (hash_t*)NULL) {
      hash_close(provider->pSymbolHashTable);
      provider->pSymbolHashTable = (hash_t*)NULL;
    }

    if (provider->pSymbolHashTablePGI != (hash_t*)NULL) {
      hash_close(provider->pSymbolHashTablePGI);
      provider->pSymbolHashTablePGI = (hash_t*)NULL;
    }

    if (provider->pSampleList.TSP_sample_symbol_info_list_t_len != 0) {
      g_free(provider->pSampleList.TSP_sample_symbol_info_list_t_val);
      provider->pSampleList.TSP_sample_symbol_info_list_t_val = (TSP_sample_symbol_info_t*)NULL;
    }

    g_string_free(provider->pUrl        ,TRUE);
    g_string_free(provider->pOriginalUrl,TRUE);

    g_free(provider);
    provider = (Provider_T*)NULL;

    providerItem = g_list_next(providerItem);

  }

  if (kernel->providerList != (GList*)NULL) {
    g_list_free(kernel->providerList);
    kernel->providerList = (GList*)NULL;
  }


  /*
   * Destroy all hosts & URLs.
   */
#if defined(GD_DESTROY_HOSTS_AND_URLS)
  gdisp_destroyHosts(kernel);
  gdisp_destroyUrls (kernel);
#endif


  /*
   * Leave consumming services.
   */
  TSP_consumer_end();

}

