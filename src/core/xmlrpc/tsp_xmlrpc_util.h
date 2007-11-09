#ifndef _TSP_XMLRPC_UTIL_H_
#define _TSP_XMLRPC_UTIL_H_

#include <xmlrpc.h>

#include "tsp_rpc.h"

#define die_if_fault_occurred(x) \
{ \
    if ((x)->fault_occurred) { \
        STRACE_ERROR(("XML-RPC Fault: %s (%d)\n", \
					  (x)->fault_string, (x)->fault_code)); \
        exit(1); \
    } \
}

/* TSP => XMLRPC */

xmlrpc_value* TSP_answer_sample_to_xmlrpc_value (xmlrpc_env *env, TSP_answer_sample_t *ans_sample);
xmlrpc_value* TSP_request_sample_to_xmlrpc_value (xmlrpc_env *env, TSP_request_sample_t *req_sample);
xmlrpc_value* TSP_answer_sample_init_to_xmlrpc_value (xmlrpc_env *env, TSP_answer_sample_init_t *ans_sample_init);
xmlrpc_value* TSP_answer_sample_destroy_to_xmlrpc_value(xmlrpc_env *env, TSP_answer_sample_destroy_t *ans_sample_destroy);

/* XMLRPC => TSP */

TSP_provider_info_t* xmlrpc_value_to_TSP_provider_info (xmlrpc_env* env, xmlrpc_value *xr_prov_info);
TSP_answer_open_t * xmlrpc_value_to_TSP_answer_open (xmlrpc_env *env, xmlrpc_value *xr_ans_open);
TSP_answer_sample_t* xmlrpc_value_to_TSP_answer_sample (xmlrpc_env *env, xmlrpc_value *array);
TSP_request_sample_t* xmlrpc_value_to_TSP_request_sample (xmlrpc_env *env, xmlrpc_value *param_array);
TSP_request_sample_init_t* xmlrpc_value_to_TSP_request_sample_init (xmlrpc_env *env, xmlrpc_value *param);
TSP_request_sample_destroy_t* xmlrpc_value_to_TSP_request_sample_destroy(xmlrpc_env *env, xmlrpc_value *xr_req_sample_destroy);

#endif /* _TSP_XMLRPC_UTIL_H_ */
