/*

$Id: glu_genreader.c,v 1.2 2006-04-23 20:06:48 erk Exp $

-----------------------------------------------------------------------
 
TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Eric NOULARD and Arnaud MORVAN

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

Purpose   : Implementation for the glue_server

-----------------------------------------------------------------------
*/
#include <string.h>

#include <tsp_sys_headers.h>
#include <tsp_glu.h>
#include <tsp_ringbuf.h>
#include <tsp_time.h>
#include <tsp_datapool.h>
#include <generic_reader.h>


#define _LIBUTIL_REENTRANT 1
#include "libUTIL.h"

#define GLU_STREAM_INIT_USAGE "TSP RES Reader usage : filename[.res]  [-eof N]\n\t\twait N seconds after EOF to check file increment, default = 0 (NO WAIT)\n"
#define GLU_RES_FILE_ARG_NUMBER 1
#define GLU_RES_WAIT_EOF       	3

static int _wait_eof=0;
static int _started = FALSE;
static GLU_handle_t* gen_GLU = NULL;

struct GLU_state_t
{
  void* res_values;
  int time_stamp;
  int nbvar;
  int use_dbl;
  double freq;
  d_rhandle h_res;
  TSP_sample_symbol_info_t* sample_symbol_info_list_val;
};

typedef struct GLU_state_t GLU_state_t;

GLU_state_t glu_handler;


int 
GENREADER_GLU_start(GLU_handle_t* this)
{
  int32_t rep;
  uint32_t indice_pgi=0;
  glu_item_t  item;

  GenericReader_T* genreader = (GenericReader_T*) (this->private_data);


  item.time=0;
  item.provider_global_index=0;
  item.raw_value=calloc(1,genreader->max_size_raw_value);

  assert(item.raw_value);
 
  /*  tant qu'il y a des valeurs */
  /* lire le fichier et envoyer au GLU */
  while(EOF!=(rep=genreader->handler->read_value(genreader,&item)))
  {

    TSP_datapool_push_next_item(this->datapool,&item);
  
    ++item.provider_global_index;

    if(END_SAMPLE_SET==rep)
    {
      TSP_datapool_push_commit(this->datapool, item.time, state);  
      ++item.time;
      item.provider_global_index=0;
      item.size=0;
      memset(item.raw_value,'\0',genreader->max_size_raw_value);
    }
  }

  free(item.raw_value);

  return TRUE;
}

GLU_handle_t*
GENREADER_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[])
{

  char *error_info;

  return GENREADER_GLU_get_instance(this, fallback_argc,fallback_argv,&error_info);
  
}


int  GENREADER_GLU_get_sample_symbol_info_list(GLU_handle_t* h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  GenericReader_T* genreader = (GenericReader_T*) (h_glu->private_data);


  symbol_list->TSP_sample_symbol_info_list_t_len =genreader->ssi_list->TSP_sample_symbol_info_list_t_len;
  symbol_list->TSP_sample_symbol_info_list_t_val =genreader->ssi_list->TSP_sample_symbol_info_list_t_val ;
	    
  return TRUE;
}


int32_t  GENREADER_GLU_get_sample_symbol_extended_info_list(GLU_handle_t* h_glu,
							    int* pgis, 
							    int32_t pgis_len, 
							    TSP_sample_symbol_extended_info_list_t* ssei_list)
{
  uint32_t i;
  uint32_t j;
  uint32_t nb_info=0;

  GenericReader_T* genreader = (GenericReader_T*) (h_glu->private_data);

  assert(ssei_list);
  
  ssei_list->TSP_sample_symbol_extended_info_list_t_len=pgis_len;
  ssei_list->TSP_sample_symbol_extended_info_list_t_val=
    (TSP_sample_symbol_extended_info_t *)calloc(1,sizeof(TSP_sample_symbol_extended_info_t)*pgis_len);

  for(i=0;i<genreader->ssei_list->TSP_sample_symbol_extended_info_list_t_len;++i)
  {
    for(j=0;j<pgis_len;++j)
    {
      if(genreader->ssei_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index==pgis[j])
      {

	TSP_SSEI_copy(ssei_list->TSP_sample_symbol_extended_info_list_t_val[nb_info],
		      genreader->ssei_list->TSP_sample_symbol_extended_info_list_t_val[i]);
	++nb_info;
	break;
      }

    }

  }
    
  return TRUE;
}


double GENREADER_GLU_get_base_frequency(GLU_handle_t* this)
{
  GLU_state_t* obj = &glu_handler;
  /* Server is passive, frequency is computed in GLU_init */
  return obj->freq;
}

GLU_handle_t* 
GENREADER_GLU_get_instance(GLU_handle_t* this,
		     int custom_argc,
		     char* custom_argv[],
		     char** error_info) {
  
  int ret = TRUE;
  GLU_state_t* obj = &glu_handler;
  int i;
  double t1, t2;
  int nbrec;  
  char namev[RES_NAME_LEN];
  char descv[RES_DESC_LEN];
  int  opt_ok=1;
  char c_opt;


  if (argc < 2) {
    opt_ok  = 0;
    retcode = -1;
    fprintf(stderr,"%s: Insufficient number of options\n",argv[0]);
  }

  /* Analyse command line parameters */
  c_opt = getopt(argc,argv,"x:f");

  if(opt_ok && EOF != c_opt)
  {
    opt_ok  = 1;
    do
    {    
      /* Analyse command line parameters */
  			/*-------*/
      switch (c_opt) 
      {
    	case 'x':
      		input_filename = strdup(optarg);
      		fprintf(stdout,"%s: source file is <%s>\n",argv[0],input_filename);
      		break;
	case 'f':
      		format_file = strdup(optarg);
      		fprintf(stdout,"%s: format file is <%s>\n",argv[0],format_file);
      		break;
    	case '?':
      		fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
      		opt_ok = 0;
      		break;
    	default:
      		opt_ok = 0;
      		break;
      } /* end of switch */    
      c_opt = getopt(argc,argv,"x:f");  
    }
    while (opt_ok && (EOF != c_opt));
  }
  else
  {
    opt_ok = 0;
  }
	 
  if (!opt_ok) 
  {
    	printf("Usage: %s -x=<source_file> [-f <format]>\n", argv[0]);
    	printf("   -x   determine the source file\n");
   	printf("   -f   specifying the format of source file\n");
    	return NULL;
  }

  fmt_handler=genreader_createFmHandler(format_file,input_filename);
		
  genreader_create(&(this->private_data),fmt_handler);	
  genreader_open((GenericReader_T*)(this->private_data));	
  genreader_read_header(generic_reader);	
  genreader_read_header_create_symbole(generic_reader);
 
  return this;

} /* end of GLU_get_instance_default */

GLU_handle_t* GENREADER_GLU_create() {
  
  /* create a default GLU */
  GLU_handle_create(&gen_GLU,"GenReaderServer",GLU_SERVER_TYPE_PASSIVE,1.0);
  
  gen_GLU->initialize         = &GENREADER_GLU_init;
  gen_GLU->get_ssi_list       = &GENREADER_GLU_get_sample_symbol_info_list;
  gen_GLU->get_ssei_list       = &GENREADER_GLU_get_sample_symbol_extended_info_list;
  /* override default method */
  gen_GLU->get_base_frequency = &GENREADER_GLU_get_base_frequency;
  gen_GLU->start              = &GENREADER_GLU_start;
  /* FIXME seems that resreader should have been multi-instance but has never been? */
  gen_GLU->get_instance       = &GENREADER_GLU_get_instance;

  return gen_GLU;
}
