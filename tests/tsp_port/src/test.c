// test.c : Defines the entry point for the console application.
//

#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "test_api.h"
#include "test_rpc_client.h"

int main(int argc, char* argv[])
{
    init_affichage(&printf);
    init_saisie(&scanf);

    /* client main */
    rpc_clnt_main(argc, argv);

    return 0;
}
