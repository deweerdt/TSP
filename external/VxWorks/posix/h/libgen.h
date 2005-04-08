#ifndef _SYS_LIBGEN_H
#define _SYS_LIBGEN_H

int getuid(void);
int getpid(void);
char * basename(char * path);


typedef uint32_t rpcprog_t;
typedef uint32_t rpcvers_t;
typedef uint32_t rpcprot_t;

ushort getrpcport(char *host, rpcprog_t  prognum,  rpcvers_t versnum, rpcprot_t proto);

#endif /* _SYS_LIBGEN_H */


