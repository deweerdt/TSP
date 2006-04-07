/*

$Header: /home/def/zae/tsp/tsp/src/core/common/tsp_encoder.h,v 1.2 2006-04-07 10:37:17 morvan Exp $

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

#include <tsp_abs_types.h>
#include <tsp_rpc.h>



/**
 * Generic function to encode data
 * @param[in] datavalue2encode data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
typedef uint32_t(*TSP_data_encoder_t)(void* datavalue2encode, uint32_t dimension, char* out_buf, uint32_t out_buf_size); 

/**
 * function to encode double
 * @param[in] v_double data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_double_encoder(void* v_double,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode float
 * @param[in] v_float data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_float_encoder(void* v_float,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode int8
 * @param[in] v_int8 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_int8_encoder(void* v_int8,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode int16
 * @param[in] v_int16 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_int16_encoder(void* v_int16,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode int32
 * @param[in] v_int32 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_int32_encoder(void* v_int32,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode int64
 * @param[in] v_int64 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_int64_encoder(void* v_int64,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode uint8
 * @param[in] v_uint8 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_uint8_encoder(void* v_uint8,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode uint16
 * @param[in] v_uint16 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_uint16_encoder(void* v_uint16,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode uint32
 * @param[in] v_uint32 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_uint32_encoder(void* v_uint32,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode uint64
 * @param[in] v_uint64 data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_uint64_encoder(void* v_uint64,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode char
 * @param[in] v_char data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_char_encoder(void* v_char,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode uchar
 * @param[in] v_uchar data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_uchar_encoder(void* v_uchar,uint32_t dimension,  char* out_buf, uint32_t size);

/**
 * function to encode user
 * @param[in] v_user data to encode.
 * @param[in]  dimension of the data
 * @param(out] out_buf buffer to write the data
 * @param[in]  out_buf_size size of the buffer
 * @return TRUE or FALSE. TRUE = OK
 */
uint32_t TSP_data_channel_user_encoder(void* v_user,uint32_t dimension,  char* out_buf, uint32_t size);


/**
 * Returns the function used to encode the data type.
 * This value is stored in the group table to encode the data
 * as fast as possible.
 * @param[in] type data type to encode.
 * @return The data type encoder function
 */
TSP_data_encoder_t TSP_data_channel_get_encoder(TSP_datatype_t type);

#endif /* _TSP_ENCODER_H */
