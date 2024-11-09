/* taken from nacl-20110221, from curvecp/byte_copy.c curvecp/byte_isequal.c curvecp/byte_zero.c */
#include "byte.h"

void byte_copy(void *yv,long long ylen,const void *xv)
{
  char *y = (char *)yv;
  const char *x = (const char *)xv;
  while (ylen > 0) { *y++ = *x++; --ylen; }
}

int byte_isequal(const void *yv,long long ylen,const void *xv)
{
  const unsigned char *y = (const unsigned char *)yv;
  const unsigned char *x = (const unsigned char *)xv;
  unsigned char diff = 0;
  while (ylen > 0) { diff |= (*y++ ^ *x++); --ylen; }
  return (256 - (unsigned int) diff) >> 8;
}

void byte_zero(void *yv,long long ylen)
{
  char *y = (char *)yv;
  while (ylen > 0) { *y++ = 0; --ylen; }
}
