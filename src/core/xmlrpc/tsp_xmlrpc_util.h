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

xmlrpc_value* TSP_answer_sample_to_xmlrpc_value (const xmlrpc_env *env, const TSP_answer_sample_t *ans_sample);
xmlrpc_value* TSP_request_sample_to_xmlrpc_value (const xmlrpc_env *env, const TSP_request_sample_t *req_sample);

/* XMLRPC => TSP */

TSP_provider_info_t* xmlrpc_value_to_TSP_provider_info (const xmlrpc_env* env, const xmlrpc_value *xr_prov_info);
TSP_answer_open_t * xmlrpc_value_to_TSP_answer_open (const xmlrpc_env *env, const xmlrpc_value *xr_ans_open);
TSP_answer_sample_t* xmlrpc_value_to_TSP_answer_sample (const xmlrpc_env *env, const xmlrpc_value *array);
TSP_request_sample_t* xmlrpc_value_to_TSP_request_sample (const xmlrpc_env *env, const xmlrpc_value *param_array);

#endif /* _TSP_XMLRPC_UTIL_H_ */