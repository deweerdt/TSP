/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/consumers/ascii_writer/tsp_ascii_writer.h,v 1.1 2004-09-21 21:59:58 erk Exp $

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

Purpose   : TSP ascii writer consumer

-----------------------------------------------------------------------
 */
#ifndef _TSP_ASCII_WRITER_H_
#define _TSP_ASCII_WRITER_H_

#include "tsp_abs_types.h"
#include "tsp_consumer.h"
#include <pthread.h>
#include <stdio.h>

/**
 * @defgroup Ascii_Writer
 * A TSP ascii writer consumer.
 */

extern pthread_cond_t  tsp_ascii_writer_condvar;
extern pthread_mutex_t tsp_ascii_writer_mutex;
extern int tsp_ascii_writer_nb_line;
extern int tsp_ascii_writer_nb_var;
extern int tsp_ascii_writer_current_var;

/**
 * Initialise ascii TSP consumer.
 * We must pass main arguments to TSP lib for
 * specific TSP arg handling.
 * Parameter unknown to TSP are unchanged on return.
 * 
 * @param argc The main 'argc' argument
 * @param argv The main 'argv' argument
 * @return 0 if init OK  -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_initialise(int* argc, char** argv[]);

/**
 * Add a variable to the global symbols array
 * used by the Lex+Yacc lexer+parser.
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
 * @param conffilename IN, the name of the config file.
 * @param tsp_symbols OUT, pointer to the array of symbols found in config file.
 *                         the array is allocated by the function.
 * @param nb_symbols OUT, the number of symbols found in file
 * @return 0 if config file loaded properly (no syntax error) -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_load_config(const char* conffilename, 
			     TSP_consumer_symbol_requested_t**  tsp_symbols,
			     int32_t* nb_symbols);
/**
 * Validate the requested symbol against the TSP provider 
 * located on tsp_provider_hostname.
 * If symbol name match an array symbol all element aoff the array
 * are requested.
 * @param tsp_symbols IN/OUT,  array of symbols to be validated.
 * @param nb_symbols IN, the size of the tsp_symbols array.
 * @param tsp_provider_hostname IN, TSP provider hostname used for validating symbols.
 * @param tsp_symbol_list TSP IN/OUT, TSP validated symbol list.
 * @return 0 OK -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_validate_symbols(TSP_consumer_symbol_requested_t*  tsp_symbols,
				  int32_t nb_symbols,
				  const char* tsp_provider_hostname,
				  TSP_consumer_symbol_requested_list_t* tsp_symbol_list);

/**
 * Start TSP data receive and archive.
 * Should have called tsp_ascii_writer_validate_symbols first.
 * @param sfile IN, the stream file used for data saving.
 * @param nb_sample_max_infile IN, the maximum number of sample stored in the file
 *                                 if 0<= then no limit, if >0 then when about
 *                                 to save the nb_sample_max_infile-th sample
 *                                 we rewind the file.
 * @return 0 OK -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_start(FILE* sfile, int32_t nb_sample_max_infile);

/**
 * Function to be used with pthread_create(3).
 * @param files pointer to an opened stdio file stream (FILE*).
 * @return 0 if OK -1 otherwise.
 * @ingroup Ascii_Writer
 */
void*
tsp_ascii_writer_thread(void* sfile);

/**
 * Stop data archiving.
 * @return 0 if OK -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_stop();

/**
 * Finalise TSP consumer.
 * @return 0 if OK -1 otherwise.
 * @ingroup Ascii_Writer
 */
int32_t 
tsp_ascii_writer_finalise();


#endif /* _TSP_ASCII_WRITER_H_ */
