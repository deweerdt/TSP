/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_group_algo.c,v 1.9 2002-12-24 14:14:18 tntdev Exp $

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

Purpose   : Implementation for the function used to calculate
and use groups

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_group_algo.h"
#include "tsp_group_algo_data.h"

#include "tsp_datapool.h"
#include "tsp_data_sender.h"

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
* @symbols list of symbols on which the number of groups must be calculated
* @return Total Number of groups for the list
*/
int TSP_group_algo_get_nb_groups(const TSP_sample_symbol_info_list_t* symbols)
{
  SFUNC_NAME(TSP_group_algo_get_nb_groups);

	
  guint32 nb_symbols = symbols->TSP_sample_symbol_info_list_t_len;
  guint32 i;
  int nb_groups = 0;
   
    
  STRACE_IO(("-->IN"));

    
  assert(symbols);
    
  if( nb_symbols > 0 )
    {
        
      /* We calculate the LCM for the period of all symbols */
      int ppcm = (symbols->TSP_sample_symbol_info_list_t_val[0].period);
      STRACE_DEBUG(("frequency_ratio No0= %d", ppcm));

      for( i = 1 ; i < nb_symbols ; i++)
	{
                
                 
	  int frequency_ratio =
	    (symbols->TSP_sample_symbol_info_list_t_val[i].period);
                  
	  STRACE_DEBUG(("frequency_ratio No%u= %d", i, frequency_ratio));

         /* LCM */         
	  PPCM(ppcm,frequency_ratio, ppcm); 
	}
        
      nb_groups = ppcm;
        
    }
  else
    {
      STRACE_WARNING(("No symbols in list !"));

    }

  STRACE_DEBUG(("Nombre de groupes = %d", nb_groups));

  STRACE_IO(("-->OUT"));

  return nb_groups;
    
}

/**
* Get the size of a given group.
* @symbols list of symbols on which the group size must be calculated
* @group_id Id of the group that must be processed
* @return Number of symbols in the group group_id;
*/
static int TSP_group_algo_get_group_size(const TSP_sample_symbol_info_list_t* symbols, 
					 int group_id)

{
    
  SFUNC_NAME(TSP_group_algo_get_group_size);

    

    
  int group_size = 0;
  guint32 nb_symbols = symbols->TSP_sample_symbol_info_list_t_len;
  guint32 i;

  STRACE_IO(("-->IN"));

    
  assert(symbols);
    
  /* We search all symbols that belong to the group group_number*/
  for( i = 0 ; i < nb_symbols ; i++ )
    {
      int frequency_ratio = symbols->TSP_sample_symbol_info_list_t_val[i].period;
      int phase = symbols->TSP_sample_symbol_info_list_t_val[i].phase;
      /* Does - it belong to the group */
      if(BELONGS_TO_GROUP(frequency_ratio, phase , group_id) )
        {
	  /*Yes ! */
	  group_size++;
        }
        
    }
    
  STRACE_IO(("-->OUT group_size for group[%d] is %d", group_id, group_size));

    
  return group_size;
}

/**
* Get the summed size of all groups.
* @symbols list of symbols on which the group size must be calculated
* @nb_groups Total number of groups
* @return The summed size of all groups
*/
static int TSP_group_algo_get_groups_summed_size(const TSP_sample_symbol_info_list_t* symbols,
						 int nb_groups)
{
    
  SFUNC_NAME(TSP_group_algo_get_groups_summed_size);

    

    
  int groups_summed_size = 0;
  int group_id;

  STRACE_IO(("-->IN"));

    
  assert(symbols);
    
  /* For all groups, get the size and sum the result */
  for( group_id = 0 ; group_id < nb_groups ; group_id++ )
    {
        
      groups_summed_size += TSP_group_algo_get_group_size(symbols, group_id); 
    }
    
  STRACE_IO(("-->OUT groups_summed_size is %d", groups_summed_size));

    
  return groups_summed_size;
}




/**
* Allocate the group table.
* This function initialize several size informations located in the structures
* @symbols list of symbols on which the group table must be calculated
* @return The allocated table of groups
*/
static TSP_algo_table_t*
TSP_group_algo_allocate_group_table(const TSP_sample_symbol_info_list_t* symbols)
{
  SFUNC_NAME(TSP_group_algo_allocate_group_table);

    
  int group_id;
    
  TSP_algo_table_t* table = 0;
  TSP_algo_group_item_t* items_table = 0;
    
  STRACE_IO(("-->IN"));

  assert(symbols);
    
  table = (TSP_algo_table_t*)calloc(1, sizeof(TSP_algo_table_t));
  TSP_CHECK_ALLOC(table, 0);
    
  /* Get total number of groups */
  table->table_len = TSP_group_algo_get_nb_groups(symbols);
    
  /*Allocate room for all groups */
  table->groups = (TSP_algo_group_t*)calloc(table->table_len, sizeof(TSP_algo_group_t));
  TSP_CHECK_ALLOC(table->groups, 0);
    
  /*Allocate room for all group items*/
  table->groups_summed_size = TSP_group_algo_get_groups_summed_size(symbols, table->table_len);
  items_table = (TSP_algo_group_item_t*)calloc(table->groups_summed_size, sizeof(TSP_algo_group_item_t));
  table->all_items = items_table;
  TSP_CHECK_ALLOC(items_table, 0);
        
  /*Initialize groups items*/
  /* And make them point at the right place in the item list */
  for(group_id = 0 ;
      group_id < table->table_len ;
      group_id++)
    {
            
      /* Get size of group i */
      table->groups[group_id].group_len = TSP_group_algo_get_group_size(symbols,
                                                                        group_id);
      /* Correct items pointer */
      table->groups[group_id].items = items_table;

      /* Memorise max group len */
      if (table->max_group_len <  table->groups[group_id].group_len)
	{
	  table->max_group_len = table->groups[group_id].group_len;
	}

      
                
      /* Get ready for next round ! */        
      items_table += table->groups[group_id].group_len;

    }
    
  
  STRACE_DEBUG(("Max group size = %d", table->max_group_len));
  STRACE_IO(("-->OUT"));

    
  return table;
}
                                                             
void TSP_group_algo_destroy_symbols_table(TSP_groups_t* groups)
{
   SFUNC_NAME(TSP_group_algo_destroy_symbols_table);

   TSP_algo_table_t* table = (TSP_algo_table_t*)groups;

   STRACE_IO(("-->IN"));
   if(table)
     {
       free(table->all_items);
       free(table->groups);
       free(table);
     }

   STRACE_IO(("-->OUT"));

}

void TSP_group_algo_create_symbols_table_free_call(TSP_sample_symbol_info_list_t* symbols)
{
  SFUNC_NAME(TSP_group_algo_create_symbols_table_free_call);
  int i;

   STRACE_IO(("-->IN"));
   
   for( i=0 ; i < symbols->TSP_sample_symbol_info_list_t_len ; i++)
     {
       free(symbols->TSP_sample_symbol_info_list_t_val[i].name);
       symbols->TSP_sample_symbol_info_list_t_val[i].name = 0;
     }

   free(symbols->TSP_sample_symbol_info_list_t_val);

   STRACE_IO(("-->OUT"));
}

int TSP_group_algo_create_symbols_table(const TSP_sample_symbol_info_list_t* in_symbols,
					TSP_sample_symbol_info_list_t* out_symbols,
					TSP_groups_t* out_groups,
					TSP_datapool_t datapool)
{
       
  SFUNC_NAME(TSP_group_algo_create_symbols_table);

    
  TSP_algo_table_t* table;
    
  int rank; /**< rank in a group */
  int group_id; /**< id for a group */
    
  /* total number of in symbols*/
  guint32 nb_symbols = in_symbols->TSP_sample_symbol_info_list_t_len;
    
  guint32 out_current_info, in_current_info;
  TSP_sample_symbol_info_t* in_info;
  TSP_sample_symbol_info_t* out_info;
    
  STRACE_IO(("-->IN"));

    
  table = TSP_group_algo_allocate_group_table(in_symbols);
  *out_groups = table;
    
  if(table)
    {
      int frequency_ratio;
      int phase;
    
      /* Allocate memory for the out_symbols */
      out_symbols->TSP_sample_symbol_info_list_t_len = table->groups_summed_size;
      out_symbols->TSP_sample_symbol_info_list_t_val = 
	(TSP_sample_symbol_info_t*)calloc(table->groups_summed_size, sizeof(TSP_sample_symbol_info_t));
      TSP_CHECK_ALLOC(out_symbols->TSP_sample_symbol_info_list_t_val, FALSE);
        
      /* For each group...*/
      for( out_current_info = 0, group_id = 0 ; group_id < table->table_len ; group_id++)
        {
            
	  /* For each symbol, does it belong to the group ? */
	  /* Start at first rank */
	  for(rank = 0, in_current_info = 0 ; in_current_info < nb_symbols ; in_current_info++)
            {
	      out_info = &(out_symbols->TSP_sample_symbol_info_list_t_val[out_current_info]);
	      in_info = &(in_symbols->TSP_sample_symbol_info_list_t_val[in_current_info]);

            
	      /* Does - it belong to the group */
	      if(BELONGS_TO_GROUP(in_info->period, in_info->phase , group_id) )
                {
		  
		  /*Yes, we add it*/
		  STRACE_DEBUG(("Adding provider_global_index %d at group %d and rank %d",
                                in_info->provider_global_index,
                                group_id,
                                rank))
                    /* 1 - In the out group table */
		    
                    /* FIXME : When more types (RAW, STRING...) will be managed, there will be
		       other functions like TSP_data_sender_get_string_encoder or TSP_data_sender_get_raw_encoder */
                    table->groups[group_id].items[rank].data_encoder = TSP_data_sender_get_double_encoder();
		  
		  
		  table->groups[group_id].items[rank].data = 
		    TSP_datapool_get_symbol_value(datapool, in_info->provider_global_index, 0);
		  
		  /* 2 - In the out symbol table */
		  
		  (*out_info) = (*in_info);
                    
		  out_info->name = strdup(in_info->name);
		  TSP_CHECK_ALLOC(out_info->name, FALSE);
                    
		  out_info->provider_group_index = group_id;
		  out_info->provider_group_rank = rank;
                    
		  /* inc */
		  rank++;
		  out_current_info++;
                    
		  /* Some optimization  : the group might be full*/
		  if(rank == table->groups[group_id].group_len) break;
                }
                
            }
        }
    }
  else
    {
      STRACE_ERROR(("Unable to allocate group table"));

    }
    
  STRACE_IO(("-->OUT"));

    
  return TRUE;
}

int TSP_group_algo_get_group_number(TSP_groups_t* groups)
{
       
  SFUNC_NAME(TSP_group_algo_get_group_number);

    
  TSP_algo_table_t* group_table = (TSP_algo_table_t*)groups;
    
  STRACE_IO(("-->IN"));

    
  assert(groups);
    
  STRACE_IO(("-->OUT"));

    
  return group_table->table_len;
}

int TSP_group_algo_get_biggest_group_size(TSP_groups_t* groups)
{
  SFUNC_NAME(TSP_group_algo_get_biggest_group_size);

    
  TSP_algo_table_t* group_table = (TSP_algo_table_t*)groups;
    
  STRACE_IO(("-->IN"));
    
  assert(groups);
    
  STRACE_IO(("-->OUT"));
    
  return group_table->max_group_len;

}
