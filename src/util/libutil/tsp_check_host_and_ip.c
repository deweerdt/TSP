#include <stdio.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#ifdef LINUX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#endif
extern int h_errno;                                                                                                                                                             
int main(int argc, char* argv[]) {
  struct hostent*  myhost;
  struct hostent*  myhost_byaddr;
  union {
    uint32_t    addr;
    uint8_t     parts[4];
  } myu;

  char           name[1024];
#ifdef LINUX
  int                fdIPV4;
  struct ifreq       myifrq;
#endif

  
  gethostname(name,1024);
  printf("hostname is  <%s> (as reported by gethostname)\n",name);
  myhost = gethostbyname(name);
  if (myhost == NULL) {
    fprintf(stderr,"Cannot 'gethostbyname (hostname --> @IP)' check your /etc/hosts file.\n");
    exit(1);
  }
  myu.addr = (uint32_t)ntohl(*((uint32_t*)myhost->h_addr_list[0]));
  printf("@IP returned is <%d.%d.%d.%d> <addrtype= %s> (as reported by gethostbyname)\n",
	 myu.parts[3], myu.parts[2], myu.parts[1], myu.parts[0],
	 (myhost->h_addrtype == AF_INET6 ?  "AF_INET6" : "AF_INET"));
  
  myhost_byaddr = gethostbyaddr(myhost->h_addr_list[0], myhost->h_length,myhost->h_addrtype);
  
  if (myhost_byaddr == NULL) {
    fprintf(stderr, "Cannot 'gethostbyaddr (@IP --> hostname)' check your /etc/hosts file\n");
    exit(1);
  }
  myu.addr = (uint32_t)ntohl(*((uint32_t*)myhost_byaddr->h_addr_list[0]));
  
  printf("hostname returned is <%s> for @IP <%d.%d.%d.%d> <addrtype= %s> (as reported by gethostbyaddr)\n",
	 myhost_byaddr->h_name,
	 myu.parts[3], myu.parts[2], myu.parts[1], myu.parts[0],
	 (myhost_byaddr->h_addrtype == AF_INET6 ?  "AF_INET6" : "AF_INET"));

#ifdef LINUX
  fdIPV4 = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
  ioctl(fdIPV4, SIOCGIFADDR, &myifrq);
#endif
  return 0;
}
