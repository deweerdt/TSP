/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_abs_types.h,v 1.11 2004-08-31 09:58:52 dufy Exp $

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
Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : Type abstraction : Stolen from GLIB public headers

-----------------------------------------------------------------------
 */

#ifndef  __TSP_ABS_TYPES_H
#define  __TSP_ABS_TYPES_H 1

#define TSP_LITTLE_ENDIAN 1234
#define TSP_BIG_ENDIAN    4321
#define TSP_PDP_ENDIAN    3412		/* unused, need specific PDP check */	

/* This include should offers all the types like uint16_t and others */
/* Choose to base TSP on the STANDARD POSIX types */
#include <sys/types.h>

/*** Platform specific ***/

/* SUN / sparc (32 bits) */
#if  defined(__sun) && defined(__sparc) && !defined(__sparcv9)
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
# define TSP_GUINT64_FORMAT "llu"
# define TSP_SYSTEM_HAVE_GETHRTIME
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1
#endif /* SUN / sparc */


/* Linux / Intel */
#if defined (__linux__) &&  defined (__i386__) 
# include <stdint.h>
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_GUINT64_FORMAT "llu"
# define TSP_SYSTEM_HAVE_NANOSLEEP 1
# define TSP_HAVE_INT64 1
#endif /* Linux / Intel */


/* OSF1 / Alpha */
#if defined (__osf__) && defined (__alpha)
# include <inttypes.h>
# define TSP_INT64_CONSTANT(val)  (val##L)
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
# define TSP_GUINT64_FORMAT "lu"
# define TSP_SYSTEM_HAVE_NANOSLEEP 1
# define TSP_HAVE_INT64 1
#endif /* OSF1 / Alpha */

/* VXWORKS */
#if  defined (VXWORKS)
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
# define TSP_GUINT64_FORMAT "llu"
# undef TSP_SYSTEM_HAVE_GETHRTIME 
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1
# define TSP_INT64_CONSTANT(val)  (val##LL)
#endif /* VXWORKS */

/*-----------------------------------------------------*/

#ifndef	NULL
#define	NULL	((void*) 0)
#endif

#ifndef	FALSE
#define	FALSE	(0)
#endif

#ifndef	TRUE
#define	TRUE	(!FALSE)
#endif

#undef	ABS
#define ABS(a)	   (((a) < 0) ? -(a) : (a))


/* =================================================
 * Warning : All this macro were stolen from GLIB
 * Thanks to those people who makes so beautiful things
 * But we didn't want to oblige people to link TSP with glib
 * So our choice is POSIX types + renammed macro
 * this header defines TSP_BYTE_ORDER which expands to one of
 * the below macros.
 */

#if TSP_BYTE_ORDER == TSP_BIG_ENDIAN

#define TSP_INT16_TO_BE(val)	((int16_t) (val))
#define TSP_UINT16_TO_BE(val)	((uint16_t) (val))
#define TSP_INT16_TO_LE(val)	((int16_t) TSP_UINT16_SWAP_LE_BE (val))
#define TSP_UINT16_TO_LE(val)	(TSP_UINT16_SWAP_LE_BE (val))
#define TSP_INT32_TO_BE(val)	((int32_t) (val))
#define TSP_UINT32_TO_BE(val)	((uint32_t) (val))
#define TSP_INT32_TO_LE(val)	((int32_t) TSP_UINT32_SWAP_LE_BE (val))
#define TSP_UINT32_TO_LE(val)	(TSP_UINT32_SWAP_LE_BE (val))
#define TSP_INT64_TO_BE(val)	((int64_t) (val))
#define TSP_UINT64_TO_BE(val)	((uint64_t) (val))
#define TSP_INT64_TO_LE(val)	((int64_t) TSP_UINT64_SWAP_LE_BE (val))
#define TSP_UINT64_TO_LE(val)	(TSP_UINT64_SWAP_LE_BE (val))

#define TSP_LONG_TO_LE(val)	((long_t) TSP_INT32_TO_LE (val))
#define TSP_ULONG_T_TO_LE(val)	((ulong_t) TSP_UINT32_TO_LE (val))
#define TSP_LONG_TO_BE(val)	((long_t) TSP_INT32_TO_BE (val))
#define TSP_ULONG_T_TO_BE(val)	((ulong_t) TSP_UINT32_TO_BE (val))

#define TSP_INT_TO_LE(val)	((int) TSP_INT32_TO_LE (val))
#define TSP_UINT_TO_LE(val)	((uint_t) TSP_UINT32_TO_LE (val))
#define TSP_INT_TO_BE(val)	((int) TSP_INT32_TO_BE (val))
#define TSP_UINT_TO_BE(val)	((uint_t) TSP_UINT32_TO_BE (val))

#endif

#if TSP_BYTE_ORDER == TSP_LITTLE_ENDIAN

#define TSP_INT16_TO_LE(val)	((int16_t) (val))
#define TSP_UINT16_TO_LE(val)	((uint16_t) (val))
#define TSP_INT16_TO_BE(val)	((int16_t) TSP_INT16_SWAP_LE_BE (val))
#define TSP_UINT16_TO_BE(val)	(TSP_UINT16_SWAP_LE_BE (val))
#define TSP_INT32_TO_LE(val)	((int32_t) (val))
#define TSP_UINT32_TO_LE(val)	((uint32_t) (val))
#define TSP_INT32_TO_BE(val)	((int32_t) TSP_UINT32_SWAP_LE_BE (val))
#define TSP_UINT32_TO_BE(val)	(TSP_UINT32_SWAP_LE_BE (val))
#define TSP_INT64_TO_LE(val)	((int64_t) (val))
#define TSP_UINT64_TO_LE(val)	((uint64_t) (val))
#define TSP_INT64_TO_BE(val)	((int64_t) TSP_UINT64_SWAP_LE_BE (val))
#define TSP_UINT64_TO_BE(val)	(TSP_UINT64_SWAP_LE_BE (val))

#define TSP_LONG_TO_LE(val)	((long_t) TSP_INT32_TO_LE (val))
#define TSP_ULONG_TO_LE(val)	((ulong_t) TSP_INT32_TO_LE (val))
#define TSP_LONG_TO_BE(val)	((long_t) TSP_INT32_TO_BE (val))
#define TSP_ULONG_TO_BE(val)	((ulong_t) TSP_INT32_TO_BE (val))

#define TSP_INT_TO_LE(val)	((int) TSP_INT32_TO_LE (val))
#define TSP_UINT_TO_LE(val)	((uint_t) TSP_UINT32_TO_LE (val))
#define TSP_INT_TO_BE(val)	((int) TSP_INT32_TO_BE (val))
#define TSP_UINT_TO_BE(val)	((uint_t) TSP_UINT32_TO_BE (val))

#endif

/* Basic bit swapping functions
 */
#define TSP_UINT16_SWAP_LE_BE_CONSTANT(val)	((uint16_t) ( \
    (((uint16_t) (val) & (uint16_t) 0x00ffU) << 8) | \
    (((uint16_t) (val) & (uint16_t) 0xff00U) >> 8)))
#define TSP_UINT32_SWAP_LE_BE_CONSTANT(val)	((uint32_t) ( \
    (((uint32_t) (val) & (uint32_t) 0x000000ffU) << 24) | \
    (((uint32_t) (val) & (uint32_t) 0x0000ff00U) <<  8) | \
    (((uint32_t) (val) & (uint32_t) 0x00ff0000U) >>  8) | \
    (((uint32_t) (val) & (uint32_t) 0xff000000U) >> 24)))

#  define TSP_UINT16_SWAP_LE_BE(val) (TSP_UINT16_SWAP_LE_BE_CONSTANT (val))
#  define TSP_UINT32_SWAP_LE_BE(val) (TSP_UINT32_SWAP_LE_BE_CONSTANT (val))

#ifdef TSP_HAVE_INT64
#  define TSP_UINT64_SWAP_LE_BE_CONSTANT(val)	((uint64_t) ( \
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x00000000000000ffU)) << 56) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x000000000000ff00U)) << 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x0000000000ff0000U)) << 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x00000000ff000000U)) <<  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x000000ff00000000U)) >>  8) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x0000ff0000000000U)) >> 24) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0x00ff000000000000U)) >> 40) |	\
      (((uint64_t) (val) &						\
	(uint64_t) TSP_INT64_CONSTANT(0xff00000000000000U)) >> 56)))
#    define TSP_UINT64_SWAP_LE_BE(val) (TSP_UINT64_SWAP_LE_BE_CONSTANT(val))
#endif

/* The G*_TO_?E() macros are defined in glibconfig.h.
 * The transformation is symmetric, so the FROM just maps to the TO.
 */
#define TSP_INT16_FROM_LE(val)	(TSP_INT16_TO_LE (val))
#define TSP_UINT16_FROM_LE(val)	(TSP_UINT16_TO_LE (val))
#define TSP_INT16_FROM_BE(val)	(TSP_INT16_TO_BE (val))
#define TSP_UINT16_FROM_BE(val)	(TSP_UINT16_TO_BE (val))
#define TSP_INT32_FROM_LE(val)	(TSP_INT32_TO_LE (val))
#define TSP_UINT32_FROM_LE(val)	(TSP_UINT32_TO_LE (val))
#define TSP_INT32_FROM_BE(val)	(TSP_INT32_TO_BE (val))
#define TSP_UINT32_FROM_BE(val)	(TSP_UINT32_TO_BE (val))

#ifdef TSP_HAVE_INT64
#  define TSP_INT64_FROM_LE(val)	(TSP_INT64_TO_LE (val))
#  define TSP_UINT64_FROM_LE(val)	(TSP_UINT64_TO_LE (val))
#  define TSP_INT64_FROM_BE(val)	(TSP_INT64_TO_BE (val))
#  define TSP_UINT64_FROM_BE(val)	(TSP_UINT64_TO_BE (val))
#endif

#define TSP_LONG_FROM_LE(val)	(TSP_LONG_TO_LE (val))
#define TSP_ULONG_FROM_LE(val)	(TSP_ULONG_TO_LE (val))
#define TSP_LONG_FROM_BE(val)	(TSP_LONG_TO_BE (val))
#define TSP_ULONG_FROM_BE(val)	(TSP_ULONG_TO_BE (val))

#define TSP_INT_FROM_LE(val)	(TSP_INT_TO_LE (val))
#define TSP_UINT_FROM_LE(val)	(TSP_UINT_TO_LE (val))
#define TSP_INT_FROM_BE(val)	(TSP_INT_TO_BE (val))
#define TSP_UINT_FROM_BE(val)	(TSP_UINT_TO_BE (val))

#endif /*ABS_TYPES_H*/
