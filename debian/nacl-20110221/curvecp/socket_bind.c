#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "e.h"
#include "socket.h"
#include "byte.h"

int socket_bind(int fd,const unsigned char *ip,const unsigned char *port)
{
  struct sockaddr_in sa;
  byte_zero(&sa,sizeof sa);
  sa.sin_family = PF_INET;
  byte_copy(&sa.sin_addr,4,ip);
  byte_copy(&sa.sin_port,2,port);
  return bind(fd,(struct sockaddr *) &sa,sizeof sa);
}
