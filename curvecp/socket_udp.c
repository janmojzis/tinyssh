#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "socket.h"
#include "blocking.h"

#ifndef SOL_IP
#ifdef IPPROTO_IP
#define SOL_IP IPPROTO_IP
#endif
#endif

static void enable_bsd_fragmentation(int fd)
{
#ifdef IP_DONTFRAG
#ifdef SOL_IP
  const int x = 0;
  setsockopt(fd,SOL_IP,IP_DONTFRAG,&x,sizeof x);
#endif
#endif
}

static void enable_linux_fragmentation(int fd)
{
#ifdef IP_MTU_DISCOVER
#ifdef IP_PMTUDISC_DONT
#ifdef SOL_IP
  const int x = IP_PMTUDISC_DONT;
  setsockopt(fd,SOL_IP,IP_MTU_DISCOVER,&x,sizeof x);
#endif
#endif
#endif
}

int socket_udp(void)
{
  int fd = socket(PF_INET,SOCK_DGRAM,0);
  if (fd == -1) return -1;
  fcntl(fd,F_SETFD,1);
  blocking_disable(fd);
  enable_bsd_fragmentation(fd);
  enable_linux_fragmentation(fd);
  return fd;
}
