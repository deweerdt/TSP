/*

$Header: /home/def/zae/tsp/tsp/src/core/common/tsp_encoder.h,v 1.7 2007-02-11 21:45:56 erk Exp $

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

#ifndef _TSP_ENCODER_H
#define _TSP_ENCODER_H

#include <tsp_prjcfg.h>
#include <tsp_rpc.h>

/**
 * @defgroup TSP_CommonLib_Encode Encoding Library
 * Before sending TSP sample value on the TSP data channel
 * the data should be encoding in order to handle
 * machine heterogeneity or different endianess.
 * The encoding library contains function used to encode
 * the different type of data handled by TSP.
 * The idea behind the TSP encoding API is to be able
 * to use different encoding scheme. Current encoding
 * implements XDR encoding but we may switch or support 
 * alternative encoding like CDR.
 * @ingroup TSP_CommonLib
 * @{
 */

/**
 * Encoder function type.
 * As you will see the encoder function pointer is generic and though
 * does not have any 'datatype' parameter. This
 * done on purpose. This is the encoder itself which will assume
 * tha datatype of the pointed value datavalue2encode.
 * @param[in]  datavalue2encode pointer to the beginning of data to encode.
 *             If NULL then encoder will not encode anything but returns
 *             the number of byte(s) that WOULD have been used if
 *             there were data to encode.
 * @param[in]  dimension of the data, that the number of data item
 *             to encode beginning at datavalue2encode.
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
typedef uint32_t(*TSP_data_encoder_t)(void*    datavalue2encode, 
				      uint32_t dimension, 
				      char*    out_buf, 
				      uint32_t out_buf_size); 

/**
 * double value type encoder @see TSP_data_encoder_t.
 * @param[in] v_double data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_double_encoder(void*    v_double,
				uint32_t dimension,  
				char*    out_buf, 
				uint32_t out_buf_size);

/**
 * float value type encoder @see TSP_data_encoder_t.
 * @param[in] v_float data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_float_encoder(void*    v_float,
			       uint32_t dimension,  
			       char*    out_buf, 
			       uint32_t out_buf_size);

/**
 * int8 value type encoder @see TSP_data_encoder_t.
 * @param[in] v_int8 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_int8_encoder(void*    v_int8,
			      uint32_t dimension,
			      char*    out_buf,
			      uint32_t out_buf_size);

/**
 * int16 value type encoder @see TSP_data_encoder_t.
 * @param[in] v_int16 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_int16_encoder(void*    v_int16,
			       uint32_t dimension,
			       char*    out_buf,
			       uint32_t out_buf_size);

/**
 * int32 value type encoder @see TSP_data_encoder_t.
 * @param[in] v_int32 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_int32_encoder(void*    v_int32,
			       uint32_t dimension,
			       char*    out_buf,
			       uint32_t out_buf_size);

/**
 * int64 value type encoder @see TSP_data_encoder_t.
 * @param[in]  v_int64 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_int64_encoder(void*    v_int64,
			       uint32_t dimension,
			       char*    out_buf,
			       uint32_t out_buf_size);

/**
 * uint8 value type encoder @see TSP_data_encoder_t.
 * @param[in]  v_uint8 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_uint8_encoder(void*    v_uint8,
			       uint32_t dimension,
			       char*    out_buf,
			       uint32_t out_buf_size);

/**
 * uint16 value type encoder @see TSP_data_encoder_t.
 * @param[in] v_uint16 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_uint16_encoder(void*    v_uint16,
				uint32_t dimension,
				char*    out_buf,
				uint32_t out_buf_size);

/**
 * uint32 value type encoder @see TSP_data_encoder_t.
 * @param[in]  v_uint32 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_uint32_encoder(void*    v_uint32,
				uint32_t dimension,
				char*    out_buf,
				uint32_t out_buf_size);

/**
 * uint64 value type encoder @see TSP_data_encoder_t.
 * @param[in]  v_uint64 data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_uint64_encoder(void*    v_uint64,
				uint32_t dimension,  
				char*    out_buf,
				uint32_t out_buf_size);

/**
 * char value type encoder @see TSP_data_encoder_t.
 * @param[in]  v_char data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_char_encoder(void*    v_char,
			      uint32_t dimension,
			      char*    out_buf,
			      uint32_t out_buf_size);

/**
 * uchar value type encoder @see TSP_data_encoder_t.
 * @param[in] v_uchar data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t 
TSP_data_channel_uchar_encoder(void*    v_uchar,
			       uint32_t dimension,  
			       char*    out_buf,
			       uint32_t out_buf_size);

/**
 * user value type encoder @see TSP_data_encoder_t.
 * @param[in] v_user data to encode.
 * @param[in]  dimension of the data
 * @param[out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return number of byte(s) used to encode the data, 0 means encoder failed.
 */
uint32_t TSP_data_channel_user_encoder(void* v_user,
				       uint32_t dimension,
				       char* out_buf,
				       uint32_t out_buf_size);

/**
 * Returns the encoder/decoder method used 
 * by TSP Library. TSP may be compiled either with
 *  - TSP_NO_XDR_ENCODE (default) which means that
 *    TSP library use its own XDR encoding MACRO
 *  - TSP_XDRLIB_XDR_ENCODE which means that 
 *    TSP library use the found XDR library to
 *    encode.
 * This is a compile-time choice. The function is returning
 * a static constant character string.
 * @return The TSP XDR encoder method name.
 */
_EXPORT_TSP_COMMON const char* TSP_data_channel_get_encoder_method();

/**
 * Returns the function used to encode the data type.
 * This value is stored in the group table to encode the data
 * as fast as possible.
 * @param[in] type data type to encode.
 * @return The data type encoder function
 */
_EXPORT_TSP_COMMON TSP_data_encoder_t TSP_data_channel_get_encoder(TSP_datatype_t type);

/** @} */

#endif /* _TSP_ENCODER_H */
