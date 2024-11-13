/*
derived from tests from lib25519/libntruprime
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/resource.h>
#include "crypto_uint8.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include "crypto_declassify.h"
#include <randombytes.h>

const char *targeto = 0;
const char *targetp = 0;
const char *targeti = 0;
const char *targetn = 0;
const char *targetoffset = 0;

int ok = 1;

#define fail ((ok = 0),printf)

/* ----- valgrind support */

int valgrind = 0;
static unsigned char valgrind_undefined_byte = 0;
static char *volatile valgrind_pointer = 0;

static char *valgrind_malloc_1(void)
{
  char *x = malloc(1);
  if (!x) abort();
  *(char **volatile) &valgrind_pointer = x;
  return valgrind_pointer;
}

static void valgrind_init(void)
{
  char *e = getenv("valgrind_multiplier");
  char *x;
  if (!e) return;
  x = valgrind_malloc_1();
  valgrind_undefined_byte = x[0]+1;
  valgrind_undefined_byte *= atoi(e);
  valgrind_undefined_byte ^= x[0]+1;
  free(x);
  valgrind = 1;
}

void secret(void *xvoid,long long xlen)
{
  unsigned char *x = xvoid;
  while (xlen > 0) {
    *x ^= valgrind_undefined_byte;
    ++x;
    --xlen;
  }
}

void public(void *x,long long xlen)
{
  crypto_declassify(x,xlen);
}

/* ----- rng and hash, from supercop/try-anything.c */

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

static void core_salsa(u8 *out,const u8 *in,const u8 *k)
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

static void salsa20(u8 *c,u64 b,const u8 *n,const u8 *k)
{
  u8 z[16],x[64];
  u32 u,i;
  if (!b) return;
  FOR(i,16) z[i] = 0;
  FOR(i,8) z[i] = n[i];
  while (b >= 64) {
    core_salsa(x,z,k);
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
    core_salsa(x,z,k);
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
                if (!++n[7]) {
                  ;
                }
}

static unsigned char testvector_n[8];

static void testvector_clear(void)
{
  memset(testvector_n,0,sizeof testvector_n);
}

static void testvector(unsigned char *x,unsigned long long xlen)
{
  static const unsigned char testvector_k[33] = "generate inputs for test vectors";
  salsa20(x,xlen,testvector_n,testvector_k);
  increment(testvector_n);
}

unsigned long long myrandom(void)
{
  unsigned char x[8];
  unsigned long long result;
  testvector(x,8);
  result = x[7];
  result = (result<<8)|x[6];
  result = (result<<8)|x[5];
  result = (result<<8)|x[4];
  result = (result<<8)|x[3];
  result = (result<<8)|x[2];
  result = (result<<8)|x[1];
  result = (result<<8)|x[0];
  return result;
}

static unsigned char canary_n[8];

static void canary(unsigned char *x,unsigned long long xlen)
{
  static const unsigned char canary_k[33] = "generate pad to catch overwrites";
  salsa20(x,xlen,canary_n,canary_k);
  increment(canary_n);
}

void double_canary(unsigned char *x2,unsigned char *x,unsigned long long xlen)
{
  if (valgrind) return;
  canary(x - 16,16);
  canary(x + xlen,16);
  memcpy(x2 - 16,x - 16,16);
  memcpy(x2 + xlen,x + xlen,16);
}

void input_prepare(unsigned char *x2,unsigned char *x,unsigned long long xlen)
{
  testvector(x,xlen);
  if (valgrind) {
    memcpy(x2,x,xlen);
    return;
  }
  canary(x - 16,16);
  canary(x + xlen,16);
  memcpy(x2 - 16,x - 16,xlen + 32);
}

void input_compare(const unsigned char *x2,const unsigned char *x,unsigned long long xlen,const char *fun)
{
  if (valgrind) return;
  if (memcmp(x2 - 16,x - 16,xlen + 32)) {
    fail("failure: %s overwrites input\n",fun);
  }
}

void output_prepare(unsigned char *x2,unsigned char *x,unsigned long long xlen)
{
  if (valgrind) {
    memcpy(x2,x,xlen);
    return;
  }
  canary(x - 16,xlen + 32);
  memcpy(x2 - 16,x - 16,xlen + 32);
}

void output_compare(const unsigned char *x2,const unsigned char *x,unsigned long long xlen,const char *fun)
{
  if (valgrind) return;
  if (memcmp(x2 - 16,x - 16,16)) {
    fail("failure: %s writes before output\n",fun);
  }
  if (memcmp(x2 + xlen,x + xlen,16)) {
    fail("failure: %s writes after output\n",fun);
  }
}

/* ----- knownrandombytes */

static const int knownrandombytes_is_only_for_testing_not_for_cryptographic_use = 1;
#define knownrandombytes randombytes

#define QUARTERROUND(a,b,c,d) \
  a += b; d = L32(d^a,16); \
  c += d; b = L32(b^c,12); \
  a += b; d = L32(d^a, 8); \
  c += d; b = L32(b^c, 7);

static void core_chacha(u8 *out,const u8 *in,const u8 *k)
{
  u32 x[16],y[16];
  int i,j;
  FOR(i,4) {
    x[i] = ld32(sigma+4*i);
    x[12+i] = ld32(in+4*i);
  }
  FOR(i,8) x[4+i] = ld32(k+4*i);
  FOR(i,16) y[i] = x[i];
  FOR(i,10) {
    FOR(j,4) { QUARTERROUND(x[j],x[j+4],x[j+8],x[j+12]) }
    FOR(j,4) { QUARTERROUND(x[j],x[((j+1)&3)+4],x[((j+2)&3)+8],x[((j+3)&3)+12]) }
  }
  FOR(i,16) st32(out+4*i,x[i]+y[i]);
}

static void chacha20(u8 *c,u64 b,const u8 *n,const u8 *k)
{
  u8 z[16],x[64];
  u32 u,i;
  if (!b) return;
  FOR(i,16) z[i] = 0;
  FOR(i,8) z[i+8] = n[i];
  while (b >= 64) {
    core_chacha(x,z,k);
    FOR(i,64) c[i] = x[i];
    u = 1;
    FOR(i,8) {
      u += (u32) z[i];
      z[i] = u;
      u >>= 8;
    }
    b -= 64;
    c += 64;
  }
  if (b) {
    core_chacha(x,z,k);
    FOR(i,b) c[i] = x[i];
  }
}

#define crypto_rng_OUTPUTBYTES 736

static int crypto_rng(
        unsigned char *r, /* random output */
        unsigned char *n, /* new key */
  const unsigned char *g  /* old key */
)
{
  static const unsigned char nonce[8] = {0};
  unsigned char x[32+crypto_rng_OUTPUTBYTES];
  chacha20(x,sizeof x,nonce,g);
  memcpy(n,x,32);
  memcpy(r,x+32,crypto_rng_OUTPUTBYTES);
  return 0;
}

static unsigned char knownrandombytes_g[32];
static unsigned char knownrandombytes_r[crypto_rng_OUTPUTBYTES];
static unsigned long long knownrandombytes_pos = crypto_rng_OUTPUTBYTES;

static void knownrandombytes_clear(void)
{
  memset(knownrandombytes_g,0,sizeof knownrandombytes_g);
  memset(knownrandombytes_r,0,sizeof knownrandombytes_r);
  knownrandombytes_pos = crypto_rng_OUTPUTBYTES;
}

void knownrandombytes_main(void *xvoid,long long xlen)
{
  unsigned char *x = xvoid;
  assert(knownrandombytes_is_only_for_testing_not_for_cryptographic_use);

  while (xlen > 0) {
    if (knownrandombytes_pos == crypto_rng_OUTPUTBYTES) {
      crypto_rng(knownrandombytes_r,knownrandombytes_g,knownrandombytes_g);
      knownrandombytes_pos = 0;
    }
    *x++ = knownrandombytes_r[knownrandombytes_pos];
    xlen -= 1;
    knownrandombytes_r[knownrandombytes_pos++] = 0;
  }
}

void knownrandombytes(void *xvoid,long long xlen)
{
  knownrandombytes_main(xvoid,xlen);
  secret(xvoid,xlen);
}

/* ----- checksums */

static unsigned char checksum_state[64];
static char checksum_hex[65];

void checksum_expected(const char *expected)
{
  long long i;
  for (i = 0;i < 32;++i) {
    checksum_hex[2 * i] = "0123456789abcdef"[15 & (checksum_state[i] >> 4)];
    checksum_hex[2 * i + 1] = "0123456789abcdef"[15 & checksum_state[i]];
  }
  checksum_hex[2 * i] = 0;

  if (strcmp(checksum_hex,expected))
    fail("failure: checksum mismatch: %s expected %s\n",checksum_hex,expected);
}

void checksum_clear(void)
{
  memset(checksum_state,0,sizeof checksum_state);
  knownrandombytes_clear();
  testvector_clear();
  /* not necessary to clear canary */
}

void checksum(const unsigned char *x,unsigned long long xlen)
{
  u8 block[16];
  unsigned long long i;
  while (xlen >= 16) {
    core_salsa(checksum_state,x,checksum_state);
    x += 16;
    xlen -= 16;
  }
  FOR(i,16) block[i] = 0;
  FOR(i,xlen) block[i] = x[i];
  block[xlen] = 1;
  checksum_state[0] ^= 1;
  core_salsa(checksum_state,block,checksum_state);
}

static void limits()
{
#ifdef RLIM_INFINITY
  struct rlimit r;
  r.rlim_cur = 0;
  r.rlim_max = 0;
#ifdef RLIMIT_NOFILE
  setrlimit(RLIMIT_NOFILE,&r);
#endif
#ifdef RLIMIT_NPROC
  setrlimit(RLIMIT_NPROC,&r);
#endif
#ifdef RLIMIT_CORE
  setrlimit(RLIMIT_CORE,&r);
#endif
#endif
}

void *callocplus(long long len)
{
  if (valgrind) {
    unsigned char *x = malloc(len);
    if (!x) abort();
    return x;
  } else {
    unsigned char *x = calloc(1,len + 256);
    long long i;
    if (!x) abort();
    for (i = 0;i < len + 256;++i) x[i] = random();
    return x;
  }
}

void *aligned(void *x,long long len)
{
  if (valgrind)
    return x;
  else {
    long long i;
    unsigned char *y = x;
    y += 64;
    y += 63 & (-(unsigned long) y);
    for (i = 0;i < len;++i) y[i] = 0;
    return y;
  }
}

/* ----- catching SIGILL, SIGBUS, SIGSEGV, etc. */

void forked(void (*test)(long long),long long impl)
{
  if (valgrind) {
    test(impl);
    return;
  }
  fflush(stdout);
  pid_t child = fork();
  int childstatus = -1;
  if (child == -1) {
    fprintf(stderr,"fatal: fork failed: %s",strerror(errno));
    exit(111);
  }
  if (child == 0) {
    ok = 1;
    limits();
    test(impl);
    if (!ok) exit(100);
    exit(0);
  }
  if (waitpid(child,&childstatus,0) != child) {
    fprintf(stderr,"fatal: wait failed: %s",strerror(errno));
    exit(111);
  }
  if (childstatus)
    fail("failure: process failed, status %d\n",childstatus);
  fflush(stdout);
}


#include "_crypto-test_hash_sha256.inc"
#include "_crypto-test_hash_sha512.inc"
#include "_crypto-test_sign_ed25519.inc"


/* ----- top level */

int main(int argc,char **argv)
{
  (void) argc;
  valgrind_init();
  if (valgrind) limits();

  setvbuf(stdout,0,_IOLBF,0);

  if (valgrind) {
    printf("valgrind %d",(int) valgrind);
    printf(" declassify %d",(int) crypto_declassify_uses_valgrind);
    if (!crypto_declassify_uses_valgrind)
      printf(" (expect false positives)");
    printf("\n");
  }

  if (*argv) ++argv;
  if (*argv) {
    targeto = *argv++;
    if (*argv) {
      targetp = *argv++;
      if (*argv) {
        targeti = *argv++;
        if (*argv) {
          targetn = *argv++;
          if (*argv) {
            targetoffset = *argv++;
          }
        }
      }
    }
  }

  test_hash_sha256();
  test_hash_sha512();
  test_sign_ed25519();
#if 0
  test_verify_32();
  test_hashblocks_sha512();
  test_pow_inv25519();
  test_powbatch_inv25519();
  test_nP_montgomery25519();
  test_nPbatch_montgomery25519();
  test_nG_merged25519();
  test_nG_montgomery25519();
  test_mGnP_ed25519();
  test_multiscalar_ed25519();
  test_dh_x25519();
#endif

  if (!ok) {
    printf("some tests failed\n");
    return 100;
  }
  printf("all tests succeeded\n");
  return 0;
}

