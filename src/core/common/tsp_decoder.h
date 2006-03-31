/*

$Header: /home/def/zae/tsp/tsp/src/core/common/tsp_decoder.h,v 1.1 2006-03-31 12:59:07 erk Exp $

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

Purpose   :  Implementation for the functions used to decode the data received

-----------------------------------------------------------------------
 */

#ifndef _TSP_DECODER_H
#define _TSP_DECODER_H


#include <tsp_abs_types.h>


#define TSP_SIZEOF_ENCODED_DOUBLE RNDUP(sizeof(double))
#define TSP_SIZEOF_ENCODED_FLOAT RNDUP(sizeof(float))
#define TSP_SIZEOF_ENCODED_INT8 RNDUP(sizeof(int8_t))
#define TSP_SIZEOF_ENCODED_INT16 RNDUP(sizeof(int16_t))
#define TSP_SIZEOF_ENCODED_INT32 RNDUP(sizeof(int32_t))
#define TSP_SIZEOF_ENCODED_INT64 RNDUP(sizeof(int64_t))
#define TSP_SIZEOF_ENCODED_UINT8 RNDUP(sizeof(uint8_t))
#define TSP_SIZEOF_ENCODED_UINT16 RNDUP(sizeof(uint16_t))
#define TSP_SIZEOF_ENCODED_UINT32 RNDUP(sizeof(uint32_t))
#define TSP_SIZEOF_ENCODED_UINT64 RNDUP(sizeof(uint64_t))
#define TSP_SIZEOF_ENCODED_CHAR RNDUP(sizeof(char))
#define TSP_SIZEOF_ENCODED_UCHAR RNDUP(sizeof(uint8_t))
#define TSP_SIZEOF_ENCODED_USER RNDUP(sizeof(uint8_t))

/**
 * Generic function to decode data
 * @param[out] out_buf buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
typedef int32_t(*TSP_data_decoder_t)(void* out_buf, uint32_t dimension, char* in_buf); 

/**
 * function to decode double
 * @param[out] out_double buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_double_decoder(void* out_double, uint32_t dimension,  char* in_buf);

/**
 * function to decode float
 * @param[out] out_float buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_float_decoder(void* out_float, uint32_t dimension,  char* in_buf);

/**
 * function to decode int8
 * @param[out] out_int8 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_int8_decoder(void* out_int8, uint32_t dimension,  char* in_buf);

/**
 * function to decode int16
 * @param[out] out_int16 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_int16_decoder(void* out_int16, uint32_t dimension,  char* in_buf);

/**
 * function to decode int32
 * @param[out] out_int32 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_int32_decoder(void* out_int32, uint32_t dimension,  char* in_buf);

/**
 * function to decode int64
 * @param[out] out_int64 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_int64_decoder(void* out_int64, uint32_t dimension,  char* in_buf);

/**
 * function to decode uint8
 * @param[out] out_uint8 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_uint8_decoder(void* out_uint8, uint32_t dimension,  char* in_buf);

/**
 * function to decode uint16
 * @param[out] out_uint16 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_uint16_decoder(void* out_uint16, uint32_t dimension,  char* in_buf);

/**
 * function to decode uint32
 * @param[out] out_uint32 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_uint32_decoder(void* out_uint32, uint32_t dimension,  char* in_buf);

/**
 * function to decode uint64
 * @param[out] out_uint64 buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_uint64_decoder(void* out_uint64, uint32_t dimension,  char* in_buf);

/**
 * function to decode char
 * @param[out] out_char buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_char_decoder(void* out_char, uint32_t dimension,  char* in_buf);

/**
 * function to decode uchar
 * @param[out] out_uchar buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_uchar_decoder(void* out_uchar, uint32_t dimension,  char* in_buf);

/**
 * function to decode user type
 * @param[out] out_user buffer where decoded data are stored.
 * @param[in]  dimension of the data
 * @param[in]  in_buf data to decode
 * @return TRUE or FALSE. TRUE = OK
 */
int32_t TSP_data_channel_user_decoder(void* out_user, uint32_t dimension,  char* in_buf);


/**
 * Get the function that's used to decode a data type encoded
 * in the data stream.
 * This function address is stored by the group object to 
 * decode the incoming data as fast as possible.
 * @param[in] type data type to determine the decoder to use
 * @return The address of the decoder function for the data type
 */
TSP_data_decoder_t TSP_data_channel_get_decoder(TSP_datatype_t type);

/**
 * Get the size of an encoded data type coming from the data stream
 * @param[in] type data type to determine the data type size
 * @return The size of an encoded ddata for the data stream 
 */
int32_t TSP_data_channel_get_encoded_size(TSP_datatype_t type);



#endif /* _TSP_DECODER_H */

