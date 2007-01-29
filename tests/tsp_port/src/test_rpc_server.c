#include <stdio.h>
#include <rpc/rpc.h>
#include <pthread.h>
#ifdef WIN32
    #include <ws2tcpip.h>
    #include "getopt.h" 
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/socket.h>  
    #include <netdb.h>
#if defined(sun) || defined(__sun)
    #include <strings.h>
    #include <stdlib.h>
#endif
#endif
#include "test_port_rpc.h"
#include "test_api.h"
#include "test_rpc_server.h"

static int count = 1;
static TSP_port_enum port_enum = TSP_port_e0;
static TSP_port_struct port_struct = {0,0.0,NULL};

TSP_port_enum   *get_next_1_svc(TSP_port_enum * val, struct svc_req *rqstp)
{
    switch (*val)
    {
    case TSP_port_e0 :
        port_enum = TSP_port_e1;
        break;
    case TSP_port_e1 :
        port_enum = TSP_port_e2;
        break;
    case TSP_port_e2 :
        port_enum = TSP_port_e3;
        break;
    case TSP_port_e3 :
        port_enum = TSP_port_e4;
        break;
    case TSP_port_e4 :
        port_enum = TSP_port_e1;
        break;
    }
    test_printf("get next called; result: %d\n", port_enum);
    return &port_enum;
}

TSP_port_struct *
get_1_svc(void *argp, struct svc_req *rqstp)
{
    TSP_port_struct * val = (TSP_port_struct *)argp;
    test_printf("value in struct: %d %f %s\n", val->i, val->a, val->status_str);
    port_struct.i = val->i + 1;
    port_struct.a = val->a + 1.0;
    if ( NULL == val->status_str )
    {
        if (NULL != port_struct.status_str)
        {
            free(port_struct.status_str);
        }
        port_struct.status_str = strdup("a");
    }
    else
    {
        if (NULL == port_struct.status_str)
        {
            port_struct.status_str = strdup("g");    
        }
        else
        {
            port_struct.status_str[0] = val->status_str[0] + 1;
        }
    }
    printstruct("Get value struct : ", &port_struct);
    return &port_struct;
}
int *
set_1_svc(TSP_port_struct *val, struct svc_req *rqstp)
{
    port_struct.i = val->i;
    port_struct.a = val->a;
    if (port_struct.status_str != NULL)
    {
        free(port_struct.status_str);
    }
    if ( NULL == val->status_str )
    {
        port_struct.status_str = strdup("s"); 
    }
    else
    {
        port_struct.status_str = strdup(val->status_str);
    }
    printstruct("Set value struct : ", &port_struct);
    return &count;
}

void main_rpc()
{
    main();
    exit(0);
}

#ifndef WIN32
    #define WSACleanup()
    #define closesocket(x) close(x)
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    typedef int SOCKET;
#endif

int server_socket()
{
#ifdef WIN32
    WSADATA wsaData;
#endif
    SOCKET ListenSocket = INVALID_SOCKET,
           ClientSocket = INVALID_SOCKET;
    
    struct addrinfo *result = NULL,
                    hints;
    char recvbuf[DEFAULT_BUFLEN];
    int  iResult,
         recvbuflen = DEFAULT_BUFLEN,
        offset;
    
    memset(&recvbuf[0], '\0', DEFAULT_BUFLEN);

#if WIN32
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        test_printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        test_printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
#ifdef WIN32
        test_printf("socket failed: %ld\n", WSAGetLastError());
#else
	perror("socket");
#endif
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
#ifdef WIN32
        test_printf("bind failed: %d\n", WSAGetLastError());
#else
	perror("bind");
#endif
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
#ifdef WIN32
        test_printf("listen failed: %d\n", WSAGetLastError());
#else
	perror("listen");
#endif
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
#ifdef WIN32
        test_printf("accept failed: %d\n", WSAGetLastError());
#else
	perror("accept");
#endif
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket
    closesocket(ListenSocket);
    
    // Receive until the peer shuts down the connection
    offset = 0;
    do {
        iResult = recv(ClientSocket, &recvbuf[offset], recvbuflen, 0);
        if (iResult > 0)
        {
            test_printf("Bytes <%s> received: %d\n", &recvbuf[offset], iResult);
            offset += iResult;
            if (0 == teststringrecu(recvbuf))
            {
                test_printf ("reception finished : %s\n", recvbuf);
            }
        }
        else if (iResult == 0)
        {
            test_printf("Connection closing...\n");
        }
        else  
        {
#ifdef WIN32
            test_printf("recv failed: %d\n", WSAGetLastError());
#else
	    perror("recv");
#endif
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }

    } while (iResult > 0);

    // Echo the buffer back to the sender
    iResult = send( ClientSocket, recvbuf, strlen(recvbuf), 0 );
    if (iResult == SOCKET_ERROR) {
#ifdef WIN32
        test_printf("send failed: %d\n", WSAGetLastError());
#else
	perror("send");
#endif

        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    test_printf("Bytes <%s> Sent: %ld\n", recvbuf, iResult);
    
#ifdef WIN32
    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        test_printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
#endif

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}

void rpc_svc_main(int argc, char* argv[])
{
    /* Main options handling */
    /*  char*   error_string;*/
    int     opt_ok  = 1;
    char    c_opt;
    int    help     = 0;
    int    test_api = 0;
    int    test_rpc = 0;
    int    test_sock = 0;
    
#ifdef WIN32
    rpc_nt_init();
#endif

    /* Analyse command line parameters */
    while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"ht:")))) {    
        switch (c_opt) {
            case 'h':
                opt_ok++;
                help = 1;                
            break;
            case 't':
                opt_ok+=2;
                if (0 == strcmp(optarg, "api")) {
                    test_api = 1;
                }
                else if (0 == strcmp(optarg, "socket")) {
                    test_sock = 1;
                }
                else if (0 == strcmp(optarg, "rpc")) {
                    test_rpc = 1;
                }
                else {
                    fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
                    opt_ok = 0;
                }
            break;
            case '?':
                fprintf(stderr,"Invalid command line option(s), correct it and rerun\n");
                opt_ok = 0;
            break;
            default:
                opt_ok = 0;
            break;
        } /* end of switch */
    } /* end of while */
    
    /* check if global options are OK */
    if (opt_ok < 2) {
        fprintf(stderr,"    -h help\n");
        fprintf(stderr,"    -t [api][socket][rpc] launch corresponding test\n");
    }
    else
    {
        if (help) {
            fprintf(stderr,"Command line option(s)\n");
            fprintf(stderr,"    -h help\n");
            fprintf(stderr,"    -t [api][socket][rpc] launch corresponding test\n");
        }
        if (test_api) {
            api_main();
        }        
        if (test_rpc) {
            main_rpc();
        }
        if (test_sock) {
            server_socket();
        }
    }

#ifdef WIN32
    rpc_nt_exit();
#endif
}
