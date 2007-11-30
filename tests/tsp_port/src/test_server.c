// test.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "test_api.h"
#include "test_rpc_server.h"

int main(int argc, char* argv[])
{
    init_affichage(printf);
    init_saisie(scanf);
    
    /* server main */
    rpc_svc_main(argc, argv);

	return 0;
}
