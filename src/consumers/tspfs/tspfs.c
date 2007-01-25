/*
 * tspfs: a usermode filesystem that uses fuse
 * (see http://fuse.sourceforge.net) to export TSP
 * symbols as files in a directory.
 *
 * (c) Frederik Deweerdt, Eric Noulard 2006
 *
 * This program is distributed under the terms of the GNU LGPL.
 * See COPYING.
 */

/** 
 * @defgroup TSPFS tspfs
 * @ingroup TSP_Consumers
 *
 * tspfs: is a usermode filesystem that uses fuse
 * (see http://fuse.sourceforge.net) to export TSP
 * symbols as files in a directory.
 *
 * \par usage: tspfs [--url=\<tsp_provider_url\>] [--sync=\<sampling period\>]|[--async] [--filter] \<mount_point\>
 * \par 
 * <ul>
 *   <li> \b --url                   the url to connect to, default: rpc://localhost </li>
 *   <li> \b --sync=\<period\>         use synchronous mode to read the sample values with a period \<period\> </li>
 *   <li> \b --async                 use asynchronous mode to read the sample values, this is the default </li>
 *   <li> \b --filter                basic filter used to match the symbol names </li>
 *   <li> \b --ext=\<ext\>             Suffix file names with \<ext\>, handy for windows browsing </li>
 * </ul>
 * \section Sample tspfs session
 * \verbatim
 
 # Start a sample bb
 $ bb_simu -s > /dev/null 2>&1 &
 
 # TSPize the bb's data
 $ bb_tsp_provide bb_simu 10 &
 
 # Create the mount point, mount the tspfs filesystem
 $ mkdir mp/
 $ tspfs --ext=.txt --url=rpc://localhost/bb_simu --sync=1 mp/
 
 # List the symbols
 $ ls -l mp/
 total 0
 -rw-rw-rw-  1 root root 512 jan  1  1970 bb_simu_1_HugeArray[0].txt
 [...]
 -rw-rw-rw-  1 root root 512 jan  1  1970 ECLA_0_d_ecl_lune.txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 ECLA_0_d_ecl_sol.txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[0].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[1].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[2].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirLun[0].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirLun[1].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirLun[2].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirSol[0].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirSol[1].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 POSA_0_d_DirSol[2].txt
 -rw-rw-rw-  1 root root 512 jan  1  1970 Sequenceur_0_d_t_s.txt
 
 # Display a particular value
 $ cat mp/Sequenceur_0_d_t_s.txt
 time=197600 value=3961.280000
 $ cat mp/Sequenceur_0_d_t_s.txt
 time=197680 value=3962.880000
 
 # Change the display format (notice we use \012, not \n, this needs to be fixed)
 $ setfattr -n format -v 't=%d v=%e\012' ECLA_0_d_ecl_sol.txt
 $ cat ECLA_0_d_ecl_sol.txt
 t=208880 v=1.000000e+00
 
 # Get the format from a symbol
 $ getfattr -n format bb_simu_int8\[0\]
 # file: bb_simu_int8[0]
 format="time=%d value=%f\012"
 
 # Go wild, umount the filesystem
 $ fusermount -u mp/
 \endverbatim
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <attr/xattr.h>
#include <signal.h>

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>

#define _GNU_SOURCE
#include <getopt.h>


#include <tsp_sys_headers.h>
#include <tsp_prjcfg.h>
#include <tsp_consumer.h>

#define MAX_SYM_DISP_SIZE 512
#define MAX_FORMAT_SIZE 512
#define MAX_URL_SIZE 512

#define TSPFS_DEBUG 0

static int is_tsp_symbol(const char *path);
static char *get_symname_from_path(const char *path);

static int tspfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	char *sym_name;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return res;
	}

	sym_name = get_symname_from_path(path);
	if (!sym_name) {
		printf("ENOMEM %s\n", path);
		return -ENOMEM;
	}

	if (is_tsp_symbol(sym_name)) {
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = MAX_SYM_DISP_SIZE;
	} else
		res = -ENOENT;

	free(sym_name);
	return res;
}

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
};

static struct tspfs tspfs;

static void set_sym_format(int index, const char *new_format)
{
	assert(index >= 0 && index < tspfs.nr_samples);
	tspfs.formats[index] = malloc(sizeof(char) * MAX_FORMAT_SIZE);
	strncpy(tspfs.formats[index], new_format, MAX_FORMAT_SIZE);
}

static char *get_sym_format(int index)
{
	assert(index >= 0 && index < tspfs.nr_samples);
	if (tspfs.formats[index] != NULL)
		return tspfs.formats[index];
	return "time=%d value=%f\n";
}

static char *get_symname_from_path(const char *path)
{
	char *sym_name;
	int len = strlen(path);

	/* Handle special case for root path */
	if (strcmp("/", path) == 0) {
		return strdup("/");
	}
	/* skip leading "/" */
	len -= sizeof(char);
	/* trim extension if needed */
	if (tspfs.ext)
		len -= sizeof(char)*tspfs.ext_len;

	sym_name = (char *)malloc((len+1)*sizeof(char));
	if (!sym_name)
		return NULL;

	strncpy(sym_name, path + sizeof(char), len);
	sym_name[len] = '\0';
	return sym_name;
}

static inline struct tspfs_sample *get_tsp_symbol_value(int index)
{
	struct tspfs_sample *s = NULL;
	assert(index >= 0 && index < tspfs.nr_samples);

	s = (struct tspfs_sample *)malloc(sizeof(struct tspfs_sample));
	if (tspfs.sync) {
		int new_sample;	
#ifdef TSPFS_DEBUG
		fprintf(stderr, "QSD:get_value: idx = %d, pgi = %d\n", index,
				tspfs.samples[index]->provider_global_index);
#endif
		TSP_consumer_read_sample(tspfs.provider,
					 tspfs.samples[index],
					 &new_sample);
		s->sync = *tspfs.samples[index];
	} else {
		int ret;
		/* 
		 * Beware that PGI is not the tspfs local 'index' since
		 *      - filtered info may gives you non-contiguous PGI
		 */
  		s->async.provider_global_index = tspfs.symbols
							.TSP_sample_symbol_info_list_t_val[index]
							.provider_global_index; 
    		s->async.value_ptr  = &s->value.d;
      		s->async.value_size = sizeof(s->value.d);
		ret = TSP_consumer_request_async_sample_read(tspfs.provider,&(s->async));	
	}
	return s;
}
static int get_tsp_symbol_index(const char *symbol_name)
{
	int i;
	/* Compare symbols names */
	for (i = 0; i < tspfs.symbols.TSP_sample_symbol_info_list_t_len; i++) {
		if (!strcmp (symbol_name,
		     tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name)) {
#ifdef TSPFS_DEBUG
			fprintf(stderr, "AZE:Matched: %s, %d\n", 
				tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name, 
				tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].provider_global_index); 
#endif
			return i;
		}

	}

	return -1;
}

static inline int is_tsp_symbol(const char *symbol_name)
{
	return get_tsp_symbol_index(symbol_name) >= 0;
}

static void tspfs_destroy(void *unused)
{
	int i;

	for (i = 0; i < tspfs.nr_samples; i++) {
		free(tspfs.samples[i]);
	}
	for (i = 0; i < tspfs.nr_samples; i++) {
		if (!tspfs.formats[i])
			free(tspfs.formats[i]);
	}

	if (tspfs.sync) 
		free(tspfs.idx_to_pgi);

	if (tspfs.sync && (TSP_STATUS_OK!=TSP_consumer_request_sample_destroy(tspfs.provider))) {
		STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed"));
	}

	if (TSP_STATUS_OK!=TSP_consumer_request_close(tspfs.provider)) {
		STRACE_ERROR(("Function TSP_consumer_request_close failed"));
	}

	TSP_consumer_end();
}




static int tspfs_init_connect(int argc, char **argv, char *url)
{

	int i;

	if (TSP_STATUS_OK!=TSP_consumer_init(&argc, &argv)) {
	  printf("TSP Initialization failed\n");
	  exit(-1);
	}
	tspfs.provider = TSP_consumer_connect_url(url);
	if (tspfs.provider == NULL) {
		printf("Could not connect to provider %s\n", url);
		exit(-1);
	}
	if (TSP_STATUS_OK!=TSP_consumer_request_open(tspfs.provider, 0, 0)) {
	  printf("TSP Request Open failed\n");
	  exit(-1);
	}

	if (TSP_STATUS_OK!=TSP_consumer_request_information(tspfs.provider)) {
	   printf("TSP Request Information failed\n");
	   exit(-1);
	}

	if (tspfs.filter) {
	  if (TSP_STATUS_OK!=TSP_consumer_request_filtered_information(tspfs.provider, TSP_FILTER_SIMPLE, tspfs.filter)) {
	    printf("TSP Request Filtered Information failed\n");
	    exit(-1);
	  }
	}

	tspfs.information = TSP_consumer_get_information(tspfs.provider);

	tspfs.symbols.TSP_sample_symbol_info_list_t_len = tspfs.information->symbols.TSP_sample_symbol_info_list_t_len;
	tspfs.symbols.TSP_sample_symbol_info_list_t_val =
	    (TSP_sample_symbol_info_t *) calloc(tspfs.symbols.TSP_sample_symbol_info_list_t_len,
						      sizeof(TSP_sample_symbol_info_t));
	if (!tspfs.symbols.TSP_sample_symbol_info_list_t_val)
		return -1;

	for (i = 0; i < tspfs.symbols.TSP_sample_symbol_info_list_t_len; i++) {
		tspfs.symbols.TSP_sample_symbol_info_list_t_val[i] =
		    tspfs.information->symbols.TSP_sample_symbol_info_list_t_val[i];
		tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name =
		    strdup(tspfs.information->symbols.TSP_sample_symbol_info_list_t_val[i].name);
		tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].period = tspfs.sync;
		tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].phase = 0;
	}

	tspfs.nr_samples = tspfs.information->symbols.TSP_sample_symbol_info_list_t_len;
	tspfs.samples =
	    (TSP_sample_t **) malloc(sizeof(TSP_sample_t *) *
				     tspfs.nr_samples);
	if (tspfs.sync) {
		int i;
		tspfs.idx_to_pgi = (int *)malloc(sizeof(int)*tspfs.nr_samples);
		for (i=0; i < tspfs.nr_samples; i++) {
			tspfs.idx_to_pgi[i] = -1;
		}
		tspfs.idx_to_pgi_len = 0;
	}
	if (!tspfs.samples) {
		free(tspfs.symbols.TSP_sample_symbol_info_list_t_val);
		return -1;
	}
	for (i = 0; i < tspfs.nr_samples; i++) {
		tspfs.samples[i] = malloc(sizeof(TSP_sample_t));
		if (!tspfs.samples[i]) {
			free(tspfs.symbols.TSP_sample_symbol_info_list_t_val);
			free(tspfs.samples);
			return -1;
		}
	}

	tspfs.formats =
	    (char **) malloc(sizeof(char *) * tspfs.nr_samples);

	if (!tspfs.formats) {
		free(tspfs.symbols.TSP_sample_symbol_info_list_t_val);
		free(tspfs.samples);
		return -1;
	}

	for (i = 0; i < tspfs.nr_samples; i++) {
		tspfs.formats[i] = NULL;
	}

	return 1;
}

/*
 * TODO: this really wants to be a hashtable. 
 * Find a portable implementation
 */
static int pgi_to_idx(int pgi) {
	int i;
	for (i=0; tspfs.idx_to_pgi[i] != -1 && i < tspfs.nr_samples; i++) {
		if (tspfs.idx_to_pgi[i] == pgi) {
			return i;
		}
	}
	/* shouldn't happen */
	assert(1==1);
	return -1;
}
/*
 * Called back each time a symbol's value changes. 
 * Only used in sync mode 
 */
static void tspfs_sync_read_sample_cb (TSP_sample_t* s, void* user_data)
{
	if (tspfs.nr_samples != tspfs.idx_to_pgi_len && pgi_to_idx(s->provider_global_index) == -1) {
		tspfs.idx_to_pgi[tspfs.idx_to_pgi_len] = s->provider_global_index;
		tspfs.idx_to_pgi_len++;
	}
	*tspfs.samples[pgi_to_idx(s->provider_global_index)] = *s;
	return;
}


static void *tspfs_init(void)
{
	/* Do specific sync mode initialization */
	if (tspfs.sync) {
		TSP_consumer_request_sample(tspfs.provider, &tspfs.symbols);
		TSP_consumer_request_sample_init(tspfs.provider, 
						 tspfs_sync_read_sample_cb, 
						 0);
	}

	return NULL;
}

static int tspfs_readdir(const char *path, void *buf,
			 fuse_fill_dir_t filler, off_t offset,
			 struct fuse_file_info *fi)
{
	int i;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

	for (i = 0; i < tspfs.symbols.TSP_sample_symbol_info_list_t_len; i++) {
		char *fullname;
		if (tspfs.ext) {
			fullname = 
				(char *)malloc(sizeof(char)
				*strlen(tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name)
				+strlen(tspfs.ext)+2);
			if (!fullname)
				return -ENOMEM;
			sprintf(fullname, "%s%s",
				tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name, 
				tspfs.ext);
			filler(buf, fullname, NULL, 0);
			free(fullname);
		} else {
			fullname = tspfs.symbols.TSP_sample_symbol_info_list_t_val[i].name;
			filler(buf, fullname, NULL, 0);
		}
	}

	return 0;
}

static int tspfs_open(const char *path, struct fuse_file_info *fi)
{
	int res = 0;
	char *sym_name;

	sym_name = get_symname_from_path(path);
	if (!sym_name)
		return -ENOMEM;

	if (!is_tsp_symbol(sym_name))
		res = -ENOENT;

	free(sym_name);
	return res;
}
static int tspfs_write(const char *path, const char *buf, size_t size,
		       off_t offset, struct fuse_file_info *fi)
{
	double value;
	struct tspfs_sample s;
	char *sym_name;
	int idx;
	 
	sym_name = get_symname_from_path(path);
	if (!sym_name)
		return -ENOMEM;

	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0) {
		free(sym_name);
		return -ENOENT;
	}

	value = strtod(buf, NULL);
  	s.async.provider_global_index = tspfs.symbols
					.TSP_sample_symbol_info_list_t_val[idx].provider_global_index; 
    	s.async.value_ptr  = &value;
      	s.async.value_size = sizeof(value);
	TSP_consumer_request_async_sample_write(tspfs.provider,&s.async);	
	
	free(sym_name);
	return size;
}

static int tspfs_read(const char *path, char *buf, size_t size,
		      off_t offset, struct fuse_file_info *fi)
{
	size_t len;
	int idx;
	struct tspfs_sample *sample;
	char *sym_name;
	char sym_display[MAX_SYM_DISP_SIZE];

	sym_name = get_symname_from_path(path);
	if (!sym_name)
		return -ENOMEM;

	if (!is_tsp_symbol(sym_name)) {
		free(sym_name);
		return -ENOENT;
	}

	idx = get_tsp_symbol_index(sym_name);
	sample = get_tsp_symbol_value(idx);
	if (tspfs.sync) {
		char *format;
		format = "";
#ifdef TSPFS_DEBUG
		fprintf(stderr,"AZE:Symbol is : %s type: %d pgi: %d, idx: %d\n", sym_name, sample->sync.type, sample->sync.provider_global_index, idx);
#endif
		switch (sample->sync.type) {
			case TSP_TYPE_DOUBLE:
				sprintf(sym_display, "t=%d v=%e %%e\n", sample->sync.time,
					sample->sync.uvalue.double_value);
				break;
			case TSP_TYPE_FLOAT:
				sprintf(sym_display, "t=%d v=%e %%e\n", sample->sync.time,
					sample->sync.uvalue.float_value);
				break;
			case TSP_TYPE_INT8:
				sprintf(sym_display, "t=%d v=%d %%d\n", sample->sync.time,
					sample->sync.uvalue.int8_value);
				break;
			case TSP_TYPE_INT16:
				sprintf(sym_display, "t=%d v=%d %%d\n", sample->sync.time,
					sample->sync.uvalue.int16_value);
				break;
			case TSP_TYPE_INT32:
				sprintf(sym_display, "t=%d v=%d %%d\n", sample->sync.time,
					sample->sync.uvalue.int32_value);
				break;
			case TSP_TYPE_INT64:
				sprintf(sym_display, "t=%d v=%lld %%lld\n", sample->sync.time,
					sample->sync.uvalue.int64_value);
				break;
			case TSP_TYPE_UINT8:
				sprintf(sym_display, "t=%d v=%u %%u\n", sample->sync.time,
					sample->sync.uvalue.uint8_value);
				break;
			case TSP_TYPE_UINT16:
				sprintf(sym_display, "t=%d v=%u %%u\n", sample->sync.time,
					sample->sync.uvalue.uint16_value);
				break;
			case TSP_TYPE_UINT32:
				sprintf(sym_display, "t=%d v=%u %%u\n", sample->sync.time,
					sample->sync.uvalue.uint32_value);
				break;
			case TSP_TYPE_UINT64:
				sprintf(sym_display, "t=%d v=%llu %%llu\n", sample->sync.time,
					sample->sync.uvalue.uint64_value);
				break;
			case TSP_TYPE_CHAR:
				sprintf(sym_display, "t=%d v=%c %%c\n", sample->sync.time,
					sample->sync.uvalue.char_value);
				break;
			case TSP_TYPE_UCHAR:
				sprintf(sym_display, "t=%d v=%c %%c\n", sample->sync.time,
					sample->sync.uvalue.uchar_value);
				break;
			case TSP_TYPE_RAW:
				sprintf(sym_display, "t=%d v=%d %%d\n", sample->sync.time,
					sample->sync.uvalue.raw_value);
				break;
			case TSP_TYPE_LAST:
			case TSP_TYPE_UNKNOWN:
			default:
				sprintf(sym_display, "unknown type");
				break;
		}
	} else {
		/* async case */
		sprintf(sym_display, get_sym_format(idx), 0,
			*((double *)(sample->async.value_ptr)));
	}

	len = strlen(sym_display);
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, sym_display + offset, size);
	} else
		size = 0;

	free(sym_name);
	free(sample); 
	return size;
}

static int tspfs_setxattr(const char *path, const char *name,
			  const char *value, size_t size, int flags)
{
	int idx;
	char *sym_name;
	int ret = 0;

	sym_name = get_symname_from_path(path);
	if (!sym_name)
		return -ENOMEM;

	if (strcmp(name, "format")) {
		ret = -ENOATTR;
	}

	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0) {
		ret = -ENOENT;
		goto out;
	}

	if (size > MAX_FORMAT_SIZE) {
		ret = -ERANGE;
		goto out;
	}

	if (flags & XATTR_REPLACE) {
		if (tspfs.formats[idx] == NULL) {
			ret = -ENOATTR;
			goto out;
		}
	}
	if (flags & XATTR_CREATE) {
		if (tspfs.formats[idx] != NULL) {
			ret = -EEXIST;
			goto out;
		}
	}
	set_sym_format(idx, value);
out:
	free(sym_name);
	return ret;
}

static int tspfs_getxattr(const char *path, const char *name, char *value,
			  size_t size)
{
	int idx;
	char *format;
	int format_len;
	int ret = 0;
	char *sym_name;

	sym_name = get_symname_from_path(path);
	if (!sym_name)
		return -ENOMEM;

	if (strcmp(name, "format")) {
		ret = -ENOATTR;
		goto out;
	}

	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0) {
		ret = -ENOENT;
		goto out;
	}

	format = get_sym_format(idx);
	format_len = strlen(format);
	/* the user just wants to know the symbol's size */
	if (size == 0) {
		ret = format_len;
		goto out;
	}

	if (size < format_len) {
		ret = -ERANGE;
		goto out;
	}

	strncpy(value, format, format_len);
	ret = format_len;

out:
	free(sym_name);
	return ret;
}

static int tspfs_chmod (const char *path, mode_t mode)
{
	return -ENOSYS;
}
static int tspfs_chown (const char *path, uid_t uid, gid_t gid)
{
	return -ENOSYS;
}
static int tspfs_truncate (const char *path, off_t offset)
{
	return 0;
}
static int tspfs_utime (const char *path, struct utimbuf *utim)
{
	return -ENOSYS;
}

static struct fuse_operations tspfs_oper = {
	.chmod = tspfs_chmod,
	.chown = tspfs_chown,
	.truncate = tspfs_truncate,
	.utime = tspfs_utime,
	.init = tspfs_init,
	.setxattr = tspfs_setxattr,
	.getxattr = tspfs_getxattr,
	.getattr = tspfs_getattr,
	.readdir = tspfs_readdir,
	.open = tspfs_open,
	.read = tspfs_read,
	.write = tspfs_write,
	.destroy = tspfs_destroy,
};


#define TSPFS_OPT(t, p, v) { t, offsetof(struct tspfs, p), v }

static struct fuse_opt tspfs_opts[] = {
	TSPFS_OPT("--url=%s", url, 0),
	TSPFS_OPT("--sync=%d", sync, 0),
	TSPFS_OPT("--filter=%s", filter, 0),
	TSPFS_OPT("--async", async, 0),
	TSPFS_OPT("--ext=%s", ext, 0),
	FUSE_OPT_END
};

#define PROGNAME "tspfs"

static void usage()
{
	printf("usage: %s [--url=<tsp_provider_url>] [--sync=<sampling period>]|[--async] [--filter] <mount_point>\n\
\t\t--url\t\t\tthe url to connect to, default: rpc://localhost\n\
\t\t--sync=<period>\t\tuse synchronous mode to read the sample values with a period <period>\n\
\t\t--async\t\t\tuse asynchronous mode to read the sample values, this is the default\n\
\t\t--filter\t\tbasic filter used to match the symbol names\n\
\t\t--ext=<ext>\t\tSuffix file names with <ext>, handy for windows browsing\n",
	       PROGNAME);
}

static int tspfs_opt_proc(void *data, const char *arg, int key,
			  struct fuse_args *outargs)
{
	if(!strcmp(arg, "--help") || !strcmp(arg, "--help") || !strcmp(arg, "--help")) {
		usage();
		return 0;
	}
	return 1;
}


int main(int argc, char *argv[])
{
	int i, _argc, ret;
	char **_argv;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	if (argc < 2) {
		usage();
		exit(-1);
	}
	_argc = argc;
	_argv = malloc(sizeof(char *) * _argc);
	i = 0;
	while (i < argc) {
		_argv[i] = strdup(argv[i]);
		i++;
	}

	tspfs.sync = 0;
	tspfs.async = 0;
	tspfs.url = "rpc://localhost";
	tspfs.filter = NULL;

	if (fuse_opt_parse(&args, &tspfs, tspfs_opts, tspfs_opt_proc) == -1)
		exit(1);

	if (tspfs.sync && tspfs.async) {
		printf("--sync and --async are mutually exclusive. Exiting.\n");
		usage();
		exit(1);
	}
	if (tspfs_init_connect(_argc, _argv, tspfs.url) < 0) {
		printf("Could not init connection to %s. Exiting.\n", tspfs.url);
		exit(1);
	}

	if (tspfs.ext)
		tspfs.ext_len = strlen(tspfs.ext);

	ret =  fuse_main(args.argc, args.argv, &tspfs_oper);
	return ret;
}
