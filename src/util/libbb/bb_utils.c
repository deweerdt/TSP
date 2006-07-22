/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_utils.c,v 1.12 2006-07-22 16:58:39 deweerdt Exp $

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

#include <tsp_sys_headers.h>
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
bb_utils_convert_string2hexbuf(const char* string, unsigned char* buf, size_t buflen, int hexval) {

  int32_t retval        = 0;
  int32_t lenstr        = strlen(string);
  int32_t remain        = lenstr; 
  const char* current   = string;
  int32_t  bufidx       = 0;
  uint8_t  utmp8;
  char toconvert[3];
  
  /* terminate string */
  toconvert[2] = '\0';

  while ((remain>0) && (bufidx<buflen)) {
    memcpy(toconvert,current,2);
    current += 2;
    utmp8 = strtol(toconvert,(char **)NULL,hexval ? 16 : 10);
    memcpy(buf+bufidx,&utmp8,1);
    remain  -= 2;
    bufidx  += 1;
  }    
  return retval;
}

int32_t 
bb_utils_parseone_array(const char* provided_symname, 
			char* symname_part,
			int32_t symname_part_maxlen,
			int32_t* array_index,
			char** remaining_symname, 
			int32_t remaining_symname_len) {
										 
  char*    array_name;
  char*    symname;
  int32_t  retcode = 0;
  int32_t  symlen = 0;
  assert(provided_symname);

  symname = strdup(provided_symname);
  symlen  = strlen(symname);

  array_name = strstr(symname,"[");
  if (array_name) {
    char* temp = "%d";
    char* temp2;
    temp2  = strdup(symname);
    array_name  = strtok(temp2,"[");
    strncpy(symname_part,array_name,symname_part_maxlen);
    array_name = strtok(NULL,"]");
    if (sscanf(array_name,temp,array_index)<1) {
      retcode = -1;
    }
	 
	 array_name = strtok(NULL,"\0");
	 if (NULL != array_name) {
	 	strncpy(*remaining_symname,array_name,remaining_symname_len);
	 } else {
	 	*remaining_symname=NULL;
	 }
	 
    free(temp2);
  } else {
    *array_index = -1;
    strncpy(symname_part,provided_symname,symname_part_maxlen);
	 *remaining_symname=NULL;
  }
  free(symname); 
  return retcode;
  
} /* end of bb_utils_parseone_array */

int32_t
bb_utils_parsearrayname(const char*    provided_symname, 
			char*          parsed_symname,
			const int32_t  parsed_symname_maxlen,
			int32_t*       array_index, 
			int32_t*       array_index_len) {

  char current_symname[parsed_symname_maxlen];
  char remaining_symname[parsed_symname_maxlen];
  char symname_part[parsed_symname_maxlen];
  char* remain;
  int i = 0;
  int symname_current_index = 0;
  int32_t retcode = 0;

  /* RAZ the array index */
  memset(array_index,0,(*array_index_len)*sizeof(int32_t));
  *array_index_len=0;

  strncpy(current_symname,provided_symname,parsed_symname_maxlen-1);
  strncpy(remaining_symname,provided_symname,parsed_symname_maxlen-1);
  remain = remaining_symname;
  
  while ((NULL != remain) && (0 == retcode)) {
    retcode &= bb_utils_parseone_array(current_symname,
				      symname_part,parsed_symname_maxlen,
				      &array_index[i],
				      &remain,parsed_symname_maxlen);
    if (0 == retcode) {
      strncat(&(parsed_symname[symname_current_index]),symname_part,
	      parsed_symname_maxlen-symname_current_index);
      symname_current_index += strlen(symname_part);
      if (NULL != remain) {
	strncpy(current_symname,remain,parsed_symname_maxlen);
      }
      if (-1 != array_index[i])  {
	*array_index_len += 1;
      }
      i++;
    }
  }
  
  return retcode;	
} /* end bb_utils_parsearrayname */
