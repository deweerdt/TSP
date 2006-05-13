/*

$Id: gdisp_hosts.c,v 1.8 2006-05-13 20:55:02 esteban Exp $

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

File      : HOSTS Management.

-----------------------------------------------------------------------
*/


/*
 * System includes.
 */
#include <stdio.h>
#include <assert.h>
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
 * Find a host by its name.
 */
static Host_T*
gdisp_findHostByName ( Kernel_T *kernel,
		       gchar    *hostName )
{

  Host_T *host     = (Host_T*)NULL;
  GList  *hostItem =  (GList*)NULL;

  /*
   * Loop over the kernel host list.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    if (strcmp(host->hName->str,hostName) == 0) {
      return host;
    }

    hostItem = g_list_next(hostItem);

  }

  return (Host_T*)NULL;

}


/*
 * Compare 2 URLs.
 */
static gint
gdisp_compareUrl ( gconstpointer vUrl1,
		   gconstpointer vUrl2 )
{

  gchar *url1 = (gchar*)vUrl1;
  gchar *url2 = (gchar*)vUrl2;

  return strcmp(url1,url2);

}


/*
 --------------------------------------------------------------------
                             PUBLIC ROUTINES
 --------------------------------------------------------------------
*/


/*
 * Build the host list according to user specifications.
 */
void
gdisp_addHost ( Kernel_T *kernel,
		gchar    *hostName )
{

  Host_T *host = (Host_T*)NULL;

  /*
   * Check.
   */
  if (hostName == (gchar*)NULL) {
    return;
  }

  /*
   * Add a new host into the host list, if this host does not already exist.
   */
  if (gdisp_findHostByName(kernel,hostName) == (Host_T*)NULL) {

    /*
     * Allocate a host structure.
     */
    host = g_malloc0(sizeof(Host_T));
    assert(host);

    host->hName = g_string_new(hostName);

    /*
     * Insert this new host into the host list.
     */
    kernel->hostList = g_list_append(kernel->hostList,
				     (gpointer)host);

  }

}


/*
 * Build the URL list according to user specifications.
 */
void
gdisp_addUrl ( Kernel_T *kernel,
	       gchar    *urlName )
{

  gchar *url = (gchar*)NULL;

  /*
   * Check.
   */
  if (urlName == (gchar*)NULL) {
    return;
  }

  if (g_list_find_custom(kernel->urlList,
			 (gpointer)urlName,
			 gdisp_compareUrl) == (GList*)NULL) {

    /*
     * Duplicate URL name
     */
    url = gdisp_strDup(urlName);
    assert(url);

    /*
     * Insert this new URL into the kernel URL list.
     */
    kernel->urlList = g_list_append(kernel->urlList,
				    (gpointer)url);

  }

}


/*
 * Destroy host list.
 */
void
gdisp_destroyHosts ( Kernel_T *kernel )
{

  GList  *hostItem =  (GList*)NULL;
  Host_T *host     = (Host_T*)NULL;


  /*
   * Release all hosts.
   */
  hostItem = g_list_first(kernel->hostList);
  while (hostItem != (GList*)NULL) {

    host = (Host_T*)hostItem->data;

    g_string_free(host->hName,TRUE);

    g_free(host);

    hostItem = g_list_next(hostItem);

  }

  g_list_free(kernel->hostList);
  kernel->hostList = (GList*)NULL;

}


/*
 * Destroy URL list.
 */
void
gdisp_destroyUrls ( Kernel_T *kernel )
{

  GList  *urlItem = (GList*)NULL;
  gchar  *url     = (gchar*)NULL;


  /*
   * Release all URLs.
   */
  urlItem = g_list_first(kernel->urlList);
  while (urlItem != (GList*)NULL) {

    url = (gchar*)urlItem->data;

    g_free(url);

    urlItem = g_list_next(urlItem);

  }

  g_list_free(kernel->urlList);
  kernel->urlList = (GList*)NULL;

}
