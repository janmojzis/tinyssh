#ifndef _MISC_H____
#define _MISC_H____

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include "crypto_uint8.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"

typedef crypto_uint8  u8;
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

static void checksum(const unsigned char *x,unsigned long long xlen)
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

static int checksum_check(unsigned char *o) {
    int j;
    for (j = 0; j < 32; ++j) if (checksum_state[j] != o[j]) return 0;
    return 1;
}

static void checksum_zero(void) {
    long long i;
    for (i = 0; i < 64; ++i) checksum_state[i] = 0;
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

static void pseudorandombytes(unsigned char *x,unsigned long long xlen)
{
  const static unsigned char randombytes_k[33] = "answer randombytes from crypto_*";
  static unsigned char randombytes_n[8];
  s20(x,xlen,randombytes_n,randombytes_k);
  increment(randombytes_n);
}

static void unsaferandombytes(unsigned char *x,unsigned long long xlen)
{
  static unsigned char randombytes_k[32];
  static unsigned char randombytes_n[8];
  static int initialized = 0;
  struct timeval t;

  if (!initialized) {
    initialized = 1;
    gettimeofday(&t, (struct timezone *)0);
    st32(randombytes_k +  0, t.tv_usec);
    st32(randombytes_k +  4, t.tv_sec);
    st32(randombytes_k +  8, getpid());
    st32(randombytes_k + 12, getppid());
    s20(randombytes_k,32,randombytes_n,randombytes_k);
  }

  s20(x,xlen,randombytes_n,randombytes_k);
  increment(randombytes_n);
}


#define fail_(a, b, x) do { fprintf(stderr, "%s:%llu: %s\n", (a), (unsigned long long)(b), (x)); fflush(stderr); _exit(111); } while (0);
#define fail(x) fail_(__FILE__, __LINE__, (x))

static void fail_printdata(const char *text, unsigned char *data, long long len) {

    long long i;

    fprintf(stderr, "%s = {\n    ", text);
    for (i = 0; i < len; ++i) {
        if (i == len - 1) {
            fprintf(stderr, "0x%02x", data[i]);
        }
        else {
            fprintf(stderr, "0x%02x, ", data[i]);
            if (i % 8 == 7) fprintf(stderr, "\n    ");
        }
    }
    fprintf(stderr, "\n};\n");
    fflush(stderr);
}

#define fail_whenbadchecksum(x)                                 \
    if (!checksum_check((x))) {                                 \
        fail_printdata("checksum_expected", (x), 32);                    \
        fail_printdata("checksum_computed", checksum_state, 32);         \
        fail("checksum failure");                               \
    }



#endif
