/*!  \file 
 
$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_const_def.h,v 1.7 2002-10-01 15:39:51 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 
-----------------------------------------------------------------------
 */

#ifndef _TSP_CONST_DEF_H

#define _TSP_CONST_DEF_H

#define TSP_VERSION 1

/* Size of ringbuf  receiver */
/* FIXME : faire l'allocation en fonction de la frequence de base */
/* 1000*100*10 = 10 sec of buf for 1000 symbols 100Hz */
#define TSP_CONSUMER_RINGBUF_SIZE (1000 * 100) * 3

/* Glue server ringbuf size */
#define GLU_RING_BUFSIZE (1000 * 100 * 3)




/* Max client total number */
#define TSP_MAX_CLIENT_NUMBER 200

/* Max server total number */
#define TSP_MAX_SERVER_NUMBER 20

/* Data Pool period (µS) */
#define TSP_DATAPOOL_POOL_PERIOD 5000

typedef guint32 channel_id_t;
typedef guint32 version_id_t;
typedef guint32 xdr_and_sync_type_t;
typedef guint32 time_stamp_t;

#define UNDEFINED_CHANNEL_ID (-1)
#define UNDEFINED_VERSION_ID (-1)

/*-------- SOCKET --------*/

/* Size of buffer used to create the bite stream
that will be sent thrue the socket */
#define TSP_DATA_STREAM_CREATE_BUFFER_SIZE 1024*48

/* Socket in and out buffer for Write and Read
operations */
#define TSP_DATA_STREAM_SOCKET_BUFFER_SIZE 80*1024

/* PROTOCOL */

#define TSP_RESERVED_GROUPE_EOF        0xFFFFFFFF

#define TSP_RESERVED_GROUPE_ASYNCHRONE 0xFFFFFFFE

/*-------- XDR --------*/

/* Types XDR pouvant etre transferes */

#define XDR_DATA_TYPE_RAW  	(1 << (16 + 0))
#define XDR_DATA_TYPE_USER 	(1 << (16 + 1))
#define XDR_DATA_TYPE_STRING 	(1 << (16 + 2))
#define XDR_DATA_TYPE_MASK 	(0xFFFF0000)

#define TSP_DATA_TYPE_SYNC  	(1 << 0)
#define TSP_DATA_TYPE_ASYNC 	(1 << 1)
#define TSP_DATA_TYPE_MASK 	(0xFFFF)

/*-------- SERVER INFORMATION --------*/

/* Size of the information string for the server */
#define STRING_SIZE_SERVER_INFO 256
typedef char TSP_server_info_string_t[STRING_SIZE_SERVER_INFO+1]; 

/**
 * Server information struct.
 * The informations in this struct are used by a client
 * to choose which server will be left open.
 */
struct TSP_otsp_server_info_t
{
  TSP_server_info_string_t info; 
};

typedef struct  TSP_otsp_server_info_t TSP_otsp_server_info_t;

/*-------- RPC --------*/

/* First RPC PROG_ID that will be used 
to calculate the PROG ID for each server*/
#define TSP_RPC_PROGID_BASE_COUNT 0x31230010

/* Time out for client connection to server (secondes) )*/
#define TSP_RPC_CONNECT_TIMEOUT 20

/*------- SYSTEM -------*/

/* SUSv2 guarantees that `Host names are limited to 255 bytes,
but linux define MAXHOSTNAMELEN to be 64. We'd better be cautious :
we use 255 and we define our own MAXHOSTNAMELEN*/
#define TSP_MAXHOSTNAMELEN 255

/*-------- MACROS --------*/

#define TSP_CHECK_ALLOC(p, ret) \
	{ \
		if ( 0 == p ) \
		{ \
			STRACE_ERROR(("-->OUT : ERROR : Memory allocation failed"))  \
			return ret; \
		} \
	}	
	
#define TSP_CHECK_THREAD(status, ret) \
	{ \
		if ( 0 != status ) \
		{ \
			STRACE_ERROR(("-->OUT : ERROR : Thread API Error"))  \
			return ret; \
		} \
	}	
	
#define TSP_LOCK_MUTEX(mutex, ret) \
	{ \
		if ( 0 != pthread_mutex_lock(mutex) )\
		{ \
			STRACE_ERROR(("-->OUT : ERROR : Mutex Lock Error"))  \
			return ret; \
		} \
	}	

#define TSP_UNLOCK_MUTEX(mutex, ret) \
	{ \
		if ( 0 != pthread_mutex_unlock(mutex) ) \
		{ \
			STRACE_ERROR(("-->OUT : ERROR : Mutex Unlock Error"))  \
			return ret; \
		} \
	}	

/*-------- DATA STREAM ENDIAN CONVERSION --------*/

#define TSP_ENCODE_DOUBLE_TO_GUINT64(val) (GUINT64_TO_LE (*(guint64*)val))
#define TSP_DECODE_DOUBLE_TO_GUINT64(val) (GUINT64_FROM_LE (*(guint64*)val))

#define TSP_ENCODE_INT(val) (GINT_TO_BE (val))
#define TSP_DECODE_INT(val) (GINT_FROM_BE (val)) 

#endif /*_TSP_CONST_DEF_H*/
