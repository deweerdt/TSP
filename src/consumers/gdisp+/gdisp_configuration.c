/*!  \file 

$Id: gdisp_configuration.c,v 1.1 2005-10-05 19:21:00 esteban Exp $

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
 * XML includes.
 */
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>


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


/*
 * Check sampling symbol references.
 */
static void
gdisp_checkSamplingSymbolReferences ( Kernel_T *kernel )
{

  Symbol_T        targetSymbol;
  SymbolInConf_T *symbolInConf     = (SymbolInConf_T*)NULL;
  GList          *providerItem     = (GList*)NULL;
  Provider_T     *provider         = (Provider_T*)NULL;
  guint           cptSic           = 0;

  /*
   * Sort provider symbols by name.
   */
  gdisp_sortProviderSymbols(kernel,
			    GD_SORT_BY_NAME);

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

	targetSymbol.sInfo.name = symbolInConf->sicName;

	symbolInConf->sicReference =
	  (Symbol_T*)bsearch(&targetSymbol,
			     (void*)provider->pSymbolList,
			     provider->pSymbolNumber,
			     (size_t)sizeof(Symbol_T),
			     gdisp_sortProviderSymbolByName);

	printf("checkSampling : %s is %s/%d\n",
	       symbolInConf->sicName,
	       symbolInConf->sicReference->sInfo.name,
	       symbolInConf->sicReference->sInfo.index);

	symbolInConf++;

      } /* for */

    } /* if */

    providerItem = g_list_next(providerItem);

  } /* while */

  /*
   * Sort provider symbols by index (default sorting method).
   */
  gdisp_sortProviderSymbols(kernel,
			    GD_SORT_BY_INDEX);

}


/*
 * Get back configuration symbol by index.
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
	    printf("symbolByIndex : %s is %s/%d\n",
		   symbolInConf->sicName,
		   symbolInConf->sicReference->sInfo.name,
		   symbolInConf->sicReference->sInfo.index);
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


/*
 * Save all symbols of a graphic plot.
 */
static gint
gdisp_saveGraphicPlotSymbolList ( Kernel_T         *kernel,
				  xmlTextWriterPtr  writer,
				  xmlChar          *indentBuffer,
				  gboolean         *isFirstSymbol,
				  GList            *symbolList,
				  guchar            zoneId )
{

  gint      errorCode = 0;
  Symbol_T *symbol    = (Symbol_T*)NULL;
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
    sprintf(indexBuffer,"%d",symbol->sConfIndex);
    sprintf(zoneBuffer ,"%c",zoneId);

    errorCode =
      gdisp_xmlWriteAttributes(writer,
			       *isFirstSymbol == TRUE ?
			       GD_INCREASE_INDENTATION :
			       GD_DO_NOT_CHANGE_INDENTATION,
			       indentBuffer,
			       (xmlChar*)"sampledSymbol",
			       TRUE, /* end up element */
			       (xmlChar*)"index",
			       (xmlChar*)indexBuffer,
			       (xmlChar*)(zoneId != 0 ? "zone"     : NULL),
			       (xmlChar*)(zoneId != 0 ? zoneBuffer : NULL),
			       (xmlChar*)NULL);

    if (errorCode < 0) {
      return errorCode;
    }

    *isFirstSymbol = FALSE;
    symbolList     = g_list_next(symbolList);

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

    errorCode =
      gdisp_xmlWriteAttributes(writer,
			       plotIdentity == 0 ?
			       GD_INCREASE_INDENTATION :
			       GD_DO_NOT_CHANGE_INDENTATION,
			       indentBuffer,
			       (xmlChar*)"Plot",
			       FALSE, /* do not end up element */
			       (xmlChar*)"type",
			       (xmlChar*)plotInformation.psName,
			       (xmlChar*)"row",
			       (xmlChar*)sPlotRow,
			       (xmlChar*)"column",
			       (xmlChar*)sPlotColumn,
			       (xmlChar*)"nbRows",
			       (xmlChar*)sPlotNbRows,
			       (xmlChar*)"nbColumns",
			       (xmlChar*)sPlotNbColumns,
			       (xmlChar*)NULL);

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

    errorCode = xmlTextWriterEndElement(writer);
	  
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

  errorCode = xmlTextWriterEndElement(writer);

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
  guint         sConfIndex     = 1;
  xmlChar       indexBuffer     [256];


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
    errorCode = xmlTextWriterStartElement(writer,
					  (xmlChar*)"SampledSymbols");
    if (errorCode < 0) {
      return errorCode;
    }

    /*
     * Loop over all sampled symbol of the current provider.
     */
    pSampleList = &provider->pSampleList;
    pSampleMax  = pSampleList->len;

    for (pSampleCpt=0; pSampleCpt<pSampleMax; pSampleCpt++) {

      /*
       * Get in touch with the symbol through the global index.
       */
      symbol = &provider->pSymbolList[pSampleList->val[pSampleCpt].index];

      /*
       * If referenced... ie, used by graphic plots...
       */
      if (symbol->sReference > 0) {

	/*
	 * Sampled symbol index and name.
	 */
	symbol->sConfIndex = sConfIndex++;
	sprintf(indexBuffer,"%d",symbol->sConfIndex);

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
				   (xmlChar*)NULL);

	if (errorCode < 0) {
	  return errorCode;
	}

      } /* sReference == 0 */

    } /* loop over sampled symbols */

    /*
     * Close 'SampledSymbols' XML element.
     */
    gdisp_xmlGotoLine(writer);
    gdisp_xmlIndent  (writer,
		      indentBuffer,
		      GD_DECREASE_INDENTATION);

    errorCode = xmlTextWriterEndElement(writer);

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

    errorCode = xmlTextWriterEndElement(writer);

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

  if (symbolTableNode->nodeNr > 0) {

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

	symbolInConf->sicIndex = atoi(symbolIndex);
	symbolInConf->sicName  = strdup(symbolName);

	printf("loading name=%s index=%d\n",
	       symbolInConf->sicName,
	       symbolInConf->sicIndex);

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

	free(symbolInConf->sicName);
	/* do not free 'sicReference' field */

	symbolInConf++;

      }

      free(provider->pSymbolInConfiguration);

    }

    provider->pSymbolInConfiguration   = (void*)NULL;
    provider->pNbSymbolInConfiguration = 0;

    providerItem = g_list_next(providerItem);

  }

}


/*
 * Get back all target providers.
 */
static void
gdisp_loadTargetProviders ( Kernel_T *kernel,
			    xmlDoc   *document,
			    guchar    infoType )
{

  Provider_T   *provider          = (Provider_T*)NULL;
  xmlNodeSet   *providerTableNode = (xmlNodeSet*)NULL;
  xmlNode      *providerNode      = (xmlNode*)NULL;
  xmlChar      *propertyValue     = (xmlChar*)NULL;
  unsigned int  cptProvider       = 0;


  /*
   * Get back target providers.
   */
  providerTableNode = gdisp_xmlGetChildren(document,
					   (xmlNode*)NULL,
					   "//Kernel/Provider");

  if (providerTableNode->nodeNr > 0) {

    for (cptProvider=0;
	 cptProvider<providerTableNode->nodeNr;
	 cptProvider++) {

      providerNode  = providerTableNode->nodeTab[cptProvider];
      propertyValue = xmlGetProp(providerNode,"url");

      switch (infoType) {

      case GD_PROVIDER_URL :

	/*
	 * Store the provider url in the kernel.
	 */
	gdisp_addUrl(kernel,propertyValue);

	break;

      case GD_PROVIDER_SYMBOLS :

	provider = gdisp_getProviderByOriginalUrl(kernel,
						  propertyValue /* Url */ );

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

  /*
   * Free node set.
   */
  xmlXPathFreeNodeSet(providerTableNode);

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

  /*
   * Get back target symbols.
   */
  symbolTableNode = gdisp_xmlGetChildren(document,
					 plotNode,
					 "sampledSymbol");

  if (symbolTableNode->nodeNr > 0) {

    for (cptSymbol=0;
	 cptSymbol<symbolTableNode->nodeNr;
	 cptSymbol++) {

      symbolNode = symbolTableNode->nodeTab[cptSymbol];

      /*
       * Get back all symbol information : index, zone.
       */
      symbolIndex = xmlGetProp(symbolNode,"index");
      symbolZone  = xmlGetProp(symbolNode,"zone");

      if (symbolIndex != (xmlChar*)NULL) {

	symbol = gdisp_getSymbolInConfByIndex(kernel,
					      atoi(symbolIndex));

	if (symbol != (Symbol_T*)NULL) {

	  printf("addSymbol : name=%s index=%d\n",
		 symbol->sInfo.name,atoi(symbolIndex));

	  symbolList = g_list_append(symbolList,
				     (gpointer)symbol);

	  (*plotSystemData->plotSystem->psAddSymbols)(kernel,
						      plotSystemData->plotData,
						      symbolList,
						      symbolZone ?
						      symbolZone[0] : (gchar)0);
	  /* zone identity is a 'guchar' --------------------^-----------^ */

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


  /*
   * Get back target plots.
   */
  plotTableNode = gdisp_xmlGetChildren(document,
				       pageNode,
				       "Plot");

  if (plotTableNode->nodeNr > 0) {

    for (cptPlot=0;
	 cptPlot<plotTableNode->nodeNr;
	 cptPlot++) {

      plotNode = plotTableNode->nodeTab[cptPlot];

      /*
       * Get back all plot information : type, row, column.
       * and create the plot.
       */
      property = xmlGetProp(plotNode,"type");
      if (property != (xmlChar*)NULL) {
	plotType = (*kernel->getPlotTypeFromPlotName)(kernel,
						      (gchar*)property);
	xmlFree(property);
      }
      
      property = xmlGetProp(plotNode,"row");
      if (property != (xmlChar*)NULL) {
	plotRow = atoi(property);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode,"column");
      if (property != (xmlChar*)NULL) {
	plotColumn = atoi(property);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode,"nbRows");
      if (property != (xmlChar*)NULL) {
	plotNbRows = atoi(property);
	xmlFree(property);
      }

      property = xmlGetProp(plotNode,"nbColumns");
      if (property != (xmlChar*)NULL) {
	plotNbColumns = atoi(property);
	xmlFree(property);
      }

      plotSystemData = gdisp_addPlotToGraphicPage(kernel,
						  page,
						  plotType,
						  plotRow,
						  plotNbRows,
						  plotColumn,
						  plotNbColumns);

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
				       "//Graphics/Page");

  if (pageTableNode->nodeNr > 0) {

    for (cptPage=0;
	 cptPage<pageTableNode->nodeNr;
	 cptPage++) {

      pageNode = pageTableNode->nodeTab[cptPage];

      /*
       * Get back all page information : title, rows, columns.
       * and create the page.
       */
      pageTitle   = xmlGetProp(pageNode,"title");
      pageRows    = xmlGetProp(pageNode,"rows");
      pageColumns = xmlGetProp(pageNode,"columns");

      if (pageRows != (xmlChar*)NULL && pageColumns != (xmlChar*)NULL) {

	newPage = gdisp_allocateGraphicPage(kernel,
					    (gchar*)pageTitle,
					    atoi(pageRows),
					    atoi(pageColumns));

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
 */
static void
gdisp_loadConfiguration ( Kernel_T *kernel,
			  xmlDoc   *document )
{

  /*
   * End up any previous consuming process.
   */
  gdisp_consumingEnd(kernel);

  /*
   * Get back target providers : URL only.
   */
  gdisp_loadTargetProviders(kernel,
			    document,
			    GD_PROVIDER_URL);

  /*
   * Now that all providers have been stored, start a new consuming process.
   */
  gdisp_consumingInit(kernel);

  /*
   * Get back target providers : SYMBOLS only.
   */
  gdisp_loadTargetProviders(kernel,
			    document,
			    GD_PROVIDER_SYMBOLS);

  /*
   * Check sampling symbol references.
   */
  gdisp_checkSamplingSymbolReferences(kernel);

  /*
   * Destroy all previous graphic pages.
   */
  gdisp_destroyAllGraphicPages(kernel);

  /*
   * Create all graphic pages and internal graphic plots.
   */
  gdisp_loadTargetPages(kernel,document);

  /*
   * Free temporarily memory allocation for configuration purpose.
   */
  gdisp_freeProviderSymbolInConfiguration(kernel);

  /*
   * Do not forget to update symbols <-> providers assignments.
   */
  (*kernel->assignSymbolsToProviders)(kernel);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Configuration Management : load configuration from an XML file.
 * FALSE is returned in case of errors. TRUE otherwise.
 */
gboolean
gdisp_loadConfigurationFile ( Kernel_T *kernel,
			      gchar    *absoluteConfigurationFilename)
{

  xmlDoc *document = (xmlDoc*)NULL;

  /*
   * This initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION


  /*
   * Parse and validate document.
   */
  document = gdisp_xmlParseAndValidate(absoluteConfigurationFilename);
  if (document == (xmlDoc*)NULL) {
    return FALSE;
  }


  /*
   * Load configuration.
   */
  gdisp_loadConfiguration(kernel,
			  document);


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
gdisp_saveConfigurationFile ( Kernel_T *kernel,
			      gchar    *absoluteConfigurationFilename)
{

  gint              errorCode = 0;
  xmlTextWriterPtr  writer    = (xmlTextWriterPtr)NULL;
  xmlChar          *tmp       = (xmlChar*)NULL;
  xmlChar           indentBuffer[256];

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
  writer = xmlNewTextWriterFilename(absoluteConfigurationFilename,
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
  errorCode = xmlTextWriterStartElement(writer,
					(xmlChar*)"GDispConfiguration");

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
  tmp = gdisp_xmlConvertInput("A GDisp+ Configuration",
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

}
