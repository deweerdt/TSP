/*!  \file 
 
$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_const_def.h,v 1.13 2002-12-03 16:14:20 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 
-----------------------------------------------------------------------
 */

#ifndef _TSP_CONST_DEF_H
#define _TSP_CONST_DEF_H


/* TSP Version */
#define TSP_VERSION 1



/* Size of ringbuf  receiver */
/* FIXME : faire l'allocation en fonction de la frequence de base */
/* 1000*100*10 = 10 sec of buf for 1000 symbols 100Hz */
#define TSP_CONSUMER_RINGBUF_SIZE (1000 * 100) * 3

/* Glue server ringbuf size */
#define GLU_RING_BUFSIZE (1000 * 100 * 3)


/* Max client total number */
#define TSP_MAX_CLIENT_NUMBER 100

/* Max server total number */
#define TSP_MAX_SERVER_NUMBER 20

/** Max period that can be asked by a consumer, for a symbol */
#define TSP_MAX_PERIOD 100000


/*-------- SOCKET --------*/

/* Size of socket buffer used to create the bite stream
that will be sent thrue the socket */
#define TSP_DATA_STREAM_CREATE_BUFFER_SIZE 1024*48

/** Socket in and out buffer for Write and Read
operations.i*/
/* FIXME : verifier cette taille par rappor a la taille du groupe max */
/* FIXME : Il faut un ringbuf dont la taille peut etre allouee a la volee*/
#define TSP_DATA_STREAM_SOCKET_BUFFER_SIZE (8*2000)

/** Duration of stream sender fifo in secondes */
#define TSP_STREAM_SENDER_RINGBUF_SIZE 10

/** Size of stream sender fifo item.
 * must be the size of the socket buffer, because this buffer
 * must be copied into this fifo.
 */
#define TSP_STREAM_SENDER_RINBUF_ITEM_SIZE  TSP_DATA_STREAM_SOCKET_BUFFER_SIZE


/*-------- STREAM CONTROL -------*/

enum TSP_msg_ctrl_t
  {

    /** Message control, end of stream */
    TSP_MSG_CTRL_EOF,

    /** Message control, end of stream and list of symbols changed */
    TSP_MSG_CTRL_RECONF,

    /** data were lost for all consumers by the GLU (ex : provider too slow) */
    TSP_MSG_CTRL_GLU_DATA_LOST,

    /** data were lost for a consumer (ex : consumer too slow) */
    TSP_MSG_CTRL_CONSUMER_DATA_LOST

  };

typedef enum TSP_msg_ctrl_t TSP_msg_ctrl_t;

/*-------- TSP PROTOCOL --------*/

#define TSP_RESERVED_GROUP_EOF        0xFFFFFFFF

#define TSP_RESERVED_GROUP_ASYNCHRONE 0xFFFFFFFE

#define TSP_RESERVED_GROUP_RECONF     0xFFFFFFFD

#define TSP_RESERVED_GROUP_GLU_DATA_LOST     0xFFFFFFFC

#define TSP_RESERVED_GROUP_CONSUMER_DATA_LOST     0xFFFFFFFB

/*-------- MISC --------*/

typedef guint32 channel_id_t;
typedef guint32 version_id_t;
typedef guint32 xdr_and_sync_type_t;

/* We do not want to use g types outside,
and time stamp are used in the public consumer api */
typedef int time_stamp_t;

#define UNDEFINED_CHANNEL_ID (-1)
#define UNDEFINED_VERSION_ID (-1)

/*-------- TSP COMMAND LINE ARGS --------*/


/* Provider and consumer side */
#define TSP_ARG_PREFIX            "--tsp-"
#define TSP_ARG_STREAM_INIT_START TSP_ARG_PREFIX"stream-init-start"
#define TSP_ARG_STREAM_INIT_STOP  TSP_ARG_PREFIX"stream-init-stop"

/* Provider side only */
#define TSP_ARG_SERVER_NUMBER     TSP_ARG_PREFIX"server-number"


/* Stuff...*/
#define TSP_ARG_DUMMY_PROG_NAME   "GLU"

#define TSP_ARG_CONSUMER_USAGE   "TSP consumer usage : " \
                                 "[" TSP_ARG_STREAM_INIT_START \
                                 " commands ... " TSP_ARG_STREAM_INIT_STOP "] "

#define TSP_ARG_PROVIDER_USAGE   "TSP provider Usage : " \
                                 "[" TSP_ARG_STREAM_INIT_START \
                                 " commands ... " TSP_ARG_STREAM_INIT_STOP "] " \
                                 " [" TSP_ARG_SERVER_NUMBER " number ]"


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
