/*
 * tspfs: a usermode filesystem that uses fuse
 * (see http://fuse.sourceforge.net) to export TSP
 * symbols as files in a directory.
 *
 * (c) Frederik Deweerdt, Eric Noulard 2006
 *
 *
 * ==========================
 * == Sample tspfs session ==
 * ==========================
 *
 * # Start a sample bb
 * $ bb_simu -s > /dev/null 2>&1 &
 *
 * # TSPize the bb's data
 * $ bb_tsp_provide bb_simu 10 &
 *
 * # Create the mount point, mount the tspfs filesystem
 * $ mkdir mp/
 * $ tspfs --url=rpc://localhost/bb_simu --sync=1 mp/
 *
 * # List the symbols
 * $ ls -l mp/
 * total 0
 * -r--r--r--  1 root root 512 jan  1  1970 bb_simu_1_HugeArray[0]
 * [...]
 * -r--r--r--  1 root root 512 jan  1  1970 ECLA_0_d_ecl_lune
 * -r--r--r--  1 root root 512 jan  1  1970 ECLA_0_d_ecl_sol
 * -r--r--r--  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[0]
 * -r--r--r--  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[1]
 * -r--r--r--  1 root root 512 jan  1  1970 ORBT_0_d_possat_m[2]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirLun[0]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirLun[1]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirLun[2]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirSol[0]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirSol[1]
 * -r--r--r--  1 root root 512 jan  1  1970 POSA_0_d_DirSol[2]
 * -r--r--r--  1 root root 512 jan  1  1970 Sequenceur_0_d_t_s
 *
 * # Display a particular value
 * $ cat mp/Sequenceur_0_d_t_s
 * time=197600 value=3961.280000
 * $ cat mp/Sequenceur_0_d_t_s
 * time=197680 value=3962.880000
 *
 * # Change the display format (notice we use \012, not \n, this needs to be fixed)
 * $ setfattr -n format -v 't=%d v=%e\012' ECLA_0_d_ecl_sol
 * $ cat ECLA_0_d_ecl_sol
 * t=208880 v=1.000000e+00
 *
 * # Get the format from a symbol
 * $ getfattr -n format bb_simu_int8\[0\]
 * # file: bb_simu_int8[0]
 * format="time=%d value=%f\012"
 *
 * # Go wild, umount the filesystem
 * $ fusermount -u mp/
 *
 *
 * This program is distributed under the terms of the GNU LGPL.
 * See COPYING.
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


#include "tsp_sys_headers.h"
#include "tsp_prjcfg.h"
#include "tsp_consumer.h"

#define MAX_SYM_DISP_SIZE 512
#define MAX_FORMAT_SIZE 512
#define MAX_URL_SIZE 512


static inline int is_tsp_symbol(const char *path);

static int tspfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if (is_tsp_symbol((char *) &path[1])) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = MAX_SYM_DISP_SIZE;
	} else
		res = -ENOENT;

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
	char *url;
	char *filter;
	int sync;
	int async;
	const TSP_consumer_information_t *information;
	TSP_consumer_symbol_requested_list_t symbols;
	TSP_provider_t provider;
	TSP_sample_t **samples;
	char **formats;
	unsigned int nr_samples;
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

static inline struct tspfs_sample *get_tsp_symbol_value(int index)
{
	struct tspfs_sample *s;
	assert(index >= 0 && index < tspfs.nr_samples);

	s = (struct tspfs_sample *)malloc(sizeof(struct tspfs_sample));
	if (tspfs.sync) {
		s->sync = *tspfs.samples[index]; 
	} else {
		int ret;
		/* 
		 * Beware that PGI is not the tspfs local 'index' since
		 *      - filtered info may gives you non-contiguous PGI
		 */
  		s->async.provider_global_index = tspfs.information->symbols.val[index].index; 
    		s->async.value_ptr  = &s->value.d;
      		s->async.value_size = sizeof(s->value.d);
		ret = TSP_consumer_request_async_sample_read(tspfs.provider,&(s->async));	
		printf("ret was %d\n", ret);
	}
	return s;
}
static int get_tsp_symbol_index(const char *symbol_name)
{
	int i;
	/* Compare symbols names */
	for (i = 0; i < tspfs.information->symbols.len; i++) {
		if (!strcmp (symbol_name,
		     tspfs.information->symbols.val[i].name)) {
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

	if (tspfs.sync && !TSP_consumer_request_sample_destroy(tspfs.provider)) {
		STRACE_ERROR(("Function TSP_consumer_request_sample_destroy failed"));
	}

	if (!TSP_consumer_request_close(tspfs.provider)) {
		STRACE_ERROR(("Function TSP_consumer_request_close failed"));
	}

	TSP_consumer_disconnect_one(tspfs.provider);

	TSP_consumer_end();
}



static void *data_collector(void *unused)
{
	int i = 0;
	int usleep_length = 1e6 / tspfs.sync;

	TSP_consumer_request_sample(tspfs.provider, &tspfs.symbols);
	TSP_consumer_request_sample_init(tspfs.provider, 0, 0);

	while (1) {
		int new_sample;
		int ret;

		ret =
		    TSP_consumer_read_sample(tspfs.provider,
					     tspfs.samples[i],
					     &new_sample);
		if (!ret) {
			printf
			    ("Error while reading data from provider, exiting.\n");
			return NULL;
		}
		i = (i + 1) % tspfs.nr_samples;
		if (i == 0)
			usleep(usleep_length);
	}
	return NULL;
}


static int tspfs_init_connect(int argc, char **argv, char *url)
{

	int i;

	TSP_consumer_init(&argc, &argv);
	tspfs.provider = TSP_consumer_connect_url(url);
	if (tspfs.provider == NULL) {
		printf("Could not connect to provider %s\n", url);
		exit(-1);
	}
	TSP_consumer_request_open(tspfs.provider, 0, 0);

	TSP_consumer_request_information(tspfs.provider);

	if (tspfs.filter) {
		TSP_consumer_request_filtered_information(tspfs.provider, TSP_FILTER_SIMPLE, tspfs.filter);
	}

	tspfs.information = TSP_consumer_get_information(tspfs.provider);

	tspfs.symbols.len = tspfs.information->symbols.len;
	tspfs.symbols.val =
	    (TSP_consumer_symbol_requested_t *) calloc(tspfs.symbols.len,
						      sizeof(TSP_consumer_symbol_requested_t));
	if (!tspfs.symbols.val)
		return -1;

	for (i = 0; i < tspfs.symbols.len; i++) {
		tspfs.symbols.val[i].name =
		    tspfs.information->symbols.val[i].name;
		tspfs.symbols.val[i].period = tspfs.sync;
		tspfs.symbols.val[i].phase = 0;
	}

	tspfs.nr_samples = tspfs.information->symbols.len;
	tspfs.samples =
	    (TSP_sample_t **) malloc(sizeof(TSP_sample_t *) *
				     tspfs.nr_samples);
	if (!tspfs.samples) {
		free(tspfs.symbols.val);
		return -1;
	}
	for (i = 0; i < tspfs.nr_samples; i++) {
		tspfs.samples[i] = malloc(sizeof(TSP_sample_t));
		if (!tspfs.samples[i]) {
			free(tspfs.symbols.val);
			free(tspfs.samples);
			return -1;
		}
	}

	tspfs.formats =
	    (char **) malloc(sizeof(char *) * tspfs.nr_samples);

	if (!tspfs.formats) {
		free(tspfs.symbols.val);
		free(tspfs.samples);
		return -1;
	}

	for (i = 0; i < tspfs.nr_samples; i++) {
		tspfs.formats[i] = NULL;
	}

	return 1;
}

static void *tspfs_init(void)
{
	int err;
	pthread_t pthread_data_collector;
	sigset_t oldset;
	sigset_t newset;

	/* We only lauch the data_collector thread in sync mode */
	if (!tspfs.sync)
		return NULL;

	sigemptyset(&newset);
	sigaddset(&newset, SIGINT);
	sigaddset(&newset, SIGHUP);
	sigaddset(&newset, SIGQUIT);
	pthread_sigmask(SIG_BLOCK, &newset, &oldset);
	err = pthread_create(&pthread_data_collector, NULL, data_collector, NULL);
	if (err) {
		fprintf(stderr, "failed to create thread: %s\n",
		strerror(err));
		return NULL;
	}
	pthread_detach(pthread_data_collector);
	pthread_sigmask(SIG_SETMASK, &oldset, NULL);

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

	for (i = 0; i < tspfs.information->symbols.len; i++) {
		filler(buf, tspfs.information->symbols.val[i].name, NULL,
		       0);
	}

	return 0;
}

static int tspfs_open(const char *path, struct fuse_file_info *fi)
{
	if (!is_tsp_symbol((char *)(path + sizeof(char))))
		return -ENOENT;

	return 0;
}
static int tspfs_write(const char *path, const char *buf, size_t size,
		       off_t offset, struct fuse_file_info *fi)
{
	double value;
	struct tspfs_sample s;
	const char *sym_name = (path + sizeof(char));
	int idx;
	 
	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0)
		return -ENOENT;

	value = strtod(buf, NULL);
	/* idx != PGI */
  	s.async.provider_global_index = tspfs.information->symbols.val[idx].index; 
    	s.async.value_ptr  = &value;
      	s.async.value_size = sizeof(value);
	TSP_consumer_request_async_sample_write(tspfs.provider,&s.async);	
	
	return size;
}
static int tspfs_read(const char *path, char *buf, size_t size,
		      off_t offset, struct fuse_file_info *fi)
{
	size_t len;
	int idx;
	struct tspfs_sample *sample;
	const char *sym_name = (path + sizeof(char));
	char sym_display[MAX_SYM_DISP_SIZE];

	if (!is_tsp_symbol(sym_name))
		return -ENOENT;

	idx = get_tsp_symbol_index(sym_name);
	sample = get_tsp_symbol_value(idx);
	if (tspfs.sync) {
		sprintf(sym_display, get_sym_format(idx), sample->sync.time,
			sample->sync.user_value);
	} else {
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

	free(sample); 
	return size;
}

static int tspfs_setxattr(const char *path, const char *name,
			  const char *value, size_t size, int flags)
{
	int idx;
	const char *sym_name = (path + sizeof(char));

	if (strcmp(name, "format"))
		return -ENOATTR;

	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0)
		return -ENOENT;

	if (size > MAX_FORMAT_SIZE)
		return -ERANGE;

	if (flags & XATTR_REPLACE) {
		if (tspfs.formats[idx] == NULL) {
			return -ENOATTR;
		}
	}
	if (flags & XATTR_CREATE) {
		if (tspfs.formats[idx] != NULL) {
			return -EEXIST;
		}
	}
	set_sym_format(idx, value);
	return 0;
}

static int tspfs_getxattr(const char *path, const char *name, char *value,
			  size_t size)
{
	int idx;
	char *format;
	int format_len;
	const char *sym_name = (path + sizeof(char));

	if (strcmp(name, "format"))
		return -ENOATTR;

	idx = get_tsp_symbol_index(sym_name);
	if (idx < 0)
		return -ENOENT;

	format = get_sym_format(idx);
	format_len = strlen(format);
	/* the user just wants to know the symbol's size */
	if (size == 0)
		return format_len;

	if (size < format_len)
		return -ERANGE;

	strncpy(value, format, format_len);
	return format_len;
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
	FUSE_OPT_END
};

#define PROGNAME "tspfs"

static void usage()
{
	printf("usage: %s [--url=<tsp_provider_url>] [--sync=<sampling period>]|[--async] [--filter] <mount_point>\n\
\t\t--url\t\t\tthe url to connect to, default: rpc://localhost\n\
\t\t--sync=<period>\t\tuse synchronous mode to read the sample values with a period <period>\n\
\t\t--async\t\t\tuse asynchronous mode to read the sample values, this is the default\n\
\t\t--filter\t\tbasic filter used to match the symbol names\n",
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

	ret =  fuse_main(args.argc, args.argv, &tspfs_oper);
	return ret;
}
