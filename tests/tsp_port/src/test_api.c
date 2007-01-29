// test.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#ifdef WIN32
  #include <windows.h>
#else
  #include <netdb.h>
  #include <string.h>
  #include <stdlib.h>
  #include <unistd.h>
#if defined(sun) || defined(__sun)
  #include <alloca.h>
#endif
#endif

#include "test_api.h"
#include "test_port_rpc.h"

static pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;

static affiche_appli_fp affiche_appli = &printf;
static saisie_appli_fp  saisie_appli  = &scanf;

void init_affichage_argv(affiche_appli_fp function)
{
    affiche_appli = function;
}

void init_affichage(affiche_appli_fp function)
{
    affiche_appli = function;
}

void init_saisie(saisie_appli_fp function)
{
    saisie_appli = function;
}

int test_printf( char * format, ... )
{
   va_list args;
   int len, iRet = -1;
   char * buffer;

   va_start( args, format );
#ifdef WIN32
   len = _vscprintf( format, args ) // _vscprintf doesn't count
                               + 1; // terminating '\0'
#else 
   /* FIXME find a way to bound buffer length */
   len = 1024;
#endif
   buffer = malloc( len * sizeof(char) );
   vsprintf( buffer, format, args );
    
   if (NULL != affiche_appli)
   {
    iRet = affiche_appli( buffer );
   }

   fflush(stdout);
   free( buffer );

   return iRet;
}

int 
test_scan_1_car( char * uncar ) {
  int iRet = -1;
  
  fflush(stdin);
   if (NULL != saisie_appli)
   {
    iRet = saisie_appli( "%c", uncar );
   }

   return iRet;
}

typedef struct wrapper_s
{
    int id;
} wrapper_t;

#if 0
void affiche_boite()
{ 
  HWND parent; 
  parent = GetDesktopWindow(); 
  MessageBox(parent, "Ca marche", "Ca marche", MB_OKCANCEL); 
} 
#endif

int tsp_usleep(int useconds)
{

#ifdef TSP_SYSTEM_HAVE_NANOSLEEP
  struct timespec ts;
  ts.tv_sec = useconds / 1000000;
  ts.tv_nsec = (useconds % 1000000) * 1000;
  return nanosleep(&ts, NULL);
#else

#ifdef TSP_SYSTEM_HAVE_THREADSAFE_USLEEP
  return usleep(useconds);
#else

#ifdef TSP_SYSTEM_USE_WIN32_SLEEP
  Sleep(useconds * 1000);
  return 0;
#else
  ERROR__you_should_try_to_find_some_kind_of_wait_function
#endif /*TSP_SYSTEM_HAVE_SLEEP*/

#endif /*TSP_SYSTEM_HAVE_THREADSAFE_USLEEP*/

#endif /*TSP_SYSTEM_HAVE_NANOSLEEP*/

}

/* The wrapper function in charge for setting the itimer value */
static void * wrapper_routine(void * data)
{
    int i;
    int id;
    wrapper_t * wrapper_data = (wrapper_t *)data;    
    
    id = wrapper_data->id;
     
    // synchro de thread par mutex
    for(i=0; i<10; i++)
    {
        pthread_mutex_lock(&my_mutex);
        test_printf("%d", id);        
        pthread_mutex_unlock(&my_mutex);
        tsp_usleep(id*2);
    }

    test_printf("\n"); 

    return NULL;
}

void printstruct(char * mes, void * arg)
{
	TSP_port_struct *val = (TSP_port_struct *)arg;
    if (NULL == val)
    {
        test_printf("%s valeur nulle\n", mes);
    }
    else
    {
        test_printf("%s %d %f %s\n", mes, val->i, val->a, val->status_str);
    }
}

char* stringtosend()
{
    char *tosend;
    tosend = (char *)malloc(sizeof(char)*DEFAULT_BUFLEN);
    tosend = strdup("this is a test");
    return tosend;
}

int teststringrecu(char * recu)
{
    return strcmp(recu, stringtosend());
}

void test_gethostx()
{
    // header : WinSock2.h
    // library : Ws2_32.lib
    // DLL : Ws2_32.dll
    // vient de oncrpc recompiler avec

    char             name[1024];
    struct hostent*  myhost;
    struct hostent*  myhost_byaddr;
    union {
        u_int       addr;
        u_short     parts[4];
    } myu;
    char* host;
    char* last; /**<For strtok_r*/

    gethostname(name,1024);
    test_printf("hostname is  <%s> (as reported by gethostname)\n",name);
    myhost = gethostbyname(name);

    if (myhost == NULL) {
        fprintf(stderr,"Cannot 'gethostbyname (hostname --> @IP)' check your /etc/hosts file.\n");
        return ;
    }
    memset((char *)&myu, 0, sizeof(myu));
    myu.addr = (u_int)ntohl(*((u_int*)myhost->h_addr_list[0]));
    test_printf("@IP returned is <%d.%d.%d.%d> <addrtype= %s> (as reported by gethostbyname)\n",
	    myu.parts[3], myu.parts[2], myu.parts[1], myu.parts[0],
	    (myhost->h_addrtype == AF_INET6 ?  "AF_INET6" : "AF_INET"));
      
    myhost_byaddr = gethostbyaddr(myhost->h_addr_list[0], myhost->h_length,myhost->h_addrtype);
      
    if (myhost_byaddr == NULL) {
        fprintf(stderr, "Cannot 'gethostbyaddr (@IP --> hostname)' check your /etc/hosts file\n");
        return ;
    }
    myu.addr = (u_int)ntohl(*((u_int*)myhost_byaddr->h_addr_list[0]));
      
    test_printf("hostname returned is <%s> for @IP <%d.%d.%d.%d> <addrtype= %s> (as reported by gethostbyaddr)\n",
	    myhost_byaddr->h_name,
	    myu.parts[3], myu.parts[2], myu.parts[1], myu.parts[0],
	    (myhost_byaddr->h_addrtype == AF_INET6 ?  "AF_INET6" : "AF_INET"));

    /* htonl(INADDR_ANY); what's that */
    
    host = strdup(name);
    last = strstr(host, ".");
    test_printf("strstr : %s\n", last);

 /*   host = strtok_r((char* )name, ":", &last);
    test_printf("strtok_r : %s", host);*/

    last = strtok(host, ".");
    test_printf("strtok : %s\n", last);

}

void test_memoire()
{
    char *val;
    TSP_port_struct* ssi;
    int* out_temp;
    int i;    
    
    test_printf("test memoire\n");

    val = (char *)malloc(sizeof(char)*5);
    val[0]='0';
    val[1]='1';
    val[2]='2';
    val[3]='3';
    val[4]='\0';
    test_printf("malloc %s\n", val);
    free(val);
    
    ssi = calloc(1,sizeof(TSP_port_struct));
    ssi->a = 1;
    ssi->i = 2;    
    ssi->status_str = "t";
    test_printf("calloc %d %d %c\n", ssi->a, ssi->i, ssi->status_str); 
    free(ssi);
    
#ifdef WIN32
    out_temp = _alloca(sizeof(int)*4);
#else
    out_temp = alloca(sizeof(int)*4);
#endif
    out_temp[0] = 10;
    out_temp[1] = 20;
    out_temp[2] = 30;
    out_temp[3] = 40;
    test_printf("alloca : ");
    for (i=0;i<4;i++)
    {
        test_printf("%d", out_temp[i]);
    }
    test_printf("\n");

#ifdef WIN32
    Sleep(1000);
#else
    sleep(1);
#endif

    test_printf("Fin test memoire\n");
}

void test_pthread()
{
    pthread_t threadid1, threadid2;
    wrapper_t wrapper_data1, wrapper_data2;
    struct timeval tp;

    test_printf("test pthread");

    assert(1);

    // test pthread
    wrapper_data1.id=1;
    wrapper_data2.id=2;        
    
    pthread_create(&threadid1, NULL, &wrapper_routine,
                                            &wrapper_data1);

    pthread_create(&threadid2, NULL, &wrapper_routine,
                                            &wrapper_data2);
    
    
    test_printf("Joining thread 1\n");
    pthread_join(threadid1, NULL);
    test_printf("Joining thread 2\n");
    pthread_join(threadid2, NULL);

    test_printf("\n");        

    /* Save memory ! */
    //pthread_detach(pthread_self());
    
    //sigfillset(&s_mask);
    //pthread_sigmask(SIG_BLOCK,&s_mask,NULL);

    //pthread_cancel(thread);
    
    // header : WinSock.h
    gettimeofday(&tp, (void*)NULL);
    test_printf("gettimeofday %l %l\n", tp.tv_sec, tp.tv_usec);

    //affiche_boite();

    test_printf("Fin test pthread");
}

int api_main()
{
    char c;

    test_printf(" 0 pour les test host,\n");
    test_printf(" 1 pour les test memoire,\n");
    test_printf(" 2 pour les test api,\n");
    test_printf(" entrer une valeur : ");
    
    test_scan_1_car(&c);
    
    if ( '0' == c )
    {
        test_gethostx();
    }

    if ( '1' == c )
    {
        test_memoire();
    }

    if ( '2' == c )
    {
        test_pthread();
    }
    
    test_printf("\nFin test api\n");
	return 0;
}
