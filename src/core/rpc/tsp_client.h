/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_client.h,v 1.3 2002-11-29 17:33:34 tntdev Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */

#ifndef _TSP_CLIENT
#define _TSP_CLIENT

#include "tsp_prjcfg.h"

#include "tsp_rpc.h"

typedef  void* TSP_server_t;

int TSP_remote_open_server( const char *target_name,
			    int server_id, 
			    TSP_server_t* server,
			    TSP_server_info_string_t server_info);

void TSP_remote_close_server(TSP_server_t server);

int TSP_get_server_max_number(void);

TSP_answer_open_t* TSP_request_open(const TSP_request_open_t* req_open,
				    TSP_server_t server);
int TSP_request_close(const TSP_request_close_t* req_close,
		      TSP_server_t server);	
		      
TSP_answer_sample_t * TSP_request_information(
					      const TSP_request_information_t* req_info,
					      TSP_server_t server);
	
TSP_answer_sample_t * TSP_request_sample(
					 const TSP_request_sample_t* req_sample,
					 TSP_server_t server);

TSP_answer_sample_init_t * TSP_request_sample_init(
					      const TSP_request_sample_init_t* req_sample,
					      TSP_server_t server);

TSP_answer_sample_destroy_t * TSP_request_sample_destroy(const TSP_request_sample_destroy_t* req_sample,
							 TSP_server_t server);




#endif /* _TSP_CLIENT */
