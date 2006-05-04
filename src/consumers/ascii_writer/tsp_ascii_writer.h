/*

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer.h,v 1.14 2006-05-04 21:44:47 erk Exp $

------------------------------------------------------------------------

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

Purpose   : TSP ascii writer consumer

-----------------------------------------------------------------------
 */
#ifndef _TSP_ASCII_WRITER_H_
#define _TSP_ASCII_WRITER_H_

#include <pthread.h>
#include <stdio.h>

#include <tsp_abs_types.h>
#include <tsp_consumer.h>

/**
 * @defgroup TSP_AsciiWriterLib ASCII Writer Library 
 * The TSP ascii writer consumer library API.
 * @ingroup TSP_AsciiWriter
 * @ingroup TSP_Libraries
 * @{
 */

extern pthread_cond_t  tsp_ascii_writer_condvar;
extern pthread_mutex_t tsp_ascii_writer_mutex;
extern int tsp_ascii_writer_parse_error;
extern int tsp_ascii_writer_lineno;
extern int tsp_ascii_writer_colno;
extern int tsp_ascii_writer_nb_var;
extern int tsp_ascii_writer_current_var;

typedef int32_t  (* tsp_ascii_writer_logMsg_ft)(char* fmt, ...);

void tsp_ascii_writer_set_logMsgCB(tsp_ascii_writer_logMsg_ft logMsgCB);

int32_t tsp_ascii_writer_logMsg_stdout(char* fmt, ...);

/**
 * Output file format handled by the ascii_writer
 */
typedef enum OutputFileFormat {
  /** 
   * The simple ASCII file format
   */
  SimpleAsciiTabulated_FileFmt=0,
  MACSIM_FileFmt,
  BACH_FileFmt,
  LAST_FileFmt
} OutputFileFormat_t;

#ifdef ASCII_WRITER_C
const char* OutputFileFormat_name_tab[] = {"simple_ascii",
					   "macsim",
					   "bach",
					   "LAST"
};
const char* OutputFileFormat_desc_tab[] = {"Simple tabulated ASCII format",
					   "CNES MACSIM file format",
					   "CNES BACH file format",
					   "LAST"
};
#else
extern const char* OutputFileFormat_name_tab[];
extern const char* OutputFileFormat_desc_tab[];
#endif

/**
 * Defines Ascii writer specific error codes 
 */
#define TSP_STATUS_ERROR_AW_CONFIG_FILE_INVALID      TSP_STATUS_ERROR_CUSTOM_BEGIN+1
#define TSP_STATUS_ERROR_AW_CONFIG_FILE_PARSE_ERROR  TSP_STATUS_ERROR_CUSTOM_BEGIN+2
#define TSP_STATUS_ERROR_AW_DUPLICATE_SYMBOLS        TSP_STATUS_ERROR_CUSTOM_BEGIN+3
#define TSP_STATUS_ERROR_AW_OUTPUT_FILE_ERROR        TSP_STATUS_ERROR_CUSTOM_BEGIN+4

/**
 * Initialise ascii TSP consumer.
 * We must pass main arguments to TSP lib for
 * specific TSP arg handling.
 * Parameter unknown to TSP are unchanged on return.
 * 
 * @param argc The main 'argc' argument
 * @param argv The main 'argv' argument
 * @return TSP_STATUS_OK on success TSP_STATUS_ERROR_xxx  otherwise.
 */
int32_t 
tsp_ascii_writer_initialise(int* argc, char** argv[]);

/**
 * Add a variable to the global symbols array
 * used by the Lex+Yacc lexer+parser.
 * @param symbol_name IN, the symbol name to add to the list of requested symbol.
 */
int32_t
tsp_ascii_writer_add_var(char* symbol_name);

int32_t
tsp_ascii_writer_add_var_period(int32_t period);
int32_t 
tsp_ascii_writer_add_comment(char* comment);


/**
 * Load a configuration file and build the array of
 * of requested symbol in config file.
 * @param[in] conffilename the name of the config file.
 * @param[out] tsp_symbols  pointer to the array of symbols found in config file.
 *                         the array is allocated by the function.
 * @param[out] nb_symbols  the number of symbols found in file
 * @return TSP_STATUS_OK if config file loaded properly (no syntax 
 *         TSP_STATUS_ERROR_xxx otherwise.
 */
int32_t 
tsp_ascii_writer_load_config(const char* conffilename, 
			     TSP_sample_symbol_info_t**  tsp_symbols,
			     uint32_t* nb_symbols);

/**
 * Make the requested symbol unique in the provided list.
 * The unicity may be enforced by reducing the number 
 * of requested symbols if ever duplicate symbols have
 * same name, period and phase.
 * @param[in,out] tsp_symbols pointer to the array of symbols 
 *                           to be checked for unicity on entry,
 *                           the array of unique symbols on return.
 * @param[in,out] nb_symbols the number of symbols in the provided array on entry
 *                          the number of remaining (unique) symbols on return.
 *                          If ERROR nb_symbols contains the rank of the first
 *                          wrong duplicate.
 * @return TSP_STATUS_OK if uniticy may be enforced or TSP_STATUS_ERROR_AW_DUPLICATE_SYMBOLS.
 */
int32_t 
tsp_ascii_writer_make_unique(TSP_sample_symbol_info_t**  tsp_symbols,
			     uint32_t* nb_symbols);

/**
 * Validate the requested symbol against the TSP provider specified by tsp_url.
 * If symbol name match an array symbol all element aoff the array
 * are requested.
 * @param[in] requestedSSIL  the requested symbols list to be validated.
 * @param[in] tsp_url   The TSP url of the provider to be used for validating symbols.
 * @param[out] validatedSSIL  the validated symbol list.
 * @return TSP_STATUS_OK on success TSP_STATUS_ERROR_xxx otherwise.
 */
int32_t 
tsp_ascii_writer_validate_symbols(TSP_sample_symbol_info_list_t* requestedSSIL,
				  const char* tsp_url,
				  TSP_sample_symbol_info_list_t* validatedSSIL);

/**
 * Start TSP data receive and archive.
 * Should have called tsp_ascii_writer_validate_symbols first.
 * @param[in] sfile the stream file used for data saving.
 * @param[in] nb_sample_max_infile the maximum number of sample stored in the file
 *                                 if 0<= then no limit, if >0 then when about
 *                                 to save the nb_sample_max_infile-th sample
 *                                 we rewind the file.
 * @param[in] file_format  the header style to be used for the output. 
 * @param[in] validatedSSIL the validated list of symbols to retrieve
 * @return 0 OK -1 otherwise.
 */
int32_t 
tsp_ascii_writer_start(FILE* sfile, int32_t nb_sample_max_infile, OutputFileFormat_t file_format,
		       TSP_sample_symbol_info_list_t* validatedSSIL);


/**
 * Function to be used with pthread_create(3).
 * @param sfile pointer to an opened stdio file stream (FILE*).
 * @return 0 if OK -1 otherwise.
 */
void*
tsp_ascii_writer_thread(void* sfile);

/**
 * Stop data archiving.
 * @return 0 if OK -1 otherwise.
 */
int32_t 
tsp_ascii_writer_stop();

/**
 * Finalise TSP consumer.
 * @return 0 if OK -1 otherwise.
 */
int32_t 
tsp_ascii_writer_finalise();

/**
 * display the value of the sample.
 * @param sfile [in] print file.
 * @param sample [in] sample value to display
 * @return 0 OK -1 otherwise.
 */
int32_t 
tsp_ascii_writer_display_value(FILE* sfile,TSP_sample_t sample);

/**
 * edit the column title for the array.
 * @param[in] libelle libelle to display
 * @param[in] profil  profil of the array (5*3*2)
 * @param[in] ordre   order of the indice (2,1) or (1,2)
 * @param[in] recursif if it's recursif or no
 * @return pinter on the different label
 */
char* new_array_label(const char* libelle,const char* profil, const char* ordre, const int recursif);

/** 
 * Read a sample symbol.
 * @param provider The provider handle
 * @param sample The returned symbol if there is one
 * @param new_sample When TRUE, there is a new sample, else the sample value is
 *                    meaningless
 * @return TSP_STATUS_OK on success TSP_STATUS_ERROR_xxx on error.
 * @todo
 *  - Some kind of get_last_error func must be implemented to read
 *    the error codes (EOF, RECONF ... ).
 *  - When the other types will be implemented (RAW, STRING) the TSP_sample_t
 *    type will not work anymore as it is double specific for now.
 */				          
int32_t 
TSP_asciiwriter_read_sample(TSP_provider_t provider,
			 TSP_sample_t* sample,
			 int* new_sample);

/** @} */

#endif /* _TSP_ASCII_WRITER_H_ */
