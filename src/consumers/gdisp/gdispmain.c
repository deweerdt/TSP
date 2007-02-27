/*

$Header: /home/def/zae/tsp/tsp/src/consumers/gdisp/gdispmain.c,v 1.14 2007-02-27 05:52:22 deweerdt Exp $

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
 * This a gtk+1.2 GUI, it may be used to efficiently draw
 * or view TSP sample symbol. It has a simple XML configuration
 * file which may be used to described the TSP Symbols you
 * want to draw or view.
 * The GDisp command line is the following:
 * \par tsp_gdisp  [-u TSP_provider URL ] -x config.xml
 *
 * \section Gdisp configuration file format (libpage)
 * \verbatim
  <?xml version="1.0"?> 
  <page_config display_frequency="10.0" period="1"  widget="draw" visible="true" no_border="false" rows="3" duration="50.0"> 
 
    <page title="Draw" x="50" y="50" width="450" height="440" rows="3" > 
       <variable name="t" type="DOUBLE"   /> 
       <variable name="Symbol2" type="DOUBLE"   /> 
       <variable name="Symbol3" type="DOUBLE"   /> 
       <variable name="Symbol4" type="DOUBLE"   /> 
       <variable name="Symbol5" type="DOUBLE"   /> 
       <variable name="Symbol6" type="DOUBLE"   />
    </page>
      <page title="View" x="510" y="50" width="0" height="0" widget="view" rows="14">
     <variable name="Title1" type="TITLE"   />
     <variable name="Symbol101" type="DOUBLE"   />
     <variable name="Symbol102" type="DOUBLE"   />
     <variable name="Symbol103" type="DOUBLE"   />
     <variable name="Symbol104" type="DOUBLE"   />
     <variable name="Title2" type="TITLE"   />
     <variable name="Symbol105" type="DOUBLE"   />
     <variable name="Symbol106" type="DOUBLE"   />
     <variable name="Title3" type="TITLE"   />
     <variable name="Symbol107" type="DOUBLE"   />
     <variable name="Symbol108" type="DOUBLE"   />
     <variable name="Symbol108" type="DOUBLE"   />
     <variable name="Symbol109" type="DOUBLE"   />
     <variable name="Symbol110" type="DOUBLE"   />
 
    <variable name="Title4" type="TITLE"   />
    <variable name="Symbol108" type="DOUBLE"   />
    <variable name="Symbol109" type="DOUBLE"   />
    <variable name="Symbol110" type="DOUBLE"   />
    <variable name="Title5" type="TITLE"   />
    <variable name="Symbol201" type="DOUBLE"   />
    <variable name="Symbol202" type="DOUBLE"   />
    <variable name="Symbol203" type="DOUBLE"   />
    <variable name="Symbol204" type="DOUBLE"   />
    <variable name="Symbol207" type="DOUBLE"   />
    <variable name="Symbol208" type="DOUBLE"   />
    <variable name="Symbol209" type="DOUBLE"   />
    <variable name="Symbol210" type="DOUBLE"   />
    <variable name="Symbol211" type="DOUBLE"   />
 </page>
 
 <page title="Mixed" x="510" y="330" width="0" height="0" widget="view" rows="4" >
    <variable name="Title" type="TITLE"   />
    <variable name="Symbol108" type="DOUBLE"   />
    <variable name="Symbol109" type="DOUBLE"   />
    <variable name="Symbol7" type="DOUBLE" widget="draw"  />
    <variable name="Another" type="TITLE"   />
    <variable name="Symbol308" type="DOUBLE"   />
    <variable name="Symbol309" type="DOUBLE"   />
    <variable name="Symbol8" type="DOUBLE" widget="draw"  />
 </page>
 </Page_Config>
  \endverbatim 
 * 
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
  const TSP_sample_symbol_info_list_t* symbols;
  int i, j, k;
  variable* var;

  symbols = TSP_consumer_get_requested_sample(tsp);
  assert(symbols);

  for (i=0 ; i < symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      for (j=0; j < conf_data.nb_page; j++)
	{
	  for (k=0; k < pages[j].variables->len; k++)
	    {
	      var =  g_ptr_array_index(pages[j].variables, k);	      
	      if(!strcmp(symbols->TSP_sample_symbol_info_list_t_val[i].name, var->text) && (var->type != VAR_TITLE ) )
		{
		  var->provider_global_index = symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index;
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
  const TSP_sample_symbol_info_list_t* symbols;
  int i;
  int maxid = 0;

  /* ask TSP the list of asked symbols */
  symbols = TSP_consumer_get_requested_sample(tsp);
  assert(symbols);
  
  /* find the max of all provider global index */
  for (i=0 ; i < symbols->TSP_sample_symbol_info_list_t_len ; i++)
    {
      if (maxid < symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index )
	{
	  maxid = symbols->TSP_sample_symbol_info_list_t_val[i].provider_global_index;
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


static int 
main_window_start(char* conf_file, char* tsp_prov_url) {
  char		        name[1024];
  int ret = FALSE;

  printf("Loading '%s' conf file\n", conf_file);
  /* Load configuration file  and initialise list of asked symbols */
  if (load_config(conf_file, &conf_data)) {
    tsp = TSP_consumer_connect_url(tsp_prov_url);
    if(NULL!=tsp) {
      if(TSP_STATUS_OK==TSP_consumer_request_open(tsp, 0, 0)) {
	if(TSP_STATUS_OK==TSP_consumer_request_filtered_information(tsp,TSP_FILTER_MINIMAL,MINIMAL_STRING)) {
		
	  if(TSP_STATUS_OK==TSP_consumer_request_sample(tsp, &conf_data.tsp_requested)) {	      
	    /* Create the list of variable per provider global id */
	    init_index2vars();
	    
	    if(TSP_STATUS_OK==TSP_consumer_request_sample_init(tsp,0,0)) {
	      sprintf(name, "%s @ %s", conf_file, TSP_consumer_get_connected_name(tsp));
	      create_mainwin(&conf_data, name);
	      
	      ret = TRUE;
	    }
	    else {
	      fprintf(stderr, "Error while initializing data stream '%s'\n", tsp_prov_url);
	    }
	  }
	  else {		      
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

void 
usage(char *txt) {
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

  if(TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv))
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
