/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_stream_receiver.c,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implementation for the functions that receive the data
stream  from the producer for the asked symbols. This layer is the network layer and uses sockets
-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

#include "tsp_stream_receiver.h"

struct TSP_socket_t
{
  int socketId;
};

typedef struct TSP_socket_t TSP_socket_t;

TSP_stream_receiver_t TSP_stream_receiver_create(const  char* data_address)
{
  SFUNC_NAME(TSP_stream_receiver_create);

  int status = 0;
  struct hostent* Host_p = NULL;
  int OptInt = 0;
  int ret = TRUE;
  int Len = 0;
  char* host;
  char* str_port;
  unsigned short port;
  char* last; /*For strtok_r*/
  
  /* FIXME : faire la desallocation */
  /* FIXME : si la socket ne peut pas etre connecte il y a une fuite de memoire*/
  TSP_socket_t* sock = (TSP_socket_t*)calloc(1, sizeof(TSP_socket_t));
  TSP_CHECK_ALLOC(sock, 0);

  
  /* On recupere l'adresse */
  host = strtok_r((char* )data_address, ":", &last);
  str_port = strtok_r(NULL, ":", &last);
  port = (unsigned short)atoi(str_port);
  
  STRACE_DEBUG(("Connection Data : Host='%s' port=%u", host, (unsigned int)port));

  /* Init socket */
  sock->socketId = socket(AF_INET, SOCK_STREAM, 0);

  if (sock->socketId > 0)
    {
      OptInt = TSP_DATA_STREAM_SOCKET_BUFFER_SIZE / 2;
      status = setsockopt(sock->socketId, SOL_SOCKET, SO_RCVBUF, (void * )&OptInt, sizeof(OptInt));
      if (status == -1)
	{
	  STRACE_ERROR(("Probleme with set socket size"));

	  close(sock->socketId);
	  return 0;
	}

      /* Local address reuse */

      OptInt = 1;
      status = setsockopt(sock->socketId, SOL_SOCKET, SO_REUSEADDR,
			  (void *) &OptInt, sizeof(OptInt));
      if (status == -1)
	{
          STRACE_ERROR(("pb set local address reuse"));
	  close(sock->socketId);
	  return 0;
	}

      /* No periodic connection state control */

      OptInt = 0;
      status = setsockopt(sock->socketId, SOL_SOCKET, SO_KEEPALIVE,
			  (void *) &OptInt, sizeof(OptInt));
      if (status == -1)
	{
	  STRACE_ERROR(("pb set periodic state control"));

	  close(sock->socketId);
	  return 0;
	}

      /* TCP No delay */
      OptInt = 1;
      status = setsockopt(sock->socketId, IPPROTO_TCP, TCP_NODELAY, (void *) &OptInt,
			  sizeof(OptInt));
      if (status == -1)
	{
	  STRACE_ERROR(("pb set TCP no delay"));

	  close(sock->socketId);
	  return 0;
	}

      /* Bind to socket : */
      /* Get host from the database */
      Host_p = gethostbyname(host);
      if (Host_p == (struct hostent *) NULL)
	{
	  STRACE_ERROR(("pb get host by name"));

	  close(sock->socketId);
	  return 0;
	}

      {
#ifndef __OSF1__
	long InAddr;
#else
	in_addr_t InAddr;
#endif /* __OSF1__ */
	struct sockaddr_in readAddr;

	bcopy((char *) Host_p->h_addr, (char *) &InAddr, Host_p->h_length);
	InAddr = ntohl(InAddr);

	bzero((char *) &readAddr, sizeof(struct sockaddr_in));
	readAddr.sin_family = AF_INET;
#ifdef _SOCKADDR_LEN
	readAddr.sin_len = sizeof(ListenAddr);
#endif
	readAddr.sin_addr.s_addr = htonl(InAddr);
	readAddr.sin_port = htons(port);
    
	/* connect to server */
	status = connect(sock->socketId, (struct sockaddr*)&readAddr, sizeof(readAddr));
      }
    
      if (status == -1)
	{
	  STRACE_ERROR(("pb connecting to socket"));
	  close(sock->socketId);
	  sock->socketId = 0;
	  return 0;
	}

    }

    
  return sock;
}

int TSP_stream_receiver_receive(TSP_stream_receiver_t receiver, char *buffer, int bufferLen)
{

  SFUNC_NAME(TSP_stream_receiver_create);

  int nread;
  int Total;
  int identSocket  = ((TSP_socket_t*)receiver)->socketId;
  
  Total = 0;
  
  if(identSocket > 0)
    {
      while ( bufferLen > 0)
	{
	  if ( (nread = read(identSocket, &buffer[Total], bufferLen)) < 0)
	    {
	      if( errno == EINTR )
		{
		  /* The read might have been interrupted by a signal */
		  nread = 0;
		}
	      else 
		{
		  
		  STRACE_ERROR(("send failed"));
		  return FALSE;
		}
	    }else if (nread == 0)
	      {
		STRACE_ERROR(("Received EOF"));
		return FALSE;
	      }
	  
	  Total += nread;
	  bufferLen -= nread;
      	}
    }
  return TRUE;
}
