/*

$Id: generic_reader.c,v 1.1 2006-03-21 09:56:59 morvan Exp $

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

#include <tsp_sys_headers.h>
#include <generic_reader.h>

/* create handler for file type
*/
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
int genreader_create(GenericReader_T** genreader,FmtHandler* fmtHandler)
{
	*genreader=(GenericReader_T*)malloc(sizeof(GenericReader_T));
	(*genReader)->handler=fmtHandler;

	
	return(0);
	
}

/* open file
*/
int genreader_open(GenericReader_T* genreader)
{
	//ouverture du fichier
	genReader->handler->file=fmtHandler->open(genReader->handler->fileName);
	
	return(0);
	
}

/* first read for create BB
*/
int genreader_read_header_create_bb(GenericReader_T* genreader)
{
	S_BB_T*       mybb;
	
	//appel pour calculer le nb de variable et leur type
	fmtHandler->read_header(genReader,JUSTCOUNT_SIZE_BB);
	
	/* Create BB */
	/***************/
	if (BB_NOK==bb_create(&mybb,"GENREADER",genreader->nbSymbol,genreader->symbolSize)) 
	{
		bb_destroy(&mybb); 
		bb_create(&mybb,"GENREADER",genreader->nbSymbol,genreader->symbolSize);
	}
	genreader->myBB=mybb;
	
	return(0);
	
}


/*second read for create symbol
*/
int genreader_read_header_create_symbole(GenericReader_T* genreader)
{
	
	fmtHandler->read_header((*genReader)->handler->file, (*genReader),ADDSYMBOLE_TO_BB);
	
	return(0);
	
}

/* read data file
*/
int genreader_addvar_bb(char* symbname, int dimension, int type_len, E_BB_TYPE_T type, char* unit)
{
	S_BB_DATADESC data_desc;

	/* Publish data in the BB */
	/**************************/ 
	strcpy(data_desc.name,symbname);
	data_desc.type=type;
	data_desc_type_size=type_len;
	data_desc.dimension=dimension;
					
	data_desc.data_offset=0;
	data_desc.alias_target=0;
					
	/* add data to the BB
	*/
  	if(NULL==bb_publish(mybb,&data_desc))
	{
		/* publish error
		*/
		fprinf(stderr,"ERROR: not enough memory or existing data!!!!!");
	}


	return(0);
}

/* write data in the BB
*/
int genreader_write(GenericReader_T* genreader, char* data_var, int indice)
{

	S_BB_DATADESC* data_desc;
	
	data_desc = bb_data_desc(genreader->mybb);
	
	/* add data to the BB
	*/	
	bb_value_write(genreader->mybb,data_desc[indice],data_var,NULL,0);
	
	return(0);
}

/* send data to the BB
*/
int genreader_synchro(GenericReader_T* genreader)
{
	bb_simple_synchro_go(genreader->mybb,BB_SIMPLE_MSGID_SYNCHRO_COPY);
	return(0);
}


int genreader_finalize(GenericReader_T* genreader)
{
	genreader->fmt_handler->close(genreader->fmt_handler->file);
	free(genreader->fmt_handler->fileName);
	free(genreader->fmt_handler);
	free(genreader);
	return(0);
}

int32_t 
genreader_run(int TSPRunMode) {
  
  int32_t retcode;

  /* demarrage provider */
  TSP_provider_run(TSPRunMode);
  /* 
   * Si demarrage en mode non bloquant 
   * attendre demarrage thread provider
   * FIXME ce mode "d'attente" est pourlingue il faut une
   * API TSP pour gérer ces synchros de démarrage de thread.
   */
  if (TSP_ASYNC_REQUEST_NON_BLOCKING & TSPRunMode) {
    sleep(1);
    sched_yield();  
    while (TSP_provider_rqh_manager_get_nb_running()<1) {
      printf("Waiting TSP provider to start...\n");
      fflush(stdout);
      sleep(1);
      sched_yield();    
    } 
  }  
  return retcode;
} /* end of genreader_run */
