/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_group.c,v 1.2 2002-10-01 15:32:16 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implemenation the groups management

-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include "tsp_group_data.h"
#include "tsp_group.h"

#include "tsp_data_receiver.h"

/*---------------------------------------------------------*/
/*                  FONCTIONS INTERNES  			       */
/*---------------------------------------------------------*/

/*---------------------------------------------------------*/
/*                  FONCTIONS EXTERNE  			            */
/*---------------------------------------------------------*/

/**
* Allocate the group table.
* This function initialize several size informations located in the structures
* @symbols list of symbols on which the group table must be calculated
* @return The allocated table of groups
*/
TSP_groups_t
TSP_group_create_group_table(const TSP_sample_symbol_info_list_t* symbols, int group_number)
{
  /*FIXME : Faire la desallocation correspondante */

  SFUNC_NAME(TSP_group_create_group_table);

    
  int group_id, old_group_id;
  int i;
    
  TSP_group_table_t* table = 0;
  TSP_group_item_t* items_table = 0;

    
  STRACE_IO(("-->IN"));

  assert(symbols);
    
  table = (TSP_group_table_t*)calloc(1, sizeof(TSP_group_table_t));
  TSP_CHECK_ALLOC(table, 0);
    
    /* Get total number of groups */
  table->table_len = group_number;
    
  /*Allocate room for all groups */
  table->groups = (TSP_group_t*)calloc(table->table_len, sizeof(TSP_group_t));
  TSP_CHECK_ALLOC(table->groups, 0);
    
    /*Allocate room for all group items*/
  table->groups_summed_size = symbols->TSP_sample_symbol_info_list_t_len;
  items_table = (TSP_group_item_t*)calloc(table->groups_summed_size, sizeof(TSP_group_item_t));
  TSP_CHECK_ALLOC(items_table, 0);
        
  /*Initialize groups items*/
  /* And make them point at the right place in the item list */
    

  i = 0;
  table->max_group_len = 0;
  for(group_id = 0; group_id < group_number; group_id++) 
    {
     
      /* Rank for the symbol in a group*/
      int rank = 0;
     
      /* Correct items pointer */
      table->groups[group_id].items = items_table;
        
      while(i < symbols->TSP_sample_symbol_info_list_t_len)
	{
    
	  if(group_id == symbols->TSP_sample_symbol_info_list_t_val[i].provider_group_index)
	    {
	      
	      table->groups[group_id].items[rank].provider_global_index = symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index;

	      /* FIXME : en fonction du type, appeler la bonne fonction */
	      table->groups[group_id].items[rank].data_decoder = TSP_data_receiver_get_double_decoder();
	      table->groups[group_id].items[rank].sizeof_encoded_item = TSP_data_receiver_get_double_encoded_size();
	      table->groups[group_id].sizeof_encoded_group += table->groups[group_id].items[rank].sizeof_encoded_item;
	      
	      STRACE_DEBUG(("Added to group table Id=%d, Gr=%d, Rank=%d", 
			    table->groups[group_id].items[rank].provider_global_index,
			    group_id,
			    rank));
	      rank++; 
	      i++;
	    }  
	  else
	    {
	      break;
	    }
        
            
	}
      /* set group size */
      table->groups[group_id].group_len = rank;

      /* step thrue table */
      items_table += rank;
      
      /* calculate max group size*/
      if( table->groups[group_id].group_len > table->max_group_len)
	{
	  table->max_group_len = table->groups[group_id].group_len;
	
	}

    }
            
     
  STRACE_IO(("-->OUT"));

    
  return table;
}
                                                             
