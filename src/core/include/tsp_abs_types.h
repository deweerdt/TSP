/*

$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_abs_types.h,v 1.32 2008-06-25 11:12:27 erk Exp $

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

/* excerpt from libtool manual */
/* BEGIN_C_DECLS should be used at the beginning of your declarations,
   so that C++ compilers don't mangle their names.  Use END_C_DECLS at
   the end of C declarations. */
#undef BEGIN_C_DECLS
#undef END_C_DECLS
#ifdef __cplusplus
#  define BEGIN_C_DECLS extern "C" {
#  define END_C_DECLS }
#else
# define BEGIN_C_DECLS /* empty */
# define END_C_DECLS /* empty */
#endif

#define TSP_LITTLE_ENDIAN 1234
#define TSP_BIG_ENDIAN    4321
#define TSP_PDP_ENDIAN    3412		/* unused, need specific PDP check */	

/* This include should offers all the types like uint16_t and others */
/* Choose to base TSP on the STANDARD POSIX types */
#include <sys/types.h>

/*** Platform specific ***/

/* SUN / sparc|i386 (32/64 bits) */
#if  defined(__sun)
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1
# if  defined(__sparcv9)
#   define TSP_INT64_CONSTANT(val)  (val##L)
#   define TSP_UINT64_FORMAT "lu"
#   define TSP_XINT64_FORMAT "lx"
#   define TSP_INT64_FORMAT "l"
# else /* not sparcv9, an old one */
#   define TSP_INT64_CONSTANT(val)  (val##LL)
#   define TSP_UINT64_FORMAT "llu"
#   define TSP_XINT64_FORMAT "llx"
#   define TSP_INT64_FORMAT "ll"
    /* Old sparc lacks from simple types, need to do it by our own */
#   ifndef _SYS_INT_TYPES_H 
#	ifndef _UINT16_T
		typedef unsigned short   uint16_t;
#		define _UINT16_T
#	endif
#	ifndef _INT16_T
		typedef short   int16_t;
#		define _INT16_T
#	endif
#	ifndef _UINT32_T
		typedef unsigned long   uint32_t;
#		define _UINT32_T
#	endif
#	ifndef _INT32_T
		typedef long   int32_t;
#		define _INT32_T
#	endif
#	ifndef _UINT64_T
	    typedef u_longlong_t        uint64_t;
#		define _UINT64_T
#	endif
#	ifndef _INT64_T
	    typedef longlong_t    int64_t;
#		define _INT64_T
#	endif
#   endif /* Exists SYS_TYPES_H */ 
# endif /* ! sparcv9 */
#endif /* SUN */

/* Windows */
#if defined (WIN32)
#include <stddef.h>
/* with UWin */
#if defined (_UWIN)

#include <stdint.h>

#else /* FIXME */

/* types for compat. with various BSD and other library sources */
typedef signed char      int8_t;
typedef unsigned char  u_int8_t;
typedef short            int16_t;
typedef unsigned short u_int16_t;
typedef int              int32_t;
typedef unsigned int   u_int32_t;
/* types for compat. with Solaris 7 */
typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;
/*
 * 64bit type for BSD compatability
 */
#ifdef __GNUC__
typedef long long int 		  quad_t;
typedef unsigned long long int	u_quad_t;
typedef long long int             int64_t;
typedef unsigned long long int  u_int64_t;
#elif _MSC_VER
typedef __int64 		  quad_t;
typedef unsigned __int64 	u_quad_t;
typedef __int64 		  int64_t;
typedef unsigned __int64 	u_int64_t;
#endif /*__GNUC__*/

#ifndef _UINT64_T
    typedef unsigned long long  uint64_t;
#   define _UINT64_T
#endif
#ifndef _INT64_T
    typedef long long  int64_t;
#	define _INT64_T
#endif

#endif /* with UWin */
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_HAVE_INT64 1
#endif /* Windows with UWin */


/* Linux / Intel */
#if defined (__linux__) &&  ( defined (__i386__)  || defined (__x86_64__) )
# include <stdint.h>
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
#if defined (__x86_64__)
# define TSP_INT64_CONSTANT(val)  (val##L)
# define TSP_UINT64_FORMAT "lu"
# define TSP_XINT64_FORMAT "lx"
# define TSP_INT64_FORMAT "l"
#else
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
#endif
# define TSP_SYSTEM_HAVE_NANOSLEEP 1
# define TSP_HAVE_INT64 1
#endif /* Linux / Intel */


/* Linux / Powerpc and Arm */ 							
#if defined (__linux__) &&  (defined (__powerpc__) || defined (__arm__))
# include <stdint.h>							
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN			
# define TSP_INT64_CONSTANT(val)  (val##LL) 	
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
# define TSP_SYSTEM_HAVE_NANOSLEEP 1			
# define TSP_HAVE_INT64 1						
#endif /* Linux / Powerpc and Arm */


/* OSF1 / Alpha */
#if defined (__osf__) && defined (__alpha)
# include <inttypes.h>
typedef int_least16_t int16_t;
# define TSP_INT64_CONSTANT(val)  (val##L)
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
# define TSP_UINT64_FORMAT "lu"
# define TSP_XINT64_FORMAT "lx"
# define TSP_INT64_FORMAT "l"
# define TSP_SYSTEM_HAVE_NANOSLEEP 1
# define TSP_HAVE_INT64 1
#endif /* OSF1 / Alpha */

/* VXWORKS */
#if  defined (VXWORKS)
/* this works for VxWorks version 5.5.1 and WIND version 2.6 */
# include <inttypes.h>
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
# undef TSP_SYSTEM_HAVE_GETHRTIME 
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1
# define TSP_INT64_CONSTANT(val)  (val##LL)
#endif /* VXWORKS */

/* FreeBSD : may need testing on non i386 */
#if defined (__FreeBSD__)
# define TSP_INT64_CONSTANT(val)  (val##LL)
#if defined (__sparcv9)
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
#else
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
#endif /* Endianness */
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1

#endif /* FreeBSD */

/* OpenBSD / i386 (32 bits) (From Fred&Co) */
#if defined (__OpenBSD__) &&  defined (__i386__)
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
# define TSP_SYSTEM_HAVE_NANOSLEEP
# define TSP_HAVE_INT64 1

/* Standard C types not defined in OBSD at the moment */
typedef	int8_t	int_least8_t;
typedef	int16_t	int_least16_t;
typedef	int32_t	int_least32_t;
typedef	int64_t	int_least64_t; 

#endif /* OpenBSD / i386 */


/* RTEMS */
#if defined (__rtems__)
#ifdef _POSIX_SOURCE
#undef _POSIX_SOURCE
#endif
# include <sys/types.h>		 /* for types compatibility */
# include <stdint.h>

#if defined (__i386__)
# define TSP_BYTE_ORDER TSP_LITTLE_ENDIAN
#elif defined (sparc)
# define TSP_BYTE_ORDER TSP_BIG_ENDIAN
#else
# error Unknown hardware for rtems, please edit src/core/include/tsp_abs_types.h
#endif	/* Endianness */

# define TSP_SYSTEM_HAVE_NANOSLEEP
//# define TSP_SYSTEM_HAVE_THREADSAFE_USLEEP
# define TSP_HAVE_INT64 1
# define TSP_INT64_CONSTANT(val)  (val##LL)
# define TSP_UINT64_FORMAT "llu"
# define TSP_XINT64_FORMAT "llx"
# define TSP_INT64_FORMAT "ll"
#endif /* __rtems__ */



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
#define TSP_INT16_TO_BE(val)	((int16_t) TSP_UINT16_SWAP_LE_BE (val))
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
