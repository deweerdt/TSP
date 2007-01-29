#include <stdio.h>
#include <rpc/rpc.h>

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

#include "test_api.h"
#include "test_port_rpc.h"
#include "test_rpc_client.h"

/* Default timeout can be changed using clnt_control() */
/*static struct timeval TIMEOUT = { 25, 0 }; */

void main_rpc(const char* provided_host)
{
    CLIENT *cl;
    int i;
    TSP_port_enum deb, elem;
    TSP_port_struct val;
    TSP_port_struct *redval;
    char* host = "localhost";
    char c;
    int *res;
    
    if (NULL!=provided_host) {
      host = strdup(provided_host);
    } else {
      test_printf(" 0 client mirepoix,\n");
      test_printf(" 1 client frtldd04365721,\n");
      test_printf(" autre client local : ");
      test_scan_1_car(&c);
      
      if ('0' == c)
	{
	  host = strdup("mirepoix");
	}
      if ('1' == c)
	{
	  host = strdup("frtldd04365721");
	}
    }

    do {
	
        cl = clnt_create(host, TSP_PORT, TSP_PORT_VERS, "tcp");
        
        deb = TSP_port_e0;

        val.i = 10;
        val.a = 10.0;
        val.status_str = "0";

        if (cl == NULL) {
	        clnt_pcreateerror(host);
	        exit (1);
	        }
        for (i = 0; i < 10; i++) {
	        elem = *get_next_1(&deb, cl);
            test_printf("New value enum: %d\n", elem);
            deb = elem;
            res = set_1(&val, cl);
            if (NULL == res)
            {
                test_printf("erreur set retourne NULL\n");
            }
            else
            {
                test_printf("Set retourne %d\n", *res);
            }        
	        redval = get_1(&val, cl);
            if (NULL == redval)
            {
                test_printf("get_1 valeur null\n");
            }
            else
            {
                val = *redval;
                printstruct("New value struct : ", &val);
            }        
        }
        
        printstruct("Set value struct : ", &val);
        res = set_1(&val, cl);
        if (NULL == res)
        {
            test_printf("erreur set retourne NULL\n");
        }
        else
        {
            printstruct("Final value struct : ", &val);
        }

        clnt_destroy(cl);

        test_printf("f pour sortir : ");
        test_scan_1_car(&c);
    }while('f' != c);
}

#ifndef WIN32
#define WSACleanup() do {} while(0)
#define closesocket(x) close(x)
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#endif

int client_socket(const char* provided_host)
{
#ifdef WIN32
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
#else
    int ConnectSocket = 0;
#endif
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = "this is a test";
    char *host = "localhost";
    char recvbuf[DEFAULT_BUFLEN];
    int iResult,
        recvbuflen = DEFAULT_BUFLEN,
        offset;
    char c;
    
    if (NULL!=provided_host) {
      host = strdup(provided_host);
    } else {
      memset(&recvbuf[0], '\0', DEFAULT_BUFLEN);
      
      test_printf(" 0 client mirepoix,\n");
      test_printf(" 1 client frtldd04365721,\n");
      test_printf(" autre client local : ");
      test_scan_1_car(&c);
    
      if ('0' == c)
	{
	  host = strdup("mirepoix");
	}
      if ('1' == c)
	{
	  host = strdup("frtldd04365721");
	}
    }

    // Initialize Winsock
#ifdef WIN32
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        test_printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
#endif

    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(host, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        test_printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
#ifdef WIN32
            test_printf("Error at socket(): %ld\n", WSAGetLastError());
#else
            perror("Error at socket()");
#endif
            freeaddrinfo(result);
            WSACleanup();
            return  1;
        }
        if (ConnectSocket < 0) {
            perror("Error at socket()");
            freeaddrinfo(result);
            return  1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult < 0) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        test_printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
#ifdef WIN32
        test_printf("send failed: %d\n", WSAGetLastError());
#endif
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    test_printf("Bytes <%s> Sent: %ld\n", sendbuf, iResult);

    // shutdown the connection since no more data will be sent
#ifdef WIN32
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        test_printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
#endif

    // Receive until the peer closes the connection
    offset = 0;
    do {
        iResult = recv(ConnectSocket, &recvbuf[offset], recvbuflen-offset, 0);        
        if ( iResult > 0 )
        {
            test_printf("Bytes <%s> received: %d\n", &recvbuf[offset], iResult);
            offset += iResult;
            if (0 == teststringrecu(recvbuf))
            {
                test_printf ("reception finished : %s\n", recvbuf);
            }
        }
        else if ( iResult == 0 )
        {
            test_printf("Connection closed\n");
        }
        else
        {
#ifdef WIN32
            test_printf("recv failed: %d\n", WSAGetLastError());
#else
            perror("recv");
#endif
        }

    } while( iResult > 0 );
    
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}

void rpc_clnt_main(int argc, char* argv[])
{
    /* Main options handling */
    /*  char*   error_string;*/
    int     opt_ok  = 1;
    char    c_opt;
    int    help     = 0;
    int    test_api = 0;
    int    test_rpc = 0;
    int    test_sock = 0;
    char*  provided_host = NULL;

#ifdef WIN32
    rpc_nt_init();
#endif
    
    /* Analyse command line parameters */
    while (opt_ok && (EOF != (c_opt = getopt(argc,argv,"ht:u:")))) {    
        switch (c_opt) {
	case 'u':
	  provided_host = strdup(optarg);
	  printf("Using provided host = <%s>\n",provided_host); 
	  break;
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
        fprintf(stderr,"    -t api|socket|rpc launch corresponding test\n");
        fprintf(stderr,"    -u host (optional)\n");
    }
    else
    {
        if (help) {
            fprintf(stderr,"Command line option(s)\n");
            fprintf(stderr,"    -h help\n");
            fprintf(stderr,"    -t api|socket|rpc launch corresponding test\n");
	    fprintf(stderr,"    -u host (optional)\n");
        }
        if (test_api) {
            api_main();
        }        
        if (test_rpc) {
            main_rpc(provided_host);
        }
        if (test_sock) {
            client_socket(provided_host);
        }
    }

#ifdef WIN32
    rpc_nt_exit();
#endif

    exit(0);
}
