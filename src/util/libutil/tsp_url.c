/*

$Id: tsp_url.c,v 1.2 2007-04-26 18:00:41 deweerdt Exp $

-----------------------------------------------------------------------

TSP Utils Library - utilities for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Utils library

-----------------------------------------------------------------------

Purpose   : parse TSP url
            The TSP url parsing follows the URI syntax of RFC 3936
 *
 * Excerpt from  RFC3986
 *  ftp://ftp.rfc-editor.org/in-notes/rfc3986.txt
 *
 *  The following are two example URIs and their component parts:
 *
 *        foo://example.com:8042/over/there?name=ferret#nose
 *        \_/   \______________/\_________/ \_________/ \__/
 *         |           |            |            |        |
 *      scheme     authority       path        query   fragment
 *         |   _____________________|__
 *        / \ /					\
 *        urn:example:animal:ferret:nose
 *
 *       scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
 *                   (scheme should be case INSENSITIVE)
 *       authority   = [ userinfo "@" ] host [ ":" port ]
 *       userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
 * 
-----------------------------------------------------------------------
 */
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

#include <tsp_abs_types.h>
#define TSP_URL_C
#include <tsp_url.h>

int32_t 
tspUrlScheme_create(TspUrlScheme_t* tspUrlScheme, const char* schemeString) {
  assert(tspUrlScheme);
  /* scheme      = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
   *                   (scheme should be case INSENSITIVE)
   */
  TspUrlProtocol_t proto;
  tspUrlScheme->protocol = TSP_URL_PROTO_UNKNOWN;
  for (proto = TSP_URL_PROTO_UNKNOWN +1; proto<TSP_URL_PROTO_LAST; ++proto) {
    if (!strncasecmp(schemeString,tspUrl_proto_tab[proto])) {
      tspUrlScheme->protocol = proto;
    }
  }
  /* if scheme/protocol has been recognized return 0 */
  return (tspUrlScheme->protocol == TSP_URL_PROTO_UNKNOWN);
}

int32_t 
tspUrlScheme_destroy(TspUrlScheme_t* tspUrlScheme) {
  if (NULL != tspUrlScheme) {
    tspUrlScheme->protocol = TSP_URL_PROTO_UNKNOWN;
  }
  return 0;
}

int32_t 
tspUrlUserInfo_create(TspUrlUserInfo_t* tspUrlUserInfo, const char* userInfoString) {
  assert(tspUrlUserInfo);
  /* userinfo    = *( unreserved / pct-encoded / sub-delims / ":" ) */
  if (NULL != tspUrlUserInfo->userInfo) free(tspUrlUserInfo->userInfo);
  tspUrlUserInfo->userInfo = strdup(userInfoString);
  return 0;
}

int32_t
tspUrlUserInfo_destroy(TspUrlUserInfo_t* tspUrlUserInfo) {
  int32_t retcode =0;
  if (NULL == tspUrlUserInfo) return retcode;
  if (NULL != tspUrlUserInfo->userInfo) free(tspUrlUserInfo->userInfo);
  return retcode;
}

int32_t 
tspUrlAuthority_create(TspUrlAuthority_t* tspUrlAuth, const char* authorityString) {
  assert(tspUrlAuth);
  int32_t retcode = 0;
  char* userInfoString;
  char* myAuthString;
  char* current;
  char* idxAt;
  char* idxTwoPoints;
  /*
   *  authority   = [ userinfo "@" ] host [ ":" port ]
   *  userinfo    = *( unreserved / pct-encoded / sub-delims / ":" )
   */
  myAuthString  = strdup(authorityString);
  current       = myAuthString;
  idxAt         = index(current,'@');
  if (NULL != idxAt) {
    userInfoString = calloc(idxAt-current+1,sizeof(char));
    strncpy(userInfoString,current,idxAt-current);    
    retcode &= tspUrlUserInfo_create(&(tspUrlAuth->userinfo),userInfoString);
    free(userInfoString);
    current = idxAt+1;
  }
  idxSemiColumn = index(current,':');
  if (NULL != idxSemiColumn) {
    tspUrlAuth->host = calloc(idxSemiColumn-current+1,sizeof(char));
    strncpy(tspUrlAuth->host,current,idxAt-current);   
  } else {
    tspUrlAuth->host = strdup(current);
  }
  free(myAuthString);
}

int32_t 
tspUrlAuthority_destroy(TspUrlAuthority_t* tspUrlAuth) {
  int32_t retcode = 0;
  if (NULL == tspUrlAuth) return retcode;
  
  if (NULL != tspUrlAuth->host) free(tspUrlAuth->host);
  tspUrlAuth->port = -1;
}

int32_t 
tspUrl_create(TspUrl_t* tspUrl, const char* urlString) {
  assert(tspUrl);
  return -1;
}

int32_t 
tspUrl_destroy(TspUrl_t* tspUrl) {
  int32_t retcode = 0;

  if (NULL == tspUrl) return retcode;
  
  retcode &= tspUrlScheme_destroy(&(tspUrl->scheme));
  retcode &= tspUrlAuthority_destroy(&(tspUrl->authority));
  if (NULL != tspUrl->path)  free(tspUrl->path);
  if (NULL != tspUrl->query) free(tspUrl->query);
  if (NULL != tspUrl->free)  free(tspUrl->fragment);
  if (NULL != tspUrl->stringRepresentation)  free(tspUrl->fragment);
  return retcode;
} /* end of tspUrl_destroy */


/**
 * Get the string representation.
 */
const char* 
tspUrl_getStringRepresentation(const TspUrl_t tspUrl) {
  if (NULL == tspUrl.stringRepresentation) {
    /* should build string representation */
  }
  return tspUrl.stringRepresentation;
}

/* getter/setter on TspUrl */
const char* tspUrl_getSchemeString(const TspUrl_t tspUrl);
TspUrlProtocol_t tspUrl_getScheme(const TspUrl_t tspUrl);
int32_t tspUrl_setScheme(TspUrl_t* tspUrl, TspUrlScheme_t scheme);

const char* tspUrl_getProtocolString(const TspUrl_t tspUrl);
TspUrlProtocol_t tspUrl_getProtocol(const TspUrl_t tspUrl);
int32_t tspUrl_setProtocol(TspUrl_t* tspUrl, TspUrlProtocol_t protocol);

const char* tspUrl_getAuthorityString(const TspUrl_t tspUrl);
TspUrlAuthority_t tspUrl_getAutority(const TspUrl_t tspUrl);
int32_t tspUrl_setAuthority(TspUrl_t* tspUrl, TspUrlAuthority_t authority);


const char* tspUrl_getPath(const TspUrl_t tspUrl);
int32_t tspUrl_setPath(TspUrl_t* tspUrl, const char* path);

const char* tspUrl_getQuery(const TspUrl_t tspUrl);
int32_t tspUrl_setQuery(TspUrl_t* tspUrl, const char* query);

const char* tspUrl_getFragment(const TspUrl_t tspUrl);
int32_t tspUrl_setFragment(TspUrl_t* tspUrl, const char* fragment);



