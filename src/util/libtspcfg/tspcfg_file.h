/*

$Id: tspcfg_file.h,v 1.3 2006-06-03 21:42:04 erk Exp $

-----------------------------------------------------------------------

TSP Library - TspCfg components for read an XML file.

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

Purpose   : Use an XML file to parameter a consumer
            For ask some symbol to a provider the consumer can be parameter with an XML file
	    This XML file contain the list of the provider and the list of provider's sample

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

/**
 * TspCfgSampleProviderList_T.
 * a TSP config structure: describe the data and parameter of a sample
 * this data are extract from the XML file
 */
typedef struct TspCfgSampleList {
  /** the implicit_period */
   int32_t              implicit_period;
  /** the implicit phase */
   int32_t              implicit_phase;
  /** the implicit type */
   char                 *implicit_type;
  /** the implicit access */
   char                 *implicit_access;
  /** the implicit provider */
   char                 *implicit_provider;
  /** the implicit rename */
   char                 *renamed;

}  TspCfgSampleList_T;

/**
 * TspCfgProvider_T.
 * a TSP config structure: describe a provider
 * this data are extract from the XML file
 */
typedef struct TspCfgProvider {
  /** the provider name */
   char*                name;
  /** the url of the provider */
   char*                url;
  /** the implicit period of the provider */
   int32_t              implicit_period;

  /** the number of sample */
   int32_t              length;
  /** the sample list (data of the xml file) */
   TspCfgSampleList_T   *cfg_sample_list;

  /** the sample symbol info list */
   TSP_sample_symbol_info_list_t ssi_list;


} TspCfgProvider_T;

/**
 * TspCfgProvider_list_T.
 * a TSP config structure: contain the provider list
 */
typedef struct TspCfgProviderList {
  /** the number of provider */
   int32_t              length;
  /** the provider list */
   TspCfgProvider_T*    providers;
} TspCfgProviderList_T;

/**
 * TspCfg_T.
 * a TSP config structure: contain the data of the xml file after parse an XML config file
 * load by the function: TSP_TspCfg_load
 */
typedef struct TspCfg {
  /** Contain the XML tree */
  xmlDoc                *cfg_tree;
  /** the provider list, load by the load function*/
  TspCfgProviderList_T  cfg_provider_list;
  /** the implicit provider, load by the load function*/
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
					      char    *name);


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
					char    *name);


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
 * @param[in,out] provider_list  provider list to destroy
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
 * @param[in,out] tspConfig the xmlconfig who contain the information about the file                   
 *
 * @return TSP_STATUS_OK if OK
 */
int32_t 
TSP_TspCfg_finalize(TspCfg_T* tspConfig);


/**
 * Open and parse xml config File, and load in the TSP  config structure all the provider and all the sample describe in the XML file
 * 
 * @param[in,out] tspConfig the xmlconfig which contains the information about the TSP configuration
 * @param[in] filename  xml file
 *                      
 * @return TSP_STATUS_OK if OK
 */
int32_t TSP_TspCfg_load(TspCfg_T* tspConfig, char* filename);

/**
 * save the xml tree in the output file
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



/** @} */

#endif
