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

#define RINGBUF_RESET(name) \
	{ \
		(name).put   = 0; \
		(name).get   = 0; \
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

#define RINGBUF_DECLARE_TYPE_DYNAMIC(TypeName, ItemType) \
	typedef struct \
	{ \
		int		size; \
		int		put; \
		int		get; \
		int		missed; \
		ItemType*	buf; \
	} TypeName

#define RINGBUF_PTR_INIT(TypeName, name, ItemType, sz) \
	{ \
		name         = (TypeName*)malloc(sizeof(TypeName) + (sizeof(ItemType) * sz)); \
		(name)->size   = sz; \
		(name)->put   = 0; \
		(name)->get   = 0; \
		(name)->missed = 0; \
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
			&(name)->buf[(name)->put] \
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
		(item) = (name)->buf[(name)->get]; \
		(name)->get = ((name)->get + 1) % (name)->size; \
	}

#define RINGBUF_PTR_GETBYADDR(name) \
	( \
		((name)->get != (name)->put) ? \
			&(name)->buf[(name)->get] \
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

#define RINGBUF_PTR_RESET(name) \
	{ \
		(name)->put   = 0; \
		(name)->get   = 0; \
		(name)->missed = 0; \
	}

#define RINGBUF_PTR_SIZE(name)		((name)->size - 1)
#define RINGBUF_PTR_MISSED(name)	((name)->missed)
#define RINGBUF_PTR_ISEMPTY(name)	((name)->get == (name)->put)

#define RINGBUF_PTR_ITEMS(name) \
	( \
		((name)->put >= (name)->get) ? \
			(name)->put - (name)->get \
		: \
			(name)->size - (name)->get + (name)->put \
	)

#ifdef __cplusplus
}
#endif

#endif /* _RINGBUF_H */

