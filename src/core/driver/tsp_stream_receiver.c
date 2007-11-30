/*

$Header: /home/def/zae/tsp/tsp/src/core/driver/tsp_stream_receiver.c,v 1.14 2007-11-30 15:42:00 erk Exp $

-----------------------------------------------------------------------

TSP Library - core components for a generic Transport Sampling Protocol.

Copyright (c) 2002 Yves DUFRENNE, Stephane GALLES, Eric NOULARD and Robert PAGNOT 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

-----------------------------------------------------------------------

Project   : TSP
Maintainer : tsp@astrium-space.com
Component : Consumer

-----------------------------------------------------------------------

Purpose   : Implementation for the functions that receive the data
stream  from the producer for the asked symbols. This layer is the network layer and uses sockets
-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"

#if defined (_WIN32)
#include <Windows.h>
#include <WinSock2.h>
#else
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#endif

#ifndef SHUT_RDWR
#define SHUT_RDWR 2
#endif

#include "tsp_stream_receiver.h"

struct TSP_socket_t
{
  int socketId;

  /** TRUE if TSP_stream_receiver_is_stopped was called */
  int is_stopped;
};

typedef struct TSP_socket_t TSP_socket_t;

TSP_stream_receiver_t TSP_stream_receiver_create(const  char* data_address)
{

  int status = 0;
  struct hostent* Host_p = NULL;
  int OptInt = 0;
/*   int ret = TRUE; */
/*   int Len = 0; */
  char* host;
  char* str_port;
  unsigned short port;
  char* last; /**<For strtok_r*/
  
  TSP_socket_t* sock = (TSP_socket_t*)calloc(1, sizeof(TSP_socket_t));
  sock->is_stopped = FALSE;
  TSP_CHECK_ALLOC(sock, 0);

  
  /* Decode the data adresse (get the host name and the port) */
  host = strtok_r((char* )data_address, ":", &last);
  str_port = strtok_r(NULL, ":", &last);
  port = (unsigned short)atoi(str_port);
  
  STRACE_DEBUG(("Connection Data : Host='%s' port=%u", host, (unsigned int)port));

  /* Init socket */
  sock->socketId = socket(AF_INET, SOCK_STREAM, 0);

  if (sock->socketId > 0)
    {
      /* size of receiver fifo should be half the size of sender fifo for perfomance reason */
      OptInt = TSP_DATA_STREAM_SOCKET_FIFO_SIZE / 2;
      status = setsockopt(sock->socketId, SOL_SOCKET, SO_RCVBUF, (void * )&OptInt, sizeof(OptInt));
      if (status == -1)
	{
	  STRACE_ERROR(("Probleme with set socket size"));

	  close(sock->socketId);
	  free(sock);
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
	  free(sock);
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
	  free(sock);
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
	  free(sock);
	  return 0;
	}

      /* Bind to socket : */
      /* Get host from the database */
      Host_p = gethostbyname(host);
      if (Host_p == (struct hostent *) NULL)
	{
	  STRACE_ERROR(("pb get host by name"));

	  close(sock->socketId);
	  free(sock);
	  return 0;
	}

      {
	/* we do not use in_addr_t, since it does not work with Solaris,
	 and anyway, any system typedef this as an int or uint*/
        uint32_t InAddr;

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
	  free(sock);
	  return 0;
	}

    }
  else
    {
      free(sock);
    }

    
  return sock;
}

void 
TSP_stream_receiver_prepare_stop(TSP_stream_receiver_t receiver) {

  TSP_socket_t* sock = (TSP_socket_t*)receiver;

  sock->is_stopped = TRUE;    

}


void 
TSP_stream_receiver_stop(TSP_stream_receiver_t receiver) {

  TSP_socket_t* sock = (TSP_socket_t*)receiver;

  sock->is_stopped = TRUE;      
  shutdown(sock->socketId, SHUT_RDWR);
  close(sock->socketId);

}

void 
TSP_stream_receiver_destroy(TSP_stream_receiver_t receiver) {

  TSP_socket_t* sock = (TSP_socket_t*)receiver;

  free(sock);
}


int 
TSP_stream_receiver_is_stopped(TSP_stream_receiver_t receiver) {

  TSP_socket_t* sock = (TSP_socket_t*)receiver;

  return sock->is_stopped;
}

int 
TSP_stream_receiver_receive(TSP_stream_receiver_t receiver, char *buffer, int bufferLen) {

  int nread;
  int Total;
  int identSocket  = ((TSP_socket_t*)receiver)->socketId;
  
  Total = 0;
  
  if(identSocket > 0)
    {
      while ( bufferLen > 0)
	{
	  
/* AP : Migration sous Windows */
/* sous windows _errno est la variable globale */
#ifdef _WIN32
		if ( (nread = recv(identSocket, &buffer[Total], bufferLen,0)) < 0)
	  {
          if( _errno == EINTR )
#else
		if ( (nread = read(identSocket, &buffer[Total], bufferLen)) < 0)
	  {
	      if( errno == EINTR )
#endif
		  {
		    /* The read might have been interrupted by a signal */
		    nread = 0;
		  }
	      else 
		  {
		  
		    STRACE_INFO(("read failed"));
		    return FALSE;
		  }
	  }
      else if (nread == 0)
	  {
		  STRACE_INFO(("Received socket EOF"));
		  return FALSE;
	  }
	  
	  Total += nread;
	  bufferLen -= nread;
      	}
    }
  return TRUE;
}
