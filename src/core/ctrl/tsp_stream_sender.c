/*!  \file 

$Header: /home/def/zae/tsp/tsp/src/core/ctrl/tsp_stream_sender.c,v 1.6 2002-10-28 14:29:38 tntdev Exp $

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

/* (µs) */
#define TSP_STREAM_SENDER_CONNECTION_POOL_TIME ((int)(1e5))
#define TSP_STREAM_SENDER_FIFO_POOL_TIME       ((int)(1e5))

struct TSP_socket_t
{
  int socketId;
  int hClient;
    
  /** String for the data address ex: Myhost:27015 */
  char data_address[TSP_DATA_ADDRESS_STRING_SIZE + 1];

  /**
   * client connection status.
   * If TRUE, the client is connected to the socket
   */
  int client_is_connected;

  /**
   * Fifo for outgoing data
   */
  TSP_stream_sender_ringbuf_t* out_ringbuf;

  /** 
   * When fifo size is > 0, a thead is created to send data
   */
  int fifo_size;
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


static void* TSP_streamer_sender_thread_sender(void* arg)
{
  /* FIXME : creer le thread détaché */
    
  SFUNC_NAME(TSP_streamer_sender_sender);
    
  TSP_socket_t* sock = (TSP_socket_t*)arg;
  TSP_stream_sender_item_t* item = 0;
  int connection_ok = TRUE;
     
  STRACE_IO(("-->IN"));
 
    /* Wait for consumer connection before we send data */  

  STRACE_INFO(("Thread stream sender created : waiting for client to connect..."));
  while(!sock->client_is_connected)
    {
      tsp_usleep(TSP_STREAM_SENDER_CONNECTION_POOL_TIME);
     
    }
  STRACE_INFO(("Client connected ! Send loop starts !"));

  item = RINGBUF_PTR_GETBYADDR(sock->out_ringbuf);
  /* FIXME : gerer l'arret */
  /* Gerer l'impossibilité d'envoyer */
  while(connection_ok)
    {
      while (item && connection_ok)
	{
	 
	  connection_ok =  TSP_stream_sender_send(sock, item->buf, item->len);
	  RINGBUF_PTR_GETBYADDR_COMMIT(sock->out_ringbuf);
	  item = RINGBUF_PTR_GETBYADDR(sock->out_ringbuf);      

	}
      	  tsp_usleep(TSP_STREAM_SENDER_FIFO_POOL_TIME);
	  item = RINGBUF_PTR_GETBYADDR(sock->out_ringbuf);      
    }

  STRACE_WARNING(("Connection with consumer broken"));
      
  STRACE_IO(("-->OUT"));
}


/**
* When a fifo is needed create a fifo and a thread to send the data from the fifo 
* @param sock The data stream object instance
* @return OK or KO. TRUE = OK
*/
static int TSP_stream_sender_init_bufferized(TSP_socket_t* sock)
{


  SFUNC_NAME(TSP_stream_sender_init_bufferized);
  int status;
  pthread_t thread_id;

  STRACE_IO(("-->IN"));

  RINGBUF_PTR_INIT(TSP_stream_sender_ringbuf_t,
		   sock->out_ringbuf,
		   TSP_stream_sender_item_t, 
		   RINGBUF_SZ(sock->fifo_size) );

  assert(sock->out_ringbuf);

  status = pthread_create(&thread_id, NULL, TSP_streamer_sender_thread_sender,  sock);
  TSP_CHECK_THREAD(status, FALSE);
 
  STRACE_IO(("-->OUT"));

  return TRUE;
  
}

static TSP_stream_sender_save_address_string(TSP_socket_t* sock, 
					     char* host, unsigned short port)
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
  STRACE_INFO(("Thread acceptor started waiting for client to connect", sock->hClient));
  sock->hClient = accept(sock->socketId, NULL, &Len);

  if(sock->hClient > 0)
    {

      /* OK, the client is connected */
      sock->client_is_connected = TRUE;
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

TSP_stream_sender_t TSP_stream_sender_create(int fifo_size)
{
  SFUNC_NAME(TSP_stream_sender_create);
  int status = 0;
  int OptInt = 0;
  int ret = TRUE;
  /*int Len = 0;*/
  char host[TSP_MAXHOSTNAMELEN+1];
  unsigned short port;  TSP_socket_t* sock;
  pthread_t thread_connect_id;
  
  STRACE_IO(("-->IN"));

  /*First disable SIGPIPE signal to avoir being crashed by a disconnected client*/
  if( SIG_ERR == signal(SIGPIPE, SIG_IGN))
    {
      STRACE_ERROR(("Unable to disable SIGPIPE signal"));
      return 0;
    }
  
  if( -1 == gethostname(host, TSP_MAXHOSTNAMELEN))
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
  sock->fifo_size = fifo_size;
  sock->out_ringbuf = 0;
  sock->client_is_connected = FALSE;
  
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
		unsigned short port =  ntohs(buf_addr.sin_port);
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

      /* If we want a bufferized connection, create a fifo, and launch the sender
	 thread that will read the fifo */
      if(sock->fifo_size > 0 )	
	{
	  /* FIXME : il faudra un jour que le ring buf puisse vérifier si l'allocation a fonctionné */
	  if(!TSP_stream_sender_init_bufferized(sock))
	    {
	      STRACE_ERROR(("Function TSP_stream_sender_init_bufferized failed"));
	      close(sock->socketId);	  
	      return 0;
	    }
	  
	}
      

      /* When the client is be connected, the thread function TSP_streamer_sender_connector will set the client_is_connected 
	 var to TRUE */
            
      
      status = pthread_create(&thread_connect_id, NULL, TSP_streamer_sender_connector,  sock);
      TSP_CHECK_THREAD(status, FALSE);

      

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


/**
 * Tells is a client is connected to the socket.
 * Note : Is a client crashes this still returns TRUE
 * @return TRUE is the client is connected
 */
int TSP_stream_sender_is_client_connected(TSP_stream_sender_t sender)
{
  TSP_socket_t* sock = (TSP_socket_t*)sender;

  return sock->client_is_connected;
}

TSP_stream_sender_ringbuf_t* TSP_stream_sender_get_ringbuf(TSP_stream_sender_t sender)
{
  TSP_socket_t* sock = (TSP_socket_t*)sender;

  return sock->out_ringbuf;
}
