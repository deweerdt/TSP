/*

$Id: glue_stub.c,v 1.20 2006-04-15 10:46:02 erk Exp $

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

#include "tsp_sys_headers.h"
#include <tsp_glu.h>
#include "tsp_ringbuf.h"
#include "tsp_time.h"
#include "tsp_datapool.h"
#include "calc_func.h"
#include <tsp_common.h>

/* TSP glue server defines */
#define TSP_STUB_FREQ 100.0 /*Hz*/
#define TSP_USLEEP_PERIOD_US (1000000/TSP_STUB_FREQ) /*given in µS, value 10ms*/
#define GLU_MAX_SYMBOLS_DOUBLE 1000
#define GLU_MAX_SYMBOLS_NOT_DOUBLE 12

/* Nasty static variables */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val;
static tsp_hrtime_t X_lasttime;
static time_stamp_t my_time = 0;
static GLU_handle_t* stub_GLU = NULL;
static int32_t taille_max_symbol=0;

void* STUB_GLU_thread(void* athis)
{
  int i, symbols_nb, *ptr_index;
  tsp_hrtime_t current_time;
  glu_item_t item;
  double memo_val[GLU_MAX_SYMBOLS_DOUBLE]; /*for debug informatin */
  GLU_handle_t* this  = (GLU_handle_t*) athis;

  current_time = X_lasttime = tsp_gethrtime();  

  item.raw_value=calloc(1,taille_max_symbol);
  assert(item.raw_value);

  /* infinite loop for symbols generation */
  while(1)
    {
      /* Must be call at each step in case of new samples wanted */
      TSP_datapool_get_reverse_list (&symbols_nb, &ptr_index); 
      for(i = 0 ; i <  symbols_nb ; i++)
	{
	  int index=ptr_index[i];
	  if(my_time%X_sample_symbol_info_list_val[index].period == 0)
	    {
	      item.size = X_sample_symbol_info_list_val[index].dimension * tsp_type_size[X_sample_symbol_info_list_val[index].type];
 
	      item.time = my_time;
	      item.provider_global_index = index;

	      switch(X_sample_symbol_info_list_val[index].type)
	      {
	      case TSP_TYPE_DOUBLE:
		if (index!=0)
		  *((double*)item.raw_value) = calc_func(index, my_time);
		else
		  *((double*)item.raw_value) = (double)(my_time) / (double)(TSP_STUB_FREQ);
		memo_val[index]=*((double*)item.raw_value);
		break;

	      case TSP_TYPE_FLOAT:
		if (index!=0)
		  *((float*)item.raw_value) = 1.102;  /* (float)calc_func(index, my_time);*/
		else
		  *((float*)item.raw_value) = (float)(my_time) / (float)(TSP_STUB_FREQ);
		break;
	
	      case TSP_TYPE_INT8:
		if (index!=0)
		  *((int8_t*)item.raw_value) = 2; /*  (int8_t)calc_func(index, my_time);*/
		else
		  *((int8_t*)item.raw_value) = (int8_t)(my_time) / (int8_t)(TSP_STUB_FREQ);
		break;
	

	      case TSP_TYPE_INT16:
		if (index!=0)
		  *((int16_t*)item.raw_value) = 3; /*(int16_t) calc_func(index, my_time);*/
		else
		  *((int16_t*)item.raw_value) = (int16_t)(my_time) / (int16_t)(TSP_STUB_FREQ);
		break;
	

	      case TSP_TYPE_INT32:
		if (index!=0)
		  *((int32_t*)item.raw_value) =  4; /*(int32_t)calc_func(index, my_time);*/
		else
		  *((int32_t*)item.raw_value) = (int32_t)(my_time) / (int32_t)(TSP_STUB_FREQ);
		break;
	

	      case TSP_TYPE_INT64:
		if (index!=0)
		  *((int64_t*)item.raw_value) = 5; /* (int64_t)calc_func(index, my_time);*/
		else
		  *((int64_t*)item.raw_value) = (int64_t)(my_time) / (int64_t)(TSP_STUB_FREQ);
		break;
	
	      case TSP_TYPE_UINT8:
		if (index!=0)
		  *((uint8_t*)item.raw_value) = 6; /* (uint8_t)calc_func(index, my_time);*/
		else
		  *((uint8_t*)item.raw_value) = (uint8_t)(my_time) / (uint8_t)(TSP_STUB_FREQ);
		break;
	

	      case TSP_TYPE_UINT16:
		if (index!=0)
		  *((uint16_t*)item.raw_value) = 7; /* (uint16_t)calc_func(index, my_time);*/
		else
		  *((uint16_t*)item.raw_value) = (uint16_t)(my_time) / (uint16_t)(TSP_STUB_FREQ);
		break;
	

	      case TSP_TYPE_UINT32:
		if (index!=0)
		  *((uint32_t*)item.raw_value) = 8; /* (uint32_t)calc_func(index, my_time);*/
		else
		  *((uint32_t*)item.raw_value) = (uint32_t)(my_time) / (uint32_t)(TSP_STUB_FREQ);
		break;
	
	      case TSP_TYPE_UINT64:
		if (index!=0)
		  *((uint64_t*)item.raw_value) = 9; /* (uint64_t)calc_func(index, my_time);*/
		else
		  *((uint64_t*)item.raw_value) = (uint64_t)(my_time) / (uint64_t)(TSP_STUB_FREQ);
		break;
	
	      case TSP_TYPE_CHAR:
	
		*((char*)item.raw_value) = 'A'; /* calc_func_char(index, my_time);*/
		  break;
			
	      case TSP_TYPE_UCHAR:
		
		*((unsigned char*)item.raw_value) = 'B'; /* (unsigned char)calc_func_char(index, my_time);*/
		  break;
	
	      case TSP_TYPE_RAW:
		if (index!=0)
		  *((uint8_t*)item.raw_value) = 10; /* (uint8_t)calc_func(index, my_time);*/
		else
		  *((uint8_t*)item.raw_value) = (uint8_t)(my_time) / (uint8_t)(TSP_STUB_FREQ);
		break;
	      default:
		STRACE_ERROR(("Unhandled TSP datatype =%d",X_sample_symbol_info_list_val[index].type));
	 
	      }


	      TSP_datapool_push_next_item(&item);
	    }
	}



      /* Finalize the datapool state with new time : Ready to send */
      TSP_datapool_push_commit(my_time, GLU_GET_NEW_ITEM);

      if( current_time <= X_lasttime )
	{ 
	  tsp_usleep(TSP_USLEEP_PERIOD_US);
	}

      X_lasttime += TSP_USLEEP_PERIOD_US*1000;
      current_time = tsp_gethrtime();
      /*      
      if(last_missed!=RINGBUF_PTR_MISSED(glu_ring))
	{
	  last_missed = RINGBUF_PTR_MISSED(glu_ring);
	  data_missed = TRUE;
	  }*/
      
      my_time++;    
      
      if (!(my_time%1000))  STRACE_INFO(("TOP %d : %s=%g \t%s=%g \t%s=%g \t%s=%g", my_time,
					 X_sample_symbol_info_list_val[0].name, memo_val[0],
					 X_sample_symbol_info_list_val[1].name, memo_val[1],
					 X_sample_symbol_info_list_val[2].name, memo_val[2],
					 X_sample_symbol_info_list_val[3].name, memo_val[3]
					));
    }
    
  return this;

}

int STUB_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[])
{
  int i;
  int32_t size;
  char symbol_buf[50];
  
  X_sample_symbol_info_list_val = calloc (GLU_MAX_SYMBOLS_DOUBLE+GLU_MAX_SYMBOLS_NOT_DOUBLE+1, sizeof (TSP_sample_symbol_info_t)) ;
  for (i=0; i<GLU_MAX_SYMBOLS_DOUBLE; i++)
    {      
      sprintf(symbol_buf, "Symbol%d",i);
      X_sample_symbol_info_list_val[i].name = strdup(symbol_buf);
      X_sample_symbol_info_list_val[i].provider_global_index = i;
      X_sample_symbol_info_list_val[i].period = 1;
      X_sample_symbol_info_list_val[i].type = TSP_TYPE_DOUBLE;
      X_sample_symbol_info_list_val[i].dimension = 1;

      size=X_sample_symbol_info_list_val[i].dimension * tsp_type_size[X_sample_symbol_info_list_val[i].type];
      if(taille_max_symbol< size)
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
  }

  

  


  /*overide first name*/
  X_sample_symbol_info_list_val[0].name = strdup("t");

  return TRUE;
}

int  
STUB_GLU_get_ssi_list(GLU_handle_t* h_glu,TSP_sample_symbol_info_list_t* symbol_list)
{
  int i = 0;
  TSP_sample_symbol_info_t* p; 
	
  for (p=X_sample_symbol_info_list_val; p->name!=0 ; p++)
    {
      i++;
    }

  symbol_list->TSP_sample_symbol_info_list_t_len = i;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  return TRUE;
} /* STUB_GLU_get_ssi_list */

int32_t
STUB_GLU_get_ssei_list_fromPGI(struct GLU_handle_t* this, 
			       int32_t* pgis, int32_t pgis_len, 
			       TSP_sample_symbol_extended_info_list_t* SSEI_list) {

  int32_t i,ret;
  ret=TSP_STATUS_OK;

  /* Store all global indexes into list including NOT FOUND ones */
  for ( i=0 ; i < pgis_len;++i) {    
    STRACE_DEBUG(("Getting extended info for PGI <%d>",pgis[i]));
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
	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[0]), "profil", "2*2*4");
 	    TSP_EI_update(&(SSEI_list->TSP_sample_symbol_extended_info_list_t_val[i].info.TSP_extended_info_list_t_val[1]), "unit", "SI");
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
GLU_handle_t* GLU_stub_create() {
  
  /* create a default GLU */
  GLU_handle_create(&stub_GLU,"StubbedServer",GLU_SERVER_TYPE_ACTIVE,TSP_STUB_FREQ);
  
  stub_GLU->initialize             = &STUB_GLU_init;
  stub_GLU->run                    = &STUB_GLU_thread;
  stub_GLU->get_ssi_list           = &STUB_GLU_get_ssi_list;
  stub_GLU->get_ssei_list_fromPGI  = &STUB_GLU_get_ssei_list_fromPGI;

  return stub_GLU;
} /* GLU_stub_create */
