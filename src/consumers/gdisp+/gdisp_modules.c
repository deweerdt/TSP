/*

$Id: gdisp_modules.c,v 1.1 2006-08-05 20:50:30 esteban Exp $

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

File      : Dynamic module utilities.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <strings.h>

/*
 * GLib modules.
 */
#include <gmodule.h>

/*
 * GDISP+ includes.
 */
#include "gdisp_kernel.h"
#include "gdisp_prototypes.h"


/*
 --------------------------------------------------------------------

 --------------------------------------------------------------------
*/



/*
 --------------------------------------------------------------------
                             STATIC ROUTINES
 --------------------------------------------------------------------
*/

/*
 * Check whether a graphic plot system is fully supported.
 */
static void
gdisp_checkGraphicModuleSupport ( PlotSystem_T *plotSystem )
{

  guint           functionSetSize = 0;
  guint           functionCpt     = 0;
  guint           functionNb      = 0;
  FunctionTable_T functionTable   = (FunctionTable_T)NULL;

  /*
   * Remove size of 'psIsSupported'.
   */
  functionSetSize = sizeof(PlotSystem_T) - sizeof(gboolean);
  functionNb      = functionSetSize / sizeof(aFunction_T);

  /*
   * By default, the plot system is supported.
   */
  plotSystem->psIsSupported = TRUE;
  functionTable             = (FunctionTable_T)plotSystem;

  for (functionCpt=0; functionCpt<functionNb; functionCpt++) {

    if (functionTable[functionCpt] == (aFunction_T)NULL) {

      plotSystem->psIsSupported = FALSE;

    } /* if */

  } /* for 'functionCpt' */

}


#if defined(GD_DYNAMIC_GRAPHIC_MODULES)

/*
 * Automatically find GLib prefix and suffix of dynamic modules.
 */
static gboolean
gdisp_findDynamicPrefixAndSuffix ( gchar **prefix,
				   gchar **suffix )
{

  gchar *moduleName  = (gchar*)NULL;
  gchar *fakePath    = "fakePath";
  gchar *sharp       = "#";
  gchar *localPrefix = (gchar*)NULL;
  gchar *localSuffix = (gchar*)NULL;

  /*
   * Init.
   */
  *prefix = (gchar*)NULL;
  *suffix = (gchar*)NULL;

  /*
   * Create fake module name.
   */
  moduleName = g_module_build_path(fakePath,sharp);

  if (strstr(moduleName,fakePath) != moduleName) {
    g_free(moduleName);
    return FALSE;
  }

  localPrefix = moduleName + strlen(fakePath);
  if (*localPrefix == G_DIR_SEPARATOR) {
    localPrefix++;
  }

  localSuffix  = strstr(localPrefix,sharp);
  *localSuffix = '\0';

  localSuffix++;

  /*
   * Return the strings.
   */
  if (strlen(localPrefix) > 0) {
    *prefix = gdisp_strDup(localPrefix);
  }
  if (strlen(localSuffix) > 0) {
    *suffix = gdisp_strDup(localSuffix);
  }

  g_free(moduleName);

  return TRUE;

}


/*
 * Load graphic modules as dynamic TARGA objects.
 */
static gboolean
gdisp_loadDynamicGraphicModules ( Kernel_T  *kernel )
{

  DIR           *directory      = (DIR*)NULL;
  struct dirent *dirEntry       = (struct dirent*)NULL;
  gchar         *prefix         = (gchar*)NULL;
  gchar         *suffix         = (gchar*)NULL;
  gboolean       isOk           = TRUE;
  guint          sPos           = 0; /* suffix position */
  GString       *messageString  = (GString*)NULL;
  GString       *fullModuleName = (GString*)NULL;

  /*
   * Graphic module entry point.
   */
  PlotSystem_T      plotSystem;
  PlotSystemInfo_T  plotInformation;
  PlotType_T        plotType        = GD_PLOT_DEFAULT;
  GModule          *dynModule       = (GModule*)NULL;
  gboolean          entryPointExist = TRUE;
  void            (*moduleEntryPoint)(Kernel_T     *kernel,
				      PlotSystem_T *plotSystem);

  /*
   * CAUTION : start with default plot which always exists.
   */
  gdisp_initDefaultPlotSystem(kernel,
			      &kernel->plotSystems[GD_PLOT_DEFAULT]);

  /*
   * Where are graphic modules ?
   */
  if (kernel->pathToGraphicModules == (gchar*)NULL) {
    messageString = g_string_new("Unknown path to graphic modules");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);	
    return FALSE;
  }

  /*
   * Look into the directory that contains the dynamic modules.
   */
  directory = opendir(kernel->pathToGraphicModules);

  if (directory == (DIR*)NULL) {
    messageString = g_string_new("Invalid path to graphic modules");
    (*kernel->outputFunc)(kernel,messageString,GD_ERROR);	
    return FALSE;
  }

  /*
   * Deduce from GLib kernel the dynamic module prefix and suffix.
   */
  isOk = gdisp_findDynamicPrefixAndSuffix(&prefix,
					  &suffix);
  if (isOk == FALSE) {
    return FALSE;
  }

  /*
   * Get back all directory entries.
   */
  dirEntry       = readdir(directory);
  fullModuleName = g_string_new((gchar*)NULL);

  while (dirEntry != (struct dirent*)NULL) {

    /*
     * Take into account only regular files.
     */
    if (dirEntry->d_type == DT_REG) {

      sPos = suffix != (gchar*)NULL ?
	     strlen(dirEntry->d_name) - strlen(suffix) : 0;

      if (( prefix == (gchar*)NULL ||
	   (prefix != (gchar*)NULL &&
	    strstr(dirEntry->d_name,prefix) == dirEntry->d_name)) &&
	  ( suffix == (gchar*)NULL ||
	   (suffix != (gchar*)NULL &&
	    strstr(dirEntry->d_name,suffix) == dirEntry->d_name + sPos))) {

	g_string_sprintf(fullModuleName,
			 "%s/%s",
			 kernel->pathToGraphicModules,
			 dirEntry->d_name);

	dynModule = g_module_open(fullModuleName->str,
				  G_MODULE_BIND_LAZY);

	if (dynModule == (GModule*)NULL) {

	  messageString = g_string_new(g_module_error());
	  (*kernel->outputFunc)(kernel,messageString,GD_MESSAGE);	

	}
	else {

	  entryPointExist = g_module_symbol(dynModule,
					    "gdisp_initGraphicSystem",
					    (gpointer)&moduleEntryPoint);

	  if (entryPointExist == TRUE) {

	    /*
	     * Init the current graphic module.
	     */
	    memset(&plotSystem,0,sizeof(PlotSystem_T));
	    (*moduleEntryPoint)(kernel,
				&plotSystem);

	    /*
	     * We must check out, at kernel level, that all functions
	     * have been initialised by the plot system.
	     */
	    gdisp_checkGraphicModuleSupport(&plotSystem);

	    /*
	     * If plot system is fully supported, get back its type.
	     */
	    if (plotSystem.psIsSupported == TRUE) {

	      plotType = (*plotSystem.psGetType)(kernel);

	      if (kernel->plotSystems[plotType].psIsSupported == FALSE) {

		kernel->plotSystems[plotType] = plotSystem;

	      }
	      else {

		(*plotSystem.psGetInformation)(kernel,
					       &plotInformation);

		messageString = g_string_new((gchar*)NULL);
		g_string_sprintf(messageString,
				 "Redefinition of %s",
				 plotInformation.psName != (gchar*)NULL ?
				 plotInformation.psName :
				 "<unknown>");
		(*kernel->outputFunc)(kernel,messageString,GD_ERROR);	

	      }

	    } /* plotSystem is supported */

	    else {

	      messageString = g_string_new((gchar*)NULL);
	      g_string_sprintf(messageString,
			       "%s not fully implemented",
			       dirEntry->d_name);
	      (*kernel->outputFunc)(kernel,messageString,GD_ERROR);	

	    }

	  } /* module entry point does not exist */

	} /* dynModule != (GModule*)NULL */

      } /* entry has a correct prefix and a correct suffic */

    } /* entry is a regular file */

    /*
     * Next entry.
     */
    dirEntry = readdir(directory);

  }

  /*
   * Properly close the directory.
   */
  g_free(prefix);
  g_free(suffix);
  g_string_free(fullModuleName,TRUE);
  closedir(directory);

  return TRUE;

}

#else

/*
 * Load graphic modules as static TARGA objects.
 */
static gboolean
gdisp_loadStaticGraphicModules ( Kernel_T  *kernel )
{

  PlotSystem_T *plotSystem = (PlotSystem_T*)NULL;
  PlotType_T    plotType   = GD_PLOT_DEFAULT;

  /*
   * Loop over all available plots.
   */
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
    gdisp_checkGraphicModuleSupport(plotSystem);

  } /* for 'plotType' */

  /*
   * Default plot type.
   */
  kernel->currentPlotType = GD_PLOT_TEXT;

  return TRUE;

}

#endif

/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/

void
gdisp_loadGraphicModules ( Kernel_T  *kernel )
{

  gboolean isOk = TRUE;

#if defined(GD_DYNAMIC_GRAPHIC_MODULES)

  isOk = gdisp_loadDynamicGraphicModules(kernel);

#else

  isOk = gdisp_loadStaticGraphicModules(kernel);

#endif

}
