/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc.x,v 1.7 2002-09-19 08:37:04 galles Exp $

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
	unsigned int channel_id;

	/* int status; FIXME : implementer, mettre un enum ?*/
};

struct TSP_request_open_t
{
	int version_id;
};

struct TSP_request_close_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_request_information_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_request_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
};


struct TSP_answer_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_exec_feature_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int int_value;
	double double_value;
	string string_value<>;
};

struct TSP_sample_symbol_info_t
{
	string name<>;
	int provider_global_index;
	int provider_group_index;
	int provider_group_rank;
	opaque xdr_tsp_t[4];
	unsigned int dimension;
	int period;
	int phase; 
};


/*struct TSP_sample_symbol_info_list_t
{
	TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;
};
*/

typedef TSP_sample_symbol_info_t TSP_sample_symbol_info_list_t<>;

/*
FIXME : utiliser...
struct TSP_endianity_t 
{
	TSP_PSEUDO_XDR_LITTLE_ENDIAN,
	TSP_XDR_BIG_ENDIAN
};*/


struct TSP_request_sample_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int consumer_timeout;
	TSP_sample_symbol_info_list_t symbols;

	/* TSP_endianity_t data_endianity; FIXME : implementer */
};

struct TSP_answer_sample_t
{
	int version_id;
	unsigned int channel_id;
	unsigned int feature_words[4];
	int provider_timeout;
	int provider_group_number;
	TSP_sample_symbol_info_list_t symbols;

	/* TSP_endianity_t data_endianity; FIXME : implementer */
	/* double base_frequency; FIXME : implementer */
	/* int max_period; FIXME : implementer */
	/* int max_client_number; FIXME : implementer */
	/* int current_client_number; FIXME : implementer */
	/* int status; FIXME : implementer */

};

struct TSP_request_sample_init_t
{
	int version_id;
	unsigned int channel_id;
};

struct TSP_answer_sample_init_t
{
	int version_id;
	unsigned int channel_id;
	string data_address<>;
	
	/* int status; FIXME : utiliser */
};


/* FIXME : utiliser ca...*/
struct TSP_asynchronous_sample_symbol_t
{
	int provider_index;
};


/* FIXME : implementer */
struct TSP_request_sample_destroy_t
{
	int version_id;
	unsigned int channel_id;	
};

/* FIXME : implementer */
struct TSP_answer_sample_destroy_t
{
	int version_id;
	unsigned int channel_id;	
	int status;
};



/* This structure is not part of TSP Protocol*/
struct TSP_provider_info_t{
	string info<>;
};


/*------------------------------------------------------*/
/*		RPC Functions for TSP Provider		*/
/*------------------------------------------------------*/
program TSP_RPC {

	version TSP_RPC_VERSION_INITIAL {

	

	/* This function is not part of TSP Protocol */	
	TSP_provider_info_t 	TSP_PROVIDER_INFORMATION(void) = 101;

	
	/* TSP Protocol */	

	TSP_answer_open_t 		TSP_REQUEST_OPEN(TSP_request_open_t req_open) = 102;
	
	void 				TSP_REQUEST_CLOSE(TSP_request_close_t req_close) = 103;
	
	TSP_answer_sample_t		TSP_REQUEST_INFORMATION(TSP_request_information_t req_info) = 104;
	
	TSP_answer_feature_t		TSP_REQUEST_FEATURE(TSP_request_feature_t req_feature) = 105;
	
	TSP_answer_sample_t		TSP_REQUEST_SAMPLE(TSP_request_sample_t req_sample) = 106;
	
	TSP_answer_sample_init_t	TSP_REQUEST_SAMPLE_INIT(TSP_request_sample_init_t req_sample) = 107;
	
	TSP_answer_sample_destroy_t	TSP_REQUEST_SAMPLE_DESTROY(TSP_request_sample_destroy_t req_sample) = 108;
	
	void				TSP_EXEC_FEATURE(TSP_exec_feature_t exec_feature) = 109;
	

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
