/*

$Id: gdisp_configuration.c,v 1.18 2007-11-09 18:25:32 rhdv Exp $

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

File      : Graphic Tool Configuration Management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"
#include "gdisp_xml.h"


/*
 --------------------------------------------------------------------
                             PRIVATE DEFINITIONS
 --------------------------------------------------------------------
*/

#define UTF8_TO_CHAR (char*)

#define GD_PROVIDER_URL     1
#define GD_PROVIDER_SYMBOLS 2

typedef struct SymbolInConf_T_ {

  guint     sicIndex;
  gchar    *sicName;
  Symbol_T *sicReference;

} SymbolInConf_T;

/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)

/*
 * Function in order to sort provider symbol pointers alphabetically.
 */
static gint
gdisp_sortProviderSymbolPtrByName ( gconstpointer data1,
				    gconstpointer data2 )
{

  Symbol_T **symbol1 = (Symbol_T**)data1,
           **symbol2 = (Symbol_T**)data2;

  return (strcmp((*symbol1)->sInfo.name,(*symbol2)->sInfo.name));

}


/*
 * Check sampling symbol references.
 */
static void
gdisp_checkSamplingSymbolReferences ( Kernel_T *kernel )
{

  SymbolInConf_T *symbolInConf     = (SymbolInConf_T*)NULL;
  GList          *providerItem     = (GList*)NULL;
  Provider_T     *provider         = (Provider_T*)NULL;
  guint           cptSic           = 0;

  Symbol_T        targetSymbol;
  Symbol_T       *targetSymbolPtr  = &targetSymbol;

  guint           searchCpt        = 0;
  guint           searchTableSize  = 0;
  Symbol_T      **searchTable      = (Symbol_T**)NULL;
  Symbol_T      **searchTablePtr   = (Symbol_T**)NULL;
  Symbol_T       *searchSymbol     = (Symbol_T*)NULL;

  Symbol_T      **foundSymbol      = (Symbol_T**)NULL;

  /*
   * Loop upon all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pSymbolInConfiguration != (void*)NULL) {
 
      /*
       * Memory allocation for temporarily search table.
       */
      if (searchTableSize == 0) {

	searchTableSize = provider->pSymbolNumber;
	searchTable     = (Symbol_T**)g_malloc0(searchTableSize *
						sizeof(Symbol_T*));

      }
      else if (searchTableSize < provider->pSymbolNumber) {

	searchTableSize = provider->pSymbolNumber;
	searchTable     = (Symbol_T**)g_realloc(searchTable,
						searchTableSize *
						sizeof(Symbol_T*));

      }

      if (searchTable == (Symbol_T**)NULL) {
	/* FIXME : memory allocation error management */
	return;
      }

      searchTablePtr = searchTable;
      searchSymbol   = provider->pSymbolList;

      for (searchCpt=0;
	   searchCpt<searchTableSize;
	   searchCpt++) {

	*searchTablePtr++ = searchSymbol++;

      }

      /*
       * Sort provider symbols by name.
       */
      qsort((void*)searchTable,
	    searchTableSize,
	    sizeof(Symbol_T*),
	    gdisp_sortProviderSymbolPtrByName);

      /*
       * Look for symbol in configuration.
       */
      symbolInConf = (SymbolInConf_T*)provider->pSymbolInConfiguration;

      for (cptSic = 0;
	   cptSic < provider->pNbSymbolInConfiguration;
	   cptSic++) {

	targetSymbol.sInfo.name = symbolInConf->sicName;

	foundSymbol = (Symbol_T**)bsearch(&targetSymbolPtr,
					  (void*)searchTable,
					  searchTableSize,
					  (size_t)sizeof(Symbol_T*),
					  gdisp_sortProviderSymbolPtrByName);

	symbolInConf->sicReference = *foundSymbol;
	symbolInConf++;

      } /* for */

    } /* if */

    providerItem = g_list_next(providerItem);

  } /* while */

  /*
   * Free temporarily search table.
   */
  if (searchTableSize > 0) {

    g_free(searchTable);

  }

}


/*
 * Get back a symbol thanks to its index in the configuration.
 */
static Symbol_T*
gdisp_getSymbolInConfByIndex ( Kernel_T *kernel,
			       guint     index )
{

  SymbolInConf_T *symbolInConf     = (SymbolInConf_T*)NULL;
  GList          *providerItem     = (GList*)NULL;
  Provider_T     *provider         = (Provider_T*)NULL;
  guint           cptSic           = 0;
  guint           startSicIndex    = 0;
  guint           endSicIndex      = 0;

  /*
   * Loop upon all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pSymbolInConfiguration != (void*)NULL) {

      symbolInConf  = (SymbolInConf_T*)provider->pSymbolInConfiguration;

      startSicIndex = symbolInConf->sicIndex;
      endSicIndex   = symbolInConf->sicIndex +
                      provider->pNbSymbolInConfiguration - 1;

      if (startSicIndex <= index && index <= endSicIndex) {

	for (cptSic = 0;
	     cptSic < provider->pNbSymbolInConfiguration;
	     cptSic++) {

	  if (symbolInConf->sicIndex == index) {
	    return symbolInConf->sicReference;
	  }

	  symbolInConf++;

	} /* for */

      } /* if */

    } /* if */

    providerItem = g_list_next(providerItem);

  } /* while */

  return (Symbol_T*)NULL;

}

#else

/*
 * Get back configuration symbol by index.
 */
static Symbol_T*
gdisp_getSymbolInConfByIndex ( Kernel_T *kernel,
			       guint     index )
{

  GList          *providerItem     = (GList*)NULL;
  Provider_T     *provider         = (Provider_T*)NULL;
  Symbol_T       *symbol           = (Symbol_T*)NULL;
  guint           cptSymbol        = 0;
  guint           startSicIndex    = 0;
  guint           endSicIndex      = 0;

  /*
   * Loop upon all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pSymbolNumber != 0) {

      symbol = provider->pSymbolList;

      startSicIndex = symbol->sPgi;
      endSicIndex   = symbol->sPgi + provider->pSymbolNumber - 1;

      if (startSicIndex <= index && index <= endSicIndex) {

	for (cptSymbol = 0;
	     cptSymbol < provider->pSymbolNumber;
	     cptSymbol++) {

	  if (symbol->sPgi == index) {
	    return symbol;
	  }

	  symbol++;

	} /* for */

      } /* if */

    } /* if */

    providerItem = g_list_next(providerItem);

  } /* while */

  return (Symbol_T*)NULL;

}

#endif

/*
 * Save all symbols of a graphic plot.
 */
static gint
gdisp_saveGraphicPlotSymbolList ( Kernel_T         *kernel,
				  PlotSystemData_T *plotSystemData,
				  xmlTextWriterPtr  writer,
				  xmlChar          *indentBuffer,
				  gboolean         *isFirstSymbol,
				  GList            *symbolList,
				  guchar            zoneId )
{

  gint      errorCode     = 0;
  Symbol_T *symbol        = (Symbol_T*)NULL;
  GList    *attributeList = (GList*)NULL;
  gchar     indexBuffer [128];
  gchar     zoneBuffer  [128];


  /*
   * Loop over all symbols.
   */
  symbolList = g_list_first(symbolList);
  while (symbolList != (GList*)NULL) {

    symbol = (Symbol_T*)symbolList->data;

    /*
     * Sampled symbol index and name.
     */
    sprintf(indexBuffer,"%d",symbol->sPgi);
    sprintf(zoneBuffer ,"%c",zoneId);

    attributeList = g_list_append(attributeList,
				  (gpointer)"index");
    attributeList = g_list_append(attributeList,
				  (gpointer)indexBuffer);

    if (zoneId != 0) {

      attributeList = g_list_append(attributeList,
				    (gpointer)"zone");
      attributeList = g_list_append(attributeList,
				    (gpointer)zoneBuffer);

    }

    /*
     * The symbol may have specific attributes
     * relative to the plot it belongs to. Get them back.
     */
    (*plotSystemData->plotSystem->psGetSymbolAttributes)
                                               (kernel,
						plotSystemData->plotData,
						symbol,
						attributeList);

    /*
     * Write all that down.
     */
    errorCode =
      gdisp_xmlWriteAttributeList(writer,
				  *isFirstSymbol == TRUE ?
				  GD_INCREASE_INDENTATION :
				  GD_DO_NOT_CHANGE_INDENTATION,
				  indentBuffer,
				  (xmlChar*)"sampledSymbol",
				  TRUE, /* end up element */
				  attributeList);

    if (errorCode < 0) {
      return errorCode;
    }

    *isFirstSymbol = FALSE;
    symbolList     = g_list_next(symbolList);

    g_list_free(attributeList);
    attributeList = (GList*)NULL;

  } /* symbolList != (GList*)NULL */

  return 0; /* ok */

}


/*
 * Save a graphic plot in the configuration.
 */
static gint
gdisp_saveOneGraphicPlot ( Kernel_T         *kernel,
			   xmlTextWriterPtr  writer,
			   xmlChar          *indentBuffer,
			   guint             plotIdentity,
			   PlotSystemData_T *plotSystemData )

{

  int               errorCode      = 0;
  GList            *symbolList     = (GList*)NULL;
  GList            *attributeList  = (GList*)NULL;
  GArray           *plotZones      = (GArray*)NULL;
  guint             currentZoneId  = 0;
  PlotSystemZone_T *currentZone    = (PlotSystemZone_T*)NULL;
  PlotSystemInfo_T  plotInformation;
  gboolean          isFirstSymbol  = TRUE;
  gchar             sPlotRow      [128];
  gchar             sPlotColumn   [128];
  gchar             sPlotNbRows   [128];
  gchar             sPlotNbColumns[128];


  /*
   * If plot system is supported, ask for referenced symbols.
   */
  if (plotSystemData->plotSystem->psIsSupported == TRUE) {

    (*plotSystemData->plotSystem->psGetInformation)(kernel,
						    &plotInformation);

    /* do not save an unknown plot */
    if (plotInformation.psName == (gchar*)NULL) {
      return 0;
    }

    /* do not save default plot */
    if (strcmp(plotInformation.psName,"Default") == 0) {
      return 0; /* ok */
    }

    /*
     * Start an element whose name is 'Plot' with its properties.
     * This element is a child of current 'Page'.
     */
    sprintf(sPlotRow      ,"%d",plotSystemData->plotRow      );
    sprintf(sPlotColumn   ,"%d",plotSystemData->plotColumn   );
    sprintf(sPlotNbRows   ,"%d",plotSystemData->plotNbRows   );
    sprintf(sPlotNbColumns,"%d",plotSystemData->plotNbColumns);

    attributeList = g_list_append(attributeList,
				  (gpointer)"type");
    attributeList = g_list_append(attributeList,
				  (gpointer)plotInformation.psName);

    attributeList = g_list_append(attributeList,
				  (gpointer)"row");
    attributeList = g_list_append(attributeList,
				  (gpointer)sPlotRow);

    attributeList = g_list_append(attributeList,
				  (gpointer)"column");
    attributeList = g_list_append(attributeList,
				  (gpointer)sPlotColumn);

    attributeList = g_list_append(attributeList,
				  (gpointer)"nbRows");
    attributeList = g_list_append(attributeList,
				  (gpointer)sPlotNbRows);

    attributeList = g_list_append(attributeList,
				  (gpointer)"nbColumns");
    attributeList = g_list_append(attributeList,
				  (gpointer)sPlotNbColumns);

    /*
     * The plot may have specific attributes. Get them back.
     */
    (*plotSystemData->plotSystem->psGetPlotAttributes)
                                               (kernel,
						plotSystemData->plotData,
						attributeList);

    /*
     * Write attributes.
     */
    errorCode =
      gdisp_xmlWriteAttributeList(writer,
				  plotIdentity == 0 ?
				  GD_INCREASE_INDENTATION :
				  GD_DO_NOT_CHANGE_INDENTATION,
				  indentBuffer,
				  (xmlChar*)"Plot",
				  FALSE, /* do not end up element */
				  attributeList);

    g_list_free(attributeList);
    attributeList = (GList*)NULL;

    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Ask for referenced symbols. Take care of zones !!!
     */
    plotZones = (*plotSystemData->plotSystem->psGetDropZones)(kernel);
 
    if (plotZones != (GArray*)NULL) {

      /*
       * Loop over all declared zones.
       */
      while (currentZoneId < plotZones->len) {

	currentZone =
	  &g_array_index(plotZones,PlotSystemZone_T,currentZoneId);

	symbolList =
	  (*plotSystemData->plotSystem->psGetSymbols)(kernel,
						      plotSystemData->plotData,
						      currentZone->pszId);

	errorCode = gdisp_saveGraphicPlotSymbolList(kernel,
						    plotSystemData,
						    writer,
						    indentBuffer,
						    &isFirstSymbol,
						    symbolList,
						    currentZone->pszId);

	currentZoneId++;

      } /* loop over all declared zones */

    } /* plotZones != (GArray*)NULL */

    else {

      /*
       * No zone.
       */
      isFirstSymbol = TRUE;
      symbolList    =
	(*plotSystemData->plotSystem->psGetSymbols)(kernel,
						    plotSystemData->plotData,
						    0 /* dummy zone id */);

      errorCode = gdisp_saveGraphicPlotSymbolList(kernel,
						  plotSystemData,
						  writer,
						  indentBuffer,
						  &isFirstSymbol,
						  symbolList,
						  0 /* dummy zone id */);

    } /* end else */

    /*
     * Close 'Plot' XML element.
     */
    gdisp_xmlGotoLine(writer);
    gdisp_xmlIndent  (writer,
		      indentBuffer,
		      GD_DECREASE_INDENTATION);
#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterEndElement(writer);
#endif
	  
    if (errorCode < 0) {
      return errorCode;
    }

  } /* psIsSupported */

  return 0; /* ok */

}


/*
 * Loop over all plot of a given graphic page.
 */
static gint
gdisp_saveOneGraphicPage ( Kernel_T         *kernel,
			   xmlTextWriterPtr  writer,
			   xmlChar          *indentBuffer,
			   Page_T           *page )
{

  int               errorCode      = 0;
  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;
  guint             plotIdentity   = 0;
  guint             nColumn        = 0;
  guint             nRow           = 0;
  gchar             pageRow    [128];
  gchar             pageColumn [128];


  /*
   * Start an element whose name is 'Page' with its properties.
   * This element is a child of 'Graphics'.
   */
  sprintf(pageRow   ,"%d",page->pRows   );
  sprintf(pageColumn,"%d",page->pColumns);

  errorCode = gdisp_xmlWriteAttributes(writer,
				       GD_INCREASE_INDENTATION,
				       indentBuffer,
				       (xmlChar*)"Page",
				       FALSE, /* do not end up element */
				       (xmlChar*)"title",
				       (xmlChar*)page->pName->str,
				       (xmlChar*)"rows",
				       (xmlChar*)pageRow,
				       (xmlChar*)"columns",
				       (xmlChar*)pageColumn,
				       (xmlChar*)NULL);

  if (errorCode < 0) {
    return errorCode;
  }


  /*
   * Loop over all graphic plots of the current page.
   */
  for (nRow=0; nRow<page->pRows; nRow++) {

    for (nColumn=0; nColumn<page->pColumns; nColumn++) {

      plotIdentity   = nRow * page->pColumns + nColumn;
      plotSystemData = &page->pPlotSystemData[plotIdentity];


      errorCode = gdisp_saveOneGraphicPlot(kernel,
					   writer,
					   indentBuffer,
					   plotIdentity,
					   plotSystemData);

      if (errorCode < 0) {
	return errorCode;
      }

    } /* columns */

  } /* rows */


  /*
   * Close 'Page' XML element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);
#ifdef XMLWRITER_SUPPORTED
  errorCode = xmlTextWriterEndElement(writer);
#endif

  if (errorCode < 0) {
    return errorCode;
  }

  return 0; /* ok */

}


/*
 * Loop over all graphic pages and save all created plots.
 */
static gint
gdisp_saveGraphicPages ( Kernel_T         *kernel,
			 xmlTextWriterPtr  writer,
			 xmlChar          *indentBuffer )
{

  int     errorCode = 0;
  GList  *pageItem  = (GList*)NULL;
  Page_T *page      = (Page_T*)NULL;


  /*
   * Search the corresponding graphic page in the kernel.
   */
  pageItem = g_list_first(kernel->pageList);
  while (pageItem != (GList*)NULL) {

    page = (Page_T*)pageItem->data;

    /*
     * Save content of current graphic page.
     */
    errorCode = gdisp_saveOneGraphicPage(kernel,
					 writer,
					 indentBuffer,
					 page);

    if (errorCode < 0) {
      return errorCode;
    }
      
    /*
     * Next page.
     */
    page     = (Page_T*)NULL;
    pageItem = g_list_next(pageItem);

  }

  return 0; /* ok */

}


/*
 * Loop over all providers and save all sampled symbols.
 */
static gint
gdisp_saveProviderSampledSymbols ( Kernel_T         *kernel,
				   xmlTextWriterPtr  writer,
				   xmlChar          *indentBuffer )
{

  int           errorCode      = 0;
  GList        *providerItem   = (GList*)NULL;
  Provider_T   *provider       = (Provider_T*)NULL;
  Symbol_T     *symbol         = (Symbol_T*)NULL;
  gint          pSampleCpt     = 0;
  SampleList_T *pSampleList    = (SampleList_T*)NULL;
  guint         pSampleMax     = 0;
  guint         sPgi           = 1;
  char         *typeBuffer     = NULL;
  char          indexBuffer     [256];
  char          dimensionBuffer [256];
  char          offsetBuffer    [256];
  char          nElemBuffer     [256];
  char          periodBuffer    [256];
  char          phaseBuffer     [256];

#define GD_SAMPLE_PGI_AS_STRING_LENGTH 10
  gchar         samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH];
  gchar        *samplePGIasString =        (gchar*)NULL;

  /*
   * Loop over all providers and save all sampled symbols.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    /*
     * Start an element whose name is 'Provider' with one property
     * which is the original URL of the provider.
     * This element is a child of 'Kernel'.
     */
    errorCode =
      gdisp_xmlWriteAttributes(writer,
			       GD_INCREASE_INDENTATION,
			       indentBuffer,
			       (xmlChar*)"Provider",
			       FALSE, /* do not end up element */
			       (xmlChar*)"url",
			       (xmlChar*)provider->pOriginalUrl->str,
			       (xmlChar*)NULL);

    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Goto line and ident.
     */
    gdisp_xmlGotoLine(writer);
    gdisp_xmlIndent  (writer,
		      indentBuffer,
		      GD_INCREASE_INDENTATION);

    /*
     * Start an element whose name is 'SampledSymbols'.
     * This element is a child of provider XML node.
     */
#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterStartElement(writer,
					  (xmlChar*)"SampledSymbols");
#endif
    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Loop over all sampled symbol of the current provider.
     */
    pSampleList = &provider->pSampleList;
    pSampleMax  = pSampleList->TSP_sample_symbol_info_list_t_len;

    for (pSampleCpt=0; pSampleCpt<pSampleMax; pSampleCpt++) {

      /*
       * Get in touch with the symbol through the global index.
       */
      if (pSampleList->TSP_sample_symbol_info_list_t_val[pSampleCpt].provider_global_index >= 0) {

#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)

	symbol =
	  &provider->pSymbolList[pSampleList->TSP_sample_symbol_info_list_t_val[pSampleCpt].provider_global_index];

#else

	/*
	 * Convert PGI as an unsigned integer to a string.
	 */
	samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH-1] = '\0';
	samplePGIasString =
	  gdisp_uIntToStr(pSampleList->TSP_sample_symbol_info_list_t_val[pSampleCpt].provider_global_index,
		  &samplePGIasStringBuffer[GD_SAMPLE_PGI_AS_STRING_LENGTH-1]);

	/*
	 * Retreive target symbol.
	 */
	if (provider->pSymbolHashTablePGI == (hash_t*)NULL) {

	  symbol = (Symbol_T*)NULL;

	}
	else {

	  symbol = (Symbol_T*)
	    hash_get(provider->pSymbolHashTablePGI,samplePGIasString);

	}

#endif

	/*
	 * If referenced... ie, used by graphic plots...
	 */
	if (symbol != (Symbol_T*)NULL && symbol->sReference > 0) {

	  /*
	   * Sampled symbol index and name.
	   */
	  symbol->sPgi = sPgi++;
	  sprintf(indexBuffer,"%d",symbol->sPgi);

	  typeBuffer = gdisp_getTypeAsString(symbol);
	  sprintf(dimensionBuffer,"%d",symbol->sInfo.dimension);
	  sprintf(offsetBuffer,   "%d",symbol->sInfo.offset   );
	  sprintf(nElemBuffer,    "%d",symbol->sInfo.nelem    );
	  sprintf(periodBuffer,   "%d",symbol->sInfo.period   );
	  sprintf(phaseBuffer,    "%d",symbol->sInfo.phase    );

	  errorCode =
	    gdisp_xmlWriteAttributes(writer,
				     pSampleCpt == 0 ?
				     GD_INCREASE_INDENTATION :
				     GD_DO_NOT_CHANGE_INDENTATION,
				     indentBuffer,
				     (xmlChar*)"sampledSymbol",
				     TRUE, /* end up element */
				     (xmlChar*)"index",
				     (xmlChar*)indexBuffer,
				     (xmlChar*)"name",
				     (xmlChar*)symbol->sInfo.name,
				     (xmlChar*)"type",
				     (xmlChar*)typeBuffer,
				     (xmlChar*)"dim",
				     (xmlChar*)dimensionBuffer,
				     (xmlChar*)"offset",
				     (xmlChar*)offsetBuffer,
				     (xmlChar*)"nelem",
				     (xmlChar*)nElemBuffer,
				     (xmlChar*)"period",
				     (xmlChar*)periodBuffer,
				     (xmlChar*)"phase",
				     (xmlChar*)phaseBuffer,
				     (xmlChar*)NULL);

	  if (errorCode < 0) {
	    return errorCode;
	  }

	} /* sReference == 0 */

      } /* index >= 0 */

    } /* loop over sampled symbols */

    /*
     * Close 'SampledSymbols' XML element.
     */
    gdisp_xmlGotoLine(writer);
    gdisp_xmlIndent  (writer,
		      indentBuffer,
		      GD_DECREASE_INDENTATION);
#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterEndElement(writer);
#endif

    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Close provider XML element.
     */
    gdisp_xmlGotoLine(writer);
    gdisp_xmlIndent  (writer,
		      indentBuffer,
		      GD_DECREASE_INDENTATION);

#ifdef XMLWRITER_SUPPORTED
    errorCode = xmlTextWriterEndElement(writer);
#endif
    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Next provider.
     */
    providerItem = g_list_next(providerItem);

  }

  return 0; /* ok */

}


/*
 * Get back provider symbols for sampling.
 */
#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)

static void
gdisp_loadProviderSymbolsForSampling ( Kernel_T   *kernel,
				       xmlDoc     *document,
				       Provider_T *provider,
				       xmlNode    *providerNode )
{

  SymbolInConf_T *symbolInConf    = (SymbolInConf_T*)NULL;
  xmlNodeSet     *symbolTableNode = (xmlNodeSet*)NULL;
  xmlNode        *symbolNode      = (xmlNode*)NULL;
  xmlChar        *symbolIndex     = (xmlChar*)NULL;
  xmlChar        *symbolName      = (xmlChar*)NULL;
  unsigned int    cptSymbol       = 0;


  /*
   * Get back target symbols.
   */
  symbolTableNode = gdisp_xmlGetChildren(document,
					 providerNode,
					 "SampledSymbols/sampledSymbol");

  if (symbolTableNode != (xmlNodeSet*)NULL && symbolTableNode->nodeNr > 0) {

    /*
     * Allocate memory for storing those requested symbols at provider level.
     * Temporarily allocation.
     */
    symbolInConf = g_malloc0(symbolTableNode->nodeNr * sizeof(SymbolInConf_T));
    assert(symbolInConf);

    provider->pNbSymbolInConfiguration = 0;
    provider->pSymbolInConfiguration   = (void*)symbolInConf;

    for (cptSymbol=0;
	 cptSymbol<symbolTableNode->nodeNr;
	 cptSymbol++) {

      symbolNode  = symbolTableNode->nodeTab[cptSymbol];
      symbolIndex = xmlGetProp(symbolNode,"index");
      symbolName  = xmlGetProp(symbolNode,"name");

      if (symbolIndex != (xmlChar*)NULL && symbolName != (xmlChar*)NULL) {

	symbolInConf->sicIndex = gdisp_atoi(symbolIndex,0);
	symbolInConf->sicName  = gdisp_strDup(symbolName);

	symbolInConf++;
	provider->pNbSymbolInConfiguration++;

      }

      if (symbolIndex != (xmlChar*)NULL) {
	xmlFree(symbolIndex);
      }
      if (symbolName != (xmlChar*)NULL) {
	xmlFree(symbolName);
      }

    } /* for */

  } /* if */

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(symbolTableNode);

}


/*
 * Free memory allocation of provider "symbols in configuration".
 */
static void
gdisp_freeProviderSymbolInConfiguration ( Kernel_T *kernel )
{

  SymbolInConf_T *symbolInConf = (SymbolInConf_T*)NULL;
  GList          *providerItem     = (GList*)NULL;
  Provider_T     *provider         = (Provider_T*)NULL;
  guint           cptSic           = 0;

  /*
   * Loop upon all providers.
   */
  providerItem = g_list_first(kernel->providerList);
  while (providerItem != (GList*)NULL) {

    provider = (Provider_T*)providerItem->data;

    if (provider->pSymbolInConfiguration != (void*)NULL) {

      symbolInConf = (SymbolInConf_T*)provider->pSymbolInConfiguration;

      for (cptSic = 0;
	   cptSic < provider->pNbSymbolInConfiguration;
	   cptSic++) {

	g_free(symbolInConf->sicName);
	/* do not free 'sicReference' field */

	symbolInConf++;

      }

      g_free(provider->pSymbolInConfiguration);

    }

    provider->pSymbolInConfiguration   = (void*)NULL;
    provider->pNbSymbolInConfiguration = 0;

    providerItem = g_list_next(providerItem);

  }

}

#else

static void
gdisp_loadProviderSymbolsForSampling ( Kernel_T   *kernel,
				       xmlDoc     *document,
				       Provider_T *provider,
				       xmlNode    *providerNode )
{

  Symbol_T     *theSymbol       = (Symbol_T*)NULL;
  xmlNodeSet   *symbolTableNode = (xmlNodeSet*)NULL;
  xmlNode      *symbolNode      = (xmlNode*)NULL;
  xmlChar      *symbolIndex     = (xmlChar*)NULL;
  xmlChar      *symbolName      = (xmlChar*)NULL;
  xmlChar      *symbolType      = (xmlChar*)NULL;
  xmlChar      *symbolDimension = (xmlChar*)NULL;
  xmlChar      *symbolOffset    = (xmlChar*)NULL;
  xmlChar      *symbolNelem     = (xmlChar*)NULL;
  xmlChar      *symbolPeriod    = (xmlChar*)NULL;
  xmlChar      *symbolPhase     = (xmlChar*)NULL;
  unsigned int  cptSymbol       = 0;
  unsigned int  newSymbol       = 0;


  /*
   * Get back target symbols.
   */
  symbolTableNode = gdisp_xmlGetChildren(document,
					 providerNode,
					 BAD_CAST "SampledSymbols/sampledSymbol");

  if (symbolTableNode != (xmlNodeSet*)NULL && symbolTableNode->nodeNr > 0) {

    /*
     * Allocate memory for storing those requested symbols at provider level.
     * Temporarily allocation.
     */
    provider->pSymbolNumber = 0;
    provider->pSymbolList   =
      (Symbol_T*)g_malloc0(symbolTableNode->nodeNr * sizeof(Symbol_T));
    assert(provider->pSymbolList);

    /*
     * Allocate symbol hash table for extra-boosted search.
     */
    provider->pSymbolHashTable = hash_open('.','z');
    assert(provider->pSymbolHashTable);

    /*
     * Loop over all symbols.
     */
    theSymbol = provider->pSymbolList;
    for (cptSymbol=0;
	 cptSymbol<symbolTableNode->nodeNr;
	 cptSymbol++) {

      symbolNode      = symbolTableNode->nodeTab[cptSymbol];
      symbolIndex     = xmlGetProp(symbolNode, BAD_CAST "index" );
      symbolName      = xmlGetProp(symbolNode, BAD_CAST "name"  );
      symbolType      = xmlGetProp(symbolNode, BAD_CAST "type"  );
      if (symbolType == (xmlChar*)NULL) {
	symbolType = xmlCharStrdup("F64");
      }
      symbolDimension = xmlGetProp(symbolNode, BAD_CAST "dim"   );
      if (symbolDimension == (xmlChar*)NULL) {
	symbolDimension = xmlCharStrdup("1");
      }
      symbolOffset    = xmlGetProp(symbolNode, BAD_CAST "offset");
      if (symbolOffset == (xmlChar*)NULL) {
	symbolOffset = xmlCharStrdup("0");
      }
      symbolNelem     = xmlGetProp(symbolNode, BAD_CAST "nelem" );
      if (symbolNelem == (xmlChar*)NULL) {
	symbolNelem = xmlCharStrdup("0");
      }
      symbolPeriod    = xmlGetProp(symbolNode, BAD_CAST "period");
      if (symbolPeriod == (xmlChar*)NULL) {
	symbolPeriod = xmlCharStrdup("1");
      }
      symbolPhase     = xmlGetProp(symbolNode, BAD_CAST "phase" );
      if (symbolPhase == (xmlChar*)NULL) {
	symbolPhase = xmlCharStrdup("0");
      }

      if (symbolIndex != (xmlChar*)NULL && symbolName != (xmlChar*)NULL) {

	theSymbol->sPgi            = gdisp_atoi(UTF8_TO_CHAR symbolIndex,0);
	theSymbol->sInfo.name      = gdisp_strDup(UTF8_TO_CHAR symbolName);

	theSymbol->sInfo.type      = gdisp_getTypeFromString(UTF8_TO_CHAR symbolType);
	theSymbol->sInfo.dimension = gdisp_atoi(UTF8_TO_CHAR symbolDimension,1);
	theSymbol->sInfo.offset    = gdisp_atoi(UTF8_TO_CHAR symbolOffset,   0);
	theSymbol->sInfo.nelem     =
	              gdisp_atoi(UTF8_TO_CHAR symbolNelem,theSymbol->sInfo.dimension);
	theSymbol->sInfo.period    = gdisp_atoi(UTF8_TO_CHAR symbolPeriod,   1);
	theSymbol->sInfo.phase     = gdisp_atoi(UTF8_TO_CHAR symbolPhase,    0);

	theSymbol->sMinimum        = - G_MAXDOUBLE;
	theSymbol->sMaximum        = + G_MAXDOUBLE;

	hash_append(provider->pSymbolHashTable,
		    theSymbol->sInfo.name,
		    (void*)&provider->pSymbolList[newSymbol]);

	theSymbol++;
	newSymbol++;

      }

      if (symbolIndex != (xmlChar*)NULL) {
	xmlFree(symbolIndex);
      }
      if (symbolName != (xmlChar*)NULL) {
	xmlFree(symbolName);
      }
      if (symbolType != (xmlChar*)NULL) {
	xmlFree(symbolType);
      }
      if (symbolDimension != (xmlChar*)NULL) {
	xmlFree(symbolDimension);
      }
      if (symbolOffset != (xmlChar*)NULL) {
	xmlFree(symbolOffset);
      }
      if (symbolNelem != (xmlChar*)NULL) {
	xmlFree(symbolNelem);
      }
      if (symbolPeriod != (xmlChar*)NULL) {
	xmlFree(symbolPeriod);
      }
      if (symbolPhase != (xmlChar*)NULL) {
	xmlFree(symbolPhase);
      }

    } /* for */

    provider->pSymbolNumber = newSymbol;

  } /* if */

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(symbolTableNode);

}

#endif

/*
 * Get back all target providers.
 * Returns FALSE in case of errors. TRUE otherwise.
 */
static gboolean
gdisp_loadTargetProviders ( Kernel_T *kernel,
			    xmlDoc   *document,
			    guchar    infoType )
{

  Provider_T   *provider          = (Provider_T*)NULL;
  xmlNodeSet   *providerTableNode = (xmlNodeSet*)NULL;
  xmlNode      *providerNode      = (xmlNode*)NULL;
  xmlChar      *propertyValue     = (xmlChar*)NULL;
  unsigned int  cptProvider       = 0;
  gboolean      loadIsOk          = TRUE;


  /*
   * Get back target providers.
   */
  providerTableNode = gdisp_xmlGetChildren(document,
					   (xmlNode*)NULL,
					   BAD_CAST "//Kernel/Provider");

  if (providerTableNode != (xmlNodeSet*)NULL &&
                                    providerTableNode->nodeNr > 0) {

    for (cptProvider=0;
	 cptProvider<providerTableNode->nodeNr;
	 cptProvider++) {

      providerNode  = providerTableNode->nodeTab[cptProvider];
      propertyValue = xmlGetProp(providerNode, BAD_CAST "url");

      switch (infoType) {

      case GD_PROVIDER_URL :

	/*
	 * Store the provider url in the kernel.
	 */
	gdisp_addUrl(kernel, UTF8_TO_CHAR propertyValue);

	break;

      case GD_PROVIDER_SYMBOLS :

	provider = gdisp_getProviderByOriginalUrl(kernel,
						  UTF8_TO_CHAR propertyValue /* Url */ );

	if (provider != (Provider_T*)NULL) {

	  gdisp_loadProviderSymbolsForSampling(kernel,
					       document,
					       provider,
					       providerNode);

	}

	break;

      } /* switch */

      if (propertyValue != (xmlChar*)NULL) {
	xmlFree(propertyValue);
      }

    } /* for */

  } /* if */

  else {

    /*
     * No provider.
     */
    loadIsOk = FALSE;

  }

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(providerTableNode);

  return loadIsOk;

}


/*
 * Add sampled configuration symbols to plot.
 */
static void
gdisp_addSampledSymbolToPlot ( Kernel_T         *kernel,
			       xmlDoc           *document,
			       xmlNode          *plotNode,
			       PlotSystemData_T *plotSystemData )
{

  xmlNodeSet   *symbolTableNode  = (xmlNodeSet*)NULL;
  xmlNode      *symbolNode       = (xmlNode*)NULL;
  xmlChar      *symbolIndex      = (xmlChar*)NULL;
  xmlChar      *symbolZone       = (xmlChar*)NULL;
  unsigned int  cptSymbol        = 0;
  Symbol_T     *symbol           = (Symbol_T*)NULL;
  GList        *symbolList       = (GList*)NULL;
  GList        *attributeList    = (GList*)NULL;

  /*
   * Get back target symbols.
   */
  symbolTableNode = gdisp_xmlGetChildren(document,
					 plotNode,
					 BAD_CAST "sampledSymbol");

  if (symbolTableNode != (xmlNodeSet*)NULL && symbolTableNode->nodeNr > 0) {

    for (cptSymbol=0;
	 cptSymbol<symbolTableNode->nodeNr;
	 cptSymbol++) {

      symbolNode = symbolTableNode->nodeTab[cptSymbol];

      /*
       * Get back all symbol information : index, zone.
       */
      symbolIndex = xmlGetProp(symbolNode, BAD_CAST "index");
      symbolZone  = xmlGetProp(symbolNode, BAD_CAST "zone");

      if (symbolIndex != (xmlChar*)NULL) {

	symbol = gdisp_getSymbolInConfByIndex(kernel,
					      gdisp_atoi(UTF8_TO_CHAR symbolIndex,0));

	if (symbol != (Symbol_T*)NULL) {

	  symbolList = g_list_append(symbolList,
				     (gpointer)symbol);

	  (*plotSystemData->plotSystem->psAddSymbols)(kernel,
						      plotSystemData->plotData,
						      symbolList,
						      symbolZone ?
						      symbolZone[0] : (gchar)0);
	  /* zone identity is a 'guchar' --------------------^-----------^ */

	  /*
	   * Get back the list of plot specific attributes.
	   */
	  gdisp_xmlGetAttributeList(symbolNode,
				    &attributeList);

	  (*plotSystemData->plotSystem->psSetSymbolAttributes)
                                                  (kernel,
						   plotSystemData->plotData,
						   symbol,
						   attributeList);

	  /*
	   * Free memory allocated to the lists.
	   */
	  g_list_free(attributeList);
	  attributeList = (GList*)NULL;

	  g_list_free(symbolList);
	  symbolList = (GList*)NULL;

	}

	/* FIXME : count the number of unknown symbols */

	xmlFree(symbolIndex);

      } /* if */

      if (symbolZone != (xmlChar*)NULL) {
	xmlFree(symbolZone);
      }

    } /* for */

  } /* if */

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(symbolTableNode);

}


/*
 * Get back all plots of the page.
 */
static void
gdisp_loadTargetPlots ( Kernel_T *kernel,
			xmlDoc   *document,
			Page_T   *page,
			xmlNode  *pageNode )
{

  xmlNodeSet       *plotTableNode  = (xmlNodeSet*)NULL;
  xmlNode          *plotNode       = (xmlNode*)NULL;
  xmlChar          *property       = (xmlChar*)NULL;
  unsigned int      cptPlot        = 0;
  PlotSystemData_T *plotSystemData = (PlotSystemData_T*)NULL;
  guint             plotRow        = 0;
  guint             plotColumn     = 0;
  guint             plotNbRows     = 1;
  guint             plotNbColumns  = 1;
  PlotType_T        plotType       = GD_PLOT_DEFAULT;
  GList            *attributeList  = (GList*)NULL;


  /*
   * Get back target plots.
   */
  plotTableNode = gdisp_xmlGetChildren(document,
				       pageNode,
				       BAD_CAST "Plot");

  if (plotTableNode != (xmlNodeSet*)NULL && plotTableNode->nodeNr > 0) {

    for (cptPlot=0;
	 cptPlot<plotTableNode->nodeNr;
	 cptPlot++) {

      plotNode = plotTableNode->nodeTab[cptPlot];

      /*
       * Get back all plot information : type, row, column.
       * and create the plot.
       */
      property = xmlGetProp(plotNode, BAD_CAST "type");
      if (property != (xmlChar*)NULL) {
	plotType = (*kernel->getPlotTypeFromPlotName)(kernel,
						      (gchar*)property);
	xmlFree(property);
      }
      
      property = xmlGetProp(plotNode, BAD_CAST "row");
      if (property != (xmlChar*)NULL) {
	plotRow = gdisp_atoi(UTF8_TO_CHAR property,0);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode, BAD_CAST "column");
      if (property != (xmlChar*)NULL) {
	plotColumn = gdisp_atoi(UTF8_TO_CHAR property,0);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode, BAD_CAST "nbRows");
      if (property != (xmlChar*)NULL) {
	plotNbRows = gdisp_atoi(UTF8_TO_CHAR property,0);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode, BAD_CAST "nbColumns");
      if (property != (xmlChar*)NULL) {
	plotNbColumns = gdisp_atoi(UTF8_TO_CHAR property,0);
	xmlFree(property);
      }

      /*
       * Create the plot.
       */
      plotSystemData = gdisp_addPlotToGraphicPage(kernel,
						  page,
						  plotType,
						  plotRow,
						  plotNbRows,
						  plotColumn,
						  plotNbColumns);

      /*
       * Get back the list of plot specific attributes.
       */
      gdisp_xmlGetAttributeList(plotNode,
				&attributeList);
      attributeList = g_list_first(attributeList);

      /*
       * Set up specific propoerties.
       */
      (*plotSystemData->plotSystem->psSetPlotAttributes)
                                                  (kernel,
						   plotSystemData->plotData,
						   attributeList);

      /*
       * Free memory allocated to the lists.
       */
      g_list_free(attributeList);
      attributeList = (GList*)NULL;

      /*
       * Add symbols to be sampled to plot.
       */
      gdisp_addSampledSymbolToPlot(kernel,
				   document,
				   plotNode,
				   plotSystemData);

    }

  }

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(plotTableNode);

}


/*
 * Get back all target providers.
 */
static void
gdisp_loadTargetPages ( Kernel_T *kernel,
			xmlDoc   *document )
{

  xmlNodeSet   *pageTableNode = (xmlNodeSet*)NULL;
  xmlNode      *pageNode      = (xmlNode*)NULL;
  xmlChar      *pageRows      = (xmlChar*)NULL;
  xmlChar      *pageColumns   = (xmlChar*)NULL;
  xmlChar      *pageTitle     = (xmlChar*)NULL;
  unsigned int  cptPage       = 0;
  Page_T       *newPage       = (Page_T*)NULL;


  /*
   * Get back target pages.
   */
  pageTableNode = gdisp_xmlGetChildren(document,
				       (xmlNode*)NULL,
				       BAD_CAST "//Graphics/Page");

  if (pageTableNode != (xmlNodeSet*)NULL && pageTableNode->nodeNr > 0) {

    for (cptPage=0;
	 cptPage<pageTableNode->nodeNr;
	 cptPage++) {

      pageNode = pageTableNode->nodeTab[cptPage];

      /*
       * Get back all page information : title, rows, columns.
       * and create the page.
       */
      pageTitle   = xmlGetProp(pageNode, BAD_CAST "title");
      pageRows    = xmlGetProp(pageNode, BAD_CAST "rows");
      pageColumns = xmlGetProp(pageNode, BAD_CAST "columns");

      if (pageRows != (xmlChar*)NULL && pageColumns != (xmlChar*)NULL) {

	newPage = gdisp_allocateGraphicPage(kernel,
					    (gchar*)pageTitle,
					    gdisp_atoi(UTF8_TO_CHAR pageRows,0),
					    gdisp_atoi(UTF8_TO_CHAR pageColumns,0));

	/*
	 * Create all plots of the page.
	 */
	gdisp_loadTargetPlots(kernel,
			      document,
			      newPage,
			      pageNode);

	/*
	 * End up page creation.
	 */
	gdisp_finalizeGraphicPageCreation(kernel,
					  newPage);

      }

      if (pageTitle != (xmlChar*)NULL) {
	xmlFree(pageTitle);
      }
      if (pageRows != (xmlChar*)NULL) {
	xmlFree(pageRows);
      }
      if (pageColumns != (xmlChar*)NULL) {
	xmlFree(pageColumns);
      }

    }

  }

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(pageTableNode);

}


/*
 * Load preferences.
 * Returns FALSE in case of errors. TRUE otherwise.
 */
static gboolean
gdisp_loadConfiguration ( Kernel_T *kernel,
			  xmlDoc   *document )
{

  gboolean loadIsOk = TRUE;

  /*
   * Get back target providers : URL only.
   */
  loadIsOk = gdisp_loadTargetProviders(kernel,
				       document,
				       GD_PROVIDER_URL);

  if (loadIsOk == FALSE) {
    return loadIsOk;
  }

  /*
   * Now that all providers have been stored, start a new consuming process.
   */
  kernel->retreiveAllSymbols = FALSE;
  gdisp_consumingInit(kernel);

  /*
   * Get back target providers : SYMBOLS only.
   */
  loadIsOk = gdisp_loadTargetProviders(kernel,
				       document,
				       GD_PROVIDER_SYMBOLS);

  if (loadIsOk == FALSE) {
    return loadIsOk;
  }

  /*
   * Check sampling symbol references.
   */
#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)
  gdisp_checkSamplingSymbolReferences(kernel);
#endif

  /*
   * Create all graphic pages and internal graphic plots.
   */
  gdisp_loadTargetPages(kernel,document);

  /*
   * Free temporarily memory allocation for configuration purpose.
   */
#if defined(GD_LOAD_CONFIGURATION_WITH_ALL_SYMBOLS)
  gdisp_freeProviderSymbolInConfiguration(kernel);
#endif

  /*
   * Do not forget to update symbols <-> providers assignments.
   */
  (*kernel->assignSymbolsToProviders)(kernel);

  return loadIsOk;

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * New configuration.
 */
gboolean
gdisp_newConfiguration ( Kernel_T *kernel )
{

  /*
   * Manually stop sampling process.
   */
  (*kernel->stopSamplingProcess)(kernel);

  /*
   * Destroy all previous graphic pages.
   */
  gdisp_destroyAllGraphicPages(kernel);

  /*
   * End up any previous consuming process.
   */
  gdisp_consumingEnd(kernel);

  /*
   * Start a new consuming process.
   */
  kernel->retreiveAllSymbols = TRUE;
  gdisp_consumingInit(kernel);

  /*
   * Refresh data book content.
   */
  gdisp_refreshDataBookWindow(kernel);

  /*
   * Everything went ok.
   */
  return TRUE;

}


/*
 * Close configuration.
 */
gboolean
gdisp_closeConfiguration ( Kernel_T *kernel )
{

  /*
   * Manually stop sampling process.
   */
  (*kernel->stopSamplingProcess)(kernel);

  /*
   * Destroy all previous graphic pages.
   */
  gdisp_destroyAllGraphicPages(kernel);

  /*
   * End up any previous consuming process.
   */
  gdisp_consumingEnd(kernel);

  /*
   * Refresh data book content.
   */
  gdisp_refreshDataBookWindow(kernel);

  /*
   * Everything went ok.
   */
  return TRUE;

}


/*
 * Configuration Management : load configuration from an XML file.
 * FALSE is returned in case of errors. TRUE otherwise.
 */
gboolean
gdisp_openConfigurationFile ( Kernel_T *kernel )
{

  xmlDoc *document = (xmlDoc*)NULL;

  /*
   * Check IO Filename.
   * We really need a filename and NOT a path.
   */
  if (kernel->ioFilename == (gchar*)NULL ||
      kernel->ioFilename[strlen(kernel->ioFilename)-1] == G_DIR_SEPARATOR) {

    return FALSE;

  }

  /*
   * This initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*
   * Parse and validate document.
   */
  document = gdisp_xmlParseAndValidate(kernel->ioFilename);
  if (document == (xmlDoc*)NULL) {
    return FALSE;
  }

  /*
   * Manually stop sampling process.
   */
  (*kernel->stopSamplingProcess)(kernel);

  /*
   * Destroy all previous graphic pages.
   */
  gdisp_destroyAllGraphicPages(kernel);

  /*
   * End up any previous consuming process.
   */
  gdisp_consumingEnd(kernel);

  /*
   * Load configuration.
   */
  gdisp_loadConfiguration(kernel,
			  document);

  /*
   * Refresh data book content.
   */
  gdisp_refreshDataBookWindow(kernel);

  /*
   * Free document.
   */
  xmlFreeDoc(document);

  /*
   * No error.
   */
  return TRUE;

}


/*
 * Configuration Management : save configuration into an XML file.
 * FALSE is returned in case of errors. TRUE otherwise.
 */
gboolean
gdisp_saveConfigurationFile ( Kernel_T *kernel )
{

#if defined(XMLWRITER_SUPPORTED)

  gint              errorCode = 0;
  xmlTextWriterPtr  writer    = (xmlTextWriterPtr)NULL;
  xmlChar          *tmp       = (xmlChar*)NULL;
  xmlChar           indentBuffer[256];

  /*
   * Check IO Filename.
   * We really need a filename and NOT a path.
   */
  if (kernel->ioFilename == (gchar*)NULL ||
      kernel->ioFilename[strlen(kernel->ioFilename)-1] == G_DIR_SEPARATOR) {

    return FALSE;

  }

  /*
   * Caution.
   */
  indentBuffer[0] = '\0';

  /*
   * This initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*
   * Create a new XmlWriter for '.gdisp+', with no compression.
   */
  writer = xmlNewTextWriterFilename(kernel->ioFilename,
				    GD_NO_COMPRESSION);

  if (writer == (xmlTextWriterPtr)NULL) {
    return FALSE;
  }

  /*
   * Start the document with the xml default for the version,
   * encoding ISO 8859-1 and the default for the standalone
   * declaration.
   */

  errorCode = xmlTextWriterStartDocument(writer,
					 (const char*)NULL, /* version */
					 GD_PREFERENCE_ENCODING,
					 (const char*)NULL); /* standalone */
  if (errorCode < 0) {
    return FALSE;
  }

  /*
   * Start an element named "GDispConfiguration".
   * Since thist is the first element, this will be the root element
   * of the document.
   */
  errorCode = gdisp_xmlWriteAttributes(writer,
				       GD_DO_NOT_CHANGE_INDENTATION,
				       indentBuffer,
				       (xmlChar*)"TargaConfiguration",
				       FALSE, /* do not end up element */
				       (xmlChar*)"version",
				       (xmlChar*)"1.0",
				       (xmlChar*)NULL);

  if (errorCode < 0) {
    return FALSE;
  }

  /*
   * Goto line and ident.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_INCREASE_INDENTATION);

  /*
   * Write a comment as child of the root element.
   * Please observe, that the input to the xmlTextWriter functions
   * HAS to be in UTF-8, even if the output XML is encoded
   * in iso-8859-1.
   */
  tmp = gdisp_xmlConvertInput("A Targa Configuration",
			      GD_PREFERENCE_ENCODING);

  errorCode = xmlTextWriterWriteComment(writer, tmp);
  if (errorCode < 0) {
    return FALSE;
  }
  if (tmp != (xmlChar*)NULL) {
    xmlFree(tmp);
  }

  /*
   * Goto line.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DO_NOT_CHANGE_INDENTATION);

  /*
   * Start an element named 'Kernel' as child of root.
   */
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"Kernel");
  if (errorCode < 0) {
    return FALSE;
  }


  /*
   * Loop over all providers and save all sampled symbols.
   */
  errorCode = gdisp_saveProviderSampledSymbols(kernel,
					       writer,
					       indentBuffer);
  if (errorCode < 0) {
    return FALSE;
  }

  /*
   * Close 'Kernel' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return FALSE;
  }


  /*
   * Goto Line.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DO_NOT_CHANGE_INDENTATION);


  /*
   * Start an element named 'Graphics' as child of root.
   */
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"Graphics");
  if (errorCode < 0) {
    return FALSE;
  }


  /*
   * Loop over all providers and save all sampled symbols.
   */
  errorCode = gdisp_saveGraphicPages(kernel,
				     writer,
				     indentBuffer);
  if (errorCode < 0) {
    return FALSE;
  }

  /*
   * Close 'Graphics' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return FALSE;
  }


  /*
   * Close 'GDispConfiguration' element.
   */
  gdisp_xmlGotoLine(writer);
  gdisp_xmlIndent  (writer,
		    indentBuffer,
		    GD_DECREASE_INDENTATION);

  errorCode = xmlTextWriterEndElement(writer);

  if (errorCode < 0) {
    /* No need to print any error message, because gdisp+ is exiting */
    return FALSE;
  }

  /*
   * Goto line.
   */
  gdisp_xmlGotoLine(writer);

  /*
   * Free text writer.
   */
  xmlFreeTextWriter(writer);

  /*
   * Cleanup function for the XML library.
   */
  xmlCleanupParser();


  /*
   * No error.
   */
  return TRUE;

#else

  /*
   * Error.
   */
  return FALSE;

#endif

}

