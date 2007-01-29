/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_core.c,v 1.28 2007-01-29 17:04:16 erk Exp $

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

Purpose   : Blackboard Idiom implementation

-----------------------------------------------------------------------
 */

#ifndef __KERNEL__
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>

#include <tsp_abs_types.h>
#include <tsp_abs_types.h>
#include <tsp_sys_headers.h>
#include <tsp_simple_trace.h>

#endif /* __KERNEL__ */

#include "bb_alias.h"
#include "bb_utils.h"
#include "bb_core.h"

/**
 * Convert type to string for display use.
 */
#ifndef __KERNEL__
static const char* E_BB_2STRING[] = {"DiscoverType",
				     "double", 
				     "float", 
				     "int8_t",
				     "int16_t",
				     "int32_t",
				     "int64_t",
				     "uint8_t",
				     "uint16_t",
				     "uint32_t",
				     "uint64_t",
				     "char",
				     "uchar",
				     "UserType",
				     "NotAType end"};
#endif

static const size_t E_BB_TYPE_SIZE[] = {0,
			#ifndef __KERNEL__
					sizeof(double), 
					sizeof(float), 
                        #else 
                                        0,
                                        0,
			#endif
					sizeof(int8_t),
					sizeof(int16_t),
					sizeof(int32_t),
					sizeof(int64_t),
					sizeof(uint8_t),
					sizeof(uint16_t),
					sizeof(uint32_t),
					sizeof(uint64_t),
					sizeof(char),
					sizeof(unsigned char),
					0,
					0};

#ifdef __KERNEL__
/* In case we're compiling kernel code, there's really no need
   for the sysv code */
static struct bb_operations sysv_bb_ops;
extern struct bb_operations k_bb_ops;
#else
extern struct bb_operations sysv_bb_ops;
#if defined(linux) || defined(__linux)
extern struct bb_operations k_bb_ops;
#endif
#endif

/* Note: This is thigtly related to enum bb_type in bb_core.h,
   if you ever modify this, you'll probably need to modify
   enum bb_type */
static struct bb_operations *ops[] = { 	&sysv_bb_ops
#if defined(linux) || defined(__linux)
					,&k_bb_ops };
#else
                                     };
#endif

static enum bb_type bb_type(const char *name)
{
#ifdef __KERNEL__
	return BB_KERNEL;
#else
	if (!strncmp(name, "/dev/", 5))
		return BB_KERNEL;
	
	return BB_SYSV;
#endif /* __KERNEL__ */
}

size_t 
sizeof_bb_type(E_BB_TYPE_T bb_type) {
  size_t retval = 0;
  if ((bb_type>=1) && (bb_type<E_BB_USER)) {
    retval = E_BB_TYPE_SIZE[bb_type];
  }
  return retval;
}

E_BB_TYPE_T 
bb_type_string2bb_type(const char* bb_type_string) {
  E_BB_TYPE_T retval = 0;
#ifndef __KERNEL__
  if (!strncasecmp("double",bb_type_string,strlen("double"))) {
    retval = E_BB_DOUBLE;
  } else
  if (!strncasecmp("float",bb_type_string,strlen("float"))) {
    retval = E_BB_FLOAT;
  } else
#endif
  if (!strncasecmp("int8",bb_type_string,strlen("int8"))) {
    retval = E_BB_INT8;
  } else
  if (!strncasecmp("int16",bb_type_string,strlen("int16"))) {
    retval = E_BB_INT16;
  } else
  if (!strncasecmp("int32",bb_type_string,strlen("int32"))) {
    retval = E_BB_INT32;
  } else
  if (!strncasecmp("int64",bb_type_string,strlen("int64"))) {
    retval = E_BB_INT64;
  } else
  if (!strncasecmp("uint8",bb_type_string,strlen("uint8"))) {
    retval = E_BB_UINT8;
  } else
  if (!strncasecmp("uint16",bb_type_string,strlen("uint16"))) {
    retval = E_BB_UINT16;
  } else
  if (!strncasecmp("uint32",bb_type_string,strlen("uint32"))) {
    retval = E_BB_UINT32;
  } else
  if (!strncasecmp("uint64",bb_type_string,strlen("uint64"))) {
    retval = E_BB_UINT64;
  } else
  if (!strncasecmp("char",bb_type_string,strlen("char"))) {
    retval = E_BB_CHAR;
  } else
  if (!strncasecmp("uchar",bb_type_string,strlen("uchar"))) {
    retval = E_BB_UCHAR;
  }
  return retval;
}

int32_t
bb_check_version(volatile S_BB_T* bb) {
  int32_t retval = 0;
/*   union { */
/*     uint16_t lower; */
/*     uint16_t upper; */
/*   } version_using, version_access; */

  assert(bb);
  retval = BB_VERSION_ID - bb->bb_version_id;
/*   if (retval != 0) { */
/*       bb_logMsg(BB_LOG_WARNING, "BlackBoard::bb_check_version", */
/* 		"BB version mismatch using <0x%08X> for accessing <0x%08X>\n", */
/* 		BB_VERSION_ID,bb->bb_version_id); */
/*   } */
  return retval;
} /* end of bb_check_version */

int32_t 
bb_size(const int32_t n_data, const int32_t data_size) {
  /* The SHM segment is sized to:
   *  BB structure size +
   *  data descriptor array size +
   *  data zone size
   */
  return (sizeof(S_BB_T) + 
    sizeof(S_BB_DATADESC_T)*n_data +
    sizeof(char)*data_size);
} /* end of bb_size */

int32_t 
bb_find(volatile S_BB_T* bb, const char* var_name) {
  
  int32_t retval;  
  int32_t i;
    
  retval = -1;
  assert(bb);

  for (i=0; i< bb->n_data;++i) {
    if (!strncmp(var_name,(bb_data_desc(bb)[i]).name,VARNAME_MAX_SIZE+1)) {
      retval = i;
      break;
    }
  } /* end for */

  return retval;
} /* end of  bb_find */

S_BB_DATADESC_T* 
bb_data_desc(volatile S_BB_T* bb) {
  
  S_BB_DATADESC_T* retval;  
  
  retval = NULL;
  assert(bb);
  retval = (S_BB_DATADESC_T*) ((char*)(bb) + bb->data_desc_offset);
  
  return retval;
} /* end of bb_data_desc */

void* 
bb_data(volatile S_BB_T* bb) {
  
  void* retval;  
  
  retval = NULL;
  assert(bb);
  retval = (char*)(bb) + bb->data_offset;
  
  return retval;
} /* end of bb_data */

#ifndef __KERNEL__
double
bb_double_of(void* value, E_BB_TYPE_T bbtype) {

  double retval;

  retval = 0.0;

  switch (bbtype) {
  case E_BB_DOUBLE: 
    retval = *((double*)value);
    break;
  case E_BB_FLOAT:
    retval = *((float*)value);
    break;
  case E_BB_INT8:
    retval = *((int8_t*)value);
    break; 
  case E_BB_INT16:
    retval = *((int16_t*)value);
    break; 
  case E_BB_INT32:
    retval = *((int32_t*)value);
    break; 
  case E_BB_INT64:
    retval = *((int64_t*)value);
    break; 
  case E_BB_UINT8:
    retval = *((uint8_t*)value);
    break;  
  case E_BB_UINT16:
    retval = *((uint16_t*)value);
    break;
  case E_BB_UINT32:
    retval = *((uint32_t*)value);
    break;	
  case E_BB_UINT64:
    retval = *((uint64_t*)value);
    break;	
  case E_BB_CHAR:
  case E_BB_UCHAR:
  case E_BB_USER:
    /* FIXME could not convert set 0.0 */
    retval = 0.0;
    break; 
  default:
    retval = 0.0;
    break;
  }

  return retval;
} /* end of bb_double_of */
#endif

int32_t 
bb_data_initialise(volatile S_BB_T* bb, S_BB_DATADESC_T* data_desc,void* default_value) {
  
  int32_t retval;  
  char* data;
  int32_t i;
  int32_t idxstack[1];
  int32_t idxstack_len = 1;
  
  idxstack[0] = 0;
  
  assert(data_desc);
  /* on recupere l'adresse de la donnee dans le BB */
  /* FIXME a modifier peut etre */
  //data = (char*)bb_data(bb) + data_desc.data_offset;
  data = bb_item_offset(bb,data_desc,idxstack, idxstack_len);
  
  
  retval = BB_OK;
  for (i=0; i< data_desc->dimension; ++i) {
    switch (data_desc->type) {
#ifndef __KERNEL__
    case E_BB_DOUBLE: 
      ((double*) data)[i] = default_value ? *((double *) default_value) : 0.0;
      break;
    case E_BB_FLOAT:
      ((float*) data)[i] = default_value ? *((float *) default_value) : 0.0;
      break;
#endif
    case E_BB_INT8:
      ((int8_t*) data)[i] = default_value ? *((int8_t *) default_value) : 0;
      break;
    case E_BB_INT16:
      ((int16_t*) data)[i] = default_value ? *((short *) default_value) : 0;
      break; 
    case E_BB_INT32:
      ((int32_t*) data)[i] = default_value ? *((int *) default_value) : 0;
      break; 
    case E_BB_INT64:
      ((int64_t*) data)[i] = default_value ? *((long *) default_value) : 0;
      break;       
    case E_BB_UINT8:
      ((uint8_t*) data)[i] = default_value ? *((uint8_t *) default_value) : 0;
      break;
    case E_BB_UINT16:
      ((uint16_t*) data)[i] = default_value ? *((unsigned short *) default_value) : 0;
      break;
    case E_BB_UINT32:
      ((uint32_t*) data)[i] = default_value ? *((unsigned int *) default_value) : 0;
      break;	
    case E_BB_UINT64:
      ((uint64_t*) data)[i] = default_value ? *((unsigned long *) default_value) : 0;
      break;	
    case E_BB_CHAR:
      ((char *) data)[i] = default_value ? *((char *) default_value) : '\0';
      break;
    case E_BB_USER:
      if (NULL == default_value) {
	memset(data + (data_desc->type)*i,0,data_desc->type);
      } else {
	memcpy(data + (data_desc->type)*i,default_value,data_desc->type);
      }
      break; 
    default:
      retval = BB_NOK;
      break;
    }
  } 
  
  return retval;
} /* end of bb_data_initialise */

int32_t
bb_value_direct_rawwrite(void* data, S_BB_DATADESC_T data_desc, void* value) {  
  memcpy(data,value,data_desc.type_size);
  return BB_OK;
}

#ifndef __KERNEL__
int32_t
bb_value_direct_write(void* data, S_BB_DATADESC_T data_desc, const char* value, int hexval) {

  int retval;

  retval = BB_OK;

  switch (data_desc.type) {
#ifndef __KERNEL__
  case E_BB_DOUBLE: 
    ((double *)data)[0] = atof(value);
    break;
  case E_BB_FLOAT:
    ((float *)data)[0] = atof(value);
    break;
#endif
  case E_BB_INT8:
    ((int8_t*)data)[0] = strtol(value,(char **)NULL,hexval ? 16 : 10);
    break; 
  case E_BB_INT16:
    ((int16_t*)data)[0] = strtol(value,(char **)NULL,hexval ? 16 : 10);
    break; 
  case E_BB_INT32:
    ((int32_t*)data)[0] = strtol(value,(char **)NULL,hexval ? 16 : 10);
    break; 
  case E_BB_INT64:
    ((int64_t*)data)[0] = strtoll(value,(char **)NULL,hexval ? 16 : 10);
    break; 
  case E_BB_UINT8:
    ((uint8_t*)data)[0] = strtol(value,(char **)NULL,hexval ? 16 : 10);
    break; 
  case E_BB_UINT16:
    ((uint16_t*)data)[0] = strtoul(value,(char **)NULL,hexval ? 16 : 10);
    break;
  case E_BB_UINT32:
    ((uint32_t*)data)[0] = strtoul(value,(char **)NULL,hexval ? 16 : 10);
    break;	
  case E_BB_UINT64:
    ((uint64_t*)data)[0] = strtoull(value,(char **)NULL,hexval ? 16 : 10);
    break;	
  case E_BB_CHAR:
    memcpy(&((char*)data)[0],value,sizeof(char));
    break;
  case E_BB_UCHAR:
    memcpy(&((unsigned char*)data)[0],value,sizeof(unsigned char));
    break; 
  case E_BB_USER:
    
    retval = bb_utils_convert_string2hexbuf(hexval ? value+2 : value,
					    &((unsigned char*)data)[0*data_desc.type_size],
					    data_desc.type_size, 
					    hexval);
    retval = BB_NOK;
    break; 
  default:
    retval = BB_NOK;
    break;
  }
  return retval;
} /* bb_value_direct_write */
#endif /* !__KERNEL__ */

int32_t
bb_value_write(volatile S_BB_T* bb, 
	       S_BB_DATADESC_T data_desc, 
	       const char* value, 
	       int32_t* idxstack, int32_t idxstack_len) {

  char* data;
  int retval;
  int hexval;
  int lenval;
  assert(bb);
  
  retval = BB_OK;

  lenval = strlen(value);

  if ((NULL != strstr(value,"0x")) | 
      (NULL != strstr(value,"0X"))
      ) {
    hexval  = 1;
    lenval -= 2;
  } else {
    hexval = 0;
  }

  /* Get address of the data in BB */
  data = bb_item_offset(bb, &data_desc,idxstack,idxstack_len);
  /* Now write the value at obtained offset */
#ifndef __KERNEL__
  retval = bb_value_direct_write(data,data_desc,value,hexval);
#endif
  return retval;
} /* bb_value_write */

#ifndef __KERNEL__

int32_t
bb_data_header_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx, int32_t aliastack) {
  const char oneTab[] = "    "; 
  char tabs[MAX_ALIAS_LEVEL*5]="";
  
  int i;
  
  for (i=0; i<(aliastack-1); i++)
    {
      strncat(tabs, oneTab, strlen(oneTab));
    }
  
  fprintf(pf,"%s---------- < %s > ----------\n", tabs,data_desc.name);
  fprintf(pf,"%s  alias-target = %d\n", tabs, data_desc.alias_target);
  fprintf(pf,"%s  type         = %d  (%s)\n",tabs,data_desc.type,E_BB_2STRING[data_desc.type]);
  fprintf(pf,"%s  dimension    = %d  \n",tabs,data_desc.dimension);
  fprintf(pf,"%s  type_size    = %d  \n",tabs,data_desc.type_size);
  fprintf(pf,"%s  data_offset  = %ld \n",tabs,data_desc.data_offset);
  if ((idx>=0) && 
      ((E_BB_UCHAR != data_desc.type) &&
       (E_BB_CHAR  != data_desc.type)
       )
      ) {
    fprintf(pf,"%s  value[%d]     = ",tabs,idx);
  } else {
    fprintf(pf,"%s  value        = ",tabs);
  }
  if ((data_desc.dimension > 1) &&
      ((E_BB_UCHAR != data_desc.type) &&
       (E_BB_CHAR  != data_desc.type)
       )
      ){
    fprintf(pf," [ ");
  } 

  if ((idx>=0) && 
      ((E_BB_UCHAR != data_desc.type) &&
       (E_BB_CHAR  != data_desc.type)
       )
      ) {
    fprintf(pf,"... ");
  }
  return 0;
} /* end of bb_data_header_print */

int32_t
bb_data_footer_print(S_BB_DATADESC_T data_desc, FILE* pf, int32_t idx, int32_t aliastack) {
  const char oneTab[] = "    ";
  char tabs[MAX_ALIAS_LEVEL*5]="";
  
  int i;
  
  for (i=0; i<(aliastack-1); i++) {
    strncat(tabs, oneTab, strlen(oneTab));
  }
    
  if ((idx>=0) &&
      ((E_BB_UCHAR != data_desc.type) &&
       (E_BB_CHAR  != data_desc.type)
       )
      ) {
    fprintf(pf,"... ");
  }  

  if ((data_desc.dimension > 1) &&
      ((E_BB_UCHAR != data_desc.type) &&
       (E_BB_CHAR  != data_desc.type)
       )
      ){
    fprintf(pf,"]");
  }  
  fprintf(pf,"\n");    
  fprintf(pf,"%s---------- ---------- ----------\n", tabs);
  
  return 0;
} /* end of bb_data_footer_print */

int32_t 
bb_string_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf, 
		      int32_t* idxstack, int32_t idxstack_len) {
  int32_t i,ibeg,iend;
  char* data;
  int32_t idx;
  assert(bb);
  /* We get BB data address  */
  data = bb_item_offset(bb, &data_desc,idxstack,idxstack_len);
  
  /* check index stack to handle index */
  if ((idxstack_len>0) && (data_desc.dimension > 1)){
    idx = idxstack[idxstack_len-1];
    if (idx>=0) {
      ibeg=0;
      iend=1;
    } else {
      ibeg=0;
      iend=data_desc.dimension;
    }
  } else {
    idx = 0;
    ibeg=0;
    iend=data_desc.dimension;
  }

  /* 
   * If we have character this means we want
   * a character-like or string-like display 
   * HEX dump should be for UINT8/16/32/64
   */
  if ((E_BB_CHAR ==data_desc.type) ||
      (E_BB_UCHAR==data_desc.type)) {
    for (i=ibeg; i< iend; ++i) {
      if ('\0'==((char*) data)[i]) {
	break;
      } else {
	fprintf(pf,"%c",isprint((int)data[i]) ? ((char*) data)[i] : '?');
      }
    }
  } else {
    /* This is not a char type ? */
    fprintf(pf,"bb_string_value_print: Not a char type");
    return 1;
  }
  return 0;
} /* end of bb_string_value_print */

int32_t 
bb_value_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf, 
               int32_t* idxstack, int32_t idxstack_len) {
  
  int32_t i,j,ibeg,iend;
  char* data;
  int32_t idx;
  int32_t charNullCount;
  assert(bb);
  /* We get BB data address  */
  data = bb_item_offset(bb, &data_desc,idxstack,idxstack_len);
  
  /* check index stack to handle index */
  if ((idxstack_len>0) && (data_desc.dimension > 1)){
    idx = idxstack[idxstack_len-1];
    if (idx>=0) {
      ibeg=0;
      iend=1;
    } else {
      ibeg=0;
      iend=data_desc.dimension;
    }
  } else {
    idx = 0;
    ibeg=0;
    iend=data_desc.dimension;
  }

  /* 
   * If we have character this means we want
   * a character-like or string-like display 
   * HEX dump should be for UINT8/16/32/64
   */
  if ((E_BB_CHAR ==data_desc.type) ||
      (E_BB_UCHAR==data_desc.type)) {
    charNullCount = 0;
    for (i=ibeg; i< iend; ++i) {
      if ('\0'==((char*) data)[i]) {
	charNullCount++;
      } else {
	if (charNullCount>0) {
	  fprintf(pf,"(%d*'\\0')",charNullCount);
	  charNullCount = 0;
	}
	if (isprint((int)data[i])) {
	  fprintf(pf,"%c",((char*) data)[i]);
	} else {
	  fprintf(pf," 0x%02X ",((char*) data)[i]);
	}
      }
    }
    if (charNullCount>0) {
      fprintf(pf," (%d*'\\0')",charNullCount);
      charNullCount = 0;
    }

/*     fprintf(pf," (0x"); */
/*     for (i=ibeg; i< iend; ++i) { */
/*       fprintf(pf,"%02x",((char*) data)[i]); */
/*     } */
/*     fprintf(pf,")"); */
  } else {

    for (i=ibeg; i< iend; ++i) {    
      switch (data_desc.type) {
      case E_BB_DOUBLE: 
	fprintf(pf,"%1.16f ",((double*) data)[i]);
	break;
      case E_BB_FLOAT:
	fprintf(pf,"%f ",((float*) data)[i]);
	break;
      case E_BB_INT8:
	fprintf(pf,"%d ",((int8_t*) data)[i]);
	break;
      case E_BB_INT16:
	fprintf(pf,"%d ",((int16_t*) data)[i]);
	break; 
      case E_BB_INT32:
	fprintf(pf,"%d ",((int32_t*) data)[i]);
	break; 
      case E_BB_INT64:
	fprintf(pf,"%lld ",((int64_t*) data)[i]);
	break;
      case E_BB_UINT8:
	fprintf(pf,"0x%x ",((uint8_t*) data)[i]);
	break; 
      case E_BB_UINT16:
	fprintf(pf,"0x%x ",((uint16_t*) data)[i]);
	break;
      case E_BB_UINT32:
	fprintf(pf,"0x%x ",((uint32_t*) data)[i]);
	break;	
      case E_BB_UINT64:
	fprintf(pf,"0x%llx ",((uint64_t*) data)[i]);
	break;	
	/*     case E_BB_CHAR: */
	/*       fprintf(pf,"0x%02x<%c> ",((char*) data)[i], */
	/* 	      isprint((int)data[i]) ? ((char*) data)[i] : '?'); */
	/*       break; */
	/*     case E_BB_UCHAR: */
	/*       fprintf(pf,"0x%02x<%c> ",((char*) data)[i], */
	/* 	      isprint((int)data[i]) ? ((char*) data)[i] : '?'); */
	/*       break;*/
      case E_BB_USER:
	for (j=0; j<data_desc.type_size; ++j) {
	  fprintf(pf,"0x%02x ",((uint8_t*) data)[i*data_desc.type_size+j]);
	  //bb_data_print(bb, *(&data_desc+1), pf);  
	}
	break; 
      default:
	fprintf(pf,"0x%x ",((char*) data)[i]);
	break;
      }
    }
  } 
  return 0;
} /* end of bb_value_print */

int32_t 
bb_data_print(volatile S_BB_T* bb, S_BB_DATADESC_T data_desc, FILE* pf,
              int32_t* idxstack, int32_t idxstack_len) {
   int32_t          aliasstack_size = MAX_ALIAS_LEVEL;
   S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
   
   aliasstack[0]=data_desc;		
   bb_find_aliastack(bb, aliasstack, &aliasstack_size);
   bb_data_header_print(data_desc,pf,-1,aliasstack_size);
   bb_value_print(bb,data_desc,pf,idxstack,idxstack_len);
   bb_data_footer_print(data_desc,pf,-1,aliasstack_size);
   return BB_OK;
} /* end of bb_data_print */
#endif /* !__KERNEL__ */

int32_t 
bb_create(S_BB_T** bb, 
	       const char* pc_bb_name,
	       int32_t n_data,
	       int32_t data_size) {

  enum bb_type type;
  int32_t retcode;
  assert(bb);

  type = bb_type(pc_bb_name);
  retcode = ops[type]->bb_shmem_get(bb, pc_bb_name, n_data, data_size, 1);
  if (retcode != BB_OK)
  	goto err;

  retcode = ops[(*bb)->type]->bb_sem_get(*bb, 1);
  if (retcode != BB_OK)
  	goto err;

  retcode = ops[(*bb)->type]->bb_msgq_get(*bb, 1);
  
err:
  return retcode;
} /* end of bb_create */
#ifdef __KERNEL__
EXPORT_SYMBOL_GPL(bb_create);
#endif

int32_t 
bb_destroy(S_BB_T** bb) {
  
  int32_t retcode;
  
  assert(bb);
  assert(*bb);  
  /* 
   * On signale la destruction en cours pour les processes qui
   * resteraient attachés
   */
  (*bb)->status = BB_STATUS_DESTROYED;
  retcode = ops[(*bb)->type]->bb_sem_destroy(*bb);
  if (retcode != BB_OK)
  	goto out;
  retcode = ops[(*bb)->type]->bb_msgq_destroy(*bb);
  if (retcode != BB_OK)
  	goto out;
  retcode = ops[(*bb)->type]->bb_shmem_destroy(bb);
  if (retcode != BB_OK)
  	goto out;
    
out:
  return retcode;
} /* end of bb_destroy */

int32_t
bb_data_memset(S_BB_T* bb, const char c) { 
  int32_t retcode;  
  assert(bb);
  retcode = 0;
  memset(bb_data(bb),c,bb->max_data_size);
  return retcode;
} /* end of bb_data_memset */

int32_t 
bb_lock(volatile S_BB_T* bb) {
  return ops[bb->type]->bb_unlock(bb);
} /* end of bb_lock */

int32_t 
bb_unlock(volatile S_BB_T* bb) {
  
  return ops[bb->type]->bb_unlock(bb);
} /* end of bb_unlock */


int32_t 
bb_attach(S_BB_T** bb, const char* pc_bb_name) 
{
  enum bb_type type;
  type = bb_type(pc_bb_name);
  return ops[type]->bb_shmem_attach(bb, pc_bb_name);
} /* end of bb_attach */

int32_t 
bb_detach(S_BB_T** bb) {
  
  assert(bb);
  assert(*bb);
  
  return ops[(*bb)->type]->bb_shmem_detach(bb);
} /* end of bb_detach */

void* 
bb_publish(volatile S_BB_T *bb, S_BB_DATADESC_T* data_desc) {
  
  void* retval;
  int32_t needed_size;

  retval = NULL;
  assert(bb);
  assert(data_desc);
  
  /* Verify that the published data is not already published
   * (key unicity) and trigger automatic subscribe
   * if key already exists.
   */
  bb_lock(bb);
  if (bb_find(bb,data_desc->name) != -1) {
     bb_logMsg(BB_LOG_FINER,"BlackBoard::bb_publish",
	       "Key <%s> already exists in blackboard (automatic subscribe)!!",data_desc->name);
    bb_unlock(bb);
    retval = bb_subscribe(bb,data_desc);
    bb_lock(bb);
  } else {
    /* compute required data size  */
    needed_size = data_desc->type_size*data_desc->dimension;    
    /* verify available space in BB data descriptor zone */
    if (bb->n_data >= bb->max_data_desc_size) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_publish", 
		"No more room in BB data descriptor!! [current n_data=%d]",
		bb->n_data);
      /* verify available space in BB data zone */
    } else if ((bb->max_data_size-bb->data_free_offset) < needed_size) {
      bb_logMsg(BB_LOG_SEVERE,"BlackBoard::bb_publish", 
		"No more room in BB data zone!! [left <%d> byte(s) out of <%d> required]",
		bb->max_data_size-bb->data_free_offset,needed_size);
    } else {     
      /* Compute the free address */
      retval = (char*) bb_data(bb) + bb->data_free_offset;
      /* Update returned data descriptor */
      data_desc->data_offset = bb->data_free_offset;
      /* this is not an alias */
      data_desc->alias_target = -1;
      /* Update next free address */
      bb->data_free_offset  = bb->data_free_offset + needed_size;
      /* Update data descriptor zone */
      bb_data_desc(bb)[bb->n_data] = *data_desc;
      /* Increment number of published data */
      bb->n_data++;
    }
    /* initialize publish data zone with default value */
    bb_data_initialise(bb,data_desc,NULL);
  }    
  /* no init in case of automatic subscribe */  
  bb_unlock(bb);  
  return retval;
} /* end of bb_publish */

void* 
bb_subscribe(volatile S_BB_T *bb, 
	     S_BB_DATADESC_T* data_desc) {
				 
	int32_t          indexstack[MAX_ALIAS_LEVEL];
  	/* zero out indexstack */
	memset(indexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
	
	return bb_alias_subscribe(bb,data_desc, indexstack,MAX_ALIAS_LEVEL);
} /* end of bb_subscribe */

void* 
bb_item_offset(volatile S_BB_T *bb, 
	       S_BB_DATADESC_T* data_desc,
	       const int32_t* indexstack,
	       const int32_t indexstack_len) {
  
  void* retval;
  S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
  int32_t          aliasstack_size = MAX_ALIAS_LEVEL;
  int32_t          myIndexstack[MAX_ALIAS_LEVEL];
  int32_t          i,j;
  
  retval = NULL;
  assert(bb);
  assert(data_desc);

  /* zero out indexstack */
  memset(myIndexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
  /* We seek the data using its key (name) */

  /* ********** ALIAS publish case **************** */
  if (bb_isalias(data_desc)) {

    /* rebuild alias stack beginning with current symbol */
    aliasstack[0] = *data_desc;
    if (!bb_find_aliastack(bb,aliasstack,&aliasstack_size)) {
      /* fill index stack with missing 0 (scalar case) */
      /* provided stack index is inverted */
      for (i=0,j=indexstack_len-1;i<aliasstack_size;++i) {
	if (1==aliasstack[i].dimension) {
	  myIndexstack[i] = 0;
	} else {
	  /* *** FIXME check provided indexstack length 
	     *** in order to avoid buffer overflow 
	     *** */
	  if (j>=0) {
	    myIndexstack[i] = indexstack[j];
	    --j;
	  } else {
	    myIndexstack[i] = 0;
	  }
	}		
      }
      /* force last index (first on stack) to zero */
      if (data_desc->type == E_BB_USER){
	myIndexstack[0] = 0;
      }
      retval = (char*) bb_data(bb) + 
	bb_aliasstack_offset(aliasstack,myIndexstack,aliasstack_size);	    
    } else {
      bb_logMsg(BB_LOG_SEVERE,
		"Blackboard::bb_item_offset",
		"Cannot resolve alias stack!!");
      retval = NULL;
    }
  }
  /* ********** GENUINE publish case **************** */ 
  else {
    retval = (char*) bb_data(bb) + 
      data_desc->data_offset + 
      /* if indexstack_len is not greater than 0 then indexstack[0] have no sense */ 
      (indexstack_len>0 ? (indexstack[0])*(data_desc->type_size) :  0 ) ;
  }
  return retval;
} /* end of bb_item_offset */


#ifndef __KERNEL__

int32_t 
bb_dump(volatile S_BB_T *bb, FILE* p_filedesc) {  
  
  int32_t retcode;
  /* char syserr[MAX_SYSMSG_SIZE]; */
  int32_t i;
  int32_t j;
  int32_t indexstack[MAX_ALIAS_LEVEL];
  int32_t indexstack_len;
  int32_t          aliasstack_size = MAX_ALIAS_LEVEL;
  S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
  int32_t array_in_aliasstack;

  
  retcode = BB_OK;
  assert(bb);
  fprintf(p_filedesc,"============= <[begin] BlackBoard [%s] [begin] > ===============\n",
	  bb->name);
  fprintf(p_filedesc,"  @start blackboard    = 0x%x\n",(unsigned int) (bb));
  fprintf(p_filedesc,"  stored data          = %d / %d [max desc]\n",
	  bb->n_data,
	  bb->max_data_desc_size);
  fprintf(p_filedesc,"  free data size       = %ld / %ld\n",
	  bb->max_data_size - bb->data_free_offset,
	  bb->max_data_size);  
  fprintf(p_filedesc,"  @data_desc           = 0x%x\n",
	  (unsigned int) (bb_data_desc(bb)));
  fprintf(p_filedesc,"  @data                = 0x%x\n",
	  (unsigned int) (bb_data(bb)));
  fprintf(p_filedesc,"================ < [begin] Data [begin] > ==================\n");
  for (i=0;i<bb->n_data;++i) {
    /* NON ALIAS CASE */
    if ((bb_data_desc(bb)[i]).alias_target == -1) {
      indexstack[0]  = 0;
      indexstack_len = 0;
      bb_data_print(bb,bb_data_desc(bb)[i],p_filedesc,indexstack, indexstack_len);
    }
    /* ALIAS CASE */
    else {
      array_in_aliasstack = 0;
      aliasstack[0]=bb_data_desc(bb)[i];
      aliasstack_size = MAX_ALIAS_LEVEL;
      bb_find_aliastack(bb, aliasstack, &aliasstack_size);
      /*		for (j=0; j<aliasstack_size; j++){
	if (aliasstack[j].dimension>1){
	array_in_aliasstack = 1;
	}
	}
	if (!array_in_aliasstack)*/
      if ((aliasstack[aliasstack_size-1].dimension) <= 1) {
	for (j=0; j<bb_data_desc(bb)[i].dimension; j++){
	  indexstack[0] = j;
	  indexstack_len = 1;
	  bb_data_print(bb, bb_data_desc(bb)[i], p_filedesc,indexstack, indexstack_len);
	  
	}
      }
      
      else {
	for (j=0; j<aliasstack[aliasstack_size-1].dimension; j++){
	  indexstack[0] = j;
	  indexstack_len = 1;
	  bb_data_print(bb, bb_data_desc(bb)[i], p_filedesc,indexstack, indexstack_len);
	  
	}			
      }             
    }
  }
  fprintf(p_filedesc,"================== < [end] Data [end] > ====================\n");
  fprintf(p_filedesc,"============== < [end] BlackBoard [%s] [end] > ================\n",
	  bb->name);
  
  return retcode;
} /* end of bb_dump */

#endif /* ! __KERNEL__ */

int32_t
bb_get_nb_max_item(volatile S_BB_T *bb) {
  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb->max_data_desc_size;
  
  return retval;
}

int32_t
bb_get_nb_item(volatile S_BB_T *bb) {

  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb->n_data;
  
  return retval;
}

int32_t
bb_get_mem_size(volatile S_BB_T *bb) {

  
  int32_t retval;  
  
  
  retval = -1;
  assert(bb);
  
  retval = bb_size(bb->max_data_desc_size,
			bb->max_data_size);
  
  return retval;
}

int32_t 
bb_shadow_get(S_BB_T *bb_shadow,
	      volatile S_BB_T *bb_src) {
  
  int32_t retcode;

  bb_lock(bb_src);  
  assert(bb_src);
  assert(bb_shadow);
  retcode = BB_OK;
  /* raw copy of BB */
  memcpy(bb_shadow,
	 (void*)bb_src,
	 bb_get_mem_size(bb_src));
  bb_shadow->status = BB_STATUS_SHADOW;
  bb_unlock(bb_src);
  
  return retcode;
}

int32_t 
bb_shadow_update_data(S_BB_T *bb_shadow,
		      volatile S_BB_T *bb_src) {  
  int32_t retcode;
  
  assert(bb_src);
  assert(bb_shadow);
  retcode = BB_OK;
  bb_lock(bb_src);
  /* raw copy of BlackBoard data zone content
   * (in fact only used part of the data zone, 
   *  we do not copy unused part) */
  memcpy(bb_data(bb_shadow),
	 bb_data(bb_src),
	 bb_shadow->data_free_offset);
  bb_unlock(bb_src);
  
  return retcode;
} /* end of bb_shadow_update_data */


int32_t 
bb_snd_msg(volatile S_BB_T *bb,
		S_BB_MSG_T* msg) {
  
  int32_t retcode;
  
  retcode = BB_OK;
  assert(bb);
  return ops[bb->type]->bb_msgq_send(bb, msg);
} /* end of bb_snd_msg */


int32_t 
bb_rcv_msg(volatile S_BB_T *bb,
		S_BB_MSG_T* msg) {
    
  assert(bb);

  return ops[bb->type]->bb_msgq_recv(bb, msg);
} /* end of bb_rcv_msg */


int32_t
bb_get_array_name(char * array_name,
		  int array_name_size_max,
		  S_BB_DATADESC_T * aliasstack, int32_t aliasstack_size,
		  int32_t * indexstack, int32_t indexstack_len) {

  char * part_of_name;
  int32_t indexstack_curr;
  int j;
    
#ifdef __KERNEL__
  part_of_name = kmalloc(array_name_size_max, GFP_KERNEL);
#else
  part_of_name = malloc(array_name_size_max);
#endif /* __KERNEL__ */

  indexstack_curr = 0;
  for (j=aliasstack_size-1; j>=0; j--){
    /* If this alias is an array */
    if (aliasstack[j].dimension > 1){
      if (j==aliasstack_size-1){
	snprintf(part_of_name, array_name_size_max, "%s[%0d]",
		 aliasstack[j].name,
		 indexstack[indexstack_curr]);
      } else {
	snprintf(part_of_name, array_name_size_max, "%s[%0d]",
		 strstr(aliasstack[j].name,
			aliasstack[j+1].name)+strlen(aliasstack[j+1].name),
		 indexstack[indexstack_curr]);
      }      
      strncat(array_name, part_of_name, array_name_size_max);
      /* go to next index in the index stack */
      indexstack_curr++;
    }
    /* The current alias is a scalar */
    else {
      if (j==aliasstack_size-1){
	snprintf(part_of_name, array_name_size_max, "%s", aliasstack[j].name);
      } else {
	snprintf(part_of_name, array_name_size_max, "%s", 
		 strstr(aliasstack[j].name, aliasstack[j+1].name) +
		 strlen(aliasstack[j+1].name)
		 );
      }
      strncat(array_name, part_of_name, array_name_size_max);
    }
  }
#ifdef __KERNEL__
  kfree (part_of_name);
#else
  free (part_of_name);
#endif
  return BB_OK;

} /* end of get_array_name */

int32_t
bb_msgq_isalive(S_BB_T *bb)
{
	return ops[bb->type]->bb_msgq_isalive(bb);
}

#ifdef __KERNEL__
int32_t 
bb_logMsg(const BB_LOG_LEVEL_T level, const char* who, char* fmt, ...) 
{
	va_list args;	
	char message[2048];

	memset(message,0,2048);
	va_start(args, fmt);
	vsnprintf(message, 2048, fmt, args);
	va_end(args);
	printk("bb: %s : %s", who, message);
	return 0;
}
#else
int32_t 
bb_logMsg(const BB_LOG_LEVEL_T level, const char* who, char* fmt, ...) {
  va_list args;
  char message[2048];
  char *tmp_str;

  memset(message,0,2048);
  va_start(args, fmt);
  vsnprintf(message, 2048, fmt, args);
  va_end(args);
  /* add strerror, if needed */
  if (errno != 0) {
    tmp_str = strdup(message);
    if (!tmp_str) {
      	printf("Cannot allocate memory for message %s\n", message);
	return BB_NOK;
    }
    snprintf(message, 2048, "%s: %s", tmp_str, strerror(errno));

    free(tmp_str);
  }
  
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
} /* end of bb_logMsg */

#endif /* __KERNEL__ */


