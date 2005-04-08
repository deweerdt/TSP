
#ifndef RPC_SAMPLE_H
#define RPC_SAMPLE_H

#include "rpc_parse.h"

void add_sample_msg();
void write_sample_svc(definition *def);
int write_sample_clnt(definition *def);
void write_sample_clnt_main();

#endif
