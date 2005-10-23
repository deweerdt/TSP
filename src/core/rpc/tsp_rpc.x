/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/rpc/tsp_rpc.x,v 1.20 2005-10-23 16:01:18 erk Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer / Provider

-----------------------------------------------------------------------

Purpose   : 

-----------------------------------------------------------------------
 */

/**
 * @defgroup RPC
 * The TSP RPC module is the one which concentrate the
 * RPC interface of TSP for both the Client and Server part.
 * @ingroup Core
 * @{
 */



/** Status for all functions */
enum TSP_status_t 
{
	/** OK */
	TSP_STATUS_OK,

	/** Fatal error */
	TSP_STATUS_ERROR_UNKNOWN,

	/** 
	 * Means that a detailed error string is provided by the function,
	 * and the details of the error are in this string
         */
	TSP_STATUS_ERROR_SEE_STRING,	

	/** 
         * The requested version for the protocol does not match
         */
	TSP_STATUS_ERROR_VERSION,

	/**
         * Error with the symbols (asked or returned )
         */

	TSP_STATUS_ERROR_SYMBOLS,

	/**
	 * The requested symbols filter is ill-formed
	 */
	TSP_STATUS_ERROR_SYMBOL_FILTER,
	/**
         * The requested feature is not supported (by this particular consumer/provider)
         */
	TSP_STATUS_ERROR_NOT_SUPPORTED,
	/**	
         * The requested feature is not implemented
         */
	TSP_STATUS_ERROR_NOT_IMPLEMENTED
	
};

/** 
 * The TSP answer open is the answer
 * from a TSP request open.
 */
struct TSP_answer_open_t
{
        /** The Version of TSP protocol */
        int version_id;            
        /** The Session id used to send forthcoming request,
	    which is a session identifier stored by the
	    TSP provider in order to handle provider-side
	    consumer state or data */
        unsigned int channel_id;   
        /** The provider status */
        TSP_status_t status;  
        /** A string describing the provider status */
	string status_str<>;

};


typedef string   TSP_argv_item_t<>;
typedef TSP_argv_item_t TSP_argv_t<>; 

/**  
 * The TSP request open is the first TSP request
 * a TSP consumer may send to obtain a TSP channel id
 * in order to be able to send other TSP request.
 */
struct TSP_request_open_t
{
  int version_id;        /**< 
/*	string stream_init<>;
	int use_stream_init;*/

	TSP_argv_t argv;	

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
FIXME : Use it when the consumer will be able
to ask for a given endianity
enum TSP_endianity_t 
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

	int provider_timeout;
	int provider_group_number;
	TSP_sample_symbol_info_list_t symbols;
	double base_frequency;
	int max_period; 
	int max_client_number; 
	int current_client_number; 
	TSP_status_t status;		

	/*unsigned int feature_words[4]; FIXME*/
	/* TSP_endianity_t data_endianity; FIXME : implementer */


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
	TSP_status_t status;			
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

/* tsp async read/write */
struct TSP_async_sample_t
{
	int provider_global_index;
        opaque data<>;
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
	
	int 				TSP_REQUEST_CLOSE(TSP_request_close_t req_close) = 103;
	
	TSP_answer_sample_t		TSP_REQUEST_INFORMATION(TSP_request_information_t req_info) = 104;
	
	TSP_answer_feature_t		TSP_REQUEST_FEATURE(TSP_request_feature_t req_feature) = 105;
	
	TSP_answer_sample_t		TSP_REQUEST_SAMPLE(TSP_request_sample_t req_sample) = 106;
	
	TSP_answer_sample_init_t	TSP_REQUEST_SAMPLE_INIT(TSP_request_sample_init_t req_sample) = 107;
	
	TSP_answer_sample_destroy_t	TSP_REQUEST_SAMPLE_DESTROY(TSP_request_sample_destroy_t req_destroy) = 108;
	
	int				TSP_EXEC_FEATURE(TSP_exec_feature_t exec_feature) = 109;
	
	int 				TSP_REQUEST_ASYNC_SAMPLE_WRITE(TSP_async_sample_t async_sample_write) = 110;

	TSP_async_sample_t              TSP_REQUEST_ASYNC_SAMPLE_READ(TSP_async_sample_t async_sample_read) = 111;

	TSP_answer_sample_t		TSP_REQUEST_FILTERED_INFORMATION(TSP_request_information_t req_info, int filter_kind, string filter_string<>) = 114;
	

	} = 1;
} = 0x31230010 ;


/** @} */

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
