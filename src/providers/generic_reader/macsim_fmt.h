/*

$Id: macsim_fmt.h,v 1.2 2006-03-21 15:20:11 morvan Exp $

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

#define LG_MAX_CHAINE        128
#define MAX_BUFFER_MACSIM    500
#define MAX_NOM_VAR_MACSIM   100
#define MAX_DIMENSION_MACSIM 100
#define MAX_TYPE_MACSIM      20
#define MAX_UNITE_MACSIM     10

#define CARACTERE_TAB        '	'
#define CARACTERE_BLANC      ' '

#define MAX_DATA_LEN         100

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
 * extract the space from a string.
 * 
 * @param[in] string the string to modify
 *                      
 * @return the same string without the spaces
 */
char *str_strip (const char *string);

/**
 * calculate the dimension of a macsim's symbol
 * 
 * @param[in] dimension_var the string who contain the dimension
 *                      
 * @return the dimension
 */
uint32_t macsim_dimension_data(char* dimension_var);

/**
 * determine BB type and size type.
 * 
 * @param[in] type_var the data type extract from macsim file 
 * @param[out] type_var_bb the BB type container
 *                      
 * @return the type's size and the BB data type
 */
size_t macsim_size_data(char* type_var,E_BB_TYPE_T* type_var_bb);

/**
 * initialize the fmtHandler (for generic handler)
 * 
 * @param[out] fmtHandler the fmtHandler to initialze
 *                      
 * @return 0 if OK
 */
int macsim_createHandler(FmtHandler_T** fmtHandler);

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
 * Count the size of the BB for this creation and create the symbol of the BB by reading the header file.
 * 
 * @param[in] genreader the generic reader ( contain data information (pointer file...)
 * @param[in] justcount Indicate if we want count the size of the BB or create the symbol of the BB
 *                      
 * @return 0 if OK 
 */
int macsim_read_header(GenericReader_T* genreader, int32_t justcount);

/**
 * Read the data containing in the file and send it to the BB.
 * 
 * @param[in] genreader the generic reader (contain data information (pointer file....)
 *                      
 * @return 0 if OK
 */
int macsim_read(GenericReader_T* genreader);

/** @} */

#endif
