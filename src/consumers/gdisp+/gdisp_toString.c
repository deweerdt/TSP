/*!  \file 

$Id: gdisp_toString.c,v 1.1 2005-10-05 19:21:01 esteban Exp $

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
  else /* default "SortByProvider" */ {

    kernel->sortingMethod = GD_SORT_BY_PROVIDER;

  }

}


/*
 * Set up DnD Scope from string.
 */
static void
gdisp_setDnDScopeFromString ( Kernel_T *kernel,
			      gchar    *requestedScope )
{

  if (strcmp(requestedScope,"Unicast") == 0) {

    kernel->dndScope = GD_DND_UNICAST;

  }
  else if (strcmp(requestedScope,"Multicast") == 0) {

    kernel->dndScope = GD_DND_MULTICAST;

  }
  else /* default "Broadcast" */ {

    kernel->dndScope = GD_DND_BROADCAST;

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

  gchar *myString = (gchar*)"Unknown";

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

  case GD_SORT_BY_PROVIDER :
    myString = "SortByProvider";
    break;

  default :
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

  gchar *myString = (gchar*)"Unknown";

  switch (kernel->dndScope) {

  case GD_DND_UNICAST :
    myString = "Unicast";
    break;

  case GD_DND_MULTICAST :
    myString = "Multicast";
    break;

  case GD_DND_BROADCAST :
    myString = "Broadcast";
    break;

  default :
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


