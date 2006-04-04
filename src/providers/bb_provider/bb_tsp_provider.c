/*

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.c,v 1.23 2006-04-04 12:36:25 morvan Exp $

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Blackboard TSP Provider

-----------------------------------------------------------------------
 */
#include <tsp_abs_types.h>
#include <bb_core.h>
#include <bb_utils.h>
#include <bb_alias.h>
#include <bb_simple.h>
#define BB_TSP_PROVIDER_C
#include <bb_tsp_provider.h>

#include <tsp_provider_init.h>
#include <tsp_datapool.h>

/*
 * Ugly declare for smooth TSP startup
 * FIXME.
 */
int TSP_provider_rqh_manager_get_nb_running();

/* 
 * We include TSP header which are necessary for
 * implementing a TSP GLU.
 * See 'tsp_glu.h' in order to know what function needs to be coded.
 */
#include <tsp_sys_headers.h>
#include <tsp_glu.h>
#include <tsp_ringbuf.h>

#include <signal.h>


/*
 * Some static
 */
/* The sample symbol list */
static TSP_sample_symbol_info_t *X_sample_symbol_info_list_val = NULL;

/* The BB and its shadow BB */
static S_BB_T* the_bb      = NULL;
static char*   the_bbname  = NULL;
static S_BB_T* shadow_bb   = NULL;
static int     nb_symbols  = 0;

/* GLU time stamp */
static time_stamp_t glu_time = 0;
/* 
 * Array of value address in shadow BB indexified by PGI
 */
static void** value_by_pgi = NULL;
/* 
 * Array of data description structure address in shadow BB indexified by PGI
 */
static S_BB_DATADESC_T** bbdatadesc_by_pgi = NULL;
/*
 * Correspondance array mapping bbindex to pgi 
 * This is some sort of hack due to the fact that
 * TSP does not hanlde every BB data type.
 */ 
static int32_t* bbindex_to_pgi = NULL;

/*
 * Boolean array for TSP_write right management
 */
static int* allow_to_write = NULL;

/*
 * The provider base frequency should the one of the simulator
 * which could be published IN the blackboard itself 
 * we gives a default one, which may not correspond to a "real one".
 */
/*
static double frequence_provider = 64.0;
static double* bb_tsp_provider_frequency = &frequence_provider;
*/

static GLU_handle_t* bbGLU = NULL;

int 
BB_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[]) {

  int retcode;
  int i;
  int j;
  int i_temp;
  int i_pg_index;
  int i_nb_item_scalaire;
  int dimension_globale_element;
  int32_t          aliasstack_size = MAX_ALIAS_LEVEL;
  S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
  int32_t indexstack[MAX_ALIAS_LEVEL];
  int32_t indexstack_len;    

  retcode = TRUE;
  /* We don't need fallback for now */
  /* 
   * !!! We need to attach to BB iff
   * !!! we are in a separate process
   */
  if (bb_attach(&the_bb,the_bbname) != BB_OK) {
    bb_logMsg(BB_LOG_SEVERE,
	      "bb_tsp_provider::GLU_init","Cannot attach to BlackBoard <%s>!!",
	      the_bbname);
    retcode = FALSE;
    return retcode;
  } 
  
  /* 
   * Get direct blackboard access in order to 
   * to build symbol list more easily
   */
  if (TRUE == retcode) {
    /* Allocate shadow blackboard */
    shadow_bb = malloc(bb_get_mem_size(the_bb));
    if (NULL == shadow_bb) {
      retcode = FALSE;
    }
  }
  if (TRUE == retcode) {
    /* Shadow BBPE */
    if (BB_NOK == bb_shadow_get(shadow_bb, the_bb)) {
      retcode = FALSE;
    }  
  }
  if (TRUE == retcode) {
    /* Initial update */
    if (BB_NOK ==  bb_shadow_update_data(shadow_bb,the_bb)) {
      retcode = FALSE;
    } 
  }

  /* Build symbols list */
  /* We should compute the number of SCALAR items
   * since TSP does not handle array type [yet...]
   */
  i_nb_item_scalaire = 0;
  for (i=0;i<bb_get_nb_item(shadow_bb);++i) {
    /* 
     * We skip unhandled BB_TYPE 
     * because TSP does not handle other type than double FIXME.
     */
	 if (bb_data_desc(shadow_bb)[i].type < E_BB_CHAR) {
		aliasstack[0]=bb_data_desc(shadow_bb)[i];
		aliasstack_size = MAX_ALIAS_LEVEL;
		bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);
		dimension_globale_element = 1;
		for (j=0; j<aliasstack_size; j++) {
			dimension_globale_element *= aliasstack[j].dimension;
		}
		i_nb_item_scalaire += dimension_globale_element;
	 }
  }
  nb_symbols = i_nb_item_scalaire;

  /* 
   * Allocate symbol list whose size
   * correspond to number of data (scalar) published in BB
   */
  X_sample_symbol_info_list_val = calloc (i_nb_item_scalaire,
					  sizeof (TSP_sample_symbol_info_t));
  assert(X_sample_symbol_info_list_val);

  /* 
   * Allocate array of pointer to data
   */
  value_by_pgi      = (void **) calloc(i_nb_item_scalaire,sizeof(void*));
  bbdatadesc_by_pgi = (S_BB_DATADESC_T **) calloc(i_nb_item_scalaire,sizeof(S_BB_DATADESC_T*));
  bbindex_to_pgi    = (int32_t *) calloc(bb_get_nb_item(shadow_bb),sizeof(int32_t));  
  /*
   * Allocate write 'right' management array
   */
  allow_to_write = (int *) calloc(i_nb_item_scalaire,sizeof(int));

  assert(value_by_pgi);	
  assert(bbdatadesc_by_pgi);	
  assert(bbindex_to_pgi);
  assert(allow_to_write);
  
  
  /* initialize the provider global index to 0 */
  i_pg_index = 0;
  for (i=0; i<bb_get_nb_item(shadow_bb);++i) {
    /* 
     * TSP do not handle array type [yet]
     * we generate symbol name with array index suffix [%d]
     */
    /* we skip unhandled BB_TYPE */ 
    if (bb_data_desc(shadow_bb)[i].type < E_BB_CHAR) {
      /* memorize the first PGI used for this bbindex */
      bbindex_to_pgi[i] = i_pg_index;
		
		/* alias case */
		if (bb_isalias(&bb_data_desc(shadow_bb)[i])) {
			aliasstack[0]=bb_data_desc(shadow_bb)[i];
			aliasstack_size = MAX_ALIAS_LEVEL;
			bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);
			memset(indexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
			indexstack_len = 0;
			for (j=0; j<aliasstack_size; j++) {
				if (aliasstack[aliasstack_size-1-j].dimension > 1) {
					indexstack[indexstack_len]=0;
					++indexstack_len;
				}
			}
						
			/** alisa array */
			if (indexstack_len){
				i_temp = strlen(bb_data_desc(shadow_bb)[i].name) + 10*indexstack_len;
				do {
					X_sample_symbol_info_list_val[i_pg_index].name = malloc(i_temp);
					assert(X_sample_symbol_info_list_val[i_pg_index].name);
					memset(X_sample_symbol_info_list_val[i_pg_index].name, 0, i_temp);
					get_array_name(X_sample_symbol_info_list_val[i_pg_index].name,
										i_temp,
										aliasstack, aliasstack_size,
										indexstack, indexstack_len);
					X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
					X_sample_symbol_info_list_val[i_pg_index].period = 1;
					X_sample_symbol_info_list_val[i_pg_index].type   = TSP_TYPE_DOUBLE;
					/* update data pointer with appropriate value */
					value_by_pgi[i_pg_index] = bb_item_offset(shadow_bb, &bb_data_desc(shadow_bb)[i], indexstack, indexstack_len);
					bbdatadesc_by_pgi[i_pg_index] = &bb_data_desc(shadow_bb)[i];
					allow_to_write[i_pg_index]    = TSP_ASYNC_WRITE_ALLOWED;
					++i_pg_index;
				}
				while (BB_OK == bb_alias_increment_idxstack(aliasstack, aliasstack_size, indexstack, indexstack_len));
			}
			
			
			/* simple alias */
			else {
				X_sample_symbol_info_list_val[i_pg_index].name = strdup(bb_data_desc(shadow_bb)[i].name);
				X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
				X_sample_symbol_info_list_val[i_pg_index].period = 1;
				X_sample_symbol_info_list_val[i_pg_index].type   = TSP_TYPE_DOUBLE;
				value_by_pgi[i_pg_index] = bb_item_offset(shadow_bb, &bb_data_desc(shadow_bb)[i], indexstack, indexstack_len);
				bbdatadesc_by_pgi[i_pg_index] = &bb_data_desc(shadow_bb)[i];
				allow_to_write[i_pg_index]    = TSP_ASYNC_WRITE_ALLOWED;
				++i_pg_index;
				
			}
		}
		
		
		
		
		/* non alias array */
      else if (bb_data_desc(shadow_bb)[i].dimension > 1) {
			for (j=0;j<bb_data_desc(shadow_bb)[i].dimension; ++j) {
			/* FIXME calculer la taille exacte j*log10 */
			i_temp = strlen(bb_data_desc(shadow_bb)[i].name)+10;
			X_sample_symbol_info_list_val[i_pg_index].name = malloc(i_temp);
			assert(X_sample_symbol_info_list_val[i_pg_index].name);
			snprintf(X_sample_symbol_info_list_val[i_pg_index].name, 
					i_temp,
					"%s[%0d]",
					bb_data_desc(shadow_bb)[i].name,
					j);
			X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
			X_sample_symbol_info_list_val[i_pg_index].period = 1;
			X_sample_symbol_info_list_val[i_pg_index].type   = TSP_TYPE_DOUBLE;
			/* update data pointer with appropriate value */
			value_by_pgi[i_pg_index]  = ((void*) ((char*)bb_data(shadow_bb) + bb_data_desc(shadow_bb)[i].data_offset)) + j*bb_data_desc(shadow_bb)[i].type_size;
			bbdatadesc_by_pgi[i_pg_index] = &bb_data_desc(shadow_bb)[i];
			allow_to_write[i_pg_index]    = TSP_ASYNC_WRITE_ALLOWED;
			++i_pg_index;
			}
      } 
      /* creation simple pour les scalaires */
      else {
			X_sample_symbol_info_list_val[i_pg_index].name = strdup(bb_data_desc(shadow_bb)[i].name);
			X_sample_symbol_info_list_val[i_pg_index].provider_global_index = i_pg_index;
			X_sample_symbol_info_list_val[i_pg_index].period = 1;
			X_sample_symbol_info_list_val[i_pg_index].type   = TSP_TYPE_DOUBLE;
			value_by_pgi[i_pg_index] = ((void*) ((char*)bb_data(shadow_bb) + bb_data_desc(shadow_bb)[i].data_offset));
			bbdatadesc_by_pgi[i_pg_index] = &bb_data_desc(shadow_bb)[i];
			allow_to_write[i_pg_index]    = TSP_ASYNC_WRITE_ALLOWED;
			++i_pg_index;
      }
    }
	 else  { /* skip unhandled BB type */ 
      STRACE_INFO(("Skipping unhandled symbol type <%d> name <%s>",bb_data_desc(shadow_bb)[i].type,bb_data_desc(shadow_bb)[i].name));
    }
  } /* loop over bb items */
    
  return retcode;
} /* end of BB_GLU_init */


int  BB_GLU_get_symbol_number(GLU_handle_t* this) {

  return nb_symbols;

}  /* end of GLU_get_symbol_number */

int  
BB_GLU_get_sample_symbol_info_list(GLU_handle_t* h_glu, 
				   TSP_sample_symbol_info_list_t* symbol_list) {

  symbol_list->TSP_sample_symbol_info_list_t_len = BB_GLU_get_symbol_number(h_glu);
  symbol_list->TSP_sample_symbol_info_list_t_val = X_sample_symbol_info_list_val;
	    
  return TRUE;
}

int 
BB_GLU_get_pgi(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes) {
  
  int     i=0;
  int 	 j=0;
  int     ret=TRUE;

  int     index=0;
  int32_t bbidx;
  S_BB_DATADESC_T sym_data_desc;
  int32_t array_index[MAX_ALIAS_LEVEL];
  int32_t array_index_len=MAX_ALIAS_LEVEL;
  int32_t array_index_ptr;
  int32_t          aliasstack_size = MAX_ALIAS_LEVEL;
  S_BB_DATADESC_T  aliasstack[MAX_ALIAS_LEVEL];
  int32_t previous_array_ptr;
  void *sym_value;


  
  STRACE_INFO(("Starting symbol Valid nb_symbol=%u",symbol_list->TSP_sample_symbol_info_list_t_len));
  /* For each requested symbols, check by name, and find the provider global index */
  for( i = 0 ; i < symbol_list->TSP_sample_symbol_info_list_t_len ; i++) {
  
    /* Get short name (without [XXX], for array) and indexstack and its length */                                
    memset(&sym_data_desc,0,sizeof(S_BB_DATADESC_T));
    if (bb_utils_parsearrayname(symbol_list->TSP_sample_symbol_info_list_t_val[i].name, 
				sym_data_desc.name,
				VARNAME_MAX_SIZE,
				array_index, &array_index_len)) {
	   STRACE_INFO  (("%s: cannot parse symname <%s>",
			  "B_GLU_get_pgi",
			  symbol_list->TSP_sample_symbol_info_list_t_val[i].name));
	   ret = FALSE;
	   continue;
	 } 	 
	 else {
	   /* search if the symbol is published in the blackboard */
	   sym_data_desc.type      = E_BB_DISCOVER;
	   sym_data_desc.type_size = 0;
	   sym_data_desc.dimension = 0;
	   sym_value = bb_alias_subscribe(shadow_bb,&sym_data_desc,array_index,array_index_len);    	
	   
	   if (NULL==sym_value) {
	     STRACE_INFO  (("symbol <%s> not found in BB <%s>",
			    sym_data_desc.name,
			    shadow_bb->name));
	     pg_indexes[i] = -1;
	     ret = FALSE;
	     continue;
	   }
	   else { 		
	     /* find the index of the symbol in the BB */
	     bbidx = bb_find(shadow_bb, sym_data_desc.name);
	     
	     STRACE_DEBUG(("Validate symbol: orig_name=<%s>, short=<%s>",
			   symbol_list->TSP_sample_symbol_info_list_t_val[i].name,
			   sym_data_desc.name));
	     /* Initialise aliasstack */
	     array_index_ptr = array_index_len-1;
	     aliasstack_size = MAX_ALIAS_LEVEL;
	     aliasstack[0]=sym_data_desc;
	     bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);
	     
	     
	     /* symbol not found skip to next symname */
	     if (-1==bbidx) {
	       pg_indexes[i] = -1;
	       ret=FALSE;
	       STRACE_INFO(("Symbol=%s, not found",symbol_list->TSP_sample_symbol_info_list_t_val[i].name));
	       continue;
	     } else {
	       /* 
		* examine whether symbol is of array type or not
		* and validate index range (vs dimension specified in aliasstack) if it is of array type.
		*/
	       for (j=0; j<aliasstack_size; j++) {
		 if ((aliasstack[j].dimension > 1) && (aliasstack[j].dimension < array_index[array_index_ptr])) {
		   STRACE_INFO(("Symbol=%s, found but index=%d out of range for element <%s>",
				symbol_list->TSP_sample_symbol_info_list_t_val[i].name,
				array_index[array_index_ptr],
				aliasstack[j].name));
		   ret = FALSE;
		 }
		 else if (aliasstack[j].dimension > 1){
		   --array_index_ptr;
		 }
	       }
	       if (ret == FALSE) {continue;}
	       else {
		 
		 /* do not validate array name as first element of array
		  * consumer should ask for precise element array until
		  * TSP support array type.
		  */
		 if ((NULL==strstr(symbol_list->TSP_sample_symbol_info_list_t_val[i].name,"[")) &&
		     (bb_data_desc(shadow_bb)[bbidx].dimension > 1)) {
		   pg_indexes[i] = -1;
		   STRACE_INFO(("Symbol=%s, found but array index wasn't given",symbol_list->TSP_sample_symbol_info_list_t_val[i].name));
		 } else {
		   /* magic formula for fast rebuild of PGI from indexstack and aliasstack */
		   array_index_ptr = array_index_len-1;
		   index = 0;
		   for (j=0; j<aliasstack_size; j++) {
		     if (aliasstack[j].dimension > 1) {
		       if (array_index_ptr == array_index_len-1) {
			 index += array_index[array_index_ptr];
		       }
		       else {
			 index += array_index[array_index_ptr] * aliasstack[previous_array_ptr].dimension;
		       }
		       previous_array_ptr = j;
		       --array_index_ptr;
		     }
		   }
		   
		   pg_indexes[i] = bbindex_to_pgi[bbidx] + index;
		   STRACE_INFO(("Symbol=%s, found index=%d",symbol_list->TSP_sample_symbol_info_list_t_val[i].name,pg_indexes[i]));
		 }
	       }
	     }
	   }
	 }
  }
  
  STRACE_INFO(("End of symbol Valid")); 
  
  return ret;
}

void* BB_GLU_thread(void* arg) {
  
  int i;
  glu_item_t item;
  sigset_t s_mask;
  int nb_consumed_symbols;
  int* ptr_consumed_index;
  int pgi;
  
  bb_logMsg(BB_LOG_INFO,
	      "bb_tsp_provider::GLU_thread",
	      "Provider thread started with <%d> symbols",
	    BB_GLU_get_symbol_number((GLU_handle_t*)arg) );
  /*
   * On masque les signaux indesirables i.e. tous :))
   */
  sigfillset(&s_mask);
  pthread_sigmask(SIG_BLOCK,&s_mask,NULL);
  /* 
   * Initialise le temps propre du GLU 
   */
  glu_time      = 0;

  /* boucle infinie tant que le blackboard n'est pas detruit */
  while(BB_STATUS_DESTROYED != the_bb->status) {
    /* On attend le déblocage du PE */
    if (BB_NOK == bb_simple_synchro_wait(the_bb,BB_SIMPLE_MSGID_SYNCHRO_COPY)) {
      /* on sort car le BB a été détruit */
      break;
    }
    /* 
     * Update shadow BB data zone
     */    
    bb_shadow_update_data(shadow_bb,the_bb);
    /* 
     * Refresh the [reverse list of consumed symbols]
     * Must be call at each step in case of new samples wanted 
     */
    TSP_datapool_get_reverse_list (&nb_consumed_symbols, &ptr_consumed_index); 

    GLU_handle_t* gh = (GLU_handle_t*)arg;
    /* acknowledge copy end if bb_provider was telled to do so */
    if ( *((int*)(gh->private_data)) ) {
      bb_simple_synchro_go(the_bb,BB_SIMPLE_MSGID_SYNCHRO_COPY_ACK); 
    }

    /* PUSH des valeurs directement dans le datapool */
    for(i = 0 ; i <  nb_consumed_symbols ; ++i) {
      /* retrieve the pgi of the consumed symbol */
      pgi = ptr_consumed_index[i];
      item.time                  = glu_time;
      item.provider_global_index = pgi;
      /* we return a double value even if 
       * the blackboard type is different
       * since TSP only knows double ... till now */
      *((double*)item.raw_value) = bb_double_of(value_by_pgi[pgi],bbdatadesc_by_pgi[pgi]->type);
      TSP_datapool_push_next_item(&item);      
    }
    TSP_datapool_push_commit(glu_time, GLU_GET_NEW_ITEM);
      
    ++glu_time;
  }

  return NULL;
  
} /* end of BB_GLU_thread */

int 
BB_GLU_async_sample_write(GLU_handle_t* glu, int provider_global_index, void* value_ptr, uint32_t value_size)
{
	S_BB_DATADESC_T* data_desc;
	int retcode = BB_NOK;       	
	double value;
	char   strvalue[256];
	void*  genuineBBdata;
	
	STRACE_INFO(("BB_PROVIDER want to AsyncWrite : pgi <%d> with value : 0x%X (value_size=%d)",provider_global_index, (uint32_t)value_ptr,value_size));
	
	/* FIXME : Should use the pgi to cast properly the data versus the real type */
	value = *(double*)value_ptr;
	/* FIXME really ugly double to string convert used to easy bb_value_write */
	sprintf(strvalue,"%0f",value);

	/* try to write */
	if (provider_global_index>=0 && provider_global_index<nb_symbols) {		
	  if (allow_to_write[provider_global_index]==TSP_ASYNC_WRITE_ALLOWED) { 
	    data_desc = bbdatadesc_by_pgi[provider_global_index];
	    STRACE_INFO(("About to write on symbol <%s> value <%f> (strvalue=%s)...",data_desc->name,value,strvalue));	    
	    /* 
	     * Note that we should write to genuine BB not the shadow ... 
	     * since the shadow may be overwritten immediatly on next update cycle
	     * We recompute genuine bb offset from the one store on shadow BB
	     * since they MUST be the same.
	     */
	    genuineBBdata = bb_data(the_bb) + 
	                   (value_by_pgi[provider_global_index] - bb_data(shadow_bb));
	    retcode = bb_value_direct_write(genuineBBdata,*data_desc,strvalue,0);
	  } else {
	    STRACE_INFO(("BB_GLU : pgi = %d is not allowed to be written",provider_global_index));
	  }
	} else {
	  STRACE_INFO(("BB_GLU : pgi = %d is not valid provider_global_index",provider_global_index));	
	}
	
	STRACE_DEBUG(("BB_PROVIDER After AsyncWrite : value %f return :%d",*((double*)value_by_pgi[provider_global_index]), retcode));

	return retcode;
} /* end of BB_GLU_async_sample_write */

int 
BB_GLU_async_sample_read(GLU_handle_t* glu, int provider_global_index, void* value_ptr, uint32_t* value_size)
{
	S_BB_DATADESC_T* data_desc;
	int retcode = BB_NOK; 
	void*  genuineBBdata;
	
	STRACE_DEBUG(("BB_PROVIDER want to AsyncRead : pgi <%d> (value_size allowed=%d)",provider_global_index,*value_size));
	
	/* try to read */
	if (provider_global_index>=0 && provider_global_index<nb_symbols) {

	    data_desc = bbdatadesc_by_pgi[provider_global_index];
	
	    STRACE_INFO(("About to read from symbol <%s> value...",bbdatadesc_by_pgi[provider_global_index]->name));
	    /* 
	     * Note that we should read from genuine BB not the shadow ... 
	     * since the shadow may be overwritten immediatly on next update cycle
	     * We recompute genuine bb offset from the one store on shadow BB
	     * since they MUST be the same.
	     */
	    genuineBBdata = bb_data(the_bb) + 
	                   (value_by_pgi[provider_global_index] - bb_data(shadow_bb));
	    
	    *((double*)value_ptr) = bb_double_of(genuineBBdata,
						 bbdatadesc_by_pgi[provider_global_index]->type);
	    STRACE_INFO(("AsyncRead value is <%f>.",*((double*)value_ptr)));
	    retcode = BB_OK;	
	} else {
	  STRACE_INFO(("BB_GLU : pgi = %d is not valid provider_global_index",provider_global_index));	
	}
	
	STRACE_DEBUG(("BB_PROVIDER After AsyncRead : value %f return :%d",*((double*)value_ptr), retcode));

	return retcode;
} /* end of BB_GLU_async_sample_read */


int32_t  
bb_tsp_provider_allow_write_symbol(int provider_global_index){

  int32_t retcode;
  retcode = BB_NOK;
  
  if(provider_global_index>=0 && provider_global_index<nb_symbols){
  	allow_to_write[provider_global_index] = TSP_ASYNC_WRITE_ALLOWED;
	retcode = BB_OK;
  }	
  return retcode;
}


int32_t  
bb_tsp_provider_forbid_write_symbol(int provider_global_index){

  int32_t retcode;
  retcode = BB_NOK;
  
  if(provider_global_index>=0 && provider_global_index<nb_symbols){
  	allow_to_write[provider_global_index] = TSP_ASYNC_WRITE_FORBIDDEN;
	retcode = BB_OK;
  }	
  return retcode;
}


void
bb_tsp_provider_setbbname(const char* bbname) {
  if (NULL != bbGLU) {
    free(bbGLU->name);
    bbGLU->name = strdup(bbname);
    free(the_bbname);
    the_bbname = strdup(bbname);
  } else {
    the_bbname = strdup(bbname);
  }
}

GLU_handle_t*
bb_tsp_provider_create_GLU(double frequency, int32_t acknowledgeCopy, int GLUServerType) {
  
  /* create a default GLU */
  GLU_handle_create(&bbGLU,NULL == the_bbname ? "BBGLU" : the_bbname,GLUServerType,frequency);

  /* now override default methods with more efficient BB specific methods */
  bbGLU->initialize     = &BB_GLU_init;
  bbGLU->run            = &BB_GLU_thread;
  bbGLU->get_ssi_list   = &BB_GLU_get_sample_symbol_info_list;
  bbGLU->get_nb_symbols = &BB_GLU_get_symbol_number;
  bbGLU->get_pgi        = &BB_GLU_get_pgi;
  bbGLU->async_write    = &BB_GLU_async_sample_write;
  bbGLU->async_read     = &BB_GLU_async_sample_read;  
  /* Update private data with acknowlegdeCopy flag 
   * (will be used by BB_GLU_thread)
   */
  bbGLU->private_data            = malloc(sizeof(int));
  * ((int*)bbGLU->private_data)  = acknowledgeCopy;
  
  return bbGLU;
}


int32_t 
bb_tsp_provider_initialise(int* argc, char** argv[],int TSPRunMode, const char* bbname,
			   double frequency, int32_t acknowledgeCopy) {
  
  int32_t retcode;

  bb_tsp_provider_create_GLU(frequency,acknowledgeCopy,GLU_SERVER_TYPE_ACTIVE);
  bb_tsp_provider_setbbname(bbname);
  
  /* Init LibTSP provider */
  if (FALSE==TSP_provider_init(bbGLU,argc, argv)) {
    retcode = BB_NOK;
    return retcode;
  }
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
} /* end of bb_tsp_provider_initialise */

int32_t 
bb_tsp_provider_finalize() {
  int32_t retcode;
  
  TSP_provider_end();
  retcode = BB_OK;

  return retcode;
}


