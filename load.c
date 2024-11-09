/* taken from nacl-20110221, from curvecp/load.c */
#include <unistd.h>
#include "readall.h"
#include "open.h"
#include "e.h"
#include "load.h"

int load(const char *fn,void *x,long long xlen)
{
  int fd;
  int r;
  fd = open_read(fn);
  if (fd == -1) return -1;
  r = readall(fd,x,xlen);
  close(fd);
  return r;
}
