#ifndef _MISC_H____
#define _MISC_H____

#include "crypto_uint8.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"

typedef crypto_uint8 u8;
typedef crypto_uint32 u32;
typedef crypto_uint64 u64;

#define FOR(i,n) for (i = 0;i < n;++i)

static u32 L32(u32 x,int c) { return (x << c) | ((x&0xffffffff) >> (32 - c)); }

static u32 ld32(const u8 *x)
{
  u32 u = x[3];
  u = (u<<8)|x[2];
  u = (u<<8)|x[1];
  return (u<<8)|x[0];
}

static void st32(u8 *x,u32 u)
{
  int i;
  FOR(i,4) { x[i] = u; u >>= 8; }
}

static const u8 sigma[17] = "expand 32-byte k";

static void core(u8 *out,const u8 *in,const u8 *k)
{
  u32 w[16],x[16],y[16],t[4];
  int i,j,m;

  FOR(i,4) {
    x[5*i] = ld32(sigma+4*i);
    x[1+i] = ld32(k+4*i);
    x[6+i] = ld32(in+4*i);
    x[11+i] = ld32(k+16+4*i);
  }

  FOR(i,16) y[i] = x[i];

  FOR(i,20) {
    FOR(j,4) {
      FOR(m,4) t[m] = x[(5*j+4*m)%16];
      t[1] ^= L32(t[0]+t[3], 7);
      t[2] ^= L32(t[1]+t[0], 9);
      t[3] ^= L32(t[2]+t[1],13);
      t[0] ^= L32(t[3]+t[2],18);
      FOR(m,4) w[4*j+(j+m)%4] = t[m];
    }
    FOR(m,16) x[m] = w[m];
  }

  FOR(i,16) st32(out + 4 * i,x[i] + y[i]);
}

static unsigned char checksum_state[64] = {0};

void checksum(const unsigned char *x,unsigned long long xlen)
{
  u8 block[16];
  int i;
  while (xlen >= 16) {
    core(checksum_state,x,checksum_state);
    x += 16;
    xlen -= 16;
  }
  FOR(i,16) block[i] = 0;
  FOR(i,xlen) block[i] = x[i];
  block[xlen] = 1;
  checksum_state[0] ^= 1;
  core(checksum_state,block,checksum_state);
}

int checksum_check(unsigned char *o) {
    int j;
    for (j = 0; j < 64; ++j) if (checksum_state[j] != o[j]) return 0;
    return 1;
}

static void s20(u8 *c,u64 b,const u8 *n,const u8 *k)
{
  u8 z[16],x[64];
  u32 u,i;
  if (!b) return;
  FOR(i,16) z[i] = 0;
  FOR(i,8) z[i] = n[i];
  while (b >= 64) {
    core(x,z,k);
    FOR(i,64) c[i] = x[i];
    u = 1;
    for (i = 8;i < 16;++i) {
      u += (u32) z[i];
      z[i] = u;
      u >>= 8;
    }
    b -= 64;
    c += 64;
  }
  if (b) {
    core(x,z,k);
    FOR(i,b) c[i] = x[i];
  }
}

static void increment(u8 *n)
{
  if (!++n[0])
    if (!++n[1])
      if (!++n[2])
        if (!++n[3])
          if (!++n[4])
            if (!++n[5])
              if (!++n[6])
                if (!++n[7])
                  ;
}

void pseudorandombytes(unsigned char *x,unsigned long long xlen)
{
  const static unsigned char randombytes_k[33] = "answer randombytes from crypto_*";
  static unsigned char randombytes_n[8];
  s20(x,xlen,randombytes_n,randombytes_k);
  increment(randombytes_n);
}

/* slen must be 36 * len * len */
static void precompute(unsigned char *s, long long len) {

    unsigned char *b = s; /* XXX - reusing first block */
    long long i, j, k;

    /* bytes */
    for (i = 0; i < len; ++i) b[i] = 0x00;

    for (i = 0; i < len; ++i) {
        b[i] = 0xff;
        for (j = 0; j < len; ++j) *s++ = b[j];
    }

    for (i = 0; i < len; ++i) {
        b[i] = 0x00;
        for (j = 0; j < len; ++j) *s++ = b[j];
    }

    for (i = 0; i < len; ++i) {
        for (j = 0; j < len; ++j) b[j] = 0x00;
        b[i] = 0xff;
        for (j = 0; j < len; ++j) *s++ = b[j];
    }

    for (i = 0; i < len; ++i) {
        for (j = 0; j < len; ++j) b[j] = 0xff;
        b[i] = 0x00;
        for (j = 0; j < len; ++j) *s++ = b[j];
    }

    /* bits */
    for (i = 0; i < len; ++i) b[i] = 0x00;

    for (i = 0; i < len; ++i) {
        b[i] = 0x00;
        for (k = 7; k >= 0; --k) {
            b[i] |= (1 << k);
            for (j = 0; j < len; ++j) *s++ = b[j];
        }
    }

    for (i = 0; i < len; ++i) {
        b[i] = 0xff;
        for (k = 7; k >= 0; --k) {
            b[i] &= ~(1 << k);
            for (j = 0; j < len; ++j) *s++ = b[j];
        }
    }

    for (i = 0; i < len; ++i) {
        for (k = 7; k >= 0; --k) {
            for (j = 0; j < len; ++j) b[j] = 0x00;
            b[i] = (1 << k);
            for (j = 0; j < len; ++j) *s++ = b[j];
        }
    }

    for (i = 0; i < len; ++i) {
        for (k = 7; k >= 0; --k) {
            for (j = 0; j < len; ++j) b[j] = 0xff;
            b[i] = 0xff & ~(1 << k);
            for (j = 0; j < len; ++j) *s++ = b[j];
        }
    }

    /* XXX - replace first block */
    for (i = 0; i < len; ++i) b[i] = 0x00;
    b[0] = 0xff;
}

#endif
