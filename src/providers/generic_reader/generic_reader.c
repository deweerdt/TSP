/*

$Id: generic_reader.c,v 1.4 2006-05-05 14:24:56 erk Exp $

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

Purpose   : Implementation for the generic reader

-----------------------------------------------------------------------
*/
#include <string.h>
#include <stdio.h>

#include <tsp_sys_headers.h>
#include <tsp_rpc.h>
#include <generic_reader.h>
#include <macsim_fmt.h>
#include <tsp_common.h>
#include <tsp_provider_init.h>

FmtHandler_T* genreader_createFmHandler(char* format_file,char* file_name)
{
  FmtHandler_T* fmt_handler;
		
  if(0==strcmp(format_file,FICHIER_MACSIM))
  {
    macsim_createHandler(&fmt_handler);					
  }
  else
  {
    if(0==strcmp(format_file,FICHIER_BACH))
      fmt_handler=NULL;			
    else
      fmt_handler=NULL;
  }
	
  fmt_handler->fileName=(char*)malloc(strlen(file_name) + 1);
  strcpy(fmt_handler->fileName,file_name);
	
  return(fmt_handler);
}

/* create generique handler for treatement file
*/
GenericReader_T* genreader_create(FmtHandler_T * fmtHandler)
{

  GenericReader_T* genreader;

  genreader=(GenericReader_T*)malloc(sizeof(GenericReader_T));
  genreader->handler=fmtHandler;

	
  return(genreader);
	
}

/* open file
*/
int32_t genreader_open(GenericReader_T* genreader)
{
	/* open the file*/
	genreader->handler->file=genreader->handler->open_file(genreader->handler->fileName);
	
	return(TSP_STATUS_OK);
	
}

/* first read for count sybol number
*/
int32_t genreader_read_header(GenericReader_T* genreader)
{

	genreader->handler->read_header(genreader,JUSTCOUNT_SIZE);
		
	return(TSP_STATUS_OK);
	
}


/*second read for create symbol
*/
int32_t genreader_read_header_create_symbole(GenericReader_T* genreader)
{

        genreader->ssi_list=TSP_SSIList_new(genreader->nbsymbol);

	genreader->ssei_list=TSP_SSEIList_new(genreader->nbsymbol);

	genreader->handler->read_header(genreader,ADDSYMBOLE);
	
	return(TSP_STATUS_OK);
	
}

/*destruction*/
int32_t genreader_finalize(GenericReader_T* genreader)
{
	genreader->handler->close_file(genreader->handler->file);
	free(genreader->handler->fileName);
	free(genreader->handler);

	TSP_SSIList_finalize(genreader->ssi_list);

        TSP_SSEIList_finalize(genreader->ssei_list);

	free(genreader);
	return(TSP_STATUS_OK);
}
