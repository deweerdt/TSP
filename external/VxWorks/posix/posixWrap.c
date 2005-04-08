/*!  \file 

$Header: /home/def/zae/tsp/tsp/external/VxWorks/posix/posixWrap.c,v 1.2 2005-04-08 14:45:20 le_tche Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

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

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : external

-----------------------------------------------------------------------

Purpose   : posix implementation for VXWORKS

-----------------------------------------------------------------------
 */

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <vxWorks.h>
#include <taskLib.h>
#include <sysLib.h>

#include "strings.h"
#include "libgen.h"


int gettimeofday (struct timeval *tp,void *vide)
{
struct timespec localtp;
if (clock_gettime (CLOCK_REALTIME,&localtp)==0) 
    {
    tp->tv_sec = (time_t)localtp.tv_sec ;
    tp->tv_usec = (time_t)(localtp.tv_nsec/1000) ;
    return 0 ;
    }
else
    return -1 ;
}

/* -------------------------------------------------------------------------------------------*/

int getuid(void)
{
  /* no user id on vxworks, return proc id instead */
  return (sysProcNumGet()+1);
}

int getpid(void)
{
  /* no processes on vxworks, only tasks */
  return taskIdSelf();
}

ushort getrpcport(char *host, rpcprog_t  prognum,  rpcvers_t versnum, rpcprot_t proto)
{
  /* always return same port */
  /* FIXME : return real port */
  return 1;
}

/* -------------------------------------------------------------------------------------------*/

char *strdup (const char *s1) 
{
char *p ;
p = (char *)malloc((strlen(s1)+1)*sizeof(char)) ;
if (p != NULL) strcpy (p,s1) ;
return p ;
}

unsigned long long strtoull(const char *str, char **endptr, int base)
{
  unsigned long long val = 0;

  if(base == 10) {
    sscanf(str, "%llu", &val);
  }
  else if (base == 16) {
    sscanf(str, "%llx", &val);
  }

  return val;
}

long long strtoll(const char *str, char **endptr, int base)
{
  long long val = 0;

  if(base == 10) {
    sscanf(str, "%lld", &val);
  }
  else if (base == 16) {
    sscanf(str, "%llx", &val);
  }

  return val;
}

char * basename(char * path)
{
  static char	*null_entry = ".";
  char		*pointer;

  /*
     ___________________________________________________________
    |        Input String         |        Output String        |
    |_____________________________|_____________________________|
    | "/usr/lib"                  | "lib"                       |
    |_____________________________|_____________________________|
    | "/usr/"                     | "usr"                       |
    |_____________________________|_____________________________|
    | "/"                         | "/"                         |
    |_____________________________|_____________________________|
    | ""                          | "."                         |
    |_____________________________|_____________________________|
  */

  if((path == NULL) || (strlen(path) == 0)) {
    return null_entry;
  }
  
  pointer = strrchr(path, '/');

  if(pointer == NULL) {
    return path;
  }

  if((*(pointer+1) == 0) && (strlen(path) > 1)) {
    *pointer = 0;
    return basename(path);
  }
  else if(*(pointer+1) != 0) {
    return (pointer+1);
  }

  return pointer;
}

int vsnprintf(char *s, size_t n, const char *format, va_list ap)
{
  /* do not check size of buffer */
  return vsprintf(s, format, ap);
}

int snprintf(char *s,  size_t  n,  const  char  *format,  /*args*/ ...)
{
  va_list	ap;
  int		ret;

  va_start(ap, format);

  ret = vsnprintf(s, n, format, ap);

  va_end(ap);

  return ret;
}
