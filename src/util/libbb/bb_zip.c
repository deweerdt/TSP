/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/Attic/bb_zip.c,v 1.1 2007-02-20 14:53:34 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2007 Frederik Deweerdt

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
Maintainer : tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Blackboard get/set varname primitives that zip the varname

-----------------------------------------------------------------------
 */

#include <zlib.h>
#include "bb_core.h"

char *bb_get_varname_zip(const S_BB_DATADESC_T *dd)
{
  unsigned long len = VARNAME_MAX_SIZE * 3;
  int ret;
  char *uncomp = malloc(len);
                          
  ret = uncompress((unsigned char *)uncomp, &len, (unsigned char *)dd->__name, VARNAME_MAX_SIZE);
  if (ret) {
    return NULL;
  }
  uncomp[len] = '\0';
  return uncomp;
}

void bb_set_varname_zip(S_BB_DATADESC_T *dd, const char *key)
{
	int ret;
  unsigned long len;
  char *comp;
	
  /* see zlib manual, they require 1% + 12 chars */
  len = strlen(key)*1.01 + 12;
  comp = malloc(len);
  ret = compress2((unsigned char *)comp, &len, (unsigned char *)key, strlen(key), 9);
  if (ret) {
    return;
  }
  assert(len < VARNAME_MAX_SIZE);
  memcpy(dd->__name, comp, len);
  free(comp);
}
