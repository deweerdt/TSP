#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#include <errno.h>


/* added for compilation compatibility */

typedef int		key_t;

/* Common IPC Definitions. */
/* Mode bits. */
#define IPC_ALLOC       0100000         /* entry currently allocated */
#define IPC_CREAT       0001000         /* create entry if key doesn't exist */
#define IPC_EXCL        0002000         /* fail if key exists */
#define IPC_NOWAIT      0004000         /* error if request must wait */

/* Keys. */
#define IPC_PRIVATE     (key_t)0        /* private key */

/* Control Commands. */

#define IPC_RMID        10      /* remove identifier */
#define IPC_SET         11      /* set options */
#define IPC_STAT        12      /* get options */



#define EIDRM		(ERRMAX+1)


#endif /* _SYS_IPC_H */
