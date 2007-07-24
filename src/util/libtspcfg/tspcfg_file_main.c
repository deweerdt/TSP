/*

$Id: tspcfg_file_main.c,v 1.4 2007-07-24 23:30:12 erk Exp $

-----------------------------------------------------------------------
 
TSP Library - util components for read a xml config file.

Copyright (c) 2006 Eric NOULARD and Arnaud MORVAN 

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
Component : lib

-----------------------------------------------------------------------

Purpose   : Implementation for read a xml config file

-----------------------------------------------------------------------
*/
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <tsp_abs_types.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <tspcfg_file.h>

/**
 * print_element_names:
 * @param[in] a_node the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlChar * donne;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
	    
	    if(!(strcmp((char*)cur_node->name,"TSP_providers")))
	    {
	      donne=xmlGetProp(cur_node,(unsigned char*)"name");
	      printf("donne: %s\n",donne);
	    }

        }

        print_element_names(cur_node->children);
    }
}


/**
 * @defgroup TSP_ConfigFileTool TSP config file tool
 * A simple command line tool used to display
 * TSP configuration files.
 *
 * \par tsp_cfg \<xmlfile\>
 *
 * Will parse the config file and display for each provider, 
 * all the data provider and all the data sample
 * @ingroup TSP_Applications
 * @ingroup TSP_ConfigFileLib
 */
int
main(int argc, char **argv)
{
  int32_t rep;
  int32_t i,j;

  TspCfg_T* xmlconfig;
  TspCfgProviderList_T* provider_list;
  TspCfgProvider_T provider;
  TSP_sample_symbol_info_list_t* ssi_list;
 
  xmlconfig=TSP_TspCfg_new(NULL);

  if (argc < 2) {
    printf("%s: Insufficient number of options\n",argv[0]);
    printf("Usage: %s XML_config_FILE_name \n",argv[0]);
    fflush(stdout);
    exit(-1);
  }


  /*parse the XML file and load the dat in the TSP config stucture*/
  rep=TSP_TspCfg_load(xmlconfig,argv[1]);

  if (TSP_STATUS_NOK==rep) {
    return -1;
  }


  /*retrieve the provider list*/
  provider_list=TSP_TspCfg_getProviderList(xmlconfig);
    
  printf("Provider_list, Length: %d\n\n",provider_list->length);
  fflush(stdout);

  /*for each provider, we display all this data*/
  for(i=0;i<provider_list->length;++i)
  {
    printf("************************\n");

    printf("Provider, Name: %s\n\n", provider_list->providers[i].name);



    provider=provider_list->providers[i];

    /*retrieve the sample symbol info list of the provider*/
    ssi_list=TSP_TspCfg_getProviderSampleList(xmlconfig,provider_list->providers[i].name);

    printf("Provider, Name: %s\n", provider.name);
    printf("Provider, Url: %s\n", provider.url);
    printf("Provider, Implicit_period: %d\n", provider.implicit_period);
    printf("\tSample List, Length: %d\n\n",provider.length);
    fflush(stdout);

    
    for(j=0;j<provider.length;++j)
    {
      printf("\tSample List, Renamed: %s\n",provider.cfg_sample_list[j].renamed);
      printf("\tSample List, Implicit_provider: %s\n",provider.cfg_sample_list[j].implicit_provider);
      printf("\tSample List, Implicit_type: %s\n",provider.cfg_sample_list[j].implicit_type);
      printf("\tSample List, Implicit_access: %s\n",provider.cfg_sample_list[j].implicit_access);
      printf("\tSample List, Implicit_phase: %d\n",provider.cfg_sample_list[j].implicit_phase);
      printf("\tSample List, Implicit_period: %d\n\n",provider.cfg_sample_list[j].implicit_period);
    }


    for(j=0;j<ssi_list->TSP_sample_symbol_info_list_t_len;++j)
      {
      printf("\tSSI, Name: %s\n",ssi_list->TSP_sample_symbol_info_list_t_val[j].name);
      printf("\tSSI, Type: %d\n",ssi_list->TSP_sample_symbol_info_list_t_val[j].type);
      printf("\tSSI, Phase: %d\n",ssi_list->TSP_sample_symbol_info_list_t_val[j].phase);
      printf("\tSSI, Period: %d\n\n",ssi_list->TSP_sample_symbol_info_list_t_val[j].period);
      fflush(stdout);
    }

    printf("************************\n\n");
    fflush(stdout);
  }

  TSP_TspCfg_save(xmlconfig,"test_write.xml");

  TSP_TspCfg_finalize(xmlconfig);

  fflush(stdout);

  return 0;
}

