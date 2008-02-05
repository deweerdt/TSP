/*

$Id: glue_stub.c,v 1.30 2008-02-05 18:54:12 rhdv Exp $

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
Maintainer: tsp@astrium-space.com
Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the glue_server, for stub test
	    Allow the output of a datapool of 1000 symbols, 
	    cyclically generated at 100Hz

-----------------------------------------------------------------------
*/
#include <string.h>

#include <tsp_sys_headers.h> /* platform independant data types definition */
#include <tsp_glu.h>         /* TSP GLU object definition and API */
#include <tsp_ringbuf.h>
#include <tsp_time.h>
#include <tsp_datapool.h>   /* TSP provider datapool API */
#include <calc_func.h>
#include <tsp_common.h>     /* TSP common structure manipulation API */

/* TSP glue server defines */
#define GLU_MAX_SYMBOLS_DOUBLE 1000
#define GLU_MAX_SYMBOLS_NOT_DOUBLE 12

/* Nasty static variables */
static TSP_sample_symbol_info_list_t X_SSI_list;
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;
static tsp_hrtime_t X_nexttime;
static time_stamp_t my_time = 0;
static GLU_handle_t* stub_GLU = NULL;
static int32_t taille_max_symbol=0;

/*
 * The GLU->run member function.
 */
void* STUB_GLU_thread(void* athis)
{
  int i,j, symbols_nb, *ptr_index;
  tsp_hrtime_t current_time;
  glu_item_t item;
  double memo_val[GLU_MAX_SYMBOLS_DOUBLE]; /*for debug informatin */
  GLU_handle_t* cthis  = (GLU_handle_t*) athis;
  int tsp_usleep_period_us = (int)(1000000/(cthis->get_base_frequency(cthis)));

  current_time = X_nexttime = tsp_gethrtime();  

  item.raw_value=calloc(1,taille_max_symbol);
  assert(item.raw_value);

  /* Infinite loop for 
   * our pseudo simulator 
   * symbols generation 
   */
  while(1) {

      /* Must be call at each step in case of new samples wanted */
      TSP_datapool_get_reverse_list(cthis->datapool,&symbols_nb, &ptr_index); 

      for (i = 0 ; i <  symbols_nb ; i++) {
	 
	  int index=ptr_index[i];
	  if (my_time%X_sample_symbol_info_list_val[index].period == 0) {
	      item.size = X_sample_symbol_info_list_val[index].dimension * tsp_type_size[X_sample_symbol_info_list_val[index].type];
 
	      item.time = my_time;
	      item.provider_global_index = index;

	      /* PGI 0 is a pseudo time */
	      if (0==index) {
		*((double*)item.raw_value) = (double)(my_time) / (double)(cthis->get_base_frequency(cthis));
		/* PUSH next sample item into the TSP provider lib DataPool 
		 * and go to next for (over requested symbols) iteration */
		TSP_datapool_push_next_item(cthis->datapool, &item);
		memo_val[index]=*((double*)item.raw_value);
		continue;
	      }
	      
	      /* cthis loop generate pseudo evolving values for each 
	       * Stubbed Server symbol 
	       */
	      for(j=0;j<X_sample_symbol_info_list_val[index].dimension;++j)
	      {
	       
	      switch(X_sample_symbol_info_list_val[index].type)
	      {
	      case TSP_TYPE_DOUBLE:
		*((double*)item.raw_value) = calc_func(index, my_time);
		memo_val[index]=*((double*)item.raw_value);
		break;

	      case TSP_TYPE_FLOAT:
		*((float*)item.raw_value) = 1.102;  /* (float)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_INT8:
		((int8_t*)item.raw_value)[j] = 2+j; /*  (int8_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_INT16:
		*((int16_t*)item.raw_value) = 3; /*(int16_t) calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_INT32:
		*((int32_t*)item.raw_value) =  4; /*(int32_t)calc_func(index, my_time);*/
		break;
	

	      case TSP_TYPE_INT64:
		*((int64_t*)item.raw_value) = 5; /* (int64_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_UINT8:
		*((uint8_t*)item.raw_value) = 6; /* (uint8_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_UINT16:	       
		*((uint16_t*)item.raw_value) = 7; /* (uint16_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_UINT32:
		*((uint32_t*)item.raw_value) = 8; /* (uint32_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_UINT64:
		*((uint64_t*)item.raw_value) = 9; /* (uint64_t)calc_func(index, my_time);*/
		break;
	
	      case TSP_TYPE_CHAR:	
		*((char*)item.raw_value) = 'C'; /* calc_func_char(index, my_time);*/
		break;
			
	      case TSP_TYPE_UCHAR:		
		*((unsigned char*)item.raw_value) = 'U'; /* (unsigned char)calc_func_char(index, my_time);*/
		break;
		  
	      case TSP_TYPE_RAW:
		*((uint8_t*)item.raw_value) = 10; /* (uint8_t)calc_func(index, my_time);*/
		break;
	      default:
		STRACE_ERROR("Unhandled TSP datatype =%d",X_sample_symbol_info_list_val[index].type);
	 
	      }
	      }

	      TSP_datapool_push_next_item(cthis->datapool, &item);
	    }
	}

      /* 
       * Finalize the datapool state with new time : Ready to send 
       * This call signals the TSP Provider library that the coherent
       * set of samples have reached the Datapool and are ready
       * to be distributed with TSP
       */
      TSP_datapool_push_commit(cthis->datapool,my_time, GLU_GET_NEW_ITEM);

      /* Update nexttime to be next time to generated symbols */
      X_nexttime += tsp_usleep_period_us*1000;
      /* get current time */
      current_time = tsp_gethrtime();
      /* wait for "missing" cycle time */
      if( current_time < X_nexttime ) { 	
	/* time are in nano-second and we Usleep in micro-second */
	tsp_usleep((X_nexttime-current_time)/1000);
      }

      my_time++;    
      
      /* print out some pseudo time for debug purpose */
      if (!(my_time%1000))  STRACE_INFO("TOP %d : %s=%g \t%s=%g \t%s=%g \t%s=%g", my_time,
					 X_sample_symbol_info_list_val[0].name, memo_val[0],
					 X_sample_symbol_info_list_val[1].name, memo_val[1],
					 X_sample_symbol_info_list_val[2].name, memo_val[2],
					 X_sample_symbol_info_list_val[3].name, memo_val[3]
					);
    }
    
  return cthis;

}


int STUB_GLU_init(GLU_handle_t* cthis, int fallback_argc, char* fallback_argv[])
{
  int i;
  int32_t size;
  char symbol_buf[50];

  /* Initialize provided sample info list */
  if (TSP_STATUS_OK!=TSP_SSIList_initialize(&X_SSI_list,GLU_MAX_SYMBOLS_DOUBLE+GLU_MAX_SYMBOLS_NOT_DOUBLE+1)) {
    return FALSE;
  }
  
  X_sample_symbol_info_list_val = X_SSI_list.TSP_sample_symbol_info_list_t_val;
  
  for (i=0; i<GLU_MAX_SYMBOLS_DOUBLE; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      TSP_SSI_initialize(TSP_SSIList_getSSI(X_SSI_list,i),
			 symbol_buf,        /* name                  */ 
			 i,                 /* provider global index */
			 0,0,               /* pgridx,pgrank         */
			 TSP_TYPE_DOUBLE,   /* type                  */
			 1,                 /* dimension             */
			 0,0,               /* offset, nelem         */
			 1,                 /* period                */
			 0);                /* phase                 */
      /* compute symbol memory size */
      size = X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      /* update symbol max size */
      if (taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }
    }

  if(0!=GLU_MAX_SYMBOLS_NOT_DOUBLE)
  {

      /*Float*/
      sprintf(symbol_buf, "FLOAT_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_FLOAT;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;
    
      /*ENTIER*/
      sprintf(symbol_buf, "INT8_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_INT8;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*Int16*/
      sprintf(symbol_buf, "INT16_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_INT16;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*Int32*/
      sprintf(symbol_buf, "INT32_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_INT32;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*Int64*/
      sprintf(symbol_buf, "INT64_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_INT64;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*uint8*/
      sprintf(symbol_buf, "UINT8_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_UINT8;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*uint16*/
      sprintf(symbol_buf, "UINT16_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_UINT16;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;


      /*uint32*/
      sprintf(symbol_buf, "UINT32_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_UINT32;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*uint64*/
      sprintf(symbol_buf, "UINT64_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_UINT64;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;



      /*CHAR*/
      sprintf(symbol_buf, "CHAR_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_CHAR;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;


      /*UCHAR*/
      sprintf(symbol_buf, "UCHAR_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_UCHAR;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;

      /*TAB INT8*/
      sprintf(symbol_buf, "TAB_INT8_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_INT8;
      X_sample_symbol_info_list_val[i].dimension = 10;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;
      /*TAB CHAR*/
      sprintf(symbol_buf, "TAB_CHAR_Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type =  TSP_TYPE_CHAR;
      X_sample_symbol_info_list_val[i].dimension = 2*10;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
      {
	taille_max_symbol= size;
      }

      ++i;
  }

  /*overide first name*/
  X_sample_symbol_info_list_val[0].name = strdup("t");

  return TRUE;
}

int  
STUB_GLU_get_ssi_list(GLU_handle_t* h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  symbol_list->TSP_sample_symbol_info_list_t_len = X_SSI_list.TSP_sample_symbol_info_list_t_len;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_SSI_list.TSP_sample_symbol_info_list_t_val;
	    
  return TRUE;
} /* STUB_GLU_get_ssi_list */

int32_t
STUB_GLU_get_ssei_list_fromPGI(struct GLU_handle_t* cthis, 
			       int32_t* pgis, int32_t pgis_len, 
			       TSP_sample_symbol_extended_info_list_t* SSEI_list) {

  int32_t i,ret;
  ret=TSP_STATUS_OK;

  /* Store all global indexes into list including NOT FOUND ones */
  for ( i=0 ; i < pgis_len;++i) {    
    STRACE_DEBUG("Getting extended info for PGI <%d>",pgis[i]);
    if(-1!=pgis[i]) {

      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index=pgis[i]; 

      if(1000>pgis[i])
      {     
	SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_len=0;
	SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val=NULL;
      }
      else
      {
	switch(pgis[i])
	  {
	  case 1000:
   	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "a");
	    break; 
	  case 1001:
   	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "b");
	    break;
	  case 1002:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "c");
	    break;
	  case 1003:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "d");
	    break;
          case 1004:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "e");
	    break;
          case 1005:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "f");
	    break;
          case 1006:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "g");
	    break;
	  case 1007:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "h");
	    break;
	  case 1008:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "i");
	    break;
         case 1009:
	   TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	   TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "j");
	   break;
	  case 1010:
	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),1);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "unit", "m");
	    break;
	  case 1011:
   	    TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),2);
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "profile", "10");
 	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[1]), "unit", "SI");
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[1]), "order", "1");
	    break;
	   case 1012:
	     TSP_EIList_initialize(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info),2);
	     TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "profile", "2*10");
	     TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[1]), "unit", "None");
	     TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[1]), "order", "2");
	     break; 
	  default:
	    /* do not forget invalid > 1012 PGIs */
	    SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index=-1;      
	    SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_len=0;
	    SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val=NULL;
	    ret=TSP_STATUS_ERROR_PGI_UNKNOWN;     
	  }
      }
    }
    else {
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].provider_global_index=-1;      
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_len=0;
      SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val=NULL;
      ret=TSP_STATUS_ERROR_PGI_UNKNOWN;      
    }
  } /*end for*/
  return  ret;

} /* end of STUB_GLU_get_ssei_list_fromPGI */


/* create the GLU handle instance for STUB */
GLU_handle_t* GLU_stub_create(double baseFrequency) {
  
  /* create a default GLU */
  GLU_handle_create(&stub_GLU,"StubbedServer",GLU_SERVER_TYPE_ACTIVE,baseFrequency);
  
  stub_GLU->initialize             = &STUB_GLU_init;
  stub_GLU->run                    = &STUB_GLU_thread;
  stub_GLU->get_ssi_list           = &STUB_GLU_get_ssi_list;
  stub_GLU->get_ssei_list_fromPGI  = &STUB_GLU_get_ssei_list_fromPGI;

  return stub_GLU;
} /* GLU_stub_create */
