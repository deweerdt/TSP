/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_utils.c,v 1.5 2004-11-07 18:23:55 erk Exp $

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
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

#include <sys/ipc.h>

#include <bb_utils.h>
#include <bb_sha1.h>
#include "tsp_simple_trace.h"

char* 
bb_utils_build_shm_name(const char* shm_name) {
  
  char* retval;
  int i_size_name;
  const char * prefix = "/";
    
  i_size_name = strlen(shm_name)+strlen(prefix)+1;
  retval = NULL;
  retval = (char *) malloc(i_size_name*sizeof(char));
  if (retval!=NULL) {
    strncpy(retval,prefix,strlen(prefix));
    strncpy(retval+strlen(prefix),shm_name,strlen(shm_name));
    retval[i_size_name-1] = '\0';
  }
  
  return retval;
} /* end of bb_utils_build_shm_name */

char*
bb_utils_build_sem_name(const char* sem_name) {
  
  char* retval;
  int i_size_name;
  const char * prefix = "/";
  const char * postfix = "_sem";
    
  i_size_name = strlen(sem_name)+strlen(prefix)+strlen(postfix)+1;
  retval = NULL;
  retval = (char *) malloc(i_size_name*sizeof(char));
  if (retval!=NULL) {
    strncpy(retval,prefix,strlen(prefix));
    strncpy(retval+strlen(prefix),sem_name,strlen(sem_name));
    strncpy(retval+strlen(prefix)+strlen(sem_name),postfix,strlen(postfix));    
    retval[i_size_name-1] = '\0';
  }
  
  return retval;
} /* end of bb_utils_build_sem_name */

char*
bb_utils_build_msg_name(const char* msg_name) {
  
  char* retval;
  int i_size_name;
  const char * prefix = "/";
  const char * postfix = "_msg";
    
  i_size_name = strlen(msg_name)+strlen(prefix)+strlen(postfix)+1;
  retval = NULL;
  retval = (char *) malloc(i_size_name*sizeof(char));
  if (retval!=NULL) {
    strncpy(retval,prefix,strlen(prefix));
    strncpy(retval+strlen(prefix),msg_name,strlen(msg_name));
    strncpy(retval+strlen(prefix)+strlen(msg_name),postfix,strlen(postfix));    
    retval[i_size_name-1] = '\0';
  }
  
  return retval;
} /* end of bb_utils_build_msg_name */

key_t
bb_utils_ntok_user(const char* name, int32_t user_specific) {
  key_t s_key;
  int32_t retcode;
  
  SHA1Context sha;
  uint8_t Message_Digest[20];

  /* We use the first byte of a SHA1 hash of the BBname
   * unless the algorithm fail.
   * If SHA1 fail we go back to poor key generation method
   * using the name length.
   * In both case we must Xored the key with user_specific in order
   * to isolate different user from using the same key
   */
  retcode  = SHA1Reset(&sha);
  retcode &= SHA1Input(&sha, (const unsigned char *) name,strlen(name));
  retcode &= SHA1Result(&sha, Message_Digest);

  /* SHA 1 NOK back to old poor method */
  if (0 != retcode) {
    s_key = ((strlen(name) << 16) & 0xFFFF0000) ^ (user_specific & 0x0000FFFF);
  } else {
    s_key = (Message_Digest[0]        |
	     (Message_Digest[1] << 8) |
	     (Message_Digest[2] << 16)|
	     (Message_Digest[3] << 24)) ^
      user_specific;	           
  }
  return s_key;
} /* end of bb_utils_ntok_user */

key_t
bb_utils_ntok(const char* name) {

  return bb_utils_ntok_user(name,getuid());

} /* end of bb_utils_ntok */

int32_t 
bb_logMsg(const BB_LOG_LEVEL_T level, const char* who, char* fmt, ...) {
  va_list args;
  char    message[2048];

  memset(message,0,2048);
  va_start(args, fmt);
  vsnprintf(message, 2048, fmt, args);
  va_end(args);
  
  switch (level) {
    
  case BB_LOG_ABORT:
  case BB_LOG_SEVERE:
    STRACE_ERROR(("%s : %s",who,message));
    break;
  case BB_LOG_WARNING:
    STRACE_WARNING(("%s : %s",who,message));
    break;
  case BB_LOG_INFO:
  case BB_LOG_CONFIG:
    STRACE_INFO(("%s : %s",who,message));
    break;
  case BB_LOG_FINE:
  case BB_LOG_FINER:
    STRACE_DEBUG(("%s : %s",who,message));

  }
  return 0;
}


