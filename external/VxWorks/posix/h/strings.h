/*

$Header $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2005 Cesare BERTONA

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
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
*/

#ifndef _STRINGS_H
#define _STRINGS_H

char *strdup (const char *s1);
unsigned long long strtoull(const char *str, char **endptr, int base);
long long strtoll(const char *str, char **endptr, int base);
int vsnprintf(char *s, size_t n, const char *format,  va_list ap);
int snprintf(char *s,  size_t  n,  const  char  *format,  /*args*/ ...);

#endif /* _STRINGS_H */
