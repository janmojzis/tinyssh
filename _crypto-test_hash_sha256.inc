/* ----- hash/sha256, derived from supercop/crypto_hash/try.c */

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
static const char *hash_sha256_checksums[] = {
  "0fb5122d471d9310c4ff212c64a73cc985f0826484c24f34b56cb1d39534ba24",
  "55ff5e29282c4d7d192a20b427c000ec121fcda33dd96d53aa8857e3f2dd1469",
} ;

static int (*crypto_hash)(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hash_BYTES crypto_hash_sha256_BYTES

static void *storage_hash_sha256_h;
static unsigned char *test_hash_sha256_h;
static void *storage_hash_sha256_m;
static unsigned char *test_hash_sha256_m;
static void *storage_hash_sha256_h2;
static unsigned char *test_hash_sha256_h2;
static void *storage_hash_sha256_m2;
static unsigned char *test_hash_sha256_m2;

static void test_hash_sha256_impl(long long impl)
{
  unsigned char *h = test_hash_sha256_h;
  unsigned char *m = test_hash_sha256_m;
  unsigned char *h2 = test_hash_sha256_h2;
  unsigned char *m2 = test_hash_sha256_m2;
  long long hlen = crypto_hash_BYTES;
  long long mlen;

  if (targetn && atol(targetn) != impl) return;
  crypto_hash = crypto_hash_sha256;

  for (long long checksumbig = 0;checksumbig < 2;++checksumbig) {
    long long loops = checksumbig ? 512 : 64;
    long long maxtest = checksumbig ? 4096 : 128;

    checksum_clear();

    for (long long loop = 0;loop < loops;++loop) {
      mlen = myrandom() % (maxtest + 1);

      output_prepare(h2,h,hlen);
      input_prepare(m2,m,mlen);
      secret(m,mlen);
      crypto_hash(h,m,mlen);
      public(m,mlen);
      public(h,hlen);
      checksum(h,hlen);
      output_compare(h2,h,hlen,"crypto_hash");
      input_compare(m2,m,mlen,"crypto_hash");

      double_canary(h2,h,hlen);
      double_canary(m2,m,mlen);
      secret(m2,mlen);
      crypto_hash(h2,m2,mlen);
      public(m2,mlen);
      public(h2,hlen);
      if (memcmp(h2,h,hlen) != 0) fail("failure: crypto_hash is nondeterministic\n");

      double_canary(h2,h,hlen);
      double_canary(m2,m,mlen);
      secret(m2,mlen);
      crypto_hash(m2,m2,mlen);
      public(m2,hlen);
      if (memcmp(m2,h,hlen) != 0) fail("failure: crypto_hash does not handle m=h overlap\n");
      memcpy(m2,m,mlen);
    }
    checksum_expected(hash_sha256_checksums[checksumbig]);
  }
}

void test_hash_sha256(void)
{
  long long maxalloc = 0;
  if (targeto && strcmp(targeto,"hash")) return;
  if (targetp && strcmp(targetp,"sha256")) return;
  storage_hash_sha256_h = callocplus(crypto_hash_BYTES);
  test_hash_sha256_h = aligned(storage_hash_sha256_h,crypto_hash_BYTES);
  if (crypto_hash_BYTES > maxalloc) maxalloc = crypto_hash_BYTES;
  storage_hash_sha256_m = callocplus(4096+crypto_hash_BYTES);
  test_hash_sha256_m = aligned(storage_hash_sha256_m,4096+crypto_hash_BYTES);
  if (4096+crypto_hash_BYTES > maxalloc) maxalloc = 4096+crypto_hash_BYTES;
  storage_hash_sha256_h2 = callocplus(maxalloc);
  test_hash_sha256_h2 = aligned(storage_hash_sha256_h2,crypto_hash_BYTES);
  storage_hash_sha256_m2 = callocplus(maxalloc);
  test_hash_sha256_m2 = aligned(storage_hash_sha256_m2,4096+crypto_hash_BYTES);

  for (long long offset = 0;offset < 2;++offset) {
    if (targetoffset && atol(targetoffset) != offset) continue;
    if (offset && valgrind) break;
    printf("hash_sha256 offset %lld\n",offset);
    forked(test_hash_sha256_impl, -1);
    ++test_hash_sha256_h;
    ++test_hash_sha256_m;
    ++test_hash_sha256_h2;
    ++test_hash_sha256_m2;
  }
  free(storage_hash_sha256_m2);
  free(storage_hash_sha256_h2);
  free(storage_hash_sha256_m);
  free(storage_hash_sha256_h);
}
#undef crypto_hash_BYTES

