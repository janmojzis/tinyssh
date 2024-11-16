/* ----- kem/sntrup761, derived from supercop/crypto_kem/try.c */

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
static const char *kem_sntrup761_checksums[] = {
  "4081ab3f61bac9aae91bff7f9b212855177591ec4427dff2a501be124c5bb97d",
  "730a572b64fa05a42bce7b1573edc04b70ef75909db309c012aa27f53f1f2f41",
} ;

static int (*crypto_kem_keypair)(unsigned char *,unsigned char *);
static int (*crypto_kem_enc)(unsigned char *,unsigned char *,const unsigned char *);
static int (*crypto_kem_dec)(unsigned char *,const unsigned char *,const unsigned char *);
#define crypto_kem_SECRETKEYBYTES crypto_kem_sntrup761_SECRETKEYBYTES
#define crypto_kem_PUBLICKEYBYTES crypto_kem_sntrup761_PUBLICKEYBYTES
#define crypto_kem_CIPHERTEXTBYTES crypto_kem_sntrup761_CIPHERTEXTBYTES
#define crypto_kem_BYTES crypto_kem_sntrup761_BYTES

static void *storage_kem_sntrup761_p;
static unsigned char *test_kem_sntrup761_p;
static void *storage_kem_sntrup761_s;
static unsigned char *test_kem_sntrup761_s;
static void *storage_kem_sntrup761_k;
static unsigned char *test_kem_sntrup761_k;
static void *storage_kem_sntrup761_c;
static unsigned char *test_kem_sntrup761_c;
static void *storage_kem_sntrup761_t;
static unsigned char *test_kem_sntrup761_t;
static void *storage_kem_sntrup761_p2;
static unsigned char *test_kem_sntrup761_p2;
static void *storage_kem_sntrup761_s2;
static unsigned char *test_kem_sntrup761_s2;
static void *storage_kem_sntrup761_k2;
static unsigned char *test_kem_sntrup761_k2;
static void *storage_kem_sntrup761_c2;
static unsigned char *test_kem_sntrup761_c2;
static void *storage_kem_sntrup761_t2;
static unsigned char *test_kem_sntrup761_t2;

static void test_kem_sntrup761_impl(long long impl)
{
  unsigned char *p = test_kem_sntrup761_p;
  unsigned char *s = test_kem_sntrup761_s;
  unsigned char *k = test_kem_sntrup761_k;
  unsigned char *c = test_kem_sntrup761_c;
  unsigned char *t = test_kem_sntrup761_t;
  unsigned char *p2 = test_kem_sntrup761_p2;
  unsigned char *s2 = test_kem_sntrup761_s2;
  unsigned char *k2 = test_kem_sntrup761_k2;
  unsigned char *c2 = test_kem_sntrup761_c2;
  unsigned char *t2 = test_kem_sntrup761_t2;
  long long plen = crypto_kem_PUBLICKEYBYTES;
  long long slen = crypto_kem_SECRETKEYBYTES;
  long long klen = crypto_kem_BYTES;
  long long clen = crypto_kem_CIPHERTEXTBYTES;
  long long tlen = crypto_kem_BYTES;

  if (targetn && atol(targetn) != impl) return;
  crypto_kem_keypair = crypto_kem_sntrup761_keypair;
  crypto_kem_enc = crypto_kem_sntrup761_enc;
  crypto_kem_dec = crypto_kem_sntrup761_dec;
  for (long long checksumbig = 0;checksumbig < 2;++checksumbig) {
    long long loops = checksumbig ? 64 : 8;

    if (checksumbig && valgrind) break;
    checksum_clear();

    for (long long loop = 0;loop < loops;++loop) {

      output_prepare(p2,p,plen);
      output_prepare(s2,s,slen);
      crypto_kem_keypair(p,s);
      public(p,plen);
      public(s,slen);
      checksum(p,plen);
      checksum(s,slen);
      output_compare(p2,p,plen,"crypto_kem_keypair");
      output_compare(s2,s,slen,"crypto_kem_keypair");
      output_prepare(c2,c,clen);
      output_prepare(k2,k,klen);
      memcpy(p2,p,plen);
      double_canary(p2,p,plen);
      secret(p,plen);
      crypto_kem_enc(c,k,p);
      public(p,plen);
      public(c,clen);
      public(k,klen);
      checksum(c,clen);
      checksum(k,klen);
      output_compare(c2,c,clen,"crypto_kem_enc");
      output_compare(k2,k,klen,"crypto_kem_enc");
      input_compare(p2,p,plen,"crypto_kem_enc");
      output_prepare(t2,t,tlen);
      memcpy(c2,c,clen);
      double_canary(c2,c,clen);
      memcpy(s2,s,slen);
      double_canary(s2,s,slen);
      secret(c,clen);
      secret(s,slen);
      crypto_kem_dec(t,c,s);
      public(c,clen);
      public(s,slen);
      public(t,tlen);
      if (memcmp(t,k,klen) != 0) fail("failure: crypto_kem_dec does not match k\n");
      checksum(t,tlen);
      output_compare(t2,t,tlen,"crypto_kem_dec");
      input_compare(c2,c,clen,"crypto_kem_dec");
      input_compare(s2,s,slen,"crypto_kem_dec");

      double_canary(t2,t,tlen);
      double_canary(c2,c,clen);
      double_canary(s2,s,slen);
      secret(c2,clen);
      secret(s2,slen);
      crypto_kem_dec(t2,c2,s2);
      public(c2,clen);
      public(s2,slen);
      public(t2,tlen);
      if (memcmp(t2,t,tlen) != 0) fail("failure: crypto_kem_dec is nondeterministic\n");

      double_canary(t2,t,tlen);
      double_canary(c2,c,clen);
      double_canary(s2,s,slen);
      secret(c2,clen);
      secret(s,slen);
      crypto_kem_dec(c2,c2,s);
      public(c2,tlen);
      public(s,slen);
      if (memcmp(c2,t,tlen) != 0) fail("failure: crypto_kem_dec does not handle c=t overlap\n");
      memcpy(c2,c,clen);
      secret(c,clen);
      secret(s2,slen);
      crypto_kem_dec(s2,c,s2);
      public(s2,tlen);
      public(c,clen);
      if (memcmp(s2,t,tlen) != 0) fail("failure: crypto_kem_dec does not handle s=t overlap\n");
      memcpy(s2,s,slen);

      c[myrandom() % clen] += 1 + (myrandom() % 255);
      crypto_kem_dec(t,c,s);
      checksum(t,tlen);
      c[myrandom() % clen] += 1 + (myrandom() % 255);
      crypto_kem_dec(t,c,s);
      checksum(t,tlen);
      c[myrandom() % clen] += 1 + (myrandom() % 255);
      crypto_kem_dec(t,c,s);
      checksum(t,tlen);
    }
    checksum_expected(kem_sntrup761_checksums[checksumbig]);
  }
}

void test_kem_sntrup761(void)
{
  long long maxalloc = 0;
  if (targeto && strcmp(targeto,"kem")) return;
  if (targetp && strcmp(targetp,"sntrup761")) return;
  storage_kem_sntrup761_p = callocplus(crypto_kem_PUBLICKEYBYTES);
  test_kem_sntrup761_p = aligned(storage_kem_sntrup761_p,crypto_kem_PUBLICKEYBYTES);
  if (crypto_kem_PUBLICKEYBYTES > maxalloc) maxalloc = crypto_kem_PUBLICKEYBYTES;
  storage_kem_sntrup761_s = callocplus(crypto_kem_SECRETKEYBYTES);
  test_kem_sntrup761_s = aligned(storage_kem_sntrup761_s,crypto_kem_SECRETKEYBYTES);
  if (crypto_kem_SECRETKEYBYTES > maxalloc) maxalloc = crypto_kem_SECRETKEYBYTES;
  storage_kem_sntrup761_k = callocplus(crypto_kem_BYTES);
  test_kem_sntrup761_k = aligned(storage_kem_sntrup761_k,crypto_kem_BYTES);
  if (crypto_kem_BYTES > maxalloc) maxalloc = crypto_kem_BYTES;
  storage_kem_sntrup761_c = callocplus(crypto_kem_CIPHERTEXTBYTES);
  test_kem_sntrup761_c = aligned(storage_kem_sntrup761_c,crypto_kem_CIPHERTEXTBYTES);
  if (crypto_kem_CIPHERTEXTBYTES > maxalloc) maxalloc = crypto_kem_CIPHERTEXTBYTES;
  storage_kem_sntrup761_t = callocplus(crypto_kem_BYTES);
  test_kem_sntrup761_t = aligned(storage_kem_sntrup761_t,crypto_kem_BYTES);
  if (crypto_kem_BYTES > maxalloc) maxalloc = crypto_kem_BYTES;
  storage_kem_sntrup761_p2 = callocplus(maxalloc);
  test_kem_sntrup761_p2 = aligned(storage_kem_sntrup761_p2,crypto_kem_PUBLICKEYBYTES);
  storage_kem_sntrup761_s2 = callocplus(maxalloc);
  test_kem_sntrup761_s2 = aligned(storage_kem_sntrup761_s2,crypto_kem_SECRETKEYBYTES);
  storage_kem_sntrup761_k2 = callocplus(maxalloc);
  test_kem_sntrup761_k2 = aligned(storage_kem_sntrup761_k2,crypto_kem_BYTES);
  storage_kem_sntrup761_c2 = callocplus(maxalloc);
  test_kem_sntrup761_c2 = aligned(storage_kem_sntrup761_c2,crypto_kem_CIPHERTEXTBYTES);
  storage_kem_sntrup761_t2 = callocplus(maxalloc);
  test_kem_sntrup761_t2 = aligned(storage_kem_sntrup761_t2,crypto_kem_BYTES);

  for (long long offset = 0;offset < 2;++offset) {
    if (targetoffset && atol(targetoffset) != offset) continue;
    if (offset && valgrind) break;
    printf("kem_sntrup761 offset %lld\n",offset);
    forked(test_kem_sntrup761_impl, -1);
    ++test_kem_sntrup761_p;
    ++test_kem_sntrup761_s;
    ++test_kem_sntrup761_k;
    ++test_kem_sntrup761_c;
    ++test_kem_sntrup761_t;
    ++test_kem_sntrup761_p2;
    ++test_kem_sntrup761_s2;
    ++test_kem_sntrup761_k2;
    ++test_kem_sntrup761_c2;
    ++test_kem_sntrup761_t2;
  }
  free(storage_kem_sntrup761_t2);
  free(storage_kem_sntrup761_c2);
  free(storage_kem_sntrup761_k2);
  free(storage_kem_sntrup761_s2);
  free(storage_kem_sntrup761_p2);
  free(storage_kem_sntrup761_t);
  free(storage_kem_sntrup761_c);
  free(storage_kem_sntrup761_k);
  free(storage_kem_sntrup761_s);
  free(storage_kem_sntrup761_p);
}
#undef crypto_kem_SECRETKEYBYTES
#undef crypto_kem_PUBLICKEYBYTES
#undef crypto_kem_CIPHERTEXTBYTES
#undef crypto_kem_BYTES

