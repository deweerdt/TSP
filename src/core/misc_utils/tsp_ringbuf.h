/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/misc_utils/tsp_ringbuf.h,v 1.3 2002-12-20 09:53:17 tntdev Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT
RINGBUF initial implementation by Ivano COLTELLACCI

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : fast Ring Buffer

-----------------------------------------------------------------------
 */

#ifndef __TSP_RINGBUF_H
#define __TSP_RINGBUF_H

#include <stdlib.h>

#include "tsp_prjcfg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
--
-- ring buffer type
--
*/

#ifdef __GNUC__
#define RINGBUF_DYNAMIC_SZ	0
#else
#define RINGBUF_DYNAMIC_SZ
#endif

/* 
 * On definit une taille qui soit un multiple
 * de 4 car certaines CPU alignent les structures
 * sur des multiples de 2 octets (MVME 162) et d'autre 
 * sur des multiples de 4 octets (MVME 26xx)
 */
#define RINGBUF_SZ(sz)		((sz) + 1 + (4-(((sz) + 1) % 4)))


/* For number x, get the nearest superior or equal number, multiple of base */
#define RINGBUF_MULTSUP(x, base)     (( (base) - ( (x) % (base) )) % (base) + (x) )

#define RINGBUF_DECLARE_TYPE(TypeName, ItemType, sz) \
	typedef struct \
	{ \
		int		size; \
		int		put; \
		int		get; \
		int		missed; \
		ItemType	buf[sz]; \
	} TypeName

/*
--
-- static implementation
--
*/

#define RINGBUF_DEFINE(TypeName, name, sz) \
	TypeName	(name) = { sz, 0, 0, 0, 0 }

#define RINGBUF_PUT(name, item) \
	{ \
		int	put; \
		\
		put = ((name).put + 1) % (name).size; \
		\
		if (put != (name).get) \
		{ \
			(name).buf[(name).put] = (item); \
			(name).put = put; \
		} \
		else \
		{ \
			++(name).missed; \
		} \
	}

#define RINGBUF_PUTBYADDR(name) \
	( \
		((((name).put + 1) % (name).size) != (name).get) ? \
			&(name).buf[(name).put] \
		: \
			( \
			++(name).missed, \
			(void*)NULL \
			) \
	)

#define RINGBUF_PUTBYADDR_WITH_SIZE(name, sizePlusOne, indexPut) \
	( \
		( ( ((indexPut)=(name).put) + 1) % sizePlusOne) != (name).get) ? \
			&(name).buf[(indexPut)] \
		: \
			( \
			++(name).missed, \
			(void*)NULL \
			) \
	)

#define RINGBUF_PUTBYADDR_COMMIT(name) \
		(name).put = ((name).put + 1) % (name).size

#define RINGBUF_PUTBYADDR_COMMIT_WITH_SIZE(name, indexPut, sizePlusOne) \
		(name).put = ((indexPut) + 1) % (sizePlusOne) 

#define RINGBUF_GET(name, item) \
	( \
		((name).get != (name).put) ? \
			( \
			(item) = (name).buf[(name).get], \
			(name).get = ((name).get + 1) % (name).size, \
			TRUE \
			) \
		: \
			FALSE \
	)

#define RINGBUF_GETBYADDR(name) \
	( \
		((name).get != (name).put) ? \
			&(name).buf[(name).get] \
		: \
			(void*)NULL \
	)

#define RINGBUF_GETBYADDR_COMMIT(name) \
		(name).get = ((name).get + 1) % (name).size

#define RINGBUF_RESTART_GET(name) \
	{ \
		(name).get = ((name).get + 1) % (name).size); \
	}

/* It is safe because we get until the put or until the end of buffer 
   then make a loop (i=firstItem; i<firstItem+nbItems; i++)
   and call RINGBUF_GETBYADDR_BURST_NEXT */
#define RINGBUF_GETBYADDR_BURST(name, firstItem, nbItems, sizePlusOne) \
	{ \
		int lastItem =   (name).put;\
		firstItem = (name).get; \
		sizePlusOne = (name).size; \
		if (firstItem<=lastItem)\
			nbItems = lastItem-firstItem; \
		else\
			nbItems = (sizePlusOne-firstItem)+lastItem;\
        }

#define RINGBUF_GETBYADDR_BURST_NEXT(name, indexItem, sizePlusOne) \
	     &(name).buf[ (indexItem) % (sizePlusOne)] 
	  
#define RINGBUF_GETBYADDR_COMMIT_BURST(name, firstItem, nbItems, sizePlusOne)\
	  (name).get = ( (firstItem) + (nbItems) )  % (sizePlusOne)

/* This reset is not safe for the producer */
#define RINGBUF_RESET_CONSUMER(name) \
	{ \
		(name).get   = (name).put \
		(name).missed = 0; \
	}

/* This reset is not safe for the consumer */
#define RINGBUF_RESET_PRODUCER(name) \
	{ \
		(name).put   = (name).get \
		(name).missed = 0; \
	}

#define RINGBUF_SIZE(name)		((name).size - 1)
#define RINGBUF_MISSED(name)		((name).missed)
#define RINGBUF_ISEMPTY(name)		((name).get == (name).put)

#define RINGBUF_ITEMS(name) \
	( \
		((name).put >= (name).get) ? \
			(name).put - (name).get \
		: \
			(name).size - (name).get + (name).put \
	)

/*
--
-- dynamic implementation
--
*/


/* 'pad' is there for alignement purpose */
#define RINGBUF_DECLARE_TYPE_DYNAMIC(TypeName, ItemType) \
	typedef struct \
	{ \
		int		size; \
		int		put; \
		int		get; \
		int		missed; \
                int             mul_offset; \
		ItemType*	buf; \
	} TypeName



#define RINGBUF_PTR_INIT(TypeName, name, ItemType, nbSpareBytes, sz) \
	{ \
          int mul_offset =  RINGBUF_MULTSUP((sizeof(ItemType) + nbSpareBytes),sizeof(ItemType)) / sizeof(ItemType);\
	  name = (TypeName*)malloc(sizeof(TypeName) + (sizeof(ItemType) * mul_offset  * sz)); \
	  (name)->size   = sz; \
	  (name)->put   = 0; \
	  (name)->get   = 0; \
	  (name)->missed = 0; \
          (name)->mul_offset = mul_offset; \
          (name)->buf = (ItemType*)((name)+1); \
	}

#define RINGBUF_PTR_DESTROY(name)  \
	{ \
		free((name)); \
		(name) = NULL; \
	}

		
#define RINGBUF_PTR_PUT(name, item) \
	{ \
		int	put; \
		\
		put = ((name)->put + 1) % (name)->size; \
		\
		if (put != (name)->get) \
		{ \
			(name)->buf[(name)->put] = (item); \
			(name)->put = put; \
		} \
		else \
		{ \
			++(name)->missed; \
		} \
	}

#define RINGBUF_PTR_PUTBYADDR(name) \
	( \
		((((name)->put + 1) % (name)->size) != (name)->get) ? \
			&(name)->buf[(name)->put * (name)->mul_offset] \
		: \
			( \
			++(name)->missed, \
			(void*)NULL \
			) \
	)

#define RINGBUF_PTR_PUTBYADDR_WITH_SIZE(name,indexPut,sizePlusOne)\
	( \
		( ( (((indexPut)=(name)->put)+1) % (sizePlusOne) ) != (name)->get) ? \
			&(name)->buf[(indexPut)] \
		: \
			( \
			++(name)->missed, \
			(void*)NULL \
			) \
	)

#define RINGBUF_PTR_PUTBYADDR_COMMIT(name) \
		(name)->put = ((name)->put + 1) % (name)->size 

#define RINGBUF_PTR_PUTBYADDR_COMMIT_WITH_SIZE(name, indexPut, sizePlusOne) \
		(name)->put = ((indexPut) + 1) % (sizePlusOne)

#define RINGBUF_PTR_GET(name,item) \
	( \
		((name)->get != (name)->put) ? \
			( \
			(item) = (name)->buf[(name)->get], \
			(name)->get = ((name)->get + 1) % (name)->size, \
			TRUE \
			) \
		: \
			FALSE \
	)

#define RINGBUF_PTR_NOCHECK_GET(name,item) \
	{ \
		(item) = (name)->buf[(name)->get * (name)->mul_offset]; \
		(name)->get = ((name)->get + 1) % (name)->size; \
	}

#define RINGBUF_PTR_GETBYADDR(name) \
	( \
		((name)->get != (name)->put) ? \
			&(name)->buf[(name)->get * (name)->mul_offset] \
		: \
			(void*)NULL \
	)

#define RINGBUF_PTR_GETBYADDR_COMMIT(name) \
		(name)->get = ((name)->get + 1) % (name)->size

#define RINGBUF_PTR_RESTART_GET(name) \
	{ \
		(name)->get = ((name)->get + 1) % (name)->size); \
	}

/* It is safe because we get until the put or until the end of buffer 
   then make a loop (i=firstItem; i<firstItem+nbItems; i++)
   and call RINGBUF_GETBYADDR_BURST_NEXT */
#define RINGBUF_PTR_GETBYADDR_BURST(name, firstItem, nbItems, sizePlusOne) \
	{ \
		int lastItem =   (name)->put;\
		firstItem = (name)->get; \
		sizePlusOne = (name)->size; \
		if (firstItem<=lastItem)\
			nbItems = lastItem-firstItem; \
		else\
			nbItems = (sizePlusOne-firstItem)+lastItem;\
        }

#define RINGBUF_PTR_GETBYADDR_BURST_NEXT(name, indexItem, sizePlusOne) \
	     &(name)->buf[ (indexItem) % (sizePlusOne)] 
	  
#define RINGBUF_PTR_GETBYADDR_COMMIT_BURST(name, firstItem, nbItems, sizePlusOne)\
	  (name)->get = ( (firstItem) + (nbItems) )  % (sizePlusOne)

/* This reset is not safe for the producer */
#define RINGBUF_PTR_RESET_CONSUMER(name) \
	{ \
		(name)->get   = (name)->put; \
		(name)->missed = 0; \
	}

/* This reset is not safe for the consumer */
#define RINGBUF_PTR_RESET_PRODUCER(name) \
	{ \
		(name)->put   = (name)->get; \
		(name)->missed = 0; \
	}


#define RINGBUF_PTR_SIZE(name)		((name)->size - 1)
#define RINGBUF_PTR_MISSED(name)	((name)->missed)
#define RINGBUF_PTR_ISEMPTY(name)	((name)->get == (name)->put)

#define RINGBUF_PTR_ITEMS(name) \
	( \
		((name)->put > (name)->get) ? \
			(name)->put - (name)->get \
		: \
			(name)->size - (name)->get + (name)->put \
	)
 
#define RINGBUF_PTR_ITEMS_LEFT(name) \
	( \
		((name)->put >= (name)->get) ? \
			(name)->size - (name)->put + (name)->get \
		: \
			(name)->get - (name)->put \
	)


#ifdef __cplusplus
}
#endif

#endif /* _RINGBUF_H */

