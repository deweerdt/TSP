/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_utils.h,v 1.14 2008-03-24 23:56:21 deweerdt Exp $

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

Purpose   : Blackboard Idiom utilities

-----------------------------------------------------------------------
 */
#ifndef _BB_UTILS_H_
#define _BB_UTILS_H_

#ifdef __KERNEL__
#include <linux/types.h>
#else
#if !defined(__rtems__)
# include <sys/ipc.h>
#endif /* !__rtems__ */

#include <sys/types.h>

#include <tsp_abs_types.h>
#endif /* __KERNEL__ */

/**
 * @defgroup BBUtilsLib The BB Utilities Library
 * @ingroup BlackBoard
 * General utility functions for TSP BlackBoard.
 */

#ifndef __KERNEL__
BEGIN_C_DECLS
#endif

/**
 * @addtogroup BBUtilsLib
 * @{
 */

/**
 * Build the string name used for creating a shm segment
 * from a user supplied name.
 * We use this for easy SHM name 'normalization' for easy
 * shm portability. One may use the name with either 
 * POSIX shm_open(3) or sys V shmget(2) through 
 * @see bb_utils_ntok or @see bb_utils_ntok_user.
 * @param shm_name IN, user supplied name.
 * @return the name to be used by shm creator. 
 *         The string is allocated (as strdup(3) do) so that
 *         caller should free it.
 */
char* 
bb_utils_build_shm_name(const char* shm_name);

/** 
 * Build the string name used for creating a semaphore
 * from a user supplied name.
 * We use this for easy SEM name 'normalization' for easy
 * semaphore portability. One may use the name with either 
 * POSIX sem_open(3) or sys V semget(2) through 
 * @see bb_utils_ntok or @see bb_utils_ntok_user.
 * @param sem_name IN, user supplied name.
 * @return the name to be used by semaphore creator. 
 *         The string is allocated (as strdup(3) do) so that
 *         caller should free it.
 */
char*
bb_utils_build_sem_name(const char* sem_name);

/**
 * Build the string name used for creating a message queue
 * from a user supplied name.
 * We use this for easy MSG name 'normalization' for easy
 * message queue portability. One may use the name with either 
 * POSIX mq_open(3) or sys V msgget(2) through 
 * @see bb_utils_ntok or @see bb_utils_ntok_user.
 * @param msg_name IN, user supplied name.
 * @return the name to be used by message queue creator. 
 *         The string is allocated (as strdup(3) do) so that
 *         caller should free it.
 */
char*
bb_utils_build_msg_name(const char* msg_name);

/**
 * Build a SysV IPC key from a name and user specific value.
 * The purpose of this function is to build a (quasi) unique
 * key from unique entry as ftok(3) do with existing
 * file name.
 * We use SHA1 hash function Xored with the user_specific
 * value supplied.
 * @param name IN, the name representing the IPC element for which
 *                 we want a key.
 * @param user_specific_value IN, any user specific value 
 *                               (for example uid).
 * @return The generated SysV IPC key corresponding to the specified entry
 */

key_t
bb_utils_ntok_user(const char* name, int32_t user_specific_value);

/**
 * Build a SysV IPC key from a name.
 * L'objectif de cette fonction est de générer des
 * clefs différentes pour des noms différents, à la manière
 * d'une fonction de hachage ou checksum parfaite.
 * Cette fonction vise les même objectifs que ftok(3)
 * avec les noms de fichiers mais avec une chaine
 * de caractere quelconque.
 * @param name un nom représentant l'élément IPC pour lequel on
 *                veut générer une clef.
 * @return SysV IPC key corresponding to the specified name.
 */
key_t
bb_utils_ntok(const char* name);


/**
 * Write the value represented as string in a buffer
 * of unsigned char. The value is understood as
 * decimal of hexadecimal value representation
 * @param string IN, the value to be written to buf
 * @param buf OUT, the buffer to be written to
 * @param buflen IN, the buffer length
 * @param hexval IN, 0 if string is to be interpreted as decimal value
 *                   1 if string is to be interpreted as hexadecimal value
 * @return 0 if success !=0 otherwise
 */
int32_t
bb_utils_convert_string2hexbuf(const char* string, unsigned char* buf, size_t buflen, int hexval);


int32_t 
bb_utils_parseone_array(const char* provided_symname, 
			char* symname_part, int32_t symname_part_maxlen,
			int32_t* array_index,
			char** remaining_symname, int32_t remaining_symname_len);

int32_t
bb_utils_parsearrayname(const char* provided_symname, 
			char* parsed_symname,
			const int32_t parsed_symname_maxlen,
			int32_t* array_index, 
			int32_t* array_index_len);

/** @} */

#ifndef __KERNEL__
END_C_DECLS
#endif

#endif /* _BB_UTILS_H_ */
