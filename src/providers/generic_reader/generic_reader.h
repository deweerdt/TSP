/*

$Id: generic_reader.h,v 1.1 2006-03-21 09:56:59 morvan Exp $

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

/**
 * @defgroup TSP_GenericReaderLib Generic Reader  Library 
 * The TSP generic reader provider library API.
 * @ingroup TSP_GenericReader
 * @ingroup TSP_Libraries
 * @{
 */

#define JUSTCOUNT_SIZE_BB 1
#define ADDSYMBOLE_TO_BB  0




typedef int   (*add_symbol_ft)(char* symname, int* dimension, int dim_len, int type, char* unit);

typedef FILE* (*fmtHandler_open_ft)(char* nom_fichier);
typedef void  (*fmtHandler_close_ft)(FILE* fichier);
typedef int   (*fmtHandler_readHeader_ft)(GenericReader_T* genreader, int32_t justcount);
typedef int   (*fmtHandler_readValue_ft)(GenericReader_T* genreader);

typedef struct FmtHandler {
		char* fileName;
		FILE* file;
		fmtHandler_open_ft* open;
		fmtHandler_close_ft* close;
		fmtHandler_readerHeader_ft* read_header;
		fmtHandler_readValue_ft* read;
} FmtHandler_T;

typedef struct GenericReader {
	  int32_t          nbSymbol;
	  int32_t          symbolSize;
	  add_symbol_ft*   add_symbol;
	  FmtHandler_T*    handler;
	  S_BB_T*          myBB;
} GenericReader_T;


/**
 * create the handler for a format file
 * 
 * @param format_file the file format to treat
 * @param file_name the file name
 *                      
 * @return the fmtHandler
 */
FmtHandler_T* genreader_createFmHandler(char* format_file,char* file_name);


/**
 * create the generic reader
 * 
 * @param genreader the generic reader to intialize
 * @param fmtHandler the file handler
 *                      
 * @return 0 if OK
 */
int genreader_create(GenericReader_T** genreader,FmtHandler* fmtHandler);

/**
 * open the file to treat
 * 
 * @param genreader the generic reader who contain the information about the file
 *                      
 * @return 0 if OK
 */
int genreader_open(GenericReader_T* genreader);

/**
 * create the BB
 * 
 * @param genreader the generic reader who contain the information about the file
 *                      
 * @return 0 if OK
 */
int genreader_read_header_create_bb(GenericReader_T* genreader);

/**
 * create the symbol read in the file to the BB
 * 
 * @param genreader the generic reader who contain the information about the file
 *                      
 * @return 0 if OK
 */
int genreader_read_header_create_symbole(GenericReader_T* genreader);

/**
 * add symbol to the BB
 * 
 * @param symbname the symbol name
 * @param dimension the symbol dimension
 * @param type_len  the type size
 * @param type      the BB type
 * @param unit      the symbol unit
 *                      
 * @return 0 if OK
 */
int genreader_addvar(char* symbname, int dimension, int type_len, E_BB_TYPE_T type, char* unit);

/**
 * open the file to treat
 * 
 * @param genreader the generic reader who contain the information about the file
 * @param data_var  the data read in the file
 * @param indice    index of the corresponding symbol
 *                      
 * @return 0 if OK
 */
int genreader_write(GenericReader_T* genreader, char* data_var, int indice);

/**
 * sned a synchro to the BB
 * 
 * @param genreader the generic reader who contain the information about the file
 *                      
 * @return 0 if OK
 */
int genreader_synchro(GenericReader_T* genreader);

/**
 * destroy the fmtHandler and generic reader
 * 
 *                      
 * @return 0 if OK
 */
int genreader_finalize();

/**
 * Initialize Blackboard TSP provider Library for generic reader.
 * 
 * @param TSPRunMode running mode, 0 = non blocking, 1 = blocking (never return).
 *
 * @return E_OK if initialization is successfull  E_NOK otherwise.
 */		   
int32_t 
genreader_run(int TSPRunMode);

/** @} */

#endif
