/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_utils.c,v 1.3 2004-10-05 22:08:06 erk Exp $

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
    strncpy(retval+strlen(prefix)+strlen(sem_name),sem_name,strlen(postfix));    
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
    strncpy(retval+strlen(prefix)+strlen(msg_name),msg_name,strlen(postfix));    
    retval[i_size_name-1] = '\0';
  }
  
  return retval;
} /* end of bb_utils_build_msg_name */

key_t
bb_utils_ntok(const char* name) {
  key_t s_key;
  uid_t uid;
  int32_t retcode;
  
  SHA1Context sha;
  uint8_t Message_Digest[20];

  uid = getuid();
  /* We use the first byte of a SHA1 hash of the BBname
   * unless the algorithm fail.
   * If SHA1 fail we go back to poor key generation method
   * using the name length.
   * In both case we must Xored the key with uid in order
   * to isolate different user from using the same key
   */
  retcode  = SHA1Reset(&sha);
  retcode &= SHA1Input(&sha, (const unsigned char *) name,strlen(name));
  retcode &= SHA1Result(&sha, Message_Digest);

  /* SHA 1 NOK back to old poor method */
  if (0 != retcode) {
    s_key = ((strlen(name) << 16) & 0xFFFF0000) | (uid & 0x0000FFFF);
  } else {
    s_key = (Message_Digest[0]        |
	     (Message_Digest[1] << 8) |
	     (Message_Digest[2] << 16)|
	     (Message_Digest[3] << 24)) &
      (~uid);
	     
      
  }
  return s_key;
} /* end of bb_utils_ntok */

int32_t 
bb_logMsg(const BB_LOG_LEVEL_T e_level, const char* pc_who, char* pc_fmt, ...) {
  va_list args;
  char    message[2048];

  memset(message,0,2048);
  va_start(args, pc_fmt);
  vsnprintf(message, 2048, pc_fmt, args);
  va_end(args);
  
  fprintf(stderr,"WARNING : %s : %s\n",pc_who, message);
 
  return 0;
}


