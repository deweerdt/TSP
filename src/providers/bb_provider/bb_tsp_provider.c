/*

$Header: /home/def/zae/tsp/tsp/src/providers/bb_provider/bb_tsp_provider.c,v 1.29 2006-04-25 21:10:21 erk Exp $

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
#include <tsp_provider_init.h>
#include <tsp_datapool.h>
#include <tsp_common.h>

#include <bb_core.h>
#include <bb_utils.h>
#include <bb_alias.h>
#include <bb_simple.h>
#define BB_TSP_PROVIDER_C
#include <bb_tsp_provider.h>

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
static TSP_sample_symbol_info_list_t* X_SSIList = NULL;
static TSP_sample_symbol_info_t*      X_SSIList_value = NULL;

/* The BB and its shadow BB */
static S_BB_T* the_bb        = NULL;
static char*   the_bbname    = NULL;
static S_BB_T* shadow_bb     = NULL;
static int     nbTspSymbols  = 0;

static int32_t bb_type2tsp_type[] = {
  TSP_TYPE_UNKNOWN, /* E_BB_DISCOVER=0, */
  TSP_TYPE_DOUBLE,  /* E_BB_DOUBLE=1,   */
  TSP_TYPE_FLOAT,   /* E_BB_FLOAT,      */
  TSP_TYPE_INT8,    /* E_BB_INT8,       */
  TSP_TYPE_INT16,   /* E_BB_INT16,      */ 
  TSP_TYPE_INT32,   /* E_BB_INT32,      */
  TSP_TYPE_INT64,   /* E_BB_INT64,      */
  TSP_TYPE_UINT8,   /* E_BB_UINT8,      */
  TSP_TYPE_UINT16,  /* E_BB_UINT16,     */
  TSP_TYPE_UINT32,  /* E_BB_UINT32,     */
  TSP_TYPE_UINT64,  /* E_BB_UINT64,     */
  TSP_TYPE_CHAR,    /* E_BB_CHAR,       */
  TSP_TYPE_UCHAR,   /* E_BB_UCHAR,      */
  TSP_TYPE_UNKNOWN, /* E_BB_USER        */
};

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
 * TSP does not handle every BB data type.
 * (in fact since TSP 0.8.0 only E_BB_USER are not handled)
 */ 
static int32_t* bbindex_to_pgi = NULL;

/*
 * Boolean array for TSP_write right management
 */
static int* allow_to_write = NULL;

static GLU_handle_t* bbGLU = NULL;

int 
BB_GLU_init(GLU_handle_t* this, int fallback_argc, char* fallback_argv[]) {

  int retcode;
  int i;
  int j;
  int i_temp;
  int pgi;
  int32_t indexStackCartesianSize; 
  char* tempName;

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
  /* 
   * We should compute the number Blackboard
   * item which may be distributed using TSP.
   * In fact every type but not USER type.
   */
  nbTspSymbols = 0;
  for (i=0;i<bb_get_nb_item(shadow_bb);++i) {
    /* 
     * We skip BB_TYPE which may not be distributed
     * using TSP, we must count alias dimension(s)
     * which may generate more symbols than the number
     * of published data in BB.
     */
    if (bb_data_desc(shadow_bb)[i].type < E_BB_USER) {
      /*
       * If an alias is a TSP handled type
       * we must resolve the aliasstack in order
       * to create as much PGI as the cartesian product
       * of the alias stack "indexstack" minus the dimension
       * of the primary alias which may be an array.
       */
      if (bb_isalias(&bb_data_desc(shadow_bb)[i])) {
	indexStackCartesianSize = 1;
	aliasstack[0]   = bb_data_desc(shadow_bb)[i];
	aliasstack_size = MAX_ALIAS_LEVEL;
	bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);
	memset(indexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
	indexstack_len = 0;
	/* 
	 * Compute cartesian product of alias stack dimension 
	 * not counting alias dimension itself
	 */
	for (j=aliasstack_size-1; j>0; --j) {
	  indexStackCartesianSize *= aliasstack[j].dimension;
	} 
	nbTspSymbols += indexStackCartesianSize;
      } else { /* non alias case */
	++nbTspSymbols;
      } /* end of if(bb_isalias) */
    } /* end if if (<E_BB_USER) */
  } /* end for all BB items */

  /* 
   * Allocate symbol list whose size
   * correspond to number of data published in BB
   * which may be distributed with TSP.
   */
  X_SSIList       = TSP_SSIList_new(nbTspSymbols);
  X_SSIList_value = &(X_SSIList->TSP_sample_symbol_info_list_t_val[0]);

  /* 
   * Allocate array of pointer to data
   */
  value_by_pgi      = (void **) calloc(nbTspSymbols,sizeof(void*));
  bbdatadesc_by_pgi = (S_BB_DATADESC_T **) calloc(nbTspSymbols,sizeof(S_BB_DATADESC_T*));
  bbindex_to_pgi    = (int32_t *) calloc(bb_get_nb_item(shadow_bb),sizeof(int32_t));  
  /*
   * Allocate write 'right' management array
   */
  allow_to_write = (int *) calloc(nbTspSymbols,sizeof(int));

  assert(value_by_pgi);	
  assert(bbdatadesc_by_pgi);	
  assert(bbindex_to_pgi);
  assert(allow_to_write);
    
  /* initialize the provider global index to 0 */
  pgi = 0;
  for (i=0; i<bb_get_nb_item(shadow_bb);++i) {
    /* we skip unhandled BB_TYPE */ 
    if (bb_data_desc(shadow_bb)[i].type < E_BB_USER) {
      /* memorize the first PGI used for this bbindex */
      bbindex_to_pgi[i] = pgi;
		
      /* 
       * Alias case 
       * The alias case will generate TSP symbol name like:
       * targetname.aliasnameL1.aliasnameL2
       * NOTE THAT WE DO NOT HANDLE THE CASE OF ALIAS
       * ON A ARRAY OF NON-USER TYPE.
       */
      if (bb_isalias(&bb_data_desc(shadow_bb)[i])) {
	aliasstack[0]=bb_data_desc(shadow_bb)[i];
	aliasstack_size = MAX_ALIAS_LEVEL;
	bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);
	memset(indexstack,0,MAX_ALIAS_LEVEL*sizeof(int32_t));
	indexstack_len = 0;
	/*
	 * Don't count first alias dimension itself since
	 * it will be published as an TSP array
	 * So j>0 and not j>=0
	 */
	for (j=aliasstack_size-1; j>0; --j) {
	  if (aliasstack[j].dimension > 1) {
	    indexstack[indexstack_len]=0;
	    ++indexstack_len;
	  }
	}
	
	/* 
	 * Alias with array a intermediate alias 
	 */
	if (indexstack_len){
	  /* Compute the maximum length of symbol
	   * which is the name of the symbol itself plus
	   * the size of successive index '[nnnnnnnn]'
	   * like target[45].aliasL1[23].aliasL2[999].aliasL3[]
	   */
	  i_temp = strlen(bb_data_desc(shadow_bb)[i].name) + 10*indexstack_len;
	  do {

	    tempName = calloc(1,i_temp);
	    assert(tempName);
	    /* 
	     * Build alias array name beginning with 
	     * second alias in aliasstack
	     * (first alias is the symbol itself which may be published
	     *  as an array if it is an array)
	     */
	    bb_get_array_name(tempName,
			      i_temp,
			      &aliasstack[1], aliasstack_size-1,
			      indexstack, indexstack_len);	   

	    /* Add last part */
	    strncat(tempName,rindex(bb_data_desc(shadow_bb)[i].name,'.'),i_temp);

	    TSP_SSI_initialize(&X_SSIList_value[pgi],
			       tempName,
			       pgi,
			       0, /* pgridx NA provider-side */
			       0, /* pgrank NA provider-side */
			       bb_type2tsp_type[bb_data_desc(shadow_bb)[i].type],
			       bb_data_desc(shadow_bb)[i].dimension,
			       0, /* offset */
			       bb_data_desc(shadow_bb)[i].dimension, /* nelem */
			       0, /* period */
			       0 /* phase  */
			       );

	    free(tempName);

	    /* Update data pointer with appropriate value */
	    value_by_pgi[pgi]      = bb_item_offset(shadow_bb, &bb_data_desc(shadow_bb)[i], indexstack, indexstack_len);
	    bbdatadesc_by_pgi[pgi] = &bb_data_desc(shadow_bb)[i];
	    allow_to_write[pgi]    = TSP_ASYNC_WRITE_ALLOWED;
	    ++pgi;
	  }
	  while (BB_OK == bb_alias_increment_idxstack(&aliasstack[1], aliasstack_size-1, indexstack, indexstack_len));
	}
		
	/* Simple alias */
	else {

	  TSP_SSI_initialize(&X_SSIList_value[pgi],
			     bb_data_desc(shadow_bb)[i].name,
			     pgi,
			     0, /* pgridx NA provider-side */
			     0, /* pgrank NA provider-side */
			     bb_type2tsp_type[bb_data_desc(shadow_bb)[i].type],
			     bb_data_desc(shadow_bb)[i].dimension,
			     0, /* offset */
			     bb_data_desc(shadow_bb)[i].dimension, /* nelem */
			     0, /* period */
			     0 /* phase  */
			     );

	  value_by_pgi[pgi]      = bb_item_offset(shadow_bb, &bb_data_desc(shadow_bb)[i], indexstack, indexstack_len);
	  bbdatadesc_by_pgi[pgi] = &bb_data_desc(shadow_bb)[i];
	  allow_to_write[pgi]    = TSP_ASYNC_WRITE_ALLOWED;
	  ++pgi;
	  
	}
      }
      /* Non-Alias case (Same as simple alias) */
      else {
	TSP_SSI_initialize(&X_SSIList_value[pgi],
			   bb_data_desc(shadow_bb)[i].name,
			   pgi,
			   0, /* pgridx NA provider-side */
			   0, /* pgrank NA provider-side */
			   bb_type2tsp_type[bb_data_desc(shadow_bb)[i].type],
			   bb_data_desc(shadow_bb)[i].dimension,
			   0, /* offset */
			   bb_data_desc(shadow_bb)[i].dimension, /* nelem */
			   0, /* period */
			   0 /* phase  */
			   );

	value_by_pgi[pgi]      = ((void*) ((char*)bb_data(shadow_bb) + bb_data_desc(shadow_bb)[i].data_offset));
	bbdatadesc_by_pgi[pgi] = &bb_data_desc(shadow_bb)[i];
	allow_to_write[pgi]    = TSP_ASYNC_WRITE_ALLOWED;
	++pgi;
      }
    } else  { /* skip unhandled BB type */ 
      STRACE_INFO(("Skipping unhandled symbol type <%d> name <%s>",bb_data_desc(shadow_bb)[i].type,bb_data_desc(shadow_bb)[i].name));
    }
  } /* loop over bb items */
    
  return retcode;
} /* end of BB_GLU_init */


int  BB_GLU_get_symbol_number(GLU_handle_t* this) {

  return nbTspSymbols;

}  /* end of GLU_get_symbol_number */

int  
BB_GLU_get_sample_symbol_info_list(GLU_handle_t* h_glu, 
				   TSP_sample_symbol_info_list_t* symbol_list) {

  symbol_list->TSP_sample_symbol_info_list_t_len = X_SSIList->TSP_sample_symbol_info_list_t_len;
  symbol_list->TSP_sample_symbol_info_list_t_val = X_SSIList->TSP_sample_symbol_info_list_t_val;
	    
  return TRUE;
} /* BB_GLU_get_sample_symbol_info_list */

int 
BB_GLU_get_pgi(GLU_handle_t* this, TSP_sample_symbol_info_list_t* symbol_list, int* pg_indexes) {
  
  int     i=0;
  int 	  j=0;
  int     ret=TRUE;

  int     pgi_offset=0;
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
  for (i = 0 ; i < symbol_list->TSP_sample_symbol_info_list_t_len ; i++) {
  
    /* Get short name (without [XXX], for array) and indexstack and its length */                                
    memset(&sym_data_desc,0,sizeof(S_BB_DATADESC_T));
    if (bb_utils_parsearrayname(symbol_list->TSP_sample_symbol_info_list_t_val[i].name, 
				sym_data_desc.name,
				VARNAME_MAX_SIZE,
				array_index, &array_index_len)) {
	   STRACE_INFO  (("%s: cannot parse symname <%s>",
			  "BB_GLU_get_pgi",
			  symbol_list->TSP_sample_symbol_info_list_t_val[i].name));
	   ret = FALSE;
	   continue;
	 } 	 
	 else {
	   STRACE_DEBUG  (("%s: array name <%s> parsed to symname <%s>",
			   "BB_GLU_get_pgi",
			   symbol_list->TSP_sample_symbol_info_list_t_val[i].name,
			   sym_data_desc.name));
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

	     /* 
	      * Array index is in reverse order of aliasstack.
	      *
	      * Array index is ordered by 'reading' order:
	      *  target[idx0].aliasL1[idx1].aliasL2[idx2]
	      * whereas aliasstack is ordered by alias to target resolution order
	      *  aliasL2 --> aliasL1 --> target
	      */
	     array_index_ptr   = array_index_len-1;
	     /* Initialise aliasstack */
	     aliasstack_size   = MAX_ALIAS_LEVEL;
	     aliasstack[0]     = sym_data_desc;
	     bb_find_aliastack(shadow_bb, aliasstack, &aliasstack_size);	     
	     
	     /* symbol not found skip to next symname */
	     if (-1==bbidx) {
	       pg_indexes[i] = -1;
	       ret=FALSE;
	       STRACE_INFO(("Symbol=%s, not found",symbol_list->TSP_sample_symbol_info_list_t_val[i].name));
	       continue;
	     } else {
	       /* 
		* Examine whether symbol is of array type or not
		* and validate index range in indexstack vs dimension specified in aliasstack
		* (remember indexstack is in the reverse order of aliasstack so the --array_index_ptr)
		* if it is of array type.
		*/
	       for (j=0; j<aliasstack_size; ++j) {
		 /* validate */
		 if ((aliasstack[j].dimension > 1) && (aliasstack[j].dimension < array_index[array_index_ptr])) {
		   STRACE_INFO(("Symbol=%s, found but index=%d out of range for element <%s>",
				symbol_list->TSP_sample_symbol_info_list_t_val[i].name,
				array_index[array_index_ptr],
				aliasstack[j].name));
		   ret = FALSE;
		 }
		 /* go to next index on (index)stack */
		 else if (aliasstack[j].dimension > 1){
		   --array_index_ptr;
		 }
	       }
	       if (ret == FALSE) {continue;}
	       else {		 
		 /* 
		  * Magic formula for fast rebuild of PGI 
		  * from indexstack and aliasstack 
		  * We need to calculate PGI the same way we did during initialize.
		  * The PGI of target[idx0].aliasL1[idx1].aliasL2[idx2]
		  * should be 
		  * pgiof(target.aliasL1.aliasL2) + 
		  *      idx0*target.dimension  + 
		  *      idx1*aliasL1.dimension
		  * 
		  * NOTE THAT WE DO NOT HANDLE THE CASE OF ALIAS
		  * ON A ARRAY OF NON-USER TYPE.
		  */

		 /* 
		  * reset array_index_ptr to the end 
		  * (again remember alias and index stacks are reversed)
		  */
		 array_index_ptr = array_index_len-1;
		 pgi_offset = 0;
		 /*
		  * Skip index of aliastack[0] if it's an array
		  * since TSP now handles arrays.
		  */
		 if (aliasstack[0].dimension > 1) {
		   --array_index_ptr;
		 }
		 /* now computes pgi offset */
		 for (j=1; j<aliasstack_size; ++j) {
		   if (aliasstack[j].dimension > 1) {
		     if (array_index_ptr == array_index_len-1) {
		       pgi_offset += array_index[array_index_ptr];
		     }
		     else {
		       pgi_offset += array_index[array_index_ptr] * aliasstack[previous_array_ptr].dimension;
		     }
		     previous_array_ptr = j;
		     --array_index_ptr;
		   }
		 }
		 
		 pg_indexes[i] = bbindex_to_pgi[bbidx] + pgi_offset;
		 STRACE_INFO(("Symbol=%s, found index=%d",symbol_list->TSP_sample_symbol_info_list_t_val[i].name,pg_indexes[i]));
	       }
	     }
	   }
	 }

    /* complete symbol validation */
    GLU_validate_sample_default(&(symbol_list->TSP_sample_symbol_info_list_t_val[i]), 
				(-1==pg_indexes[i]) ? NULL : &(X_SSIList_value[pg_indexes[i]]),
				&(pg_indexes[i]));
  }

  STRACE_INFO(("End of symbol Valid")); 
  
  return ret;
} /* BB_GLU_get_pgi */

void* BB_GLU_thread(void* arg) {
  
  int i;
  glu_item_t item;
  double     item_value;
  sigset_t s_mask;
  int nb_consumed_symbols;
  int* ptr_consumed_index;
  int pgi;
  GLU_handle_t* this = (GLU_handle_t*)arg;
  
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
  glu_time       = 0;
  /* FIXME for now its ok to only have scalar DOUBLE value */
  item.raw_value = &item_value; 
  item.size      = 1*tsp_type_size[TSP_TYPE_DOUBLE];

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
    TSP_datapool_get_reverse_list (this->datapool,&nb_consumed_symbols, &ptr_consumed_index); 


    /* acknowledge copy end if bb_provider was telled to do so */
    if ( *((int*)(this->private_data)) ) {
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
      TSP_datapool_push_next_item(this->datapool, &item);      
    }
    TSP_datapool_push_commit(this->datapool, glu_time, GLU_GET_NEW_ITEM);
      
    ++glu_time;
  }

  return NULL;
  
} /* end of BB_GLU_thread */

int 
BB_GLU_async_sample_write(GLU_handle_t* glu, 
			  int provider_global_index, void* value_ptr, uint32_t value_size) {
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
	if (provider_global_index>=0 && provider_global_index<nbTspSymbols) {		
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
BB_GLU_async_sample_read(GLU_handle_t* glu, 
			 int provider_global_index, void* value_ptr, uint32_t* value_size) {
	S_BB_DATADESC_T* data_desc;
	int retcode = BB_NOK; 
	void*  genuineBBdata;
	
	STRACE_DEBUG(("BB_PROVIDER want to AsyncRead : pgi <%d> (value_size allowed=%d)",provider_global_index,*value_size));
	
	/* try to read */
	if (provider_global_index>=0 && provider_global_index<nbTspSymbols) {

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
  
  if(provider_global_index>=0 && provider_global_index<nbTspSymbols){
  	allow_to_write[provider_global_index] = TSP_ASYNC_WRITE_ALLOWED;
	retcode = BB_OK;
  }	
  return retcode;
} /* bb_tsp_provider_allow_write_symbol */


int32_t  
bb_tsp_provider_forbid_write_symbol(int provider_global_index){

  int32_t retcode;
  retcode = BB_NOK;
  
  if(provider_global_index>=0 && provider_global_index<nbTspSymbols){
  	allow_to_write[provider_global_index] = TSP_ASYNC_WRITE_FORBIDDEN;
	retcode = BB_OK;
  }	
  return retcode;
} /* end of bb_tsp_provider_forbid_write_symbol */


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
} /* bb_tsp_provider_setbbname */

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
} /* bb_tsp_provider_create_GLU */


int32_t 
bb_tsp_provider_initialise(int* argc, char** argv[],int TSPRunMode, const char* bbname,
			   double frequency, int32_t acknowledgeCopy) {
  
  int32_t retcode;

  bb_tsp_provider_create_GLU(frequency,acknowledgeCopy,GLU_SERVER_TYPE_ACTIVE);
  bb_tsp_provider_setbbname(bbname);
  
  /* Init LibTSP provider */
  if (TSP_STATUS_OK!=TSP_provider_init(bbGLU,argc, argv)) {
    retcode = BB_NOK;
    return retcode;
  }

  /* Start-up provider Lib */
  if (TSP_STATUS_OK!=TSP_provider_run(TSPRunMode)) {
    retcode = BB_NOK;
    return retcode;
  }

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


