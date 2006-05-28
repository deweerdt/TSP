/*

$Id: macsim_fmt.h,v 1.6 2006-05-28 14:37:13 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2006 Eric NOULARD and Arnaud MORVAN 

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
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : function for file with format of macsim

-----------------------------------------------------------------------
*/


#ifndef _MACSIM_FMT_H_
#define _MACSIM_FMT_H_

#define LG_MAX_STRING_MACSIM 1025   /*size macsim + 1 for the '\0'*/
#define MAX_BUFFER_MACSIM    500    /* for read header, the file is read char by char */
#define MAX_NOM_VAR_MACSIM   100
#define MAX_DIMENSION_MACSIM 100
#define MAX_TYPE_MACSIM      20
#define MAX_UNITE_MACSIM     10

#define CARACTERE_TAB        '	'
#define CARACTERE_BLANC      ' '
#define CARACTERE_RETURN     '\r'
#define CARACTERE_ENDLINE    '\n'

#define MAX_DATA_LEN         100

/*file type*/
#define BOOLEEN_MACSIM       "booleen"
#define ENTIER_MACSIM        "entier"
#define CARACTERE_MACSIM     "character"
#define DOUBLE_MACSIM        "double"

#define CONTINUE             1
#define STOP		     0

/**
 * @defgroup MacsimFormatLib  Macsim Format Library
 * The library is used to read a Macsim File by the Generic Reader.
 * @ingroup TSP_GenericReaderLib
 * @{
 */

/**
 * extract the space from a string before and after.
 * 
 * @param[in] string the string to modify
 *                      
 * @return the same string without the spaces
 */
char *str_strip (const char *string);

/**
 * extract the space from a string before and after, in and out data in the same variable.
 * 
 * @param[in] tostrip the string to modify
 *                      
 */
void str_strip_inplace(char* tostrip);

/**
 * calculate the dimension of a macsim's symbol
 * 
 * @param[in] dimension_var the string who contain the dimension
 *                      
 * @return the dimension
 */
uint32_t macsim_dimension_data(char* dimension_var);


/**
 * determine tsp data type.
 * 
 * @param[in] type_var the data type extract from macsim file 
 *                      
 * @return the tsp  data type
 */
TSP_datatype_t macsim_type_data(char* type_var);

/**
 * initialize the fmtHandler (for generic handler)
 * 
 * @param[out] fmtHandler the fmtHandler to initialze
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t macsim_createHandler(FmtHandler_T ** fmtHandler);

/**
 * open a macsim file
 * 
 * @param[in] nom_fichier_macsim the file name
 *                      
 * @return the pointer file
 */
FILE * macsim_open(char * nom_fichier_macsim);

/**
 * close macsim file
 * 
 * @param[in] fichier_macsim the pointer file
 *                      
 * @return nothing
 */
void macsim_close(FILE * fichier_macsim);

/**
 * Read the MACSIM header.
 * If justcount is 1 then only count the number of symbols of this macsim file.
 * 
 * @param[in] genreader the generic reader (contain data information (pointer file...)
 * @param[in] justcount indicate we just want to count the number of symbol in the file.
 *                      
 * @return TSP_STATUS_OK if OK 
 */
int32_t macsim_read_header(GenericReader_T * genreader, int32_t justcount);

/**
 * Read the data contained in the file into a GLU item.
 * 
 * @param[in] genreader the generic reader (contain data information (pointer file....)
 * @param[in,out] item the GLU item where the data will be put
 * @return EOF if end of file
 */
int32_t macsim_read(GenericReader_T * genreader,glu_item_t* item);

/**
 * Read one data of macsim file
 * 
 * @param[in] fic the file pointer
 * @param[in,out] data_var contain the read data         
 * @return END_FILE_MACSIM if end of file
 *         END_SAMPLE_SET if end of MACSIM line
 *         END_SAMPLE_STREAM if end of read data
 */
int32_t read_data_file(FILE *fic,char *data_var);

/**
 * oonvert string to double
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_double(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to float
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_float(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to int8
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_int8(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to int16
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_int16(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to int32
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_int32(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to int64
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_int64(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to uint8
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_uint8(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to uint16
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_uint16(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to uint32
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_uint32(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to uint64
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_uint64(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to char
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_char(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to uchar
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_uchar(char* data_var,void* raw_value,const uint32_t indice_data);

/**
 * oonvert string to raw value
 * 
 * @param[in] data_var string data
 * @param[in,out] raw_value  contain the convert data         
 * @param[in]     indice_data indice in the array if the symbol is an array
 * @return TSP_STATUS_OK if ok
 */
int32_t load_type_raw(char* data_var,void* raw_value,const uint32_t indice_data);

/** @} */

#endif
