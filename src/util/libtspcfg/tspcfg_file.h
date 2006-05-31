/*

$Id: tspcfg_file.h,v 1.1 2006-05-31 11:54:54 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

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
Component : Provider

-----------------------------------------------------------------------

Purpose   : Allow the output of a datapool of symbols from generic file

-----------------------------------------------------------------------
*/


#ifndef _TSPCFG_FILE_H_
#define _TSPCFG_FILE_H_

#include <stdio.h>
#include <tsp_datastruct.h>
#include <tsp_abs_types.h>
#include <libxml/parser.h>

/**
 * @defgroup TSP_ConfigFileLib Configuration File Library 
 * The TSP configuration file library API.
 * @ingroup TSP_Libraries
 * @{
 */

#define TSP_CONFIG_DEFAULT_PERIOD 1
#define TSP_CONFIG_DEFAULT_PHASE 1

typedef struct TspCfgSampleList {
   int32_t              implicit_period;
   int32_t              implicit_phase;
   char                 *implicit_type;
   char                 *implicit_access;
   char                 *implicit_provider;
   char                 *renamed;

}  TspCfgSampleList_T;

typedef struct TspCfgProvider {
   char*                name;
   char*                url;
   int32_t              implicit_period;

   int32_t              length;
   TspCfgSampleList_T   *cfg_sample_list;

   TSP_sample_symbol_info_list_t ssi_list;


} TspCfgProvider_T;

typedef struct TspCfgProviderList {
   int32_t              length;
   TspCfgProvider_T*    providers;
} TspCfgProviderList_T;

typedef struct TspCfg {
  xmlDoc                *cfg_tree;
  TspCfgProviderList_T  cfg_provider_list;
  TspCfgProvider_T      *implicit_provider;

} TspCfg_T;


/**
 * create a sample 
 *
 * @param[in] implicit_period implicit period
 * @param[in] implicit_phase implicit phase
 * @param[in] implicit_type implicit type
 * @param[in] implicit_access implicit access
 * @param[in] implicit_provider implicit provider
 * @param[in] renamed renamed
 * @param[in] name name
 *                  
 * @return a sample
 */
TspCfgSampleList_T * TSP_TspCfgSampleList_new(int32_t implicit_period,
					      int32_t implicit_phase,
					      char    *implicit_type,
					      char    *implicit_access,
					      char    *implicit_provider,
					      char    *renamed,
					      char    *named);


/**
 * destroy a sample 
 *
 * @param[in,out] sample sample to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t
TSP_TspCfgSampleList_delete(TspCfgSampleList_T **sample);

 
/**
 * initialize a sample 
 *
 * @param[in,out] tspCfgSampleList sample to initialize
 * @param[in] implicit_period implicit period
 * @param[in] implicit_phase implicit phase
 * @param[in] implicit_type implicit type
 * @param[in] implicit_access implicit access
 * @param[in] implicit_provider implicit provider
 * @param[in] renamed renamed
 * @param[in] name name
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfgSampleList_initialize(TspCfgSampleList_T *tspCfgSampleList,
					int32_t implicit_period,
					int32_t implicit_phase,
					char    *implicit_type,
					char    *implicit_access,
					char    *implicit_provider,
					char    *renamed,
					char    *named);


/**
 * destroy a sample 
 *
 * @param[in] sample sample to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t
TSP_TspCfgSampleList_finalize(TspCfgSampleList_T *sample);


/**
 * create a provider
 *
 * @param[in] name provider name
 * @param[in] url provider url
 * @param[in] implicit_period implicit period
 * @param[in] length number of sample
 *                  
 * @return a provider
 */
TspCfgProvider_T*
TSP_TspCfgProvider_new(char* name, char*  url,
		       int32_t  implicit_period,int32_t length);


/**
 * delete a provider
 *
 * @param[in,out] provider provider to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t
TSP_TspCfgProvider_delete(TspCfgProvider_T** provider);


/**
 * initialize a provider
 *
 * @param[in,out] provider provider to initialize
 * @param[in] name provider name
 * @param[in] url provider url
 * @param[in] implicit_period implicit period
 * @param[in] length number of sample
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t
TSP_TspCfgProvider_initialize(TspCfgProvider_T *provider,char* name, char*  url,
			      int32_t  implicit_period,int32_t length);


/**
 * destroy a provider
 *
 * @param[in] provider provider to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t 
TSP_TspCfgProvider_finalize(TspCfgProvider_T* provider);


/**
 * create provider list
 *
 * @param[in] length number of provider
 *                  
 * @return the provider list
 */
TspCfgProviderList_T* TSP_TspCfgProviderList_new(int32_t length);


/**
 * destroy provider list
 *
 * @param[in,out] tspConfig  provider list to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t
TSP_TspCfgProviderList_delete(TspCfgProviderList_T** provider_list);


/**
 * create and initialize the tspConfig
 *
 * @param[in,out] tspCfgProviderList  provider list
 * @param[in] length   number of provider
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfgProviderList_initialize(TspCfgProviderList_T* tspCfgProviderList,
					  int32_t length);


/**
 * create and initialize the tspConfig
 *
 * @param[in] provider_list  provider list to destroy
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t 
TSP_TspCfgProviderList_finalize(TspCfgProviderList_T* provider_list);


/**
 * memory allocation
 *
 * @param[in] cfg_tree   xmlDoc
 *                  
 * @return the tsp config
 */
TspCfg_T* TSP_TspCfg_new(xmlDoc *cfg_tree);


/**
 * delete the data.
 *
 * @param[in,out] tspConfig  tsp config to destroy
 *
 * return  TSP_STATUS_OK if ok
 */
int32_t
TSP_TspCfg_delete(TspCfg_T** tspConfig);


/**
 * create and initialize the tspConfig
 *
 * @param[in,out] tspConfig  tsp config to initialize
 * @param[in] cfg_tree   xmlDoc
 *                  
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_initialize(TspCfg_T* tspConfig,
			      xmlDoc *cfg_tree);

/**
 * destroy the tspconfig
 *
 * @param[in,out tspConfig the xmlconfig who contain the information about the file                   
 *
 * @return TSP_STATUS_OK if OK
 */
int32_t 
TSP_TspCfg_finalize(TspCfg_T* tspConfig);


/**
 * open and parse xml config File
 * 
 * @param[in] tspConfig the xmlconfig who contain the information about the file
 * @param[in] filename  xml file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_load(TspCfg_T* tspConfig,char* filename);


/**
 * open and parse xml config File
 * 
 * @param[in] tspConfig the xmlconfig who contain the information about the file
 * @param[in] filename  xml file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_load(TspCfg_T* tspConfig,char* filename);

/**
 * save the xml tree in the outpu file
 * 
 * @param[in] tspConfig the xmlconfig who contain the information about the file
 * @param[in] output_filename  the xml output file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_save(TspCfg_T* tspConfig,char* output_filename);
 
/**
 * get the provider list
 * 
 * @param[in] tspConfig the tspconfig file who contain the information about the providers and sample
 *                      
 * @return the provider list
 */
TspCfgProviderList_T* TSP_TspCfg_getProviderList(TspCfg_T* tspConfig);


/**
 * get the sample list of the provider
 * 
 * @param[in] tspConfig the xmlconfig file who contain the information about the providers and sample
 * @param[in] provider_name we want the symbol list of this provider
 *                      
 * @return the sample list of the provider
 */
TSP_sample_symbol_info_list_t* TSP_TspCfg_getProviderSampleList(TspCfg_T* tspConfig,char* provider_name);

/**
 * write the config file in another file
 * 
 * @param[in] tspConfig the xmlconfig file who contain the information about the providers and sample
 * @param[in] provider_name name of the config file to write
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_save(TspCfg_T* tspConfig,char* file_name);

/**
 * load the providers in the structure
 * 
 * @param[in] tspConfig the xmlconfig file who contain the information about the providers and sample
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t load_provider(TspCfg_T* tspConfig);


/**
 * load the provider's sample  in the structure
 * 
 * @param[in] tspConfig the xmlconfig file who contain the information about the providers and sample
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t load_sample(TspCfg_T* tspConfig);

/**
 * load the data sample in the structure
 * 
 * @param[in] tspConfig the xmlconfig file who contain the information about the providers and sample
 * @param[in,out] provider sample to load in this provider
 * @param[in] implicit_period implicit period
 * @param[in] implicit_phase implicit phase
 * @param[in] implicit_type implicit type
 * @param[in] implicit_access implcit access 
 * @param[in] indice_sample indice sample 
 *                      
 * @return nothing
 */
void  load_data_sample(xmlNode *cur_node,
		       TspCfgProvider_T *provider, 
		       int32_t  implicit_period,
		       int32_t  implicit_phase,
		       char    *implicit_type,
		       char    *implicit_access,
		       int32_t  indice_sample);


/** @} */

#endif
