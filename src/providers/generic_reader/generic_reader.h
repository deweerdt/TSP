/*

$Id: generic_reader.h,v 1.3 2006-04-23 15:37:48 erk Exp $

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

Purpose   : Allow the output of a datapool of symbols from generic file

-----------------------------------------------------------------------
*/


#ifndef _GENERIC_READER_H_
#define _GENERIC_READER_H_

#include <stdio.h>
#include <tsp_datastruct.h>
#include <tsp_abs_types.h>
#include <tsp_glu.h>

/**
 * @defgroup TSP_GenericReaderLib Generic Reader  Library 
 * The TSP generic reader provider library API.
 * @ingroup TSP_GenericReader
 * @ingroup TSP_Libraries
 * @{
 */

#define JUSTCOUNT_SIZE 1
#define ADDSYMBOLE  0


#define FICHIER_MACSIM "f"
#define FICHIER_BACH "b"

/*typedef int   (*add_symbol_ft)(char* symname, int* dimension, int dim_len, int type, char* unit);*/

struct GenericReader;

typedef FILE*  (*fmtHandler_open_ft)(char* nom_fichier);
typedef void  (*fmtHandler_close_ft)(FILE* fichier);
typedef int32_t   (*fmtHandler_readHeader_ft)(struct GenericReader* genreader, int32_t justcount);
typedef int32_t   (*fmtHandler_readValue_ft)(struct GenericReader* genreader, glu_item_t* item);

typedef struct FmtHandler {
		char* fileName;
		FILE* file;
		fmtHandler_open_ft open_file;
		fmtHandler_close_ft close_file;
		fmtHandler_readHeader_ft read_header;
		fmtHandler_readValue_ft read_value;
} FmtHandler_T;

#define END_SAMPLE_SET       11
#define END_SAMPLE_STREAM    12

typedef struct GenericReader {
	  int32_t          nbSymbol;
	  int32_t          max_size_raw_value;
	  FmtHandler_T*    handler;
          
          TSP_sample_symbol_info_list_t*          ssi_list;
          TSP_sample_symbol_extended_info_list_t* ssei_list;

} GenericReader_T;


/**
 * create the handler for a format file
 * 
 * @param[in] format_file the file format to treat
 * @param[in] file_name the file name
 *                      
 * @return the fmtHandler
 */
FmtHandler_T* genreader_createFmHandler(char* format_file,char* file_name);


/**
 * create the generic reader
 * 
 * @param[out] genreader the generic reader to intialize
 * @param[in] fmtHandler the file handler
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t genreader_create(GenericReader_T** genreader,FmtHandler_T* fmtHandler);

/**
 * open the file to treat
 * 
 * @param[in] genreader the generic reader who contain the information about the file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t genreader_open(GenericReader_T* genreader);

/**
 * create the symbol info and extended info list
 * 
 * @param[in] genreader the generic reader who contain the information about the file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t genreader_read_header(GenericReader_T* genreader);

/**
 * add symbol info and extended info to the symbol and extended list
 * 
 * @param[in] genreader the generic reader who contain the information about the file
 *                      
 * @return TSP_STATUS_OK  if OK
 */
int32_t genreader_read_header_create_symbole(GenericReader_T* genreader);


/**
 * destroy the fmtHandler and generic reader
 *                   
 * @return TSP_STATUS_OK if OK
 */
int32_t genreader_finalize();

/**
 * Initialize Blackboard TSP provider Library for generic reader.
 * 
 * @param[in] TSPRunMode running mode, 0 = non blocking, 1 = blocking (never return).
 *
 * @return E_OK if initialization is successfull  E_NOK otherwise.
 */		   
int32_t 
genreader_run(int TSPRunMode);

/** @} */

#endif
