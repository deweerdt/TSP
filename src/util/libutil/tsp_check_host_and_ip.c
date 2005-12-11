/*!  \file 

$Id: tsp_check_host_and_ip.c,v 1.3 2005-12-11 18:58:08 erk Exp $

-----------------------------------------------------------------------

TSP Utils Library - utilities for a generic Transport Sampling Protocol.

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

Project    : TSP
Maintainer : tsp@astrium.eads.net
Component  : Utils library

-----------------------------------------------------------------------

Purpose   : check host and ip resolution

-----------------------------------------------------------------------
 */
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
