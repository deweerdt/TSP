/*

$Id: gdisp_toString.c,v 1.3 2006-07-30 20:25:58 esteban Exp $

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

File      : Graphic Tool 'toString' conversions.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
 * Set up Sorting methods from string.
 */
static void
gdisp_setSortingMethodFromString ( Kernel_T *kernel,
				   gchar    *requestedMethod )
{

  if (strcmp(requestedMethod,"SortByName") == 0) {

    kernel->sortingMethod = GD_SORT_BY_NAME;

  }
  else if (strcmp(requestedMethod,"SortByNameReverse") == 0) {

    kernel->sortingMethod = GD_SORT_BY_NAME_REVERSE;

  }
  else if (strcmp(requestedMethod,"SortByIndex") == 0) {

    kernel->sortingMethod = GD_SORT_BY_INDEX;

  }
  else if (strcmp(requestedMethod,"SortByType") == 0) {

    kernel->sortingMethod = GD_SORT_BY_TYPE;

  }
  else if (strcmp(requestedMethod,"SortByDim") == 0) {

    kernel->sortingMethod = GD_SORT_BY_DIM;

  }
  else if (strcmp(requestedMethod,"SortByExtInfo") == 0) {

    kernel->sortingMethod = GD_SORT_BY_EXTINFO;

  }
  else /* default "SortByProvider" */ {

    kernel->sortingMethod = GD_SORT_BY_PROVIDER;

  }

}


/*
 * Set up Sorting methods from string.
 */
static void
gdisp_setSortingDirectionFromString ( Kernel_T *kernel,
				      gchar    *requestedDirection )
{

  if (strcmp(requestedDirection,"Descending") == 0) {

    kernel->sortingDirection = GD_SORT_DESCENDING;

  }
  else {

    kernel->sortingDirection = GD_SORT_ASCENDING;

  }

}


/*
 * Set up DnD Scope from string.
 */
static void
gdisp_setDnDScopeFromString ( Kernel_T *kernel,
			      gchar    *requestedScope )
{

  if (strcmp(requestedScope,"Broadcast") == 0) {

    kernel->dndScope = GD_DND_BROADCAST;

  }
  else if (strcmp(requestedScope,"Multicast") == 0) {

    kernel->dndScope = GD_DND_MULTICAST;

  }
  else /* default "Unicast" */ {

    kernel->dndScope = GD_DND_UNICAST;

  }

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Convert Sorting methods to string.
 */
gchar*
gdisp_sortingMethodToString ( Kernel_T *kernel )
{

  gchar *myString = (gchar*)NULL;

  switch (kernel->sortingMethod) {

  case GD_SORT_BY_NAME :
    myString = "SortByName";
    break;

  case GD_SORT_BY_NAME_REVERSE :
    myString = "SortByNameReverse";
    break;

  case GD_SORT_BY_INDEX :
    myString = "SortByIndex";
    break;

  case GD_SORT_BY_TYPE :
    myString = "SortByType";
    break;

  case GD_SORT_BY_DIM :
    myString = "SortByDim";
    break;

  case GD_SORT_BY_EXTINFO :
    myString = "SortByExtInfo";
    break;

  case GD_SORT_BY_PROVIDER :
  default :
    myString = "SortByProvider";
    break;

  }

  return myString;

}


/*
 * Convert Sorting direction to string.
 */
gchar*
gdisp_sortingDirectionToString ( Kernel_T *kernel )
{

  gchar *myString = (gchar*)NULL;

  switch (kernel->sortingDirection) {

  case GD_SORT_DESCENDING :
    myString = "Descending";
    break;

  case GD_SORT_ASCENDING :
  default :
    myString = "Ascending";
    break;

  }

  return myString;

}


/*
 * Convert DnD Scope to string.
 */
gchar*
gdisp_dndScopeToString ( Kernel_T *kernel )
{

  gchar *myString = (gchar*)NULL;

  switch (kernel->dndScope) {

  case GD_DND_MULTICAST :
    myString = "Multicast";
    break;

  case GD_DND_BROADCAST :
    myString = "Broadcast";
    break;

  case GD_DND_UNICAST :
  default :
    myString = "Unicast";
    break;

  }

  return myString;

}


/*
 * Set up preference from string.
 */
void
gdisp_setUpPreferenceFromString ( Kernel_T *kernel,
				  gchar    *preference,
				  gchar    *value )
{

  gint targetXPosition = 0;
  gint targetYPosition = 0;
  gint nbMatch         = 0;

  /*
   * Symbol sorting method.
   */
  if (strcmp(preference,"sortingMethod") == 0) {

    gdisp_setSortingMethodFromString(kernel,
				     value);

  }
  /*
   * Symbol sorting direction.
   */
  else if (strcmp(preference,"sortingDirection") == 0) {

    gdisp_setSortingDirectionFromString(kernel,
					value);

  }
  /*
   * Drag & Drop scope.
   */
  else if (strcmp(preference,"dndScope") == 0) {

    gdisp_setDnDScopeFromString(kernel,
				value);

  }
  /*
   * Main board window position.
   */
  else if (strcmp(preference,"mainBoardPosition") == 0) {

    nbMatch = sscanf(value,
		     "%d,%d",
		     &targetXPosition,
		     &targetYPosition);

    if (nbMatch == 2) {

      kernel->widgets.mainBoardWindowXPosition = targetXPosition;
      kernel->widgets.mainBoardWindowYPosition = targetYPosition;

    }

  }
  /*
   * Data book window position.
   */
  else if (strcmp(preference,"dataBookPosition") == 0) {

    nbMatch = sscanf(value,
		     "%d,%d",
		     &targetXPosition,
		     &targetYPosition);

    if (nbMatch == 2) {

      kernel->widgets.dataBookWindowXPosition = targetXPosition;
      kernel->widgets.dataBookWindowYPosition = targetYPosition;

    }

  }
  else {

    /* nothing else by now */

  }

}


/*
 * Get symbol type as a string.
 */
gchar*
gdisp_getTypeAsString (Symbol_T *symbol)
{

  gchar *typeList[] = { "-?-",
			"F64",
			"F32",
			"I08",
			"I16",
			"I32",
			"I64",
			"U08",
			"U16",
			"U32",
			"U64",
			"C08",
			"UC8",
			"RAW",
			"-?-" };

  if (symbol->sInfo.type >= TSP_TYPE_LAST) {
    symbol->sInfo.type = TSP_TYPE_LAST;
  }

  return typeList[symbol->sInfo.type];

}


/*
 * Get symbol type from a string.
 */
TSP_datatype_t
gdisp_getTypeFromString (gchar *typeAsString)
{

  TSP_datatype_t  symbolType = TSP_TYPE_UNKNOWN;
  gchar          *typeList[] = { "-?-",
				 "F64",
				 "F32",
				 "I08",
				 "I16",
				 "I32",
				 "I64",
				 "U08",
				 "U16",
				 "U32",
				 "U64",
				 "C08",
				 "UC8",
				 "RAW",
				 "-?-" };

  if (typeAsString == (gchar*)NULL) {
    return TSP_TYPE_UNKNOWN;
  }

  for (symbolType=TSP_TYPE_UNKNOWN;
       symbolType<TSP_TYPE_LAST;
       symbolType++) {

    if (strcmp(typeAsString,typeList[symbolType]) == 0) {
      return symbolType;
    }

  }

  return TSP_TYPE_UNKNOWN;

}


