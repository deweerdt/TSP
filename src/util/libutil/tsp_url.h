/*

$Id: tsp_url.h,v 1.1 2006-04-13 21:25:27 erk Exp $

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

#include <tsp_abs_types.h>

typedef enum TspUrlProtocol {

  TSP_URL_PROTO_UNKNOWN = 0,
  TSP_URL_PROTO_RPC,
  TSP_URL_PROTO_XMLRPC,
  TSP_URL_PROTO_LAST
  
} TspUrlProtocol_t;

#ifdef TSP_URL_C
const char* tspUrl_proto_tab[] = {"unknown",
				  "rpc",
				  "xmlrpc",
				  "tspUrl_proto_last"
};
#else
extern const char* tspUrl_proto_tab[];
#endif

typedef struct TspUrlScheme {
  TspUrlProtocol_t protocol;
} TspUrlScheme_t;

typedef struct TspUrlUserInfo {
  char* userInfo;
} TspUrlUserInfo_t;

typedef struct TspUrlAuthority {
  TspUrlUserInfo_t userinfo;
  char*            host;
  int32_t          port;
} TspUrlAuthority_t;

typedef struct TspUrl {  
  TspUrlScheme_t     scheme;
  TspUrlAuthority_t  authority;  
  char* path;
  char* query;
  char* fragment;
  char* stringRepresentation;
} TspUrl_t;

int32_t tspUrlScheme_create(TspUrlScheme_t* tspUrlScheme, const char* schemeString);
int32_t tspUrlScheme_destroy(TspUrlScheme_t* tspUrlScheme);

int32_t tspUrlUserInfo_create(TspUrlUserInfo_t* tspUrlUserInfo, const char* userInfoString);
int32_t tspUrlUserInfo_destroy(TspUrlUserInfo_t* tspUrlUserInfo);

int32_t tspUrlAuthority_create(TspUrlAuthority_t* tspUrlAuth, const char* authorityString);
int32_t tspUrlAuthority_destroy(TspUrlAuthority_t* tspUrlAuth);

/**
 * Create a Tsp Url object from
 * the url string representation
 * @param tspUrl OUT, the built TSP url
 * @param urlString IN, the url string representation
 * @return 0 if URL string has been parsed properly
 *         non null otherwise.
 */
int32_t tspUrl_create(TspUrl_t* tspUrl; const char* urlString);

/**
 * Destroy a Tsp Url object from
 * the url string representation
 */
int32_t tspUrl_destroy(TspUrl_t* tspUrl);

/**
 * Get the string representation.
 */
const char*  tspUrl_getStringRepresentation(const TspUrl_t tspUrl);

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



