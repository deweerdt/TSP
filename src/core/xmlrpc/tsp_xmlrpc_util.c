#include "tsp_xmlrpc_util.h"
#include "tsp_simple_trace.h"



/* 
 * TSP => XMLRPC 
 */

xmlrpc_value*
TSP_answer_sample_destroy_to_xmlrpc_value(xmlrpc_env *env,
										  TSP_answer_sample_destroy_t *ans_sample_destroy)
{
  xmlrpc_value* xr_ans_sample_destroy;

  xr_ans_sample_destroy = xmlrpc_build_value(env, "{s:i,s:i,s:i}",
											 "version_id", ans_sample_destroy->version_id,
											 "channel_id", ans_sample_destroy->channel_id,
											 "status", ans_sample_destroy->status);
  die_if_fault_occurred(env);

  return xr_ans_sample_destroy;
}

xmlrpc_value* 
TSP_request_sample_to_xmlrpc_value (xmlrpc_env *env, 
									TSP_request_sample_t *req_sample) 
{
  xmlrpc_value *xr_req_sample, *xr_array;
  int i;

  xr_array = xmlrpc_build_value (env, "()");
  
  xr_req_sample = xmlrpc_build_value(env, "{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i}",
										 "version_id", req_sample->version_id,
										 "channel_id", req_sample->channel_id,
										 "feature_words[0]", req_sample->feature_words[0],
										 "feature_words[1]", req_sample->feature_words[1],
										 "feature_words[2]", req_sample->feature_words[2],
										 "feature_words[3]", req_sample->feature_words[3],
										 "consumer_timeout", req_sample->consumer_timeout,
										 "symbols.TSP_sample_symbol_info_list_t_len", req_sample->symbols.TSP_sample_symbol_info_list_t_len);
  xmlrpc_array_append_item(env, xr_array, xr_req_sample);
  
  for (i=0; i < req_sample->symbols.TSP_sample_symbol_info_list_t_len; i++) {
	xmlrpc_value *symbol;

	symbol = xmlrpc_build_value(env, "{s:s,s:i,s:i,s:i,s:i,s:i,s:i}",
								"name", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name,
								"provider_global_index", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
								"provider_group_index", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
								"provider_group_rank", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank,
								/* "xdr_tsp_t", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t, */
								"dimension", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
								"period", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].period,
								"phase", req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].phase);
	xmlrpc_array_append_item(env, xr_array, symbol);
  }


  
  return xr_array;
}

xmlrpc_value* 
TSP_answer_sample_init_to_xmlrpc_value (xmlrpc_env *env, 
										TSP_answer_sample_init_t *ans_sample_init)
{
  xmlrpc_value *xr_ans_sample_init;
  
  xr_ans_sample_init = xmlrpc_build_value(env, "{s:i,s:i,s:s,s:i}",
										  "version_id", ans_sample_init->version_id,
										  "channel_id", ans_sample_init->channel_id,
										  "data_address", ans_sample_init->data_address,
										  "status", ans_sample_init->status);

  return xr_ans_sample_init;
}

xmlrpc_value* 
TSP_request_sample_init_to_xmlrpc_value (xmlrpc_env *env, 
										 TSP_request_sample_init_t *req_sample_init)
{
  xmlrpc_value *xr_req_sample_init;
  
  xr_req_sample_init = xmlrpc_build_value(env, "{s:i,s:i}",
										  "version_id", req_sample_init->version_id,
										  "channel_id", req_sample_init->channel_id);

  return xr_req_sample_init;
}


/* 
 * XMLRPC => TSP 
 */

TSP_provider_info_t* 
xmlrpc_value_to_TSP_provider_info (xmlrpc_env* env, 
								   xmlrpc_value *xr_prov_info)
{
  TSP_provider_info_t *prov_info;

  prov_info = (TSP_provider_info_t *)malloc(sizeof(*prov_info));

  if (prov_info != NULL) {
	xmlrpc_parse_value(env, xr_prov_info, "{s:s,*}", "info", &prov_info->info);
	prov_info->info = strdup(prov_info->info);

	die_if_fault_occurred(env);
  }

  return prov_info;
}

TSP_answer_open_t * 
xmlrpc_value_to_TSP_answer_open (xmlrpc_env *env, 
								 xmlrpc_value *xr_ans_open)
{
  TSP_answer_open_t *ans_open;

  ans_open = (TSP_answer_open_t *)malloc(sizeof(*ans_open));

  if (ans_open != NULL) {
	xmlrpc_parse_value(env, xr_ans_open, "{s:i,s:i,s:i,s:s,*}", 
					   "version_id", &ans_open->version_id,
					   "channel_id", &ans_open->channel_id,
					   "status", &ans_open->status,
					   "status_str", &ans_open->status_str);
	die_if_fault_occurred(env);
    
	ans_open->status_str = strdup(ans_open->status_str);
  }

  return ans_open;
}

TSP_answer_sample_t* 
xmlrpc_value_to_TSP_answer_sample (xmlrpc_env *env, 
								   xmlrpc_value *array)
{
  TSP_answer_sample_t *ans_sample;
  xmlrpc_value *xr_ans_sample;
  int i;

  ans_sample = (TSP_answer_sample_t *)malloc(sizeof(*ans_sample));

  if (ans_sample == NULL) goto mem_alloc_failed;

  xmlrpc_array_read_item(env, array, 0, &xr_ans_sample);

  xmlrpc_parse_value(env, xr_ans_sample, "{s:i,s:i,s:i,s:i,s:d,s:i,s:i,s:i,s:i,s:i,*}",
					 "version_id", &ans_sample->version_id,
					 "channel_id", &ans_sample->channel_id,
					 "provider_timeout", &ans_sample->provider_timeout,
					 "provider_group_number", &ans_sample->provider_group_number,
					 "base_frequency", &ans_sample->base_frequency,
					 "max_period", &ans_sample->max_period,
					 "max_client_number", &ans_sample->max_client_number,
					 "current_client_number", &ans_sample->current_client_number,
					 "status", &ans_sample->status,
					 "symbols.TSP_sample_symbol_info_list_t_len", &ans_sample->symbols.TSP_sample_symbol_info_list_t_len);

  die_if_fault_occurred(env);

  ans_sample->symbols.TSP_sample_symbol_info_list_t_val = malloc(sizeof(TSP_sample_symbol_info_t)*ans_sample->symbols.TSP_sample_symbol_info_list_t_len);
  if (ans_sample->symbols.TSP_sample_symbol_info_list_t_val == NULL) goto mem_alloc_failed;

  for (i=0; i < ans_sample->symbols.TSP_sample_symbol_info_list_t_len; i++) {
	xmlrpc_value *symbol;
	

	xmlrpc_array_read_item(env, array, i+1, &symbol);
	  
	xmlrpc_parse_value(env, symbol, "{s:s,s:i,s:i,s:i,s:i,s:i,s:i,*}",
					   "name", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name,
					   "provider_global_index", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
					   "provider_group_index", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
					   "provider_group_rank", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank,
					   /* "xdr_tsp_t", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t, */
					   "dimension", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
					   "period", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].period,
					   "phase", &ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].phase);
	  
	die_if_fault_occurred(env);
	ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name = strdup(ans_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name);
	/* Dispose of our result value. */
	xmlrpc_DECREF(symbol);
  }

 mem_alloc_failed:
  
  return ans_sample;
}

TSP_request_sample_t* 
xmlrpc_value_to_TSP_request_sample (xmlrpc_env *env, 
									xmlrpc_value *param_array)
{
  TSP_request_sample_t *req_sample;
  xmlrpc_value *xr_req_sample;
  int i;

  req_sample = (TSP_request_sample_t *)malloc(sizeof(*req_sample));
  
  if (req_sample == NULL) goto mem_alloc_failed;

  xmlrpc_array_read_item(env, param_array, 0, &xr_req_sample);

  die_if_fault_occurred(env);

  xmlrpc_parse_value(env, xr_req_sample, "{s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,*}",
					 "version_id", &req_sample->version_id,
					 "channel_id", &req_sample->channel_id,
					 "feature_words[0]", &req_sample->feature_words[0],
					 "feature_words[1]", &req_sample->feature_words[1],
					 "feature_words[2]", &req_sample->feature_words[2],
					 "feature_words[3]", &req_sample->feature_words[3],
					 "consumer_timeout", &req_sample->consumer_timeout,
					 "symbols.TSP_sample_symbol_info_list_t_len", &req_sample->symbols.TSP_sample_symbol_info_list_t_len);
  
  die_if_fault_occurred(env);

  STRACE_DEBUG(("Nombre de symboles: %d\n\n", req_sample->symbols.TSP_sample_symbol_info_list_t_len));


  req_sample->symbols.TSP_sample_symbol_info_list_t_val = malloc(sizeof(TSP_sample_symbol_info_t)*req_sample->symbols.TSP_sample_symbol_info_list_t_len);

  if (req_sample->symbols.TSP_sample_symbol_info_list_t_val == NULL) goto mem_alloc_failed;

  for (i=0; i < req_sample->symbols.TSP_sample_symbol_info_list_t_len; i++) {
	xmlrpc_value *symbol;
	
	xmlrpc_array_read_item(env, param_array, i+1, &symbol);
	  
	xmlrpc_parse_value(env, symbol, "{s:s,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,s:i,*}",
					   "name", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name,
					   "provider_global_index", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index,
					   "provider_group_index", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_index,
					   "provider_group_rank", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].provider_group_rank,
					   /* "xdr_tsp_t", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].xdr_tsp_t, */
					   "dimension", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].dimension,
					   "period", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].period,
                       "phase", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].phase, 
                       "type", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].type, 
                       "dimension", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].dimension,                     
					   "nelem", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].nelem,
                       "offset", &req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].offset);
	  
	req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name = strdup(req_sample->symbols.TSP_sample_symbol_info_list_t_val[i].name);
    
	/* Dispose of our result value. */
	xmlrpc_DECREF(symbol);
  }


 mem_alloc_failed:
  return req_sample;
}

TSP_answer_sample_init_t* 
xmlrpc_value_to_TSP_answer_sample_init (xmlrpc_env *env, 
										xmlrpc_value *param)
{
  TSP_answer_sample_init_t *ans_sample_init;

  ans_sample_init = (TSP_answer_sample_init_t *)malloc(sizeof(*ans_sample_init));

  xmlrpc_parse_value(env, param, "{s:i,s:i,s:s,s:i,*}",
					 "version_id", &ans_sample_init->version_id,
					 "channel_id", &ans_sample_init->channel_id,
					 "data_address", &ans_sample_init->data_address,
					 "status", &ans_sample_init->status);

  die_if_fault_occurred(env);

  ans_sample_init->data_address	= strdup(ans_sample_init->data_address);

  return ans_sample_init;
}


TSP_request_sample_init_t* 
xmlrpc_value_to_TSP_request_sample_init (xmlrpc_env *env, 
										 xmlrpc_value *param)
{
  TSP_request_sample_init_t *req_sample_init;

  req_sample_init = (TSP_request_sample_init_t *)malloc(sizeof(*req_sample_init));

  xmlrpc_parse_value(env, param, "({s:i,s:i,*})",
					 "version_id", &req_sample_init->version_id,
					 "channel_id", &req_sample_init->channel_id);

  die_if_fault_occurred(env);
  
  return req_sample_init;
}

TSP_answer_sample_destroy_t*
xmlrpc_value_to_TSP_answer_sample_destroy(xmlrpc_env *env,
										  xmlrpc_value *xr_ans_sample_destroy)
{
  TSP_answer_sample_destroy_t *ans_sample_destroy;

  ans_sample_destroy = (TSP_answer_sample_destroy_t *)malloc(sizeof(*ans_sample_destroy));

  xmlrpc_parse_value(env, xr_ans_sample_destroy, "{s:i,s:i,s:i,*}",
					 "version_id", &ans_sample_destroy->version_id,
					 "channel_id", &ans_sample_destroy->channel_id,
					 "status", &ans_sample_destroy->status);

  die_if_fault_occurred(env);

  return ans_sample_destroy;
}

TSP_request_sample_destroy_t*
xmlrpc_value_to_TSP_request_sample_destroy(xmlrpc_env *env,
										   xmlrpc_value *xr_req_sample_destroy)
{
  TSP_request_sample_destroy_t *req_sample_destroy;

  req_sample_destroy = (TSP_request_sample_destroy_t *)malloc(sizeof(*req_sample_destroy));

  xmlrpc_parse_value(env, xr_req_sample_destroy, "({s:i,s:i,*})",
					 "version_id", &req_sample_destroy->version_id,
					 "channel_id", &req_sample_destroy->channel_id);

  die_if_fault_occurred(env);

  return req_sample_destroy;
}
