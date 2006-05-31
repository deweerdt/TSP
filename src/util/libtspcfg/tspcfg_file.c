/*

$Id: tspcfg_file.c,v 1.1 2006-05-31 11:54:54 erk Exp $

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
#include <assert.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <tsp_abs_types.h>
#include <tsp_common.h>

#include <tspcfg_file.h>

#define _TSPCFG_FILE_H_


#define XNN_TSP_SAMPLE_CONFIG  "TSP_sample_config"
#define XNN_TSP_PROVIDERS      "TSP_providers"
#define XNN_PROVIDER           "provider"
#define XNN_TSP_SAMPLES        "TSP_samples"
#define XNN_SAMPLE             "sample"
#define XNN_TSP_SAMPLES_LAYOUT "TSP_samples_layout"
#define XNN_OPAL_LAYOUT        "opal_layout"
#define XNN_OPAL_FRAME         "opal_frame"
#define XNN_OPAL_DATA          "opal_data"


TspCfgSampleList_T * TSP_TspCfgSampleList_new(int32_t implicit_period,
					    int32_t implicit_phase,
					    char    *implicit_type,
					    char    *implicit_access,
					    char    *implicit_provider,
					    char    *renamed,
					    char    *named)
{
  TspCfgSampleList_T *sample;

  sample = calloc(1,sizeof(TspCfgSampleList_T));

  if(NULL!=sample)
  {
    TSP_TspCfgSampleList_initialize(sample,
				    implicit_period,
				    implicit_phase,
				    implicit_type,
				    implicit_access,
				    implicit_provider,
				    renamed,
				    named);
  }
  return sample;

} 

int32_t
TSP_TspCfgSampleList_delete(TspCfgSampleList_T **sample) 
{
  int32_t retcode;

  assert(sample);

  retcode=TSP_STATUS_OK;

  if(NULL!=*sample) {
    retcode=TSP_TspCfgSampleList_finalize(*sample);
    if (TSP_STATUS_OK!=retcode){
      return retcode;
    }
    
    free(*sample);
    *sample=NULL;
  }
  return retcode;
} 

int32_t TSP_TspCfgSampleList_initialize(TspCfgSampleList_T *tspCfgSampleList,
					int32_t implicit_period,
					int32_t implicit_phase,
					char    *implicit_type,
					char    *implicit_access,
					char    *implicit_provider,
					char    *renamed,
					char    *named)
{
 assert(tspCfgSampleList);

 int32_t retcode = TSP_STATUS_OK;

 if (NULL!=tspCfgSampleList->implicit_type)
 {
    free(tspCfgSampleList->implicit_type);    
 }

 if (NULL!=tspCfgSampleList->implicit_access)
 {
    free(tspCfgSampleList->implicit_access);    
 }

 if (NULL!=tspCfgSampleList->implicit_provider)
 {
    free(tspCfgSampleList->implicit_provider);    
 }

 if (NULL!=tspCfgSampleList->renamed)
 {
    free(tspCfgSampleList->renamed);    
 }

 if (NULL!=implicit_type)
 {
    tspCfgSampleList->implicit_type = strdup(implicit_type);
    if (NULL==tspCfgSampleList->implicit_type)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",implicit_type));
    }
 } 
 else
 {
     tspCfgSampleList->implicit_type = NULL;
 }

 if (NULL!=implicit_access)
 {
    tspCfgSampleList->implicit_access = strdup(implicit_access);
    if (NULL==tspCfgSampleList->implicit_access)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",implicit_access));
    }
 } 
 else
 {
     tspCfgSampleList->implicit_access = NULL;
 }

 if (NULL!=implicit_provider)
 {
    tspCfgSampleList->implicit_provider = strdup(implicit_provider);
    if (NULL==tspCfgSampleList->implicit_provider)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",implicit_provider));
    }
 } 
 else
 {
     tspCfgSampleList->implicit_provider = NULL;
 }

 if (NULL!=renamed)
 {
    tspCfgSampleList->renamed = strdup(renamed);
    if (NULL==tspCfgSampleList->renamed)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",renamed));
    }
 } 
 else
 {
     tspCfgSampleList->renamed = NULL;
 }

	
 tspCfgSampleList->implicit_period= implicit_period;				
 tspCfgSampleList->implicit_phase= implicit_phase;
 
/* retcode=TSP_SSI_initialize(&(tspCfgSampleList->ssi),
			    named,
			    -1,
			    -1,
			    -1,
			     macsim_type_data(implicit_type),
			    0,
			    0,
			    0,
			    implicit_period,
			    implicit_phase);*/

 return(retcode);
}

int32_t
TSP_TspCfgSampleList_finalize(TspCfgSampleList_T *sample) 
{
  assert(sample);

  int32_t retcode = TSP_STATUS_OK;

  retcode= TSP_TspCfgSampleList_initialize(sample,
					   -1,
					   -1,
					   NULL,
					   NULL,
					   NULL,
					   NULL,
					   NULL);

  return retcode;
} /* end of retcode */

TspCfgProvider_T*
TSP_TspCfgProvider_new(char* name, char*  url,
		       int32_t  implicit_period,int32_t length)
 {
 
   TspCfgProvider_T *provider;

   provider = (TspCfgProvider_T*) calloc(1,sizeof( TspCfgProvider_T));
   if (NULL!=provider)
   {
     TSP_TspCfgProvider_initialize(provider,name,url,implicit_period,length);
   }

   return provider;
} 

int32_t
TSP_TspCfgProvider_delete(TspCfgProvider_T** provider)
{
  int32_t retcode;

  assert(provider);

  retcode=TSP_STATUS_OK;

  if (NULL!=*provider) {
    retcode=TSP_TspCfgProvider_finalize(*provider);
    free(*provider);
    *provider=NULL;
  }

  return retcode;
} /* end of TSP_SSI_delete */


int32_t
TSP_TspCfgProvider_initialize(TspCfgProvider_T *provider,char* name, char*  url,
			      int32_t  implicit_period,int32_t length)
 {

  int32_t retcode = TSP_STATUS_OK;
  int32_t i;
 
  char *save_name=NULL;
  char *save_url=NULL;

  assert(provider);
  

  if(NULL!=name)
  {
    save_name=strdup(name);
  }

  if(NULL!=url)
  {
    save_url=strdup(url);
  }



  if (NULL!=provider->name)
  {
    free(provider->name);    
  }

  if (NULL!=save_name)
  {
    provider->name = strdup(save_name);
    free(save_name);
    save_name=NULL;
    if (NULL==provider->name)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",name));
    }

  } 
  else
  {
      provider->name = NULL;
  }
  

  if (NULL!=provider->url)
  {
    free(provider->url);    
  }

  if (NULL!=save_url)
  {
    provider->url = strdup(save_url);
    free(save_url);
    save_url=NULL;
    if (NULL==provider->url)
    {
      retcode = TSP_STATUS_ERROR_MEMORY_ALLOCATION;
      STRACE_ERROR(("Cannot strdup <%s>",url));
    }
  } 
  else
  {
    provider->url = NULL;
  }
  

  provider->implicit_period=implicit_period;


  provider->length = length;
  provider->cfg_sample_list = ( TspCfgSampleList_T*) calloc (length,sizeof( TspCfgSampleList_T));
  TSP_CHECK_ALLOC( provider->cfg_sample_list,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

  for (i=0;i< provider->length;++i) {    
    retcode=TSP_TspCfgSampleList_initialize(&(provider->cfg_sample_list[i]),
					    -1,
					    -1,
					    NULL,
					    NULL,
					    NULL,
					    NULL,
					    NULL);
    /* give-up on first error */
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }

  if (TSP_STATUS_OK==retcode)
  {
    retcode=TSP_SSIList_initialize(&(provider->ssi_list),length);
  }

  return retcode;
}

int32_t 
TSP_TspCfgProvider_finalize(TspCfgProvider_T* provider)
{
  int32_t retcode = TSP_STATUS_OK;
  int32_t i;

  assert(provider);

  if(NULL!=provider->name)
  {
    free(provider->name);
    provider->name=NULL;
  }

  if(NULL!=provider->url)
  {
    free(provider->url);
    provider->url=NULL;
  }

  provider->implicit_period=-1;

  for (i=0;i<provider->length;++i) 
  {
    retcode=TSP_TspCfgSampleList_finalize(&(provider->cfg_sample_list[i]));
    /* Give-up on first error */
    if (TSP_STATUS_OK!=retcode) 
    {
      break;
    }
  }

  if (TSP_STATUS_OK==retcode) 
  {
    retcode=TSP_SSIList_finalize(&(provider->ssi_list));
  }

  if (TSP_STATUS_OK==retcode) 
  {
    free(provider->cfg_sample_list);
    provider->cfg_sample_list = NULL;
    provider->length = 0;
  }

  return retcode;
} 



TspCfgProviderList_T* TSP_TspCfgProviderList_new(int32_t length)
{
  TspCfgProviderList_T *tspCfgProviderList;

  tspCfgProviderList=(TspCfgProviderList_T*)calloc(1,sizeof(TspCfgProviderList_T));

  if (NULL!=tspCfgProviderList)
  {
    TSP_TspCfgProviderList_initialize(tspCfgProviderList,length);

  }
  return(tspCfgProviderList);

}

int32_t
TSP_TspCfgProviderList_delete(TspCfgProviderList_T** provider_list)
{
  int32_t retcode;

  assert(provider_list);

  retcode=TSP_STATUS_OK;

  if (NULL!=*provider_list) {
    retcode=TSP_TspCfgProviderList_finalize(*provider_list);
    free(*provider_list);
    *provider_list=NULL;
  }

  return retcode;
} 


/* create xmlconfig for treatement of xml file
*/
int32_t TSP_TspCfgProviderList_initialize(TspCfgProviderList_T* tspCfgProviderList,
					  int32_t length)
{
  int32_t retcode = TSP_STATUS_OK;
  int32_t i;

  assert(tspCfgProviderList);  


  tspCfgProviderList->length=length;
 
  tspCfgProviderList->providers = (TspCfgProvider_T*) calloc (length,sizeof(TspCfgProvider_T));
  TSP_CHECK_ALLOC(tspCfgProviderList->providers,TSP_STATUS_ERROR_MEMORY_ALLOCATION);

  for (i=0;i<tspCfgProviderList->length;++i) {    
    retcode=TSP_TspCfgProvider_initialize(&(tspCfgProviderList->providers[i]),NULL,NULL,-1,0);
    /* give-up on first error */
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }
  return retcode;

}

int32_t 
TSP_TspCfgProviderList_finalize(TspCfgProviderList_T* provider_list)
{
  int32_t retcode = TSP_STATUS_OK;
  int32_t i;

  assert(provider_list);

  for (i=0;i<provider_list->length;++i) {
    retcode=TSP_TspCfgProvider_finalize(&(provider_list->providers[i]));
    /* Give-up on first error */
    if (TSP_STATUS_OK!=retcode) {
      break;
    }
  }

  if (TSP_STATUS_OK==retcode) {
    free(provider_list->providers);

    provider_list->providers = NULL;
    provider_list->length = 0;
  }

  return retcode;
} 


TspCfg_T* TSP_TspCfg_new(xmlDoc *cfg_tree)
{
  TspCfg_T* tspConfig;

  tspConfig=(TspCfg_T*)calloc(1,sizeof(TspCfg_T));

  if (NULL!=tspConfig)
  {
    TSP_TspCfg_initialize(tspConfig,cfg_tree);

  }
  return(tspConfig);

}

int32_t
TSP_TspCfg_delete(TspCfg_T** tspConfig)
{
  int32_t retcode;

  assert(tspConfig);

  retcode=TSP_STATUS_OK;

  if (NULL!=*tspConfig) {
    retcode=TSP_TspCfg_finalize(*tspConfig);
    free(*tspConfig);
    *tspConfig=NULL;
  }

  return retcode;
} 


/* create xmlconfig for treatement of xml file
*/
int32_t TSP_TspCfg_initialize(TspCfg_T* tspConfig,
			      xmlDoc *cfg_tree)
{
  int32_t retcode = TSP_STATUS_OK;

  assert(tspConfig);  


  if(NULL!=tspConfig->cfg_tree)
  {
    retcode=TSP_TspCfg_finalize(tspConfig);
    if (TSP_STATUS_OK!=retcode)
    {
      return(retcode);
    }
  }
  tspConfig->cfg_tree=cfg_tree;

  retcode=TSP_TspCfgProviderList_initialize(&(tspConfig->cfg_provider_list),0);

  tspConfig->implicit_provider=NULL;


  return(retcode);
 
}

int32_t 
TSP_TspCfg_finalize(TspCfg_T* tspConfig)
{
  int32_t retcode = TSP_STATUS_OK;

  assert(tspConfig);

  if(NULL!=tspConfig->cfg_tree)
  {
    xmlFreeDoc(tspConfig->cfg_tree);
    xmlCleanupParser();
    tspConfig->cfg_tree=NULL;
  }
  tspConfig->implicit_provider=NULL;

  retcode=TSP_TspCfgProviderList_finalize(&(tspConfig->cfg_provider_list));

  return retcode;
} 


/* open file and parse xml file
*/
int32_t TSP_TspCfg_load(TspCfg_T* tspConfig,char* filename)
{
  assert(tspConfig);
  assert(filename);  

  /*
   * this initialize the library and check potential ABI mismatches
   * between the version it was compiled for and the actual shared
   * library used.
   */
  LIBXML_TEST_VERSION

  /*parse the file and get the DOM */
  tspConfig->cfg_tree = xmlReadFile(filename, NULL,XML_PARSE_RECOVER);

  if (NULL==tspConfig->cfg_tree) {
    printf("error: could not parse file %s\n", filename);
    return(TSP_STATUS_NOK);
  }

  /*chargement providers*/
  load_provider(tspConfig);


  /*chargement symbol list des providers*/
  load_sample(tspConfig);


  return(TSP_STATUS_OK);
	
}

int32_t TSP_TspCfg_save(TspCfg_T* tspConfig,char* output_filename)
{
  int32_t ret;

  assert(output_filename);
  assert(tspConfig);

  ret=xmlSaveFormatFileEnc(output_filename,tspConfig->cfg_tree,NULL,0);
  if(-1==ret)
  {
    return(TSP_STATUS_NOK);
  }

  return(TSP_STATUS_OK);
}


TspCfgProviderList_T* TSP_TspCfg_getProviderList(TspCfg_T* tspConfig)
{
  assert(tspConfig);
  
  return(&(tspConfig->cfg_provider_list));

}


TSP_sample_symbol_info_list_t* TSP_TspCfg_getProviderSampleList(TspCfg_T* tspConfig,char* provider_name)
{
  int32_t i;

  assert(tspConfig);
  assert(provider_name);
  
  for(i=0;tspConfig->cfg_provider_list.length;++i)
  {
    if(!(strcmp(provider_name,tspConfig->cfg_provider_list.providers[i].name)))
    {
      return(&(tspConfig->cfg_provider_list.providers[i].ssi_list));
    }
  }
  return(NULL);

}

int32_t load_provider(TspCfg_T* tspConfig)
{
  xmlNode *root_element = NULL;

  xmlNode *cur_node = NULL;
  xmlChar *data=NULL;

  int32_t  nb_provider=0;

 /*Get the root element node */
  root_element = xmlDocGetRootElement(tspConfig->cfg_tree);


  /*count numbers of providers*/
  cur_node=root_element;
  while(cur_node)
  {
    
      if(!(strcmp((char*)cur_node->name,XNN_TSP_SAMPLE_CONFIG)))
      {
	cur_node=cur_node->children;
      }
      else
      {
	if(!(strcmp((char*)cur_node->name,XNN_TSP_PROVIDERS)))
	{
	  cur_node=cur_node->children;
	}
	else
	{
	  if(!(strcmp((char*)cur_node->name,XNN_PROVIDER)))
	  {
	    ++nb_provider;
	  }
	
	  cur_node=cur_node->next; 
	}

       }
  
  }

  TSP_TspCfgProviderList_initialize(&(tspConfig->cfg_provider_list),nb_provider); 

  /*load provider data*/
  nb_provider=0;
  cur_node=root_element;
  while(cur_node)
  {
   
      if(!(strcmp((char*)cur_node->name,XNN_TSP_SAMPLE_CONFIG)))
      {
	cur_node=cur_node->children;
      }
      else
      {
	if(!(strcmp((char*)cur_node->name,XNN_TSP_PROVIDERS)))
	{
	  cur_node=cur_node->children;
	}
	else
	{
	  if(!(strcmp((char*)cur_node->name,XNN_PROVIDER)))
	  {
	   

	    data=xmlGetProp(cur_node,(unsigned char*)"name");
	    tspConfig->cfg_provider_list.providers[nb_provider].name=(char*)calloc(strlen((char*)data)+1,sizeof(char));
	    strcpy(tspConfig->cfg_provider_list.providers[nb_provider].name,(char*)data);

	    data=xmlGetProp(cur_node,(unsigned char*)"url");
	    tspConfig->cfg_provider_list.providers[nb_provider].url=(char*)calloc(strlen((char*)data)+1,sizeof(char));
	    strcpy(tspConfig->cfg_provider_list.providers[nb_provider].url,(char*)data);

	    data=xmlGetProp(cur_node,(unsigned char*)"implicit_period");
	    
	    tspConfig->cfg_provider_list.providers[nb_provider].implicit_period=atoi((char*)data);

	    ++nb_provider;
	  }
	  cur_node=cur_node->next;
	}

       }
   }

  return(TSP_STATUS_OK);
}

int32_t load_sample(TspCfg_T* tspConfig)
{
  xmlNode *root_element = NULL;

  xmlNode *cur_node = NULL;
  xmlNode *cur_node_save = NULL;
  xmlChar *data=NULL;

  int32_t  indice_provider=0;
  int32_t  nb_sample=0;
  int32_t  retcode;
  int      continuer=1;

  int32_t  implicit_period=-1;
  int32_t  implicit_phase=-1;
  char    *implicit_type=NULL;
  char    *implicit_access=NULL;
  char    *implicit_provider=NULL;


 /*Get the root element node */
  root_element = xmlDocGetRootElement(tspConfig->cfg_tree);


  /*search the implicit provider*/
  cur_node=root_element;
  while(cur_node && continuer)
  {
      if(!(strcmp((char*)cur_node->name,XNN_TSP_SAMPLE_CONFIG)))
      {
	cur_node=cur_node->children;
      }
      else
      {
	if(!(strcmp((char*)cur_node->name,XNN_TSP_SAMPLES)))
	{

	  data=xmlGetProp(cur_node,(unsigned char*)"implicit_provider");
	  implicit_provider=(char*)calloc(strlen((char*)data)+1,sizeof(char));
	  strcpy(implicit_provider,(char*)data);

	  data=xmlGetProp(cur_node,(unsigned char*)"implicit_type");
	  implicit_type=(char*)calloc(strlen((char*)data)+1,sizeof(char));
	  strcpy(implicit_type,(char*)data);

	  data=xmlGetProp(cur_node,(unsigned char*)"implicit_access");
	  implicit_access=(char*)calloc(strlen((char*)data)+1,sizeof(char));
	  strcpy(implicit_access,(char*)data);

	  data=xmlGetProp(cur_node,(unsigned char*)"implicit_period");
	  if(NULL!=data)
	  {
	    implicit_period=atoi((char*)data);
	  }
	  else
	  {
	    implicit_period=TSP_CONFIG_DEFAULT_PERIOD;
	  }


	  data=xmlGetProp(cur_node,(unsigned char*)"implicit_phase");  
	  if(NULL!=data)
	  {
	    implicit_phase=atoi((char*)data);
	  }
	  else
	  {
	    implicit_phase=TSP_CONFIG_DEFAULT_PHASE;
	  }

	  cur_node=cur_node->children;
	  continuer=0;
	}
	else
	{
	  cur_node=cur_node->next;
	}

       }
  }

  cur_node_save=cur_node;


  /*count nb sample by providers*/
  for(indice_provider=0;indice_provider<tspConfig->cfg_provider_list.length;++indice_provider)
  {

    while(cur_node)
    {
      if(!(strcmp((char*)cur_node->name,XNN_SAMPLE)))
      {

	data=xmlGetProp(cur_node,(unsigned char*)"provider");

	if(NULL!=data)
	{
	  if(!strcmp((char*)data,tspConfig->cfg_provider_list.providers[indice_provider].name))
	  {
	    ++nb_sample;
	  }
        }
	else
	{
	  if(!strcmp(implicit_provider,tspConfig->cfg_provider_list.providers[indice_provider].name))
	  {

	    /*load the implicit provider*/
	    if(NULL==tspConfig->implicit_provider)
	    {
	      tspConfig->implicit_provider=&(tspConfig->cfg_provider_list.providers[indice_provider]);
	    }

	    ++nb_sample;
	  }
	}
	  
      }
      cur_node=cur_node->next;
    }



  retcode=TSP_TspCfgProvider_initialize(&(tspConfig->cfg_provider_list.providers[indice_provider]),
					tspConfig->cfg_provider_list.providers[indice_provider].name, 
					tspConfig->cfg_provider_list.providers[indice_provider].url,
					tspConfig->cfg_provider_list.providers[indice_provider].implicit_period,
					nb_sample);


    nb_sample=0;
    cur_node=cur_node_save;
  }


 /*load data sample by providers*/
  for(indice_provider=0;indice_provider<tspConfig->cfg_provider_list.length;++indice_provider)
  {

    while(cur_node)
    {
      if(!(strcmp((char*)cur_node->name,XNN_SAMPLE)))
      {

	data=xmlGetProp(cur_node,(unsigned char*)"provider");

	if(NULL!=data)
	{
	  if(!strcmp((char*)data,tspConfig->cfg_provider_list.providers[indice_provider].name))
	  {

	    tspConfig->cfg_provider_list.providers[indice_provider].cfg_sample_list[nb_sample].implicit_provider=
	                                         (char*)calloc(strlen((char*)data)+1,sizeof(char));
	    strcpy(tspConfig->cfg_provider_list.providers[indice_provider].cfg_sample_list[nb_sample].implicit_provider,(char*)data);

	    load_data_sample(cur_node,
			     &(tspConfig->cfg_provider_list.providers[indice_provider]), 
			     implicit_period,
			     implicit_phase,
			     implicit_type,
			     implicit_access,
			     nb_sample);

	    ++nb_sample;
	  }
        }
	else
	{
	  if(!strcmp(implicit_provider,tspConfig->cfg_provider_list.providers[indice_provider].name))
	  {

	    tspConfig->cfg_provider_list.providers[indice_provider].cfg_sample_list[nb_sample].implicit_provider=
	                                         (char*)calloc(strlen(implicit_provider)+1,sizeof(char));
	    strcpy(tspConfig->cfg_provider_list.providers[indice_provider].cfg_sample_list[nb_sample].implicit_provider,
		   implicit_provider);

	    
	    load_data_sample(cur_node,
			     &(tspConfig->cfg_provider_list.providers[indice_provider]), 
			     implicit_period,
			     implicit_phase,
			     implicit_type,
			     implicit_access,
			     nb_sample);

	    ++nb_sample;
	  }
	}
      }
      cur_node=cur_node->next;
    }

    nb_sample=0;
    cur_node=cur_node_save;
  }

  free(implicit_type);
  free(implicit_access);
  free(implicit_provider);

  return(TSP_STATUS_OK);
}

TSP_datatype_t TSP_datatype_fromString(char* type_var)
{ 
  if(NULL!=type_var)
  {

    if (0==strcmp(type_var,"double"))
    {
      return(TSP_TYPE_DOUBLE);
    }
	
    if (0==strcmp(type_var,"entier"))
    {
      return(TSP_TYPE_INT32);
    }
	
    if (0==strcmp(type_var,"booleen"))
    {
      return(TSP_TYPE_UINT8);
    }
	
    if (0==strcmp(type_var,"character"))
    {
      return(TSP_TYPE_CHAR);
    }
  }
  return(TSP_TYPE_UNKNOWN);

}


void  load_data_sample(xmlNode *cur_node,
		       TspCfgProvider_T *provider, 
		       int32_t  implicit_period,
		       int32_t  implicit_phase,
		       char    *implicit_type,
		       char    *implicit_access,
		       int32_t  indice_sample)
{
  xmlChar *data=NULL;


  data=xmlGetProp(cur_node,(unsigned char*)"renamed");
  if(NULL!=data)
  {
    provider->cfg_sample_list[indice_sample].renamed=(char*)calloc(strlen((char*)data)+1,sizeof(char));
    strcpy(provider->cfg_sample_list[indice_sample].renamed,(char*)data);
  }

  data=xmlGetProp(cur_node,(unsigned char*)"access");
  if(NULL!=data)
  {
    provider->cfg_sample_list[indice_sample].implicit_access=(char*)calloc(strlen((char*)data)+1,sizeof(char));
    strcpy(provider->cfg_sample_list[indice_sample].implicit_access,(char*)data);
  }
  else
  {
    provider->cfg_sample_list[indice_sample].implicit_access=(char*)calloc(strlen(implicit_access)+1,sizeof(char));
    strcpy(provider->cfg_sample_list[indice_sample].implicit_access, implicit_access);
  }
  
  data=xmlGetProp(cur_node,(unsigned char*)"type");
  if(NULL!=data)
  {
    provider->cfg_sample_list[indice_sample].implicit_type=(char*)calloc(strlen((char*)data)+1,sizeof(char));
    strcpy(provider->cfg_sample_list[indice_sample].implicit_type,(char*)data);
  }
  else
  {
    provider->cfg_sample_list[indice_sample].implicit_type=(char*)calloc(strlen(implicit_type)+1,sizeof(char));
    strcpy(provider->cfg_sample_list[indice_sample].implicit_type,implicit_type);
  }
  
  data=xmlGetProp(cur_node,(unsigned char*)"phase");
  if(NULL!=data)
  {
    provider->cfg_sample_list[indice_sample].implicit_phase=atoi((char*)data);
  }
  else
  {
    provider->cfg_sample_list[indice_sample].implicit_phase=implicit_phase;
  }

  data=xmlGetProp(cur_node,(unsigned char*)"period");
  if(NULL!=data)
  {
    provider->cfg_sample_list[indice_sample].implicit_period=atoi((char*)data);
  }
  else
  {
    provider->cfg_sample_list[indice_sample].implicit_period=implicit_period;
  }

  data=xmlGetProp(cur_node,(unsigned char*)"name");


  TSP_SSI_initialize(&(provider->ssi_list.TSP_sample_symbol_info_list_t_val[indice_sample]),
		     (char *)data,
		     -1,
		     -1,
		     -1,
		     TSP_datatype_fromString(provider->cfg_sample_list[indice_sample].implicit_type),
		     0,
		     0,
		     0,
		     provider->cfg_sample_list[indice_sample].implicit_period,
		     provider->cfg_sample_list[indice_sample].implicit_phase);
  
}
