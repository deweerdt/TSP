/*!  \file 

$Id: gdisp_hosts.c,v 1.1 2004-06-17 21:07:41 esteban Exp $

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

File      : HOSTS Management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <getopt.h>
#include <assert.h>


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
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Build the host list according to user specifications.
 */
void
gdisp_buildHostList ( Kernel_T *kernel )
{

  gint    opt  = 0;
  Host_T *host = (Host_T*)NULL;

  /*
   * The user can specify several host to be looked at, with the '-h' option.
   */
  while ((opt = getopt(kernel->argCounter,
		       kernel->argTable,
		       "h:")) != EOF) {

    switch (opt) {

    case 'h' :

      /*
       * FIXME : avoid same names...
       */

      /*
       * Allocate a host structure.
       */
      host = g_malloc0(sizeof(Host_T));
      assert(host);
      host->hName = g_string_new(optarg);

      /*
       * Insert this new host into the host list.
       */
      kernel->hostList = g_list_append(kernel->hostList,
				       (gpointer)host);

      break;

    default :
      break;

    } /* end switch */

  } /* end while */

}
