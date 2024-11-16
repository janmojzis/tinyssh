/* ----- sign/ed25519, derived from supercop/crypto_sign/try.c */

/*
derived from djb work from lib25519/libntruprime
mj modifications:
- rename files to _crypto-test.c and _crypto_<>.<>.inc
- fix compiler warnings
- include crypto.h
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "crypto.h"

#define fail ((ok = 0),printf)
static const char *sign_ed25519_checksums[] = {
  "ce11fd7c1eac4dd0bc5eec49b26ad1e91aef696fae50ce377dbd806dc394da01",
  "2ed857f17c917a8185e6c296303a11772ae45683a5e7cb5b095489bad65fffde",
} ;

static int (*crypto_sign_keypair)(unsigned char *,unsigned char *);
static int (*crypto_sign)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
static int (*crypto_sign_open)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define crypto_sign_BYTES crypto_sign_ed25519_BYTES

static void *storage_sign_ed25519_p;
static unsigned char *test_sign_ed25519_p;
static void *storage_sign_ed25519_s;
static unsigned char *test_sign_ed25519_s;
static void *storage_sign_ed25519_m;
static unsigned char *test_sign_ed25519_m;
static void *storage_sign_ed25519_c;
static unsigned char *test_sign_ed25519_c;
static void *storage_sign_ed25519_t;
static unsigned char *test_sign_ed25519_t;
static void *storage_sign_ed25519_p2;
static unsigned char *test_sign_ed25519_p2;
static void *storage_sign_ed25519_s2;
static unsigned char *test_sign_ed25519_s2;
static void *storage_sign_ed25519_m2;
static unsigned char *test_sign_ed25519_m2;
static void *storage_sign_ed25519_c2;
static unsigned char *test_sign_ed25519_c2;
static void *storage_sign_ed25519_t2;
static unsigned char *test_sign_ed25519_t2;

static void test_sign_ed25519_impl(long long impl)
{
  unsigned char *p = test_sign_ed25519_p;
  unsigned char *s = test_sign_ed25519_s;
  unsigned char *m = test_sign_ed25519_m;
  unsigned char *c = test_sign_ed25519_c;
  unsigned char *t = test_sign_ed25519_t;
  unsigned char *p2 = test_sign_ed25519_p2;
  unsigned char *s2 = test_sign_ed25519_s2;
  unsigned char *m2 = test_sign_ed25519_m2;
  unsigned char *c2 = test_sign_ed25519_c2;
  unsigned char *t2 = test_sign_ed25519_t2;
  unsigned long long plen = crypto_sign_PUBLICKEYBYTES;
  unsigned long long slen = crypto_sign_SECRETKEYBYTES;
  unsigned long long mlen;
  unsigned long long clen;
  unsigned long long tlen;

  if (targetn && atol(targetn) != impl) return;
  crypto_sign_keypair = crypto_sign_ed25519_keypair;
  crypto_sign = crypto_sign_ed25519;
  crypto_sign_open = crypto_sign_ed25519_open;
  for (long long checksumbig = 0;checksumbig < 2;++checksumbig) {
    long long loops = checksumbig ? 64 : 8;
    long long maxtest = checksumbig ? 4096 : 128;

    checksum_clear();

    for (long long loop = 0;loop < loops;++loop) {
      int result;
      mlen = myrandom() % (maxtest + 1);

      output_prepare(p2,p,plen);
      output_prepare(s2,s,slen);
      crypto_sign_keypair(p,s);
      public(p,plen);
      public(s,slen);
      checksum(p,plen);
      checksum(s,slen);
      output_compare(p2,p,plen,"crypto_sign_keypair");
      output_compare(s2,s,slen,"crypto_sign_keypair");
      clen = mlen + crypto_sign_BYTES;
      output_prepare(c2,c,clen);
      input_prepare(m2,m,mlen);
      memcpy(s2,s,slen);
      double_canary(s2,s,slen);
      secret(m,mlen);
      secret(s,slen);
      crypto_sign(c,&clen,m,mlen,s);
      public(m,mlen);
      public(s,slen);
      public(c,clen);
      if (clen < mlen) fail("failure: crypto_sign returns smaller output than input\n");
      if (clen > mlen + crypto_sign_BYTES) fail("failure: crypto_sign returns more than crypto_sign_BYTES extra bytes\n");
      checksum(c,clen);
      output_compare(c2,c,clen,"crypto_sign");
      input_compare(m2,m,mlen,"crypto_sign");
      input_compare(s2,s,slen,"crypto_sign");
      tlen = clen;
      output_prepare(t2,t,tlen);
      memcpy(c2,c,clen);
      double_canary(c2,c,clen);
      memcpy(p2,p,plen);
      double_canary(p2,p,plen);
      public(c,clen);
      public(p,plen);
      result = crypto_sign_open(t,&tlen,c,clen,p);
      public(&result,sizeof result);
      if (result != 0) fail("failure: crypto_sign_open returns nonzero\n");
      public(c,clen);
      public(p,plen);
      public(t,tlen);
      if (tlen != mlen) fail("failure: crypto_sign_open does not match mlen\n");
      if (memcmp(t,m,mlen) != 0) fail("failure: crypto_sign_open does not match m\n");
      checksum(t,tlen);
      output_compare(t2,t,clen,"crypto_sign_open");
      input_compare(c2,c,clen,"crypto_sign_open");
      input_compare(p2,p,plen,"crypto_sign_open");

      double_canary(t2,t,tlen);
      double_canary(c2,c,clen);
      double_canary(p2,p,plen);
      public(c2,clen);
      public(p2,plen);
      result = crypto_sign_open(t2,&tlen,c2,clen,p2);
      public(&result,sizeof result);
      if (result != 0) fail("failure: crypto_sign_open returns nonzero\n");
      public(c2,clen);
      public(p2,plen);
      public(t2,tlen);
      if (memcmp(t2,t,tlen) != 0) fail("failure: crypto_sign_open is nondeterministic\n");

      double_canary(t2,t,tlen);
      double_canary(c2,c,clen);
      double_canary(p2,p,plen);
      public(c2,clen);
      public(p,plen);
      result = crypto_sign_open(c2,&tlen,c2,clen,p);
      public(&result,sizeof result);
      if (result != 0) fail("failure: crypto_sign_open with c=t overlap returns nonzero\n");
      public(c2,tlen);
      public(p,plen);
      if (memcmp(c2,t,tlen) != 0) fail("failure: crypto_sign_open does not handle c=t overlap\n");
      memcpy(c2,c,clen);
      public(c,clen);
      public(p2,plen);
      result = crypto_sign_open(p2,&tlen,c,clen,p2);
      public(&result,sizeof result);
      if (result != 0) fail("failure: crypto_sign_open with p=t overlap returns nonzero\n");
      public(p2,tlen);
      public(c,clen);
      if (memcmp(p2,t,tlen) != 0) fail("failure: crypto_sign_open does not handle p=t overlap\n");
      memcpy(p2,p,plen);

      c[myrandom() % clen] += 1 + (myrandom() % 255);
      if (crypto_sign_open(t,&tlen,c,clen,p) == 0)
        if ((tlen != mlen) || (memcmp(t,m,mlen) != 0))
          fail("failure: crypto_sign_open allows trivial forgeries\n");
      c[myrandom() % clen] += 1 + (myrandom() % 255);
      if (crypto_sign_open(t,&tlen,c,clen,p) == 0)
        if ((tlen != mlen) || (memcmp(t,m,mlen) != 0))
          fail("failure: crypto_sign_open allows trivial forgeries\n");
      c[myrandom() % clen] += 1 + (myrandom() % 255);
      if (crypto_sign_open(t,&tlen,c,clen,p) == 0)
        if ((tlen != mlen) || (memcmp(t,m,mlen) != 0))
          fail("failure: crypto_sign_open allows trivial forgeries\n");
    }
    checksum_expected(sign_ed25519_checksums[checksumbig]);
  }
}

void test_sign_ed25519(void)
{
  long long maxalloc = 0;
  if (targeto && strcmp(targeto,"sign")) return;
  if (targetp && strcmp(targetp,"ed25519")) return;
  storage_sign_ed25519_p = callocplus(4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  test_sign_ed25519_p = aligned(storage_sign_ed25519_p,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  if (4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES > maxalloc) maxalloc = 4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES;
  storage_sign_ed25519_s = callocplus(4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  test_sign_ed25519_s = aligned(storage_sign_ed25519_s,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  if (4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES > maxalloc) maxalloc = 4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES;
  storage_sign_ed25519_m = callocplus(4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  test_sign_ed25519_m = aligned(storage_sign_ed25519_m,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  if (4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES > maxalloc) maxalloc = 4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES;
  storage_sign_ed25519_c = callocplus(4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  test_sign_ed25519_c = aligned(storage_sign_ed25519_c,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  if (4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES > maxalloc) maxalloc = 4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES;
  storage_sign_ed25519_t = callocplus(4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  test_sign_ed25519_t = aligned(storage_sign_ed25519_t,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  if (4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES > maxalloc) maxalloc = 4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES;
  storage_sign_ed25519_p2 = callocplus(maxalloc);
  test_sign_ed25519_p2 = aligned(storage_sign_ed25519_p2,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  storage_sign_ed25519_s2 = callocplus(maxalloc);
  test_sign_ed25519_s2 = aligned(storage_sign_ed25519_s2,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  storage_sign_ed25519_m2 = callocplus(maxalloc);
  test_sign_ed25519_m2 = aligned(storage_sign_ed25519_m2,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  storage_sign_ed25519_c2 = callocplus(maxalloc);
  test_sign_ed25519_c2 = aligned(storage_sign_ed25519_c2,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);
  storage_sign_ed25519_t2 = callocplus(maxalloc);
  test_sign_ed25519_t2 = aligned(storage_sign_ed25519_t2,4096+crypto_sign_BYTES+crypto_sign_PUBLICKEYBYTES+crypto_sign_SECRETKEYBYTES);

  for (long long offset = 0;offset < 2;++offset) {
    if (targetoffset && atol(targetoffset) != offset) continue;
    if (offset && valgrind) break;
    printf("sign_ed25519 offset %lld\n",offset);
    forked(test_sign_ed25519_impl,-1);
    ++test_sign_ed25519_p;
    ++test_sign_ed25519_s;
    ++test_sign_ed25519_m;
    ++test_sign_ed25519_c;
    ++test_sign_ed25519_t;
    ++test_sign_ed25519_p2;
    ++test_sign_ed25519_s2;
    ++test_sign_ed25519_m2;
    ++test_sign_ed25519_c2;
    ++test_sign_ed25519_t2;
  }
  free(storage_sign_ed25519_t2);
  free(storage_sign_ed25519_c2);
  free(storage_sign_ed25519_m2);
  free(storage_sign_ed25519_s2);
  free(storage_sign_ed25519_p2);
  free(storage_sign_ed25519_t);
  free(storage_sign_ed25519_c);
  free(storage_sign_ed25519_m);
  free(storage_sign_ed25519_s);
  free(storage_sign_ed25519_p);
}
#undef crypto_sign_SECRETKEYBYTES
#undef crypto_sign_PUBLICKEYBYTES
#undef crypto_sign_BYTES

