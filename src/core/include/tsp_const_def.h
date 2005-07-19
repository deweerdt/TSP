/*!  \file 
 
$Header: /home/def/zae/tsp/tsp/src/core/include/tsp_const_def.h,v 1.25 2005-07-19 17:37:58 erk Exp $

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

Purpose   : definitions and const that must have a program wide scope
-----------------------------------------------------------------------
 */

#ifndef _TSP_CONST_DEF_H
#define _TSP_CONST_DEF_H


#define TSP_PROJECT_URL "https://savannah.nongnu.org/projects/tsp"

/** TSP Version */
#define TSP_VERSION 1

#include "tsp_abs_types.h"

#define TSP_MAX_SYSMSG_SIZE 256

/**
 * Size of ringbuf  receiver 
 * ex : 1000*100*10 = 10 sec depth for 1000 symbols 100Hz 
 * FIXME : calculate the ringbuf depth using the base frequency
 */
#define TSP_CONSUMER_RINGBUF_SIZE (1000 * 100) * 10



/* Max client total number */
#define TSP_MAX_CLIENT_NUMBER 100

/* Max server total number */
#define TSP_MAX_SERVER_NUMBER 20

#define TSP_MAX_REQUEST_HANDLERS TSP_MAX_SERVER_NUMBER

/** Max period that can be asked by a consumer, for a symbol */
#define TSP_MAX_PERIOD 100000


/*-------- SOCKET --------*/

/**
 * Socket in and out fifo depth for Write and Read
 * operations.
 */
#define TSP_DATA_STREAM_SOCKET_FIFO_SIZE (1024*48)

/** Duration of stream sender fifo in secondes */
#ifdef VXWORKS
#define TSP_STREAM_SENDER_RINGBUF_SIZE 2
#else
#define TSP_STREAM_SENDER_RINGBUF_SIZE 10
#endif


/** used to calculate the socket buffer size */
#define TSP_DATA_STREAM_MAX_HEADER_SIZE 1024
/* till now, all elements are doubles, but there will be strings, raw, etc ... */
#define TSP_DATA_STREAM_MAX_ITEM_SIZE sizeof(double)
#define TSP_DATA_STREAM_MAX_BUFFER_SIZE(max_nb_items) \
             ( (max_nb_items) * TSP_DATA_STREAM_MAX_ITEM_SIZE \
                              + TSP_DATA_STREAM_MAX_HEADER_SIZE)


/** 
 * Size of buffer used to receive the data
 * FIXME : autocalculate this with max group size and TSP_DATA_STREAM_MAX_BUFFER_SIZE,
 * for now, we use a huge value :(
 */
#define TSP_DATA_RECEIVER_BUFFER_SIZE (1024*512)


/*-------- STREAM CONTROL -------*/

/** 
 * Control messages that can come from a provider, included in
 * the data stream. Program level.
 */
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

/** 
 * Control messages that can come from a provider, included in
 * the data stream. TSP protocol level. See TSP_msg_ctrl_t
 */


#define TSP_RESERVED_GROUP_EOF        0xFFFFFFFF

/** FIXME : reserved for futur implementation of async symbols */
#define TSP_RESERVED_GROUP_ASYNCHRONE 0xFFFFFFFE

#define TSP_RESERVED_GROUP_RECONF     0xFFFFFFFD

#define TSP_RESERVED_GROUP_GLU_DATA_LOST     0xFFFFFFFC

#define TSP_RESERVED_GROUP_CONSUMER_DATA_LOST     0xFFFFFFFB

/*-------- MISC --------*/

typedef uint32_t channel_id_t;
typedef uint32_t version_id_t;
typedef uint32_t xdr_and_sync_type_t;

typedef int time_stamp_t;

/** Init values */
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

/**
 * FIXME : XDR types. Will be used when the consumer will be able to
 * chose several symbol types : double, string, raw ...For now
 * only doubles (USER) implemented
 */
#define XDR_DATA_TYPE_RAW  	(1 << (16 + 0))
#define XDR_DATA_TYPE_USER 	(1 << (16 + 1))
#define XDR_DATA_TYPE_STRING 	(1 << (16 + 2))
#define XDR_DATA_TYPE_MASK 	(0xFFFF0000)

/**
 * FIXME : Symbol types. Will be used when the consumer will be able to
 * chose sync symbols and async symbols (not implemented). For now
 * only sync implemented
 */
#define TSP_DATA_TYPE_SYNC  	(1 << 0)
#define TSP_DATA_TYPE_ASYNC 	(1 << 1)
#define TSP_DATA_TYPE_MASK 	(0xFFFF)

/*-------- SERVER INFORMATION --------*/

/* URL format to connect to a provider :
 <protocol:\\host\server_name:server_number> */
#define TSP_URL_FORMAT "%s://%s/%s:%d"

/* Size of the information string for the server */
#define STRING_SIZE_SERVER_INFO 256
typedef char TSP_server_info_string_t[STRING_SIZE_SERVER_INFO+1]; 

/**
 * Server information struct.
 * The informations in this struct are used by a client
 * to get server information
 */
struct TSP_otsp_server_info_t
{
  TSP_server_info_string_t info; 
};

typedef struct  TSP_otsp_server_info_t TSP_otsp_server_info_t;

/*-------- RPC --------*/

#define TSP_RPC_PROTOCOL "rpc"
/**
 * base RPC PROG_ID that will be used 
 * to calculate the PROG ID for each server
 */
#define TSP_RPC_PROGID_BASE_COUNT 0x31230010

/** Time out for client connection to server (secondes) )
 * FIXME : not used yet 
 */ 
#define TSP_RPC_CONNECT_TIMEOUT 60

/*------- SYSTEM -------*/

/** SUSv2 guarantees that `Host names are limited to 255 bytes,
 * but linux define MAXHOSTNAMELEN to be 64.
 */
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

/**
 * some macro used to respect indianity between consumer and producer.
 * FIXME : the default endianity used by the producer should be its
 * endianity so as to be CPU friendly for the producer. At least this
 * should be the default behaviour. Then, a lazy consumer should be able
 * to tell a producer that it wants to receive the data in the its endianity
 * FIXME: Erk: look at Corba CDR-encoding fashion to handle endianity.
 * FIXEM which may be instructive.
 */
#define TSP_ENCODE_DOUBLE_TO_UINT64(val) (TSP_UINT64_TO_BE (*(uint64_t*)val))
#define TSP_DECODE_DOUBLE_TO_UINT64(val) (TSP_UINT64_FROM_BE (*(uint64_t*)val))

#define TSP_ENCODE_INT(val) (TSP_INT_TO_BE (val))
#define TSP_DECODE_INT(val) (TSP_INT_FROM_BE (val)) 

#endif /*_TSP_CONST_DEF_H*/
