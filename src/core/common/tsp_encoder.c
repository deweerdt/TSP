
/*

$Header: /home/def/zae/tsp/tsp/src/core/common/tsp_encoder.c,v 1.10 2007-01-26 16:47:19 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Eric Noulard et Arnaud MORVAN 

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
Component : Provider

-----------------------------------------------------------------------

Purpose   :  Implementation for the functions used to encode the type

-----------------------------------------------------------------------
 */

#include <rpc/rpc.h>
#include <rpc/xdr.h>

#include <tsp_const_def.h>
#include <tsp_simple_trace.h>
#include <tsp_datastruct.h>
#include <tsp_encoder.h>
#include <tsp_decoder.h>

#if defined(sun) || defined(__sun)
#include <alloca.h>
#endif

#if defined (WIN32)
    #include <malloc.h>
    #define assert(exp)     ((void)0)
#else    
    #include <assert.h>
#endif

uint32_t TSP_data_channel_double_encoder(void* v_double, uint32_t dimension,  char* out_buf, uint32_t size)
{

  double *pt_double;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= sizeof(double) * dimension;
  if(size  < taille )
  {
    STRACE_ERROR(("buffer is too small"));
    return 0;
  }

  pt_double=(double*)v_double;

#ifndef TSP_NO_XDR_ENCODE
 
  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_double(&xhandle, pt_double+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_double failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for (i=0;i<dimension;++i) {
      ((uint64_t*)out_buf)[i] = TSP_ENCODE_DOUBLE_TO_UINT64(pt_double+i);
      STRACE_DEBUG_MORE(("DOUBLE = %f, encoded DOUBLE=0x%08llx",*pt_double,((uint64_t*)out_buf)[i]));
    } 

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/

}


uint32_t TSP_data_channel_float_encoder(void* v_float,uint32_t dimension,  char* out_buf, uint32_t size)
{

  float *pt_float;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= sizeof(float) * dimension;
  if(size  < taille )
  {
    STRACE_ERROR(("buffer is too small"));
    return 0;
  }

  pt_float=(float*)v_float;

#ifndef TSP_NO_XDR_ENCODE

  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_float(&xhandle, pt_float+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_float failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for(i=0;i<dimension;++i)
    {
      ((uint32_t*)out_buf)[i] = TSP_ENCODE_FLOAT_TO_UINT32(pt_float+i);

    }

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/

}

uint32_t TSP_data_channel_int8_encoder(void* v_int8,uint32_t dimension,  char* out_buf, uint32_t size)
{

  int8_t *pt_int8;
  uint32_t i;
  int32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_int8=(int8_t*)v_int8;

  for (i=0;i<dimension;++i) {
    temp[i] = (int32_t) pt_int8[i];
  }

  return  TSP_data_channel_int32_encoder(&temp[0],dimension,out_buf,size);


}


uint32_t TSP_data_channel_int16_encoder(void* v_int16,uint32_t dimension,  char* out_buf, uint32_t size)
{
  int16_t *pt_int16;
  uint32_t i;
  int32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_int16=(int16_t*)v_int16;

  for (i=0;i<dimension;++i) {
    temp[i] = (int32_t) pt_int16[i];
  }

  return  TSP_data_channel_int32_encoder(&temp[0],dimension,out_buf,size);
}

uint32_t TSP_data_channel_int32_encoder(void* v_int32,uint32_t dimension,  char* out_buf, uint32_t size)
{
  int32_t *pt_int32;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= TSP_SIZEOF_ENCODED_INT32 * dimension;
  if(size  < taille )
  {
    STRACE_ERROR(("buffer is too small"));
    return 0;
  }

  pt_int32=(int32_t*)v_int32;

#ifndef TSP_NO_XDR_ENCODE

  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_int(&xhandle, pt_int32+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_int failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for(i=0;i<dimension;++i)
    {
      ((int32_t*)out_buf)[i] = TSP_ENCODE_INT32(pt_int32+i);

    }

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/
}

uint32_t 
TSP_data_channel_int64_encoder(void* v_int64,uint32_t dimension,  char* out_buf, uint32_t size) {
  int64_t *pt_int64;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= TSP_SIZEOF_ENCODED_INT64 * dimension;
  if (size  < taille ) {
     STRACE_ERROR(("buffer is too small"));
     return 0;
  }

  pt_int64=(int64_t*)v_int64;

#ifndef TSP_NO_XDR_ENCODE

  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_hyper(&xhandle, pt_int64+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_hyper failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for(i=0;i<dimension;++i)
    {
      ((int64_t*)out_buf)[i] = TSP_ENCODE_INT64(pt_int64+i);

    }

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/
}

uint32_t TSP_data_channel_uint8_encoder(void* v_uint8,uint32_t dimension,  char* out_buf, uint32_t size)
{

  uint8_t *pt_uint8;
  uint32_t i;
  uint32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_uint8=(uint8_t*)v_uint8;

  for (i=0;i<dimension;++i) {
    temp[i] = (uint32_t) pt_uint8[i];
  }

  return  TSP_data_channel_uint32_encoder(&temp[0],dimension,out_buf,size);
}

uint32_t TSP_data_channel_uint16_encoder(void* v_uint16,uint32_t dimension,  char* out_buf, uint32_t size)
{
  uint16_t *pt_uint16;
  uint32_t i;
  uint32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_uint16=(uint16_t*)v_uint16;

  for (i=0;i<dimension;++i) {
    temp[i] = (uint32_t) pt_uint16[i];
  }

  return  TSP_data_channel_uint32_encoder(&temp[0],dimension,out_buf,size);
}

uint32_t TSP_data_channel_uint32_encoder(void* v_uint32,uint32_t dimension,  char* out_buf, uint32_t size)
{
  uint32_t *pt_uint32;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= TSP_SIZEOF_ENCODED_UINT32 * dimension;
  if(size  < taille )
  {
    STRACE_ERROR(("buffer is too small"));
    return 0;
  }

  pt_uint32=(uint32_t*)v_uint32;

#ifndef TSP_NO_XDR_ENCODE

  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_int(&xhandle, pt_uint32+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_int failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for(i=0;i<dimension;++i)
    {
      ((uint32_t*)out_buf)[i] = TSP_ENCODE_UINT32(pt_uint32+i);

    }

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/
}

uint32_t TSP_data_channel_uint64_encoder(void* v_uint64,uint32_t dimension,  char* out_buf, uint32_t size)
{

  uint64_t *pt_uint64;
  uint32_t i,taille;

#ifndef TSP_NO_XDR_ENCODE

  XDR xhandle;

#endif /*TSP_NO_XDR_ENCODE*/

  taille= TSP_SIZEOF_ENCODED_UINT64 * dimension;
  if(size  < taille )
  {
    STRACE_ERROR(("buffer is too small"));
    return 0;
  }

  pt_uint64=(uint64_t*)v_uint64;

#ifndef TSP_NO_XDR_ENCODE

  xdrmem_create(&xhandle, out_buf,  size, XDR_ENCODE);

  for(i=0;i<dimension;++i)
  {
    if( xdr_hyper(&xhandle, pt_uint64+i) != TRUE)
    {
       STRACE_ERROR(("Function xdr_long failed"));
       return 0;
    }
  }
  
  return xdr_getpos(&xhandle);
    

#else
   
    for(i=0;i<dimension;++i)
    {
      ((uint64_t*)out_buf)[i] = TSP_ENCODE_UINT64(pt_uint64+i);

    }

    return (uint32_t)(taille);

#endif /*TSP_NO_XDR_ENCODE*/

}

uint32_t TSP_data_channel_char_encoder(void* v_char,uint32_t dimension,  char* out_buf, uint32_t size)
{
  uint8_t *pt_char;
  uint32_t i;
  uint32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_char=(uint8_t*)v_char;

  for (i=0;i<dimension;++i) {
    temp[i] = (uint32_t) pt_char[i];
  }

  return  TSP_data_channel_uint32_encoder(&temp[0],dimension,out_buf,size);
}

uint32_t TSP_data_channel_uchar_encoder(void* v_uchar,uint32_t dimension,  char* out_buf, uint32_t size)
{

  uint8_t  *pt_uchar;
  uint32_t  i;
  uint32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_uchar=(uint8_t*)v_uchar;

  for (i=0;i<dimension;++i) {
    temp[i] = (uint32_t) pt_uchar[i];
  }

  return  TSP_data_channel_uint32_encoder(&temp[0],dimension,out_buf,size);
}

uint32_t TSP_data_channel_user_encoder(void* v_user,uint32_t dimension,  char* out_buf, uint32_t size)
{

  uint8_t *pt_user;
  uint32_t  i;
  uint32_t*  temp = alloca(sizeof(int32_t)*dimension);

  pt_user=(uint8_t*)v_user;

  for (i=0;i<dimension;++i) {
    temp[i] = (uint32_t) pt_user[i];
  }

  return  TSP_data_channel_uint32_encoder(&temp[0],dimension,out_buf,size);
}

const char* 
TSP_data_channel_get_encoder_method() {

  static char* XDREncodeMethod = "NotSet";
#ifdef TSP_NO_XDR_ENCODE
  XDREncodeMethod = "TSP_NO_XDR_ENCODE";
#else
  XDREncodeMethod = "TSP_XDRLIB_XDR_ENCODE";
#endif
  return XDREncodeMethod;
}

TSP_data_encoder_t 
TSP_data_channel_get_encoder(TSP_datatype_t type) {
  
  switch(type) {

  case TSP_TYPE_DOUBLE :
    return TSP_data_channel_double_encoder;
    
  case TSP_TYPE_FLOAT :
    return TSP_data_channel_float_encoder;
    
  case TSP_TYPE_INT8 :
    return TSP_data_channel_int8_encoder;
    
  case TSP_TYPE_INT16:
    return TSP_data_channel_int16_encoder;
    
  case TSP_TYPE_INT32 :
      return TSP_data_channel_int32_encoder;
      
  case TSP_TYPE_INT64 :
    return TSP_data_channel_int64_encoder;
    
  case TSP_TYPE_UINT8:
    return TSP_data_channel_uint8_encoder;
    
  case TSP_TYPE_UINT16:
    return TSP_data_channel_uint16_encoder;
    
  case TSP_TYPE_UINT32:
    return TSP_data_channel_uint32_encoder;
    
  case TSP_TYPE_UINT64:
    return TSP_data_channel_uint64_encoder;
    
  case TSP_TYPE_CHAR:
    return TSP_data_channel_char_encoder;
    
  case TSP_TYPE_UCHAR:
    return TSP_data_channel_uchar_encoder;
    
  case TSP_TYPE_RAW:
    return TSP_data_channel_user_encoder;
    
  default:
    return NULL;
  }
} /* end of TSP_data_channel_get_encoder */
