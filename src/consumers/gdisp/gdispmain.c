/*

$Header: /home/def/zae/tsp/tsp/src/consumers/gdisp/gdispmain.c,v 1.9 2006-02-26 13:36:05 erk Exp $

-----------------------------------------------------------------------

Project   : GDISP / TSP

Component : GDISP

-----------------------------------------------------------------------

Purpose   : main functions for GDISP program

-----------------------------------------------------------------------
*/


/**
 * @defgroup TSP_GDisp GDisp
 * @ingroup  TSP_Consumers
 * tsp_gdisp is the first generation TSP consumer GUI.
 * This a gtk+1.2, GUI we may be used to efficiently draw
 * or view TSP sample symbol. It has a simple XML configuration
 * file which may be used to described the TSP Symbols you
 * want to draw or view.
 * \par \c tsp_gdisp \c [-u TSP_provider URL ] \c -x config.xml
 */

#include <unistd.h>
#include <stdio.h>
#include <assert.h>

extern char *optarg;
#include <tsp_consumer.h>

#include "interface.h"
#include "page.h"


/*
 *  The TSP object
 */
TSP_provider_t tsp = 0;

/*
 *  The displayed pages
 */
display_page* pages;

/* 
 * For each provider global index this array gives
 * us a list of var that must be refreshed with this value
 * when the TSP provider send them to us
 */
variable*** index2vars;


/*
 * Find the tsp provider global index for each requested variable,
 * and store in pages array 
 */ 
void init_tsp_index(void)
{
  const TSP_consumer_symbol_requested_list_t* symbols;
  int i, j, k;
  variable* var;
  int maxid = 0;

  symbols = TSP_consumer_get_requested_sample(tsp);
  assert(symbols);

  for (i=0 ; i < symbols->len ; i++)
    {
      for (j=0; j < conf_data.nb_page; j++)
	{
	  for (k=0; k < pages[j].variables->len; k++)
	    {
	      var =  g_ptr_array_index(pages[j].variables, k);	      
	      if(!strcmp(symbols->val[i].name, var->text) && (var->type != VAR_TITLE ) )
		{
		  var->provider_global_index = symbols->val[i].index;
		}
	    }
	}      
    }
  
}

/*
 * @param index a provider global index
 * @return total number of vars for the given provider global index
 */ 
int get_nbvars_per_index(int index)
{
 
  int j, k;
  variable* var;
  int nbvars = 0;

  for (j=0; j < conf_data.nb_page; j++)
    {
      for (k=0; k < pages[j].variables->len; k++)
	{
	  var =  g_ptr_array_index(pages[j].variables, k);	      
	  if(var->provider_global_index == index)
	    {
	      nbvars++;
	    }
	}
    }      
  
  return nbvars;

}

/**
 * Find the index2vars array size, that is the max
 * of all provider global index
 * @return size of index2var array
 */ 
int get_index2vars_size(void)
{
  const TSP_consumer_symbol_requested_list_t* symbols;
  int i;
  int maxid = 0;

  /* ask TSP the list of asked symbols */
  symbols = TSP_consumer_get_requested_sample(tsp);
  assert(symbols);
  
  /* find the max of all provider global index */
  for (i=0 ; i < symbols->len ; i++)
    {
      if (maxid < symbols->val[i].index )
	{
	  maxid = symbols->val[i].index;
	}
    }
  
  return maxid+1;
}



/**
 * Find the tsp provider global index for each requested variable,
 * and store in pages array 
 */ 
void init_index2vars(void)
{
  int size;
  int i, j, k;
  

  /* for each variable, when find the linked provider global id */
  init_tsp_index();
  
  size = get_index2vars_size();
                       
  index2vars = (variable***)calloc(size, sizeof(variable**));
  assert(index2vars);

  for( i = 0 ; i < size ; i++)
    {
      int nbvars = get_nbvars_per_index(i);
      int pos = 0;
      /* some index does not have any symbol */
      if(nbvars > 0)
	{
	  /*Allocate room for all var with a trailing NULL */
	  index2vars[i] = (variable**)calloc(nbvars + 1, sizeof(variable*));
	  assert(index2vars[i]);
	  
	  /* fill the tab founding all var for the index i */
	  for (j=0; j < conf_data.nb_page; j++)
	    {
	      for (k=0; k < pages[j].variables->len; k++)
		{
		  variable* var =  g_ptr_array_index(pages[j].variables, k);	      
		  if(var->provider_global_index == i)
		    {
		      index2vars[i][pos++] = var;
		    }
		}	      	  
	    }
	}
    }        
}


static int main_window_start(char* conf_file, char* tsp_prov_url)
{
  int		        i, j, nitem, ts_ok;
  char		        *f, name[1024];
  GdkGCValues	        gcvalues;
  /*Display			dsp;*/
  GtkWidget       *widget;

  /*Scrollbar	        v_scrollbar, h_scrollbar;*/
  int ret = FALSE;

  printf("Loading '%s' conf file\n", conf_file);
  /* Load configuration file  and initialise list of asked symbols */
  if (load_config(conf_file, &conf_data)) 
    {
      tsp = TSP_consumer_connect_url(tsp_prov_url);
      if(tsp)
	{
	  if(TSP_consumer_request_open(tsp, 0, 0))
	    {
	      if(TSP_consumer_request_filtered_information(tsp,TSP_FILTER_MINIMAL,MINIMAL_STRING))
		{
		
		  if(TSP_consumer_request_sample(tsp, &conf_data.tsp_requested))
		    {		  
		    
		      /* Create the list of variable per provider global id */
		      init_index2vars();
		    
		      if(TSP_consumer_request_sample_init(tsp,0,0))
			{
			  sprintf(name, "%s @ %s", conf_file, TSP_consumer_get_connected_name(tsp));
			  create_mainwin(&conf_data, name);
			
			  ret = TRUE;
			}
		      else
			{
			  fprintf(stderr, "Error while initializing data stream '%s'\n", tsp_prov_url);
			}
		    }
		  else
		    {		      
		      fprintf(stderr, "Error while asking for TSP symbols session on host '%s'\n", tsp_prov_url);
		      TSP_consumer_print_invalid_symbols(stderr,&conf_data.tsp_requested,tsp_prov_url);
		      fprintf(stderr, "Check your configuration file <%s>.\n", conf_file);
		    }
		}
	      else
		{
		  fprintf(stderr, "Error while asking for TSP information on host '%s'\n", tsp_prov_url);
		}
		
	    }
	  else
	    {
	      fprintf(stderr, "Error while opening TSP session on host '%s'\n", tsp_prov_url);
	    }	      
	}
      else
	{
	  fprintf(stderr, "unable to find any TSP provider on host '%s'\n", tsp_prov_url);
	}
    }
  
  return ret;
}

void usage(char *txt)
{
  printf("\nUSAGE : %s -x fileconf.xml [-u tsp_serverURL]\n\n", txt);
  printf(TSP_URL_FORMAT_USAGE);
}

int
main (int argc, char **argv) 
{
  char myopt; /* Options */
  char* config_file = NULL;
  char* tsp_prov_url = "";

  gtk_init(&argc, &argv);

  if(!TSP_consumer_init(&argc, &argv))
    return -1;
			
  while ((myopt = getopt(argc, argv, "u:x:h")) != -1)
    {
      switch(myopt)
        {
        case 'u': tsp_prov_url = optarg; break;
        case 'x': config_file = optarg; break;
	default: break;
	}
    }
 
  if(!config_file)
    {
      usage(argv[0]);
      return -1;
    }

  if(!main_window_start(config_file, tsp_prov_url))
    {
      usage(argv[0]);
      return -1;
    }
  
  gtk_main();
  return 0;
}
