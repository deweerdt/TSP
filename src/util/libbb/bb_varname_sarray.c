/*

$Header: /home/def/zae/tsp/tsp/src/util/libbb/bb_varname_sarray.c,v 1.1 2007-02-25 10:13:12 deweerdt Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2007 Frederik Deweerdt

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Blackboard get/set varname primitives that store the var
            name into a static array

-----------------------------------------------------------------------
 */

#include "bb_core.h"
#include "bb_simple.h"
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>


#define MAX_NAME_LENGTH 512
#define MAX_NAMES 512

struct name_tab {
	char names[MAX_NAMES][MAX_NAME_LENGTH];
	int current_index;
};

static struct name_tab *names;

int32_t bb_varname_destroy_sarray(S_BB_T *bb)
{
	shmdt(names);
	return 0;
}

static struct name_tab *names;
int32_t bb_varname_init_sarray(S_BB_T *bb)
{
	char shmname[512];
	int fd_shm;

	sprintf(shmname, "%s_%s", bb->name, "sarray");
	fd_shm = shmget(bb_utils_ntok(shmname), sizeof(struct name_tab), IPC_CREAT | IPC_EXCL | BB_SHM_ACCESS_RIGHT);
	if (fd_shm < 0) {
		fd_shm = shmget(bb_utils_ntok(shmname), sizeof(struct name_tab), BB_SHM_ACCESS_RIGHT);
		if (fd_shm < 0) {
			return BB_NOK;
		}
	}
	names = shmat(fd_shm, NULL, 0);
	if (!names)
		return BB_NOK;

	return BB_OK;
}

char *bb_get_varname_sarray(const S_BB_DATADESC_T *dd)
{
	unsigned int index = *(unsigned int *)dd->__name;
	char *ret;
	ret = strdup(names->names[index]);
	return ret;
}

int32_t bb_set_varname_sarray(S_BB_DATADESC_T *dd, const char *key)
{
	if (names->current_index > MAX_NAMES || strlen(key) > MAX_NAME_LENGTH)
		return BB_NOK;
	strncpy(names->names[names->current_index], key, MAX_NAME_LENGTH);
	*(unsigned int *)dd->__name = names->current_index;
	names->current_index++;	

	return BB_OK;
}
