/*!  \file

$Header: /home/def/zae/tsp/tsp/src/util/libres/libUTIL.h,v 1.3 2004-09-06 16:07:33 tractobob Exp $

-----------------------------------------------------------------------

libUTIL - core components to read and write res files

Copyright (c) 2002 Marc LE ROY 

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
Component : libUTIL

-----------------------------------------------------------------------

Purpose   :

-----------------------------------------------------------------------
 */

#ifndef _RES_INTERFACE_H
#define _RES_INTERFACE_H

extern int _use_dbl;

#define RES_COM_NUM	20
#define RES_COM_LEN	256

#define RES_VAR_NUM	3000
#define RES_NAME_LEN	100
#define RES_DESC_LEN	256

typedef void* d_rhandle;
typedef void* d_whandle;



#if defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

d_rhandle d_ropen_r(char *name, int* use_dbl);
int	d_rval_r(d_rhandle h, int c);
void	d_rnam_r(d_rhandle h,char *nam, char *desc, int i);
void	d_rcom_r(d_rhandle h,char *com, int i);
int	d_read_r(d_rhandle h,void *buf);
int	d_restart_r(d_rhandle h);
int	d_dread_r(d_rhandle h,void *buf, int i);
void	d_rclos_r(d_rhandle h);


d_whandle	d_wopen_r(char *name, int use_dbl);
void	d_wcom_r(d_whandle h,char *com);
void	d_wnam_r(d_whandle h,char *nam, char *des);
void	d_writ_r(d_whandle h,void *buf);
void	d_wclos_r(d_whandle h);

#endif

#if !defined(_LIBUTIL_REENTRANT) || defined(_LIBUTIL_COMPIL)

void     d_ropen(char *name);
int	d_rval(int c);
void	d_rnam(char *nam, char *desc, int i);
void	d_rcom(char *com, int i);
int	d_read(void *buf);
int	d_dread(void *buf, int i);

void	d_wopen (char *name);
void	d_wcom(char *com);
void	d_wnam(char *nam, char *des);
void	d_writ(void *buf);

void	d_clos(void);

#endif

/*-----------------------------------------------------*\
|							|
| Macros for endianity, taken from GLIB			|
|							|
\*-----------------------------------------------------*/
#include <sys/types.h>

#define MY_LITTLE_ENDIAN 1234
#define MY_BIG_ENDIAN    4321

/*** Platform specific ***/

/* SUN / sparc (32/64 bits) */
#if  defined(__sun)  && defined(__sparc)
# define MY_BYTE_ORDER MY_BIG_ENDIAN
# if  defined(__sparcv9)
#   define INT64_CONSTANT(val)  (val##L)
# else /* not sparcv9, an old one */
#   define INT64_CONSTANT(val)  (val##LL)
#   ifndef _SYS_INT_TYPES_H
#       ifndef _UINT32_T
                typedef unsigned long   uint32_t;
#               define _UINT32_T
#       endif
#       ifndef _UINT64_T
                typedef u_longlong_t    uint64_t;
#               define _UINT64_T
#       endif
#   endif /* Exists SYS_TYPES_H */
# endif /* ! sparcv9 */
#endif /* SUN */

/* Linux / Intel */
#if defined (__linux__) &&  defined (__i386__)
# include <stdint.h>
# define MY_BYTE_ORDER MY_LITTLE_ENDIAN
# define INT64_CONSTANT(val)  (val##LL)
#endif /* Linux / Intel */

/* OSF1 / Alpha */
#if defined (__osf__) && defined (__alpha)
# include <inttypes.h>
# define INT64_CONSTANT(val)  (val##L)
# define MY_BYTE_ORDER MY_LITTLE_ENDIAN
#endif /* OSF1 / Alpha */

/* OpenBSD / i386 (32 bits) (From Fred&Co) */
#if defined (__OpenBSD__) &&  defined (__i386__)
# define INT64_CONSTANT(val)  (val##LL)
# define MY_BYTE_ORDER MY_LITTLE_ENDIAN
#endif /* OpenBSD / i386 */


/* Basic bit swapping functions
 */
#define FLOAT_SWAP_LE_BE(val)     ((uint32_t) ( \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#define DOUBLE_SWAP_LE_BE(val)   ((uint64_t) ( \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x00000000000000ffU)) << 56) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x000000000000ff00U)) << 40) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x0000000000ff0000U)) << 24) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x00000000ff000000U)) <<  8) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x000000ff00000000U)) >>  8) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0x00ff000000000000U)) >> 40) |    \
      (((uint64_t) (val) &                                              \
        (uint64_t) INT64_CONSTANT(0xff00000000000000U)) >> 56)))

#if MY_BYTE_ORDER == MY_BIG_ENDIAN
#define FLOAT_TO_BE(pval)   
#define DOUBLE_TO_BE(pval)  
#endif

#if MY_BYTE_ORDER == MY_LITTLE_ENDIAN
#define FLOAT_TO_BE(pval)  { \
         uint32_t val32 = *(uint32_t*)(pval); \
         *(uint32_t*)(pval) = FLOAT_SWAP_LE_BE(val32); \
         }
#define DOUBLE_TO_BE(pval) { \
         uint64_t val64 = *(uint64_t*)(pval); \
         *(uint64_t*)(pval) = FLOAT_SWAP_LE_BE(val64); \
         }
#endif

#endif /* _RES_INTERFACE_H */
