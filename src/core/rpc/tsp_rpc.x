/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc.x,v 1.5 2002-09-13 16:39:30 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */


struct TSP_answer_open_t
{
	int version_id;
	int channel_id;
};

struct TSP_request_open_t
{
	int version_id;
};

struct TSP_request_close_t
{
	int version_id;
	int channel_id;
};

struct TSP_request_information_t
{
	int version_id;
	int channel_id;
};

struct TSP_request_feature_t
{
	int version_id;
	int channel_id;
	unsigned int feature_words[4];
};


struct TSP_answer_feature_t
{
	int version_id;
	int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_exec_feature_t
{
	int version_id;
	int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_sample_symbol_info_t
{
	string name<>;
	int provider_global_index;
	/*opaque provider_group_index[2];*/
	int provider_group_index;
	int provider_group_rank;
	opaque xdr_tsp_t[4];
	/*int xdr_type;
	int tsp_type;*/
	unsigned int dimension;
	int period;
	int phase; 
	opaque padding_t[2];
};


/*struct TSP_sample_symbol_info_list_t
{
	TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;
};
*/

typedef TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;

struct TSP_request_sample_t
{
	int version_id;
	int channel_id;
	unsigned int feature_words[4];
	int consumer_timeout;
	TSP_sample_symbol_info_list_t symbols;
};

struct TSP_answer_sample_t
{
	int version_id;
	int channel_id;
	unsigned int feature_words[4];
	int provider_timeout;
	string data_address<>;
	TSP_sample_symbol_info_list_t symbols;
	/*opaque provider_group_number[2];*/
	int provider_group_number;
	opaque padding_t[2];
};



struct TSP_asynchronous_sample_symbol_t
{
	int provider_index;
};


struct TSP_server_info_t{
	string info<>;
};


/*------------------------------------------------------*/
/*		Programme RPC du serveur TSP		*/
/*------------------------------------------------------*/
program TSP_RPC {

	version TSP_RPC_VERSION_INITIAL {

	
	TSP_server_info_t 	TSP_SERVER_INFO(void) = 101;
	
	TSP_answer_open_t 	TSP_REQUEST_OPEN(TSP_request_open_t req_open) = 102;
	
	void 			TSP_REQUEST_CLOSE(TSP_request_close_t req_close) = 103;
	
	TSP_answer_sample_t	TSP_REQUEST_INFORMATION(TSP_request_information_t req_info) = 104;
	
	TSP_answer_feature_t	TSP_REQUEST_FEATURE(TSP_request_feature_t req_feature) = 105;
	
	TSP_answer_sample_t	TSP_REQUEST_SAMPLE(TSP_request_sample_t req_sample) = 106;
	
	TSP_answer_sample_t	TSP_REQUEST_SAMPLE_INIT(TSP_request_sample_t req_sample) = 107;
	
	TSP_answer_sample_t	TSP_REQUEST_SAMPLE_DESTROY(TSP_request_sample_t req_sample) = 108;
	
	void			TSP_EXEC_FEATURE(TSP_exec_feature_t exec_feature) = 109;
	

	} = 1;
} = 0 ;




#ifdef RPC_CLNT
%
%int tsp_wrap_rpc_clnt_set_timeout(CLIENT *client, int timeout)
%{
%	static struct timeval	tv;
%    
%	/* RPC timeout definition */
%	/* ----------------------- */
%	tv.tv_sec  = timeout;
%	tv.tv_usec = 0;
%	return clnt_control(client, CLSET_TIMEOUT, (char *)&tv);
%}
#endif

%#define TSP_STRACE_RPC_ERROR(cl, pResult) if(!pResult)  {  STRACE_ERROR(("%s", clnt_sperror(cl, "")));  }
