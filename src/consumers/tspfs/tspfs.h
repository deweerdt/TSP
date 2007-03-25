#ifndef _TSPFS_H_
#define _TSPFS_H_

#include <tsp_sys_headers.h>
#include <tsp_prjcfg.h>
#include <tsp_consumer.h>

#define __USE_GNU
#include <search.h>

#define MAX_SYM_DISP_SIZE 512
#define MAX_FORMAT_SIZE 512
#define MAX_URL_SIZE 512

#define TSPFS_DEBUG 0

struct tspfs_sample {
	union {
		TSP_sample_t sync;
		TSP_consumer_async_sample_t async;
	};
	union {
		double d;
	} value;
};

struct tspfs {
	/* 0 if not using synchronous mode. If > 0, this is the sync read period*/
	int sync;
	/* 0 if not using asynchronous mode. If > 0, use async read */
	int async;
	/* url to connect to */
	char *url;
	/* filter the symbols need to match */
	char *filter;
	/* information on the requested symbols */
	const TSP_answer_sample_t *information;
	TSP_sample_symbol_info_list_t symbols;
	/* a link to a provider */
	TSP_provider_t provider;
	/* the symbols themselves */
	TSP_sample_t **samples;
	/* display format for the symbols */
	char **formats;
	/* number of requested symbols */
	unsigned int nr_samples;
	/* optional file extension */
	char *ext;
	int ext_len;
	/* pgi -> internal index conversion table */
	int *idx_to_pgi;
	int idx_to_pgi_len;
	/* internal name -> index hashtable */
	struct hsearch_data name_to_index;
};



#endif /* _TSPFS_H_ */
