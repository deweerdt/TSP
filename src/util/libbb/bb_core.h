/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core.h,v 1.6 2004-11-12 17:29:47 erk Exp $

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
Component : Consumer

-----------------------------------------------------------------------

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */
#ifndef _BB_H_
#define _BB_H_

#include <stdio.h>
#include <sys/sem.h>
#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
#ifdef _SEM_SEMUN_UNDEFINED
#undef _SEM_SEMUN_UNDEFINED
/**
 * Union a definir et utiliser
 * pour les appels 'semop' SysV.
 * @ingroup BlackBoard
 */
union semun {
  int val;                           /* value for SETVAL */
  struct semid_ds *buf;              /* buffer for IPC_STAT & IPC_SET */
  unsigned short int *array;         /* array for GETALL & SETALL */
  struct seminfo *__buf;             /* buffer for IPC_INFO */
};
#endif
#endif


/**
 * @defgroup BlackBoard
 * BlackBoard idiom definition and functions.
 * A blackboard is an inter-process communication mean based
 * on the publish/subscribe principle.
 * Every process/thread attached to the blackboard may
 * publish or subscribe to a piece of data using a key
 * (string based in this implementation).
 * @ingroup TSP_Utils
 */

#define MAX_SYSMSG_SIZE     255

#define BB_NAME_MAX_SIZE    255
#define VARNAME_MAX_SIZE    150
#define MSG_BB_MAX_SIZE     32
#define BB_SHM_ACCESS_RIGHT 0770
#define BB_SEM_ACCESS_RIGHT 0770
#define BB_MSG_ACCESS_RIGHT 0770

#define E_OK    0
#define E_NOK  -1

/**
 * BlackBoard publishable data type.
 * @ingroup BlackBoard
 */
typedef enum {E_BB_DOUBLE=1, 
	      E_BB_FLOAT,
	      E_BB_INT8, 
	      E_BB_INT16, 
	      E_BB_INT32, 
	      E_BB_INT64, 
	      E_BB_UINT8, 
	      E_BB_UINT16, 
	      E_BB_UINT32, 
	      E_BB_UINT64,
	      E_BB_CHAR,
	      E_BB_UCHAR,
              E_BB_USER} E_BB_TYPE_T;
	      
/**
 * BlackBoard data descriptor.
 * @ingroup BlackBoard
 */     	      
typedef struct S_BB_DATADESC {
  /** Variable name */
  char name[VARNAME_MAX_SIZE+1];
  /** Variable (BlackBoard) type */
  E_BB_TYPE_T type;
  /** 
   * Dimension. 1 if scalar, > 1 for single dimension array.
   * There is no multidimensionnal array type.
   */
  int32_t dimension;
  /** 
   * Type size (in byte).
   * This size enables the appropriate computation
   * of the data offset in the data array section.
   */
  size_t type_size;
  /**
   * Data offset (in bytes) in the data array section.
   */
  unsigned long data_offset;
} S_BB_DATADESC_T;

/**
 * BlackBoard message definition.
 * This type must conform to the constraint of
 * a SysV message queue message used by msgsnd(2).
 * @ingroup BlackBoard
 */
typedef struct S_BB_MSG {
  /** 
   * Message type.
   * Should be strictly positive since negative or
   * zero value are used by the system.
   * Collaborative process should agree on the message type
   * they exchange, and what's inside those messages.
   */
  long mtype;
  /** 
   * Message content.
   * This is an opaque buffer.
   */
  unsigned char mtext[MSG_BB_MAX_SIZE];
} S_BB_MSG_T;

/**
 * BlackBoard definition.
 * A blackboard implements the publish/subsbcribe idiom.
 * @ingroup BlackBoard
 */
typedef struct S_BB {
  /** 
   * BB access semaphore. 
   * This SysV sempahore set contains only 1 semaphore
   * which should be taken before any structural BB 
   * modification and/or BB data zone copy.
   * @see bb_shadow_get.
   */
  int semid;
  /**
   * Synchronisation message queue.
   * This is a simple inter-process synchronisation mean.
   * For example a simulation process may send end of cycle
   * message to the queue in order to signal another process
   * that he may shadow the BB for further safe use.
   */
  int msgid;
  /** BlackBoard name */
  char name[BB_NAME_MAX_SIZE+1];
  /** Maximum data descriptor size in bytes. */
  int32_t  max_data_desc_size;
  /** Data descriptor array offset */  
  unsigned long  data_desc_offset; 
  /** 
   * The number of data currently stored in the BB.
   * This is the used size of the data descriptor array.
   */
  int n_data;
  /** The maximum data size in BB. */
  unsigned long max_data_size;
  /** BlackBoard data zone offset (bytes) */
  unsigned long data_offset;
  /** 
   * Offset (octets) of the next free byte 
   * in the blackboard data zone.
   */
  unsigned long data_free_offset;
  /**
   * Destroyed state of a BB.
   * Should be used by processes to detach/re-attach
   * to a destroyed BB.
   */
  int destroyed;
} S_BB_T;


/**
 * Return the size of a blackboard with 
 * n_data publishable elements
 * with a data zone of size data_size byte.
 * @param n_data IN the number of publishable data in blackboard
 * @param data_size IN the size (in byte) of the blackboard data zone.
 * @return the size (in byte) of this kind of blackboard.
 * @ingroup BlackBoard
 */
int32_t
bb_size(const int32_t n_data, const int32_t data_size);

/**
 * Search a variable within a BlackBoard.
 * @param bb IN BlackBoard pointer
 * @param var_name IN the name of the searched variable
 * @return index of the variable in the BB descriptor array
 * @ingroup BlackBoard
 */
int32_t 
bb_find(volatile S_BB_T* bb, const char* var_name);

/**
 * Return a pointer to the beginning of the data 
 * descriptor array of the specified BB.
 * @param bb IN BlackBoard pointer
 * @return start address of the data descriptor array in bb
 * @ingroup BlackBoard
 */
S_BB_DATADESC_T* 
bb_data_desc(volatile S_BB_T* bb);

/**
 * Return a pointer to the beginning of the data array
 * of the specified BB.
 * @param bb IN BlackBoard pointer
 * @return start address of the data array in bb
 * @ingroup BlackBoard
 */
void* 
bb_data(volatile S_BB_T* bb);

/**
 * Return a double value from the pointer
 * value considered as the bbtype.
 * @param value IN pointer to the value
 * @param bbtype IN the type of the pointer
 * @return double value of the pointed value
 * @ingroup BlackBoard
 */
double
bb_double_of(void *value, E_BB_TYPE_T bbtype);

/**
 * Initialise a freshly created structure to a default value.
 * If default value pointer is NULL then all type are 
 * initialized to 0 but the E_BB_USER for which
 * the data zone is set to binary 0.
 * @param bb IN pointer to the blackboard whose the data belongs.
 * @param data_desc INOUT pointer to the data descriptor structure
 *                (should NOT be NULL).
 * @param default_value INOUT default pointer to the default value used for init.
 *                         If NULL then initialize to 0.
 * @return E_OK if init OK E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_data_initialise(volatile S_BB_T* bb, S_BB_DATADESC_T* data_desc,void* default_value);

int32_t
bb_value_write(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc,const char* value, int32_t idx);

int32_t
bb_data_header_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx);

int32_t
bb_data_footer_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx);

int32_t 
bb_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx);

/**
 * Print the content of a data descriptor.
 * @param bb IN pointer to BB where the data reside
 * @param data_desc INOUT pointer to data descriptor.
 * @param pf INOUT stream file pointer to be used for printing.
 * @return always return E_OK unless pf is NULL.
 * @ingroup BlackBoard
 */
int32_t 
bb_data_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf);

/**
 * Create a blackboard.
 * Create a shared memory segment of the specified size then
 * mmap it for the current process and initialize the zone
 * to contains a fresh new blackboard structure.
 * Only one process should create a blackboard others should
 * attach through @see bb_attach(S_BB_T**, const char*). 
 * @param bb INOUT Pointer to a BB pointer.
 *                    IN, non NULL pointer.
 *                    OUT, the pointed element is the new created BB
 *                         or NULL is creation failed.
 * @param pc_bb_name IN, the blackboard name
 * @param n_data IN maximum data to be stored in blackboard.
 *                  I.e. the number of different published element in BB
 *                 (each element has an associated key)
 * @param data_size IN the maximum data zone size (in byte) of the blackboard.
 *                     This is the sum of all data published in the blackboard.
 * @return E_OK if creation succeed E_NOK if failed.
 * @ingroup BlackBoard
 */
int32_t 
bb_create(S_BB_T** bb, 
	  const char* pc_bb_name,
	  int n_data,
	  int data_size);
/**
 * Destroy a blackboard.
 * Destroy the BB  structure. Destroy the shared memory segment
 * and detach from it. Note that effective shared memory segment
 * destruction will occur when the last process attached 
 * is detached (through @see bb_detach for example).
 * @param bb INOUT Pointer to BB pointer.
 *                 Should not be NULL.
 * @return E_OK on success E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_destroy(S_BB_T** bb);

/**
 * Lock blackboard.
 * A BlackBoard should be locked befaore any structural change
 * or to protect different process against each other from
 * concurrent modifications.
 * This is a blocking call (using sys V semaphore).
 * @see bb_publish/ @see bb_subscribe automatically lock the blackboard.
 * @param bb INOUT BB pointer, should not be NULL.
 * @return E_OK if lock succeed, E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_lock(volatile S_BB_T* bb);

/**
 * Unlock blackboard.
 * @param bb INOUT, BB pointer, should not be NULL.
 * @return E_OK if unlock succeed, E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_unlock(volatile S_BB_T* bb);

/**
 * Attach to an existing blackboard.=
 * @param bb OUT, Pointer to BB pointer (should not be NULL).
 *               the pointed value is updated is BB attach succeed.
 * @param bb_name IN, blackboard name
 * @return  E_OK  if blackboard exists and attach succeed
 *                E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_attach(S_BB_T** bb, const char* bb_name);

/**
 * Detach from blackboard.
 * @param bb INOUT, Pointer to BB pointer (should not be NULL)
 * @return E_OK if blackboard exists and detach succeed E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_detach(S_BB_T** bb);

/**
 * Publish a data in a blackboard.
 * This request allocate space for the specified data in blackboard
 * and return the address of the newly allocated space.
 * This function has the same semantic as malloc(3).
 * @param bb INOUT, BB pointer (should not be NULL).
 * @param data_desc INOUT, Data descriptor of the data to be published.
 *                         OUT, if data has been properly allocated
 *                         the S_BB_DATADESC_T.data_offset is updated.
 * @return address of the allocated data, NULL
 *         if allocation failed.
 * @ingroup BlackBoard
 */
void* 
bb_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc);

/**
 * Subscribe to blackboard data.
 * The function search the data in BB and return its
 * address if found.
 * @param bb IN, pointer to BB.
 * @param data_desc INOUT, data descriptor for the searched data on entry
 *                         updated data desc if found.
 * @return data address if found NULL if not found.
 * @ingroup BlackBoard
 */
void* 
bb_subscribe(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc);

/**
 * Dump a blackboard to a file stream.
 * Blackboard description and all data content is dumped.
 * @param bb INOUT, pointer to BB.
 * @param filedesc INOUT, file stream descriptor.
 * @return E_OK if dump succeed E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_dump(volatile S_BB_T *bb, FILE* filedesc);

/**
 * Return the maximum number of data that
 * could be published in blackboard.
 * @param bb IN, pointer to blackboard.
 * @return the maximum number of data that could be published in blackboard
 * @ingroup BlackBoard
 */
int
bb_get_nb_max_item(volatile S_BB_T *bb);

/**
 * Return the number of data that
 * are currently published in blackboard. 
 * @param bb IN, pointer to blackboard. 
 * @return the number of data currently published in blackboard. 
 * @ingroup BlackBoard
 */
int
bb_get_nb_item(volatile S_BB_T *bb);

/**
 * Return the memory occupation (in byte) of a blackboard.
 * @param bb IN, pointer to BB.
 * @return memory occupied by the specified BB in bytes
 * @ingroup BlackBoard
 */
int
bb_get_mem_size(volatile S_BB_T *bb);

/**
 * Ask for a shadow blackboard creation.
 * A shadow blackboard is a blackboard which does not
 * reside in shared memory is meant to be in local memory.
 * This BB is not meant to be shared between several process
 * or thread.
 * The only valid operation on a shadow BB are:
 *      - bb_shadow_get, 
 *      - bb_shadow_update_data
 *      - direct access to BB data
 * This kind of BB is used for flip/flop BB distribution
 * with a blackboard TSP provider.
 * @param bb_shadow INOUT, pointer to pre-allocated data zone
 *                         which will receive the shadow BB.
 * @param bb_src IN, pointer to source blackboard to be shadowed.
 * @return  E_OK on success E_NOK if not.
 * @ingroup BlackBoard
 */
int32_t 
bb_shadow_get(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Update the data zone of a shadow blackboard.
 * This is essentially a memcpy of the blackboard data zone.
 * @param bb_shadow INOUT, pointer to shadow BB
 * @param bb_src IN, pointer to source BB (the same BB
 *                   initially used for making shadow)
 * @return E_OK on success.
 * @ingroup BlackBoard
 */
int32_t 
bb_shadow_update_data(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Return the BB message queue identifier.
 * @param bb INOUT, pointer to BB.
 * @return message queue id 
 * @ingroup BlackBoard
 */
int32_t 
bb_msg_id(volatile S_BB_T *bb);

/**
 * Send message through BB msg queue.
 * This is a non blocking call, if there is no more
 * room in the message queue, the message is not sent
 * and lost.
 * @param bb INOUT, pointer to BB
 * @param msg INOUT, pointer to message to be sent
 * @return E_OK on success E_NOK otherwise.
 * @ingroup BlackBoard
 */
int32_t 
bb_snd_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);


/**
 * Receive a message on the BB message queue.
 * This is a blocking call.
 * @param bb INOUT, pointer to BB.
 * @param msg INOUT, the message to be read.
 *                  The type of the message to be received should be
 *                  be specified on entry in the message structure
 *                  msg->mtype.
 * @return E_OK on success, E_NOK otherwise
 * @ingroup BlackBoard
 */
int32_t 
bb_rcv_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);
#endif /* _BB_H_ */
