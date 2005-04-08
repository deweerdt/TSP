
#ifndef RPC_HOUT_H
#define RPC_HOUT_H

#include "rpc_parse.h"

void pdeclaration(char *name, declaration *dec, int tab, char *separatorBeforeComment, char* separatorAfterComment);
void print_datadef(definition *def);
void print_funcdef(definition *def);

#endif
