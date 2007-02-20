/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core.h,v 1.30 2007-02-20 14:53:34 deweerdt Exp $

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
Component : BlackBoard

-----------------------------------------------------------------------

Purpose   : BlackBoard Idiom implementation

-----------------------------------------------------------------------
 */

#ifndef _BB_H_
#define _BB_H_

#ifdef __KERNEL__

#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/slab.h>
#include <linux/highmem.h>
#include <linux/types.h>
#include <linux/cdev.h>

#define strncasecmp strnicmp
#define assert(x) WARN_ON(!(x))

#else /* __KERNEL__ */

#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <tsp_abs_types.h>

#endif /* __KERNEL__ */

/**
 * @defgroup BlackBoard BlackBoard (BB)
 * BlackBoard idiom definition and functions.
 * A BlackBoard is an inter-process communication mean based
 * on the publish/subscribe principle. It is implemented using
 * classical IPC objects:
 *    <ul>
 *       <li> 1 shared memory ojbect </li>
 *       <li> 1 message queue (may be more in the future) </li>
 *       <li> 1 semaphore set containing 1 semaphore (may be more in the future) </li>
 *    </ul>
 * The current implementation use SystemV IPC (shmget, semget, msgget) but
 * the BlackBoard may be easily translated to any other similar IPC tools,
 * such as POSIX IPC.
 * Every process/thread attached to the BlackBoard may
 * publish (@ref bb_publish) or subscribe (@ref bb_subscribe) to a piece of data using a key
 * (string based in this implementation).
 * You may have as many BlackBoard as the local memory authorize it.
 * A BlackBoard is identified by a name, the BlackBoard Name and
 * an identifier.
 *
 * The BlackBoard is mainly composed of its shared memory object which is
 * divided in 3 areas:
 * <ul>
 *     <li> The BlackBoard descriptor itself which is represented by 
 *          one @ref S_BB structure</li>
 *     <li> The BlackBoard Data descriptor which is represented by 
 *          an array of @ref S_BB_DATADESC structure</li>
 *     <li> The BlackBoard Data area which is only raw memory where
 *          published data will effectively be stored. </li>
 * </ul>
 */

/**
 * The maximum size of a BB name.
 */
#define BB_NAME_MAX_SIZE    255
/**
 * The maximum size of a BB published data.
 */
#define VARNAME_MAX_SIZE    150

/**
 * The default access right of the shared memory 
 * used to create a BlackBoard.
 */
#define BB_SHM_ACCESS_RIGHT 0770
/**
 * The default access right to the semaphore
 * set used by a BlackBoard.
 */
#define BB_SEM_ACCESS_RIGHT 0770
/**
 * The default access right to the message queue
 * used by a BlackBoard.
 */
#define BB_MSG_ACCESS_RIGHT 0770

/**
 * The maximum BlackBoard message size.
 * This is used for building BlackBoard messages
 * to be sent by BlackBoard message queue.
 */
#define MAX_SYSMSG_SIZE     255
/**
 * The maximum size of the BB message queue(s).
 */
#define MSG_BB_MAX_SIZE     32

/**
 * The BlackBoard version identifier.
 * Since the BlackBoard is evolving, the BlackBoard structure
 * itself may change from time to time.
 * If suche change occurs the BB_VERSION_ID is changed
 * such that @ref bb_check_version may be called in order
 * to check if the BlackBoard version used by an application
 * is compatible with the process currently trying to use 
 * BlackBoard.
 */
#define BB_VERSION_ID       0x0004000

#define BB_OK        0
#define BB_NOK      -1
#define BB_SUCESS    BB_OK
#define BB_FAILURE   BB_NOK
#define BB_YES       1
#define BB_NO        0

/**
 * @defgroup BBLib The BB Core Library
 * The BlackBoard library API.
 * The BlackBoard library has two-level of usage
 * a full-fledge more complicated API which is the API
 * you are currently looking at or a more simple 
 * with less functionnality API called @ref BBSimpleLib. 
 * @ingroup BlackBoard
 * @{
 */

/**
 * BlackBoard Log message level
 * @ingroup BlackBoard
 */
typedef enum BB_LOG_LEVEL {
  BB_LOG_ABORT,
  BB_LOG_SEVERE,
  BB_LOG_WARNING,
  BB_LOG_INFO,
  BB_LOG_CONFIG,
  BB_LOG_FINE,
  BB_LOG_FINER
} BB_LOG_LEVEL_T;


/**
 * BlackBoard publishable data type.
 * Any data published with @ref bb_publish, @ref bb_alias_publish 
 * or @ref bb_simple_publish should be specified with its type.
 */
typedef enum {E_BB_DISCOVER=0, /*!< Discover is used by @ref bb_subscribe when discovering data type */
              E_BB_DOUBLE=1,   /*!< An IEEE double precision floating point  */
	      E_BB_FLOAT,      /*!< An IEEE simple precision floating point  */
	      E_BB_INT8,       /*!< An 8bit signed integer                   */
	      E_BB_INT16,      /*!< A 16bit signed integer                   */
	      E_BB_INT32,      /*!< A 32bit signed integer                   */
	      E_BB_INT64,      /*!< A 64bit signed integer                   */
	      E_BB_UINT8,      /*!< An 8bit unsigned integer                 */
	      E_BB_UINT16,     /*!< A 16bit unsigned integer                 */
	      E_BB_UINT32,     /*!< A 32bit unsigned integer                 */
	      E_BB_UINT64,     /*!< A 64bit unsigned integer                 */
	      E_BB_CHAR,       /*!< An 8bit signed character                 */
	      E_BB_UCHAR,      /*!< An 8bit unsigned character               */
              E_BB_USER        /*!< A user type of any size (should be supplied) in @ref bb_publish */
} E_BB_TYPE_T;

/**
 * BlackBoard status.
 */
typedef enum {BB_STATUS_UNKNOWN=0, /*!< Unknown status     */
	      BB_STATUS_GENUINE,   /*!< Genuine as opposed to a shadow one */
	      BB_STATUS_DIRTY,     /*!< Unused for now */
	      BB_STATUS_DESTROYED, /*!< The BlackBoard has been destroyed by one of the process that were attached to, other should detach as fast as possible */
	      BB_STATUS_SHADOW     /*!< A Shadow BlackBoard created by @ref bb_shadow_get */
} BB_STATUS_T;
	      
/**
 * BlackBoard data descriptor.
 * Each data published in a blackboard is described using
 * one such structure.
 */     	      
typedef struct S_BB_DATADESC {
  /** Variable name (private) use bb_get_varname/bb_set_varname */
  char __name[VARNAME_MAX_SIZE+1];

  /** The Variable type */
  E_BB_TYPE_T type;
  /** 
   * Dimension. 1 if scalar, > 1 for single dimension array.
   * There is no multidimensionnal array type.
   */
  uint32_t dimension;
  /** 
   * Type size (in byte).
   * This size enables the appropriate computation
   * of the data offset in the raw data BlackBoard area.
   */
  size_t type_size;
  /**
   * Data offset (in bytes) in the raw data BlackBoard area.
   */
  unsigned long data_offset;
  
  /**
   * The index of the aliases published (@ref bb_alias_publish)
   * data in the BlackBoard data descriptor array
   * -1 if genuine published data (not an alias).
   */
  int  alias_target;
  
  /**
   * Reserved for future extension of the datadesc
   */
  char __reserved[256];
} S_BB_DATADESC_T ;



struct S_BB;
struct S_BB_MSG;

/**
 * Implementation agnostic operations, for now these
 * functions abstract the following operating modes:
 * - SysV shmem + IPCs
 * - /dev/bb mmap from user space
 * - in-kernel bb_create, bb_publish
 */
struct bb_operations {
	int (*bb_shmem_get)(struct S_BB ** bb, const char *name, int n_data, 
				int data_size, int create);

	int (*bb_shmem_attach)(struct S_BB ** bb, const char *name);
	int (*bb_shmem_detach)(struct S_BB ** bb);
	int (*bb_shmem_destroy)(struct S_BB **bb);

	int (*bb_sem_get)(struct S_BB *bb, int create);
	int (*bb_lock)(volatile struct S_BB *bb);
	int (*bb_unlock)(volatile struct S_BB *bb);
	int (*bb_sem_destroy)(struct S_BB *bb);

	int (*bb_msgq_get)(struct S_BB * bb, int create);
	int (*bb_msgq_send)(volatile struct S_BB *bb, struct S_BB_MSG *);
	int (*bb_msgq_recv)(volatile struct S_BB *bb, struct S_BB_MSG *);
	int (*bb_msgq_isalive)(struct S_BB *bb);
	int (*bb_msgq_destroy)(struct S_BB *bb);
};

/* defined in bb_core_k.h, it's only 
 * used in kernel space */
struct bb_device;
/**
 * In-kernel black board specific data, part of the S_BB_T structure
 */
struct kernel_private {
	/** the size of the allocated black board */
	unsigned long shm_size;
	/** 
	 * The actual pointer to the allocated memory,
	 * this is needed as the BB must be aligned on a
	 * PAGE_SIZE boundary, kmalloc_ptr is the _real_
	 * start of the allocate memory */
	void *kmalloc_ptr;
	/** in-kernel structure, points to the char device */
	struct bb_device *dev;
	/** the index of the char device, used for cleaup
	 * purposes */
	int index;
};

/**
 * SysV specific data, part of the S_BB_T structure
 */
struct sysv_private {
	/* shm file descriptor */
	int fd;
	/** the size of the allocated black board */
	int shm_size;
	/* sysv semaphore handle */
	int sem_id;
	/* sysv message queue handle */
	int msg_id;
};


/**
 * BlackBoard message definition.
 * This type must conform to the constraint of
 * a SysV message queue message used by msgsnd(2).
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
} S_BB_MSG_T ;

/**
 * The black board type, this is tightly coupled to the 
 * struct bb_operations *ops[] defined in bb_core.c
 * it's used as a selector of the right set of operations
 */
enum bb_type {
	BB_SYSV,
	BB_KERNEL,
};
/**
 * BlackBoard description structure.
 * This structure describes the BlackBoard itself
 * (not data published in BlackBoard which are described
 *  by the @ref S_BB_DATADESC_T structure).
 */
typedef struct S_BB {
  /**
   * The BlackBoard version identifier
   * This is used by bb_tools and BB Library API in order
   * to avoid version mismatch between API blackboard access.
   */
  int32_t bb_version_id;
  /** BlackBoard name */
  char name[BB_NAME_MAX_SIZE+1];
  /** 
   * Maximum data descriptor size in bytes.
   * This the maximum number of publishable data
   * is the BlackBoard (independently of their size).
   */
  int32_t  max_data_desc_size;

  /** 
   * Data descriptor array offset.
   * The offset in the shared memory segment
   * where the BlackBoard data descriptor array begins.
   */  
  unsigned long  data_desc_offset; 

  /** 
   * The number of data currently stored (i.e. published) in the BB.
   * This is the used size of the BlackBoard data descriptor array.
   */
  int n_data;

  /** 
   * The maximum data size in BB (bytes). 
   * The sum of the size (in bytes) of all published data
   * may not exceed this number.
   */
  unsigned long max_data_size;

  /** 
   * BlackBoard data zone offset (bytes) 
   * The offset in the shared memory segment
   * where the BlackBoard raw data area array begins.
   */
  unsigned long data_offset;
  /** 
   * Offset (in bytes) of the next free byte 
   * in the blackboard data zone. This offset
   * is relative to @ref data_offset
   */
  unsigned long data_free_offset;
  /**
   * State of a BB.
   * Should be used by processes to detach/re-attach
   * to a destroyed BB and avoid some operation on shadowed BB.
   */
  BB_STATUS_T status;
  /**
   * The type of the BB, used to select the right set of 
   * bb_operations */
  enum bb_type type;
  /**
   * private structure (depends on the shmem implementation)
   */
  union {
	struct sysv_private sysv;
	struct kernel_private k;
  } priv;

} S_BB_T;

typedef struct S_BB_PRIV {
	char varname_lib[96];
	uint8_t reserved[4000];
} S_BB_PRIV_T;


#ifndef __KERNEL__
BEGIN_C_DECLS
#endif

typedef char *(*bb_get_varname_fn)(const S_BB_DATADESC_T *);
typedef void (*bb_set_varname_fn)(S_BB_DATADESC_T *, const char *);
/**
 * Get the name of a variable described by @dd
 * @param[in] dd the descriptor of the variable
 * @return the variable name, must be freed by caller
 */
extern bb_get_varname_fn bb_get_varname;
/**
 * Get the name of a variable described by @dd
 * @param[in] dd the descriptor of the variable
 * @return the variable name, must be freed by caller
 */
extern bb_set_varname_fn bb_set_varname;

#define BB_CTL_SET_NAME_ENCODE_PTR  (1<<1)
#define BB_CTL_GET_NAME_ENCODE_PTR  (1<<2)
#define BB_CTL_SET_NAME_ENCODE_NAME (1<<3)
#define BB_CTL_GET_NAME_ENCODE_NAME (1<<4)

/**
 * General purpose bb control function
 * @param[in] bb a BlackBoard pointer
 * @param[in] request the kind of request (see BB_CTL defines)
 * @return BB_OK is the operation succeeded, BB_NOK otherwise.
 */
int32_t
bb_ctl(S_BB_T *bb, unsigned int request, ...);

/**
 * The size (in byte) of a BlackBoard data type.
 * @param[in] bb_type a BlackBoard data type
 * @return The size (in byte) of a BlackBoard data type or -1 if bb_type equals @ref E_BB_USER
 *         or is unknown.
 */
size_t 
sizeof_bb_type(E_BB_TYPE_T bb_type);

/**
 * Convert a string representing a E_BB_TYPE_T to a BlackBoard data type.
 * @param[in] bb_type_string a string representing a BlackBoard data type
 * @return the BlackBoard data type or -1 if conversion failed.
 */
E_BB_TYPE_T 
bb_type_string2bb_type(const char* bb_type_string);

/**
 * Check if the accessed blackboard is of the same
 * version as the one used by this code.
 * @param[in] bb the BlackBoard to be checked 
 * @return 0 if version is the same, < 0 if current
 *         version is older than the accessed blackboard, 
 *         > 0  for the converse.
 */
int32_t
bb_check_version(volatile S_BB_T* bb);

/**
 * Return the size of a blackboard with 
 * n_data publishable elements with a data zone of size data_size byte.
 * This is the exact size of the SHM segment required
 * to create a BB.
 * @param[in] n_data the number of publishable data in blackboard
 * @param[in] data_size the size (in byte) of the blackboard data zone.
 * @return the size (in byte) of this kind of blackboard.
 */
int32_t
bb_size(const int32_t n_data, const int32_t data_size);

/**
 * Search a variable within a BlackBoard.
 * @param[in] bb BlackBoard pointer
 * @param[in] var_name the name of the searched variable
 * @return index of the variable in the BB descriptor array
 */
int32_t 
bb_find(volatile S_BB_T* bb, const char* var_name);

/*
 * Search a variable within a BlackBoard using fastfind algorithm.
 * @param[in] bb BlackBoard pointer
 * @param[in] fastfind_string the fastfind string build with @ref bb_fastfind_build
 * @param[in] var_name the name of the searched variable
 * @return index of the variable in the BB descriptor array
 */
/* int32_t */
/* bb_fastfind(volatile S_BB_T* bb, const char* fastfind_string, const char* var_name); */

/**
 * Return a pointer to the beginning of the data 
 * descriptor array of the specified BB.
 * @param[in] bb BlackBoard pointer
 * @return start address of the data descriptor array in bb
 */
S_BB_DATADESC_T* 
bb_data_desc(volatile S_BB_T* bb);

/**
 * Return a pointer to the beginning of the data array
 * of the specified BB.
 * @param[in] bb BlackBoard pointer
 * @return start address of the data array in bb
 */
void* 
bb_data(volatile S_BB_T* bb);

#ifndef __KERNEL__
/**
 * Return a double value from the pointer
 * value considered as the bbtype.
 * @param[in] value pointer to the value
 * @param[in] bbtype the type of the pointer
 * @return double value of the pointed value
 */
double
bb_double_of(void *value, E_BB_TYPE_T bbtype);
#endif

/**
 * Initialise a freshly created structure to a default value.
 * If default value pointer is NULL then all type are 
 * initialized to 0 but the E_BB_USER for which
 * the data zone is set to binary 0.
 * @param[in] bb pointer to the blackboard whose the data belongs.
 * @param[in,out] data_desc pointer to the data descriptor structure
 *                (should NOT be NULL).
 * @param[in,out] default_value default pointer to the default value used for init.
 *                         If NULL then initialize to 0.
 * @return E_OK if init OK E_NOK otherwise.
 */
int32_t 
bb_data_initialise(volatile S_BB_T* bb, S_BB_DATADESC_T* data_desc,void* default_value);

/**
 * Write value represented by string to BB published data.
 * This is a mid-level BlackBoard write API which avoid
 * the search complexity used when bb_subscribing to get
 * the data pointer.
 * Nevertheless the function take care of the string to value
 * conversion with proper hexvalue.
 * @param[in] bb the BlackBoard where data is published
 * @param[in] data_desc the BB data descriptor. Be sure to provide
 *                      a properly initialised data_desc since the
 *                      function does not verify this. 
 * @param[in] value the string representing the value to be written to BB data
 * @param[in] idxstack the indexstack (in the alias case)  @ref BBAliasLib.
 *                     it is not read if idxstack_len is 0.
 * @param[in] idxstack_len the indexstack length should be >= 0, if 0 idxstack is ignored
 * @return BB_OK on success BB_NOK otherwise.
 */
int32_t
bb_value_write(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, const char* value, int32_t* idxstack, int32_t idxstack_len);

#ifndef __KERNEL__
int32_t
bb_value_direct_write(void* data, S_BB_DATADESC_T data_desc, const char* value, int hexval);

int32_t
bb_value_direct_rawwrite(void* data, S_BB_DATADESC_T data_desc, void* value);

/**
 * Print the value off a BB published data on a STDIO file stream.
 * @param[in] bb the BlackBoard where data is published
 * @param[in] data_desc the BB data descriptor. Be sure to provide
 *                      a properly initialised data_desc since the
 *                      function does not verify this. 
 * @param[in] pf the STDIO file stream pointer, should be open for writing.
 * @param[in] idxstack the indexstack (in the alias case)  @ref BBAliasLib.
 *                     it is not read if idxstack_len is 0.
 * @param[in] idxstack_len the indexstack length should be >= 0, if 0 idxstack is ignored
 */
int32_t 
bb_string_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf,
		      int32_t* idxstack, int32_t idxstack_len);

/**
 * Print the value off a BB published data on a STDIO file stream.
 * @param[in] bb the BlackBoard where data is published
 * @param[in] data_desc the BB data descriptor. Be sure to provide
 *                      a properly initialised data_desc since the
 *                      function does not verify this. 
 * @param[in] pf the STDIO file stream pointer, should be open for writing.
 * @param[in] idxstack the indexstack (in the alias case)  @ref BBAliasLib.
 *                     it is not read if idxstack_len is 0.
 * @param[in] idxstack_len the indexstack length should be >= 0, if 0 idxstack is ignored
 */
int32_t 
bb_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf,
               int32_t* idxstack, int32_t idxstack_len);



int32_t
bb_data_header_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx, int32_t aliastack);

int32_t
bb_data_footer_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx, int32_t aliastack);



/**
 * Print the content of a data descriptor.
 * @param[in] bb  pointer to BB where the data reside
 * @param[in,out] data_desc pointer to data descriptor.
 * @param[in,out] pf stream file pointer to be used for printing.
 * @param[in] idxstack the index stack 
 * @param[in] idxstack_len  the size of the index stack
 * @return always return E_OK unless pf is NULL.
 */
int32_t 
bb_data_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf,
              int32_t* idxstack, int32_t idxstack_len);

/**
 * Dump a blackboard to a file stream.
 * Blackboard description and all data content is dumped.
 * @param[in,out] bb  pointer to BB.
 * @param[in,out] filedesc  file stream descriptor.
 * @return E_OK if dump succeed E_NOK otherwise.
 */
int32_t 
bb_dump(volatile S_BB_T *bb, FILE* filedesc);

#endif /* __KERNEL__ */

/**
 * Create a blackboard.
 * Create a shared memory segment of the specified size then
 * mmap it for the current process and initialize the zone
 * to contains a fresh new blackboard structure.
 * Only one process should create a blackboard others should
 * attach through @see bb_attach(S_BB_T**, const char*). 
 * @param[in,out] bb  Pointer to a BB pointer.
 *                    IN, non NULL pointer.
 *                    OUT, the pointed element is the new created BB
 *                         or NULL is creation failed.
 * @param[in] pc_bb_name the blackboard name
 * @param[in] n_data maximum data to be stored in blackboard.
 *                  I.e. the number of different published element in BB
 *                 (each element has an associated key)
 * @param[in] data_size the maximum data zone size (in byte) of the blackboard.
 *                     This is the sum of all data published in the blackboard.
 * @return E_OK if creation succeed E_NOK if failed.
 */
int32_t 
bb_create(S_BB_T** bb, 
	  const char* pc_bb_name,
	  int32_t n_data,
	  int32_t data_size);
/**
 * Destroy a blackboard.
 * Destroy the BB  structure. Destroy the shared memory segment
 * and detach from it. Note that effective shared memory segment
 * destruction will occur when the last process attached 
 * is detached (through @see bb_detach for example).
 * @param[in,out] bb Pointer to BB pointer.
 *                 Should not be NULL.
 * @return E_OK on success E_NOK otherwise.
 */
int32_t 
bb_destroy(S_BB_T** bb);

/**
 * Memset the data area of a BB.
 * @param[in,out]  bb  BB pointer, should not be NULL.
 * @param[in]  c  the character used to memset each byte of the 
 *               BlackBoard data area.
 */
int32_t
bb_data_memset(S_BB_T* bb, const char c);

/**
 * Lock blackboard.
 * A BlackBoard should be locked befaore any structural change
 * or to protect different process against each other from
 * concurrent modifications.
 * This is a blocking call (using sys V semaphore).
 * @see bb_publish/ @see bb_subscribe automatically lock the blackboard.
 * @param[in,out] bb BB pointer, should not be NULL.
 * @return E_OK if lock succeed, E_NOK otherwise.
 */
int32_t 
bb_lock(volatile S_BB_T* bb);

/**
 * Unlock blackboard.
 * @param[in,out] bb BB pointer, should not be NULL.
 * @return E_OK if unlock succeed, E_NOK otherwise.
 */
int32_t 
bb_unlock(volatile S_BB_T* bb);

/**
 * Attach to an existing blackboard.=
 * @param[out] bb Pointer to BB pointer (should not be NULL).
 *               the pointed value is updated is BB attach succeed.
 * @param[in] bb_name blackboard name
 * @return  E_OK  if blackboard exists and attach succeed
 *                E_NOK otherwise.
 */
int32_t 
bb_attach(S_BB_T** bb, const char* bb_name);

/**
 * Detach from blackboard.
 * @param[in,out] bb Pointer to BB pointer (should not be NULL)
 * @return E_OK if blackboard exists and detach succeed E_NOK otherwise.
 */
int32_t 
bb_detach(S_BB_T** bb);

/**
 * Publish a data in a blackboard.
 * This request allocate space for the specified data in blackboard
 * and return the address of the newly allocated space.
 * This function has the same semantic as malloc(3).
 * @param[in,out] bb  BB pointer (should not be NULL).
 * @param[in,out] data_desc  Data descriptor of the data to be published.
 *                         OUT, if data has been properly allocated
 *                         the S_BB_DATADESC_T.data_offset is updated.
 * @return address of the allocated data, NULL
 *         if allocation failed.
 */
void* 
bb_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc);

void* 
bb_item_offset(volatile S_BB_T *bb, 
	       S_BB_DATADESC_T* data_desc,
	       const int32_t* indexstack,
	       const int32_t indexstack_len);

/**
 * Subscribe to blackboard data.
 * The function search the data in BB and return its
 * address if found.
 * @param[in] bb pointer to BB.
 * @param[in,out] data_desc data descriptor for the searched data on entry
 *                         updated data desc if found.
 * @return data address if found NULL if not found.
 */
void* 
bb_subscribe(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc);

/**
 * Return the maximum number of data that
 * could be published in blackboard.
 * @param[in] bb pointer to blackboard.
 * @return the maximum number of data that could be published in blackboard
 */
int32_t
bb_get_nb_max_item(volatile S_BB_T *bb);

/**
 * Return the number of data that
 * are currently published in blackboard. 
 * @param[in] bb pointer to blackboard. 
 * @return the number of data currently published in blackboard. 
 */
int32_t
bb_get_nb_item(volatile S_BB_T *bb);

/**
 * Return the memory occupation (in byte) of a blackboard.
 * @param[in] bb pointer to BB.
 * @return memory occupied by the specified BB in bytes
 */
int32_t
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
 * @param[in,out] bb_shadow pointer to pre-allocated data zone
 *                         which will receive the shadow BB.
 * @param[in] bb_src pointer to source blackboard to be shadowed.
 * @return  E_OK on success E_NOK if not.
 */
int32_t 
bb_shadow_get(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Update the data zone of a shadow blackboard.
 * This is essentially a memcpy of the blackboard data zone.
 * @param[in,out] bb_shadow pointer to shadow BB
 * @param[in] bb_src pointer to source BB (the same BB
 *                   initially used for making shadow)
 * @return E_OK on success.
 */
int32_t 
bb_shadow_update_data(S_BB_T *bb_shadow, volatile S_BB_T *bb_src);

/**
 * Return the BB message queue identifier.
 * @param[in,out] bb pointer to BB.
 * @return message queue id 
 */
int32_t 
bb_msg_id(volatile S_BB_T *bb);

/**
 * Send message through BB msg queue.
 * This is a non blocking call, if there is no more
 * room in the message queue, the message is not sent
 * and lost.
 * @param[in,out] bb pointer to BB
 * @param[in,out] msg pointer to message to be sent
 * @return E_OK on success E_NOK otherwise.
 */
int32_t 
bb_snd_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);


/**
 * Receive a message on the BB message queue.
 * This is a blocking call.
 * @param[in,out] bb  pointer to BB.
 * @param[in,out] msg  the message to be read.
 *                  The type of the message to be received should be
 *                  be specified on entry in the message structure
 *                  msg->mtype.
 * @return E_OK on success, E_NOK otherwise
 */
int32_t 
bb_rcv_msg(volatile S_BB_T *bb, S_BB_MSG_T* msg);

/**
 * Build in array_name the name of the array defined
 * by the provided  alias stack and index stack.
 * @param[out] array_name  the string to be used to build array name
 * @param[in] array_name_size_max  the maximum (usable) size for array_name
 * @param[in] aliasstack  the alias stack
 * @param[in] aliasstack_size the size of the alias stack
 * @param[in] indexstack the index stack 
 * @param[in] indexstack_len the lenth of the index stack
 * @return E_OK on success, E_NOK otherwise
 */
int32_t
bb_get_array_name(char * array_name,
		  int array_name_size_max,
		  S_BB_DATADESC_T * aliasstack, int32_t aliasstack_size,
		  int32_t * indexstack, int32_t indexstack_len);
		  

/**
 * Allow the caller to verify if the message queue is
 * still working.
 * @param[in,out] bb pointer to BB.
 * @return E_OK on success, E_NOK otherwise
 */
int32_t bb_msgq_isalive(S_BB_T *bb);

/**
 * Log message for BB Error.
 * This function may be replaced by a project specific function
 * which want to use BB with a unified log system.
 * The default implementation use TSP STRACE facility.
 * @param[in] level the log level.
 * @param[in] modname the module who sent the message.
 * @param[in] fmt format as in printf.
 * @return 0 on success, -1 on error.
 */
int32_t 
bb_logMsg(const BB_LOG_LEVEL_T level, const char* modname, char* fmt, ...);


/** @} */

#ifndef __KERNEL__
END_C_DECLS
#endif

#endif /* _BB_H_ */
