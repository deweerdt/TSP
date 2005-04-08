
#ifndef RPC_CLNTOUT_H
#define RPC_CLNTOUT_H

#include "rpc_parse.h"

/*
 * rpc_clntout routines
 */
void write_stubs();
void printarglist(proc_list *proc, char* addargname, char* addargtype);

#endif
