/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.c,v 1.1 2002-08-27 08:56:09 galles Exp $

-----------------------------------------------------------------------

Project   : TSP

Component : Provider

-----------------------------------------------------------------------

Purpose   : Implementation for the functions that send the data
stream  from the producer for the asked symbols. This layer is the network layer and uses sockets
-----------------------------------------------------------------------
 */

#include "tsp_sys_headers.h"
  
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

#include "tsp_stream_sender.h"

#define TSP_DATA_ADDRESS_STRING_SIZE 256

struct TSP_socket_t
{
  int socketId;
  int hClient;
    
  /** String for the data address ex: Myhost:27015 */
  char data_address[TSP_DATA_ADDRESS_STRING_SIZE + 1];
};

typedef struct TSP_socket_t TSP_socket_t;

/* Signal functions */

typedef void Sigfunc(int);

static Sigfunc* signal(int signo, Sigfunc* func)
{
  struct sigaction act, oact;
  
  act.sa_handler = func;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if(signo == SIGALRM )
    {
#ifdef SA_INTERRUPT
      act.sa_flags |= SA_INTERRUPT; /*SunOS 4;x */
#endif
    }
  else
    {
#ifdef SA_RESTART
      act.sa_flags |= SA_RESTART; /*SVR4, 4.4BSD*/
#endif
    }
  if(sigaction(signo, &act, &oact) < 0)
    return(SIG_ERR);
  return (oact.sa_handler);
} 

static TSP_stream_sender_save_address_string(TSP_socket_t* sock, 
					     char* host, ushort port)
{   
  SFUNC_NAME(TSP_stream_sender_save_address_string);
        
  char strPort[10];
    
  STRACE_IO(("-->IN"));
    
  sprintf(strPort,"%u",(int)port);
  strcpy(sock->data_address, host);
  strcat(sock->data_address, ":");    
  strcat(sock->data_address, strPort);
    
  STRACE_IO(("-->OUT address='%s'", sock->data_address));
}

static void* TSP_streamer_sender_connector(void* arg)
{
  /* FIXME : creer le thread détaché */
    
  SFUNC_NAME(TSP_streamer_sender_connector);
    
  TSP_socket_t* sock = (TSP_socket_t*)arg;
  int Len = 0;
    
  STRACE_IO(("-->IN"));
    
  /* Accept connection on socket */
#ifdef __linux__
  sock->hClient = accept(sock->socketId, NULL, &(socklen_t)Len);
#else
  sock->hClient = accept(sock->socketId, NULL, &Len);
#endif
  if(sock->hClient > 0)
    {
      STRACE_INFO(("New connection accepted on socket client socket %d", sock->hClient));
    }
  else
    {
      STRACE_ERROR(("Accept error"));
      close(sock->socketId);
      return 0;
    }
    
  STRACE_IO(("-->OUT"));
}

const char* TSP_stream_sender_get_data_address_string(TSP_stream_sender_t sender)
{   
  SFUNC_NAME(TSP_stream_sender_get_data_address_string);
    
  TSP_socket_t* sock = (TSP_socket_t*)sender;
    
  STRACE_IO(("-->IN"));
  STRACE_IO(("-->OUT address='%s'", sock->data_address));

  return sock->data_address;
}

TSP_stream_sender_t TSP_stream_sender_create(void)
{
  SFUNC_NAME(TSP_stream_sender_create);
  int status = 0;
  struct hostent* Host_p = NULL;
  int OptInt = 0;
  int ret = TRUE;
  /*int Len = 0;*/
  char host[MAXHOSTNAMELEN+1];
  ushort port;  TSP_socket_t* sock;
  pthread_t thread_connect_id;
  
  STRACE_IO(("-->IN"));
  
  if( -1 == gethostname(host, MAXHOSTNAMELEN))
    {
      STRACE_ERROR(("gethostname error"));

      return 0;
    }
  
  /* FIXME : faire la desallocation */
  /* FIXME : si la socket ne peut pas etre connecte il y a une fuite de memoire*/
  sock = (TSP_socket_t*)calloc(1, sizeof(TSP_socket_t));
  TSP_CHECK_ALLOC(sock, 0);

  sock->hClient = 0;
  sock->socketId = 0;
  
  /* Init socket */
  sock->socketId = socket(AF_INET, SOCK_STREAM, 0);

  if (sock->socketId > 0)
    {
      OptInt = TSP_DATA_STREAM_SOCKET_BUFFER_SIZE;
      status = setsockopt(sock->socketId, SOL_SOCKET, SO_SNDBUF, (void * )&OptInt, sizeof(OptInt));
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
	/*#ifndef __OSF1__
	  long InAddr;
	  #else
	  in_addr_t InAddr;
	  #endif /* __OSF1__ */

	struct sockaddr_in ListenAddr;

	/* Bind to socket */
	bzero((char *) &ListenAddr, sizeof(struct sockaddr_in));
	ListenAddr.sin_family = AF_INET;
#ifdef _SOCKADDR_LEN
	ListenAddr.sin_len = sizeof(ListenAddr);
#endif
	ListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	/* The kernel will choose a free port */
	status = bind(sock->socketId, (struct sockaddr*) &ListenAddr, sizeof (struct sockaddr_in));

	if (status != -1)
	  {
	    /* Find the chosen port*/
	    struct sockaddr_in buf_addr;
	    int len = sizeof(buf_addr);
	    if( 0 ==  getsockname(sock->socketId, (struct sockaddr*) &buf_addr, &len) )
	      {
		/* Save the address in the tsp_socket struct */
		ushort port =  ntohs(buf_addr.sin_port);
		TSP_stream_sender_save_address_string(sock, host, port);
	      }
	    else
	      {
		STRACE_ERROR(("getsockname failed"));
		  
		close(sock->socketId);
		return 0;
	      }
	  }
	else
	  {
	    STRACE_ERROR(("pb bind to socket"));
	    
	    close(sock->socketId);
	    return 0;
	  }
      }
      

      /* Try and listen onto socket */
      status = listen (sock->socketId, SOMAXCONN);
      if (status == -1)
	{
	  STRACE_ERROR(("pb listening to socket"));

	  close(sock->socketId);
	  return 0;
	}

      /* Lancer le Thread de accept */
      /* Demarrage du thread */
      /* FIXME : faire l'arret du thread */
      /* FIXME : detacher le thread */
      status = pthread_create(&thread_connect_id, NULL, TSP_streamer_sender_connector,  sock);
      TSP_CHECK_THREAD(status, FALSE);

      /*Disable SIGPIPE signal */
      if( SIG_ERR == signal(SIGPIPE, SIG_IGN))
	{
	  STRACE_ERROR(("Unable to disable SIGPIPE signal"));
	  return 0;
	}
    }

  STRACE_IO(("-->OUT"));
    
  return sock;
}

int TSP_stream_sender_send(TSP_stream_sender_t sender, const char *buffer, int bufferLen)
{

  SFUNC_NAME(TSP_stream_sender_send);

  
  int Total;
  int nread;
  int identSocket = ((TSP_socket_t*)sender)->hClient;

  STRACE_IO(("-->IN"));

  Total = 0;
  if(identSocket > 0)
    {
      
      /* Emission du block */
      while (bufferLen > 0)
	{
	  if( (nread = write(identSocket,  &buffer[Total], bufferLen)) <= 0 )
	    {
	      if( errno == EINTR )
		{
		  /* The write might have been interrupted by a signal */
		  nread = 0;
		}
	      else 
		{
		  
		  STRACE_ERROR(("send failed"));
		  return FALSE;
		}
	    }
	  
	  Total += nread;
	  bufferLen -= nread;
	}
    }
  
  STRACE_IO(("-->OUT"));

  return(TRUE);
}
