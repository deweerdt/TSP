/*

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo.c,v 1.26 2007-11-30 15:38:22 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD, Robert PAGNOT and Arnaud MORVAN


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

Purpose   : Implementation for the function used to calculate
and use groups

-----------------------------------------------------------------------
 */

#include <string.h>
#ifdef _WIN32
    #define assert(exp)     ((void)0)
#else
    #include <assert.h>
#endif
#include "tsp_sys_headers.h"

#include "tsp_group_algo.h"
#include "tsp_group_algo_data.h"

#include "tsp_datapool.h"
#include "tsp_data_sender.h"
#include "tsp_common.h"
#include <tsp_encoder.h>
#include <tsp_decoder.h>

/** pgcd will be equal to the GCD od n1 and n2 */
#define PGCD(n1, n2, pgcd) \
{ \
    int ln2 = n2; \
    pgcd = n1; \
    if ( pgcd != ln2 ) \
        {for(;pgcd != ln2; (pgcd > ln2) ? (pgcd -= ln2) : (ln2 -= pgcd )); }\
}

/** ppcm will be equal to the LCM of n1 and n2 */
#define PPCM(n1, n2, ppcm) \
{ \
    int pgcd; \
    PGCD(n1, n2, pgcd); \
    ppcm = (n1 * n2) / pgcd ; \
}

/* True, if a given symbol with frequency_ratio and phase belongs to groupe
number group_number */
#define BELONGS_TO_GROUP(frequency_ratio, phase , group_number) \
    (!((group_number - NORMALIZED_PHASE(frequency_ratio, phase) ) % frequency_ratio))
    
/* Phase normalisation */
#define NORMALIZED_PHASE(frequency_ratio, phase) (phase % frequency_ratio )

/*-----------------------------------------------*/

/**
 * Get the total number of groups.
 * @param symbols list of symbols on which the number of groups must be calculated
 * @return Total Number of groups for the list
 */
int TSP_group_algo_get_nb_groups(const TSP_sample_symbol_info_list_t* symbols)
{
	
  uint32_t nb_symbols ;
  uint32_t i;
  int nb_groups       = 0;
   
  assert(symbols);

  nb_symbols  = symbols->TSP_sample_symbol_info_list_t_len;
    
  if( nb_symbols > 0 ) {
    
    /* We calculate the LCM for the period of all symbols */
    int ppcm = (symbols->TSP_sample_symbol_info_list_t_val[0].period);
    STRACE_DEBUG(("Frequency_ratio No0= %d", ppcm));
    
    for( i = 1 ; i < nb_symbols ; i++) {	
      
      int frequency_ratio =
	(symbols->TSP_sample_symbol_info_list_t_val[i].period);
      
      STRACE_DEBUG(("Frequency_ratio No%u= %d", i, frequency_ratio));
      
      /* LCM */         
      PPCM(ppcm,frequency_ratio, ppcm); 
    }
    
    nb_groups = ppcm;
    
  }
  else {
    STRACE_WARNING(("No symbols in list !"));    
  }
  STRACE_INFO(("Found <%d> group(s) for request with <%d> symbol(s)",nb_groups,nb_symbols));
  return nb_groups;    
}

/**
 * Get the size of a given group.
 * @symbols list of symbols on which the group size must be calculated
 * @group_id Id of the group that must be processed
 * @return Number of symbols in the group group_id;
 */
static int 
TSP_group_algo_get_group_size(const TSP_sample_symbol_info_list_t* symbols, 
			      int group_id, uint32_t* group_byte_size) {
    
  int group_size = 0;
  uint32_t nb_symbols = symbols->TSP_sample_symbol_info_list_t_len;
  uint32_t i;

  assert(symbols);
  assert(group_byte_size);

  *group_byte_size = 0;
    
  /* We search all symbols that belong to the group group_number*/
  for( i = 0 ; i < nb_symbols ; i++ ) {
      int frequency_ratio = symbols->TSP_sample_symbol_info_list_t_val[i].period;
      int phase           = symbols->TSP_sample_symbol_info_list_t_val[i].phase;
      /* Does - it belong to the group */
      if(BELONGS_TO_GROUP(frequency_ratio, phase , group_id) ) {
	/*Yes ! */
	group_size++;
	*group_byte_size +=  TSP_data_channel_get_encoded_size(symbols->TSP_sample_symbol_info_list_t_val[i].type)*symbols->TSP_sample_symbol_info_list_t_val[i].nelem;
      }      
    }
    
  STRACE_INFO(("Group_size for group[%d] is %d", group_id, group_size));    
  return group_size;
}

/**
* Get the summed size of all groups.
* @symbols list of symbols on which the group size must be calculated
* @nb_groups Total number of groups
* @return The summed size of all groups
*/
static int 
TSP_group_algo_get_groups_summed_size(const TSP_sample_symbol_info_list_t* symbols,
				      int nb_groups) {
    
  int groups_summed_size = 0;
  int group_id;
  uint32_t group_byte_size = 0;

  assert(symbols);
    
  /* For all groups, get the size and sum the result */
  for( group_id = 0 ; group_id < nb_groups ; group_id++ ) {    
    groups_summed_size += TSP_group_algo_get_group_size(symbols, group_id,&group_byte_size); 
  }
    
  STRACE_INFO(("Groups_summed_size is %d", groups_summed_size));
    
  return groups_summed_size;
} /* end of TSP_group_algo_get_groups_summed_size */


/**
 * Allocate the group table.
 * This function initialize several size informations located in the structures
 * @symbols list of symbols on which the group table must be calculated
 * @return The allocated table of groups, NULL on error.
 */
static TSP_algo_table_t*
TSP_group_algo_allocate_group_table(const TSP_sample_symbol_info_list_t* symbols)
{
    
  int group_id;
    
  TSP_algo_table_t*      table       = NULL;
  TSP_algo_group_item_t* items_table = NULL;
    
  assert(symbols);
    
  table = (TSP_algo_table_t*)calloc(1, sizeof(TSP_algo_table_t));
  TSP_CHECK_ALLOC(table, NULL);
    
  /* Get total number of groups */
  table->table_len = TSP_group_algo_get_nb_groups(symbols);

  /* Allocate room for all groups */
  table->groups = (TSP_algo_group_t*)calloc(table->table_len, sizeof(TSP_algo_group_t));
  TSP_CHECK_ALLOC(table->groups, NULL);
    
  /* 
   * Allocate room for all group items 
   * Note that a group item correspond to a requested symbol
   * which means each item in a group may not have the same SIZE (in byte)
   * nevertheless the "group size" is its number of item not
   * its "byte" size.
   */
  table->groups_summed_size = TSP_group_algo_get_groups_summed_size(symbols, table->table_len);
  items_table = (TSP_algo_group_item_t*)calloc(table->groups_summed_size, sizeof(TSP_algo_group_item_t));
  table->all_items = items_table;
  TSP_CHECK_ALLOC(items_table, NULL);
        
  /* Initialize groups items */
  /* And make them point at the right place in the item list */
  for(group_id = 0 ;
      group_id < table->table_len ;
      group_id++) {
            
      /* Get size of group i */
      table->groups[group_id].group_len = TSP_group_algo_get_group_size(symbols,
                                                                        group_id,
									&(table->groups[group_id].group_byte_size));
      /* Correct items pointer */
      table->groups[group_id].items = items_table;

      /* Memorize max group len */
      if (table->max_group_len <  table->groups[group_id].group_len) {
	table->max_group_len = table->groups[group_id].group_len;
      }

      /* Same with byte size */
      if (table->group_max_byte_size < table->groups[group_id].group_byte_size) {
	table->group_max_byte_size = table->groups[group_id].group_byte_size;
      }
                     
      /* Get ready for next round ! */        
      items_table += table->groups[group_id].group_len;

    }

  STRACE_INFO(("Max group size      = %d", table->max_group_len));  
  STRACE_INFO(("Group max byte size = %d", table->group_max_byte_size));
  return table;
} /* TSP_group_algo_allocate_group_table */
                                                             
void 
TSP_group_algo_destroy_symbols_table(TSP_groups_t* groups) {

   TSP_algo_table_t* table = (TSP_algo_table_t*)groups;

   if (NULL!=table) {
       free(table->all_items);
       free(table->groups);
       free(table);
   }
} /* TSP_group_algo_destroy_symbols_table */

int32_t 
TSP_group_algo_create_symbols_table(const TSP_sample_symbol_info_list_t* in_symbols,
				    TSP_sample_symbol_info_list_t* out_symbols,
				    TSP_groups_t* out_groups,
				    TSP_datapool_t* datapool) {
  
  TSP_algo_table_t* table;
    
  int rank;     /* rank in a group */
  int group_id; /* id for a group  */
    
  /* Total number of IN symbols 
   * (should be requested from incoming request_sample) 
   */
  uint32_t nb_symbols = in_symbols->TSP_sample_symbol_info_list_t_len;
    
  uint32_t out_current_info, in_current_info;
  TSP_sample_symbol_info_t* in_info;
  TSP_sample_symbol_info_t* out_info;
    
  /* Allocate group table from requested symbols */
  table = TSP_group_algo_allocate_group_table(in_symbols);
  *out_groups = table;

  /* Short circuit if allocation failed */
  TSP_CHECK_POINTER(table,TSP_STATUS_ERROR_MEMORY_ALLOCATION,"Unable to allocate group table");
    
  /* Allocate memory for the out_symbols */
  out_symbols->TSP_sample_symbol_info_list_t_len = table->groups_summed_size;
  out_symbols->TSP_sample_symbol_info_list_t_val = 
    (TSP_sample_symbol_info_t*)calloc(table->groups_summed_size, sizeof(TSP_sample_symbol_info_t));
  TSP_CHECK_ALLOC(out_symbols->TSP_sample_symbol_info_list_t_val, TSP_STATUS_ERROR_MEMORY_ALLOCATION);
  
  /* For each group...*/
  for( out_current_info = 0, group_id = 0 ; 
       group_id < table->table_len ; 
       group_id++) {
    
    /* For each symbol, does it belong to the group ? */
    /* Start at first rank */
    for(rank = 0, in_current_info = 0 ; 
	in_current_info < nb_symbols ; 
	in_current_info++) {
      
      out_info = &(out_symbols->TSP_sample_symbol_info_list_t_val[out_current_info]);
      in_info = &(in_symbols->TSP_sample_symbol_info_list_t_val[in_current_info]);
      
      /* Does - it belong to the group */
      if (BELONGS_TO_GROUP(in_info->period, in_info->phase , group_id) ) {		
	/*Yes, we add it*/
	STRACE_DEBUG(("Adding provider_global_index %d at group %d and rank %d",in_info->provider_global_index,group_id,rank));
	/* 1 - In the out group table */
	
	/* find the encoder of the data type */
	table->groups[group_id].items[rank].data_encoder = TSP_data_channel_get_encoder(in_info->type);
	table->groups[group_id].items[rank].dimension    = in_info->dimension;
	table->groups[group_id].items[rank].offset       = in_info->offset;
	table->groups[group_id].items[rank].nelem        = in_info->nelem;
	
	/* complete les infos */
    /*    
     * Beware of the following code hereafter
     * We are doing void* / char* pointer arithmetic in order
     * to get the requested element of TSP symbol
     * which may be an array.
     * So     
     *  # TSP_datapool_get_symbol_value
     *    gives us the address of the first element in the array
     * 
     *  # + in_info->offset * tsp_type_size[in_info->type]
     *    should offset to the requested first element.
     *    
     *    The static array 'tsp_type_size' gives us the
     *    appropriate size of the 'native' type.             
     */
	table->groups[group_id].items[rank].data =
	  (char*)
	  TSP_datapool_get_symbol_value(datapool, in_info->provider_global_index)
	  + in_info->offset * tsp_type_size[in_info->type];
	
	/* 2 - In the out symbol table */
	
	(*out_info) = (*in_info);
        
	out_info->name = strdup(in_info->name);
	TSP_CHECK_ALLOC(out_info->name, TSP_STATUS_ERROR_MEMORY_ALLOCATION);
        
	out_info->provider_group_index = group_id;
	out_info->provider_group_rank  = rank;
        
	/* inc */
	rank++;
	out_current_info++;
        
	/* Some optimization  : the group might be full*/
	if(rank == table->groups[group_id].group_len) break;
      }      
    }
  }
    
  return TSP_STATUS_OK;
} /* end of TSP_group_algo_create_symbols_table */

int 
TSP_group_algo_get_group_number(TSP_groups_t* groups) {
       
  TSP_algo_table_t* group_table = (TSP_algo_table_t*)groups;    
  assert(groups);
  return group_table->table_len;
}

uint32_t 
TSP_group_algo_get_biggest_group_size(TSP_groups_t* groups) {
    
  TSP_algo_table_t* group_table = (TSP_algo_table_t*)groups;
  assert(groups);

  return group_table->group_max_byte_size;
}
