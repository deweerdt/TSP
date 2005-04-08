
#ifndef RPC_SVCOUT_H
#define RPC_SVCOUT_H

#include "rpc_parse.h"

int nullproc(proc_list *proc);
void write_most(char *infile, int netflag, int nomain);
void write_rest();
void write_programs(char *storage);
void write_inetd_register(char *transp);
void write_netid_register(char *transp);
void write_nettype_register(char *transp);

#endif
