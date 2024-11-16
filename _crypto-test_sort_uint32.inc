/* ----- sort/uint32, derived from supercop/crypto_sort/try.c */

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
static const char *sort_uint32_checksums[] = {
  "83fc714d7acc0375aa6657bd36d3324a6cbc503f68019345651be8a88534c3e2",
  "30921a0be2d73a185181f0ec842fa0fa73cd6e958fc03411d435f14a3fc64c89",
} ;

static void (*crypto_sort)(void *,long long);
#define crypto_sort_BYTES crypto_sort_uint32_BYTES

static void *storage_sort_uint32_x;
static unsigned char *test_sort_uint32_x;
static void *storage_sort_uint32_x2;
static unsigned char *test_sort_uint32_x2;

static void test_sort_uint32_impl(long long impl)
{
  unsigned char *x = test_sort_uint32_x;
  unsigned char *x2 = test_sort_uint32_x2;
  long long xlen;
  long long xwords;

  if (targetn && atol(targetn) != impl) return;
  crypto_sort = crypto_sort_uint32;
  for (long long checksumbig = 0;checksumbig < 2;++checksumbig) {
    long long loops = checksumbig ? 4096 : 1024;
    long long maxtest = checksumbig ? 4096 : 128;

    if (checksumbig && valgrind) break;
    checksum_clear();

    for (long long loop = 0;loop < loops;++loop) {
      xwords = myrandom() % (maxtest + 1);
      xlen = xwords*crypto_sort_BYTES;

      input_prepare(x2,x,xlen);
      endianness(x,xwords,crypto_sort_BYTES);
      secret(x,xlen);
      crypto_sort(x,xwords);
      public(x,xlen);
      endianness(x,xwords,crypto_sort_BYTES);
      checksum(x,xlen);
      output_compare(x2,x,xlen,"crypto_sort");

      double_canary(x2,x,xlen);
      endianness(x2,xwords,crypto_sort_BYTES);
      secret(x2,xlen);
      crypto_sort(x2,xwords);
      public(x2,xlen);
      endianness(x2,xwords,crypto_sort_BYTES);
      if (memcmp(x2,x,xlen) != 0) fail("failure: crypto_sort is nondeterministic\n");
    }
    checksum_expected(sort_uint32_checksums[checksumbig]);
  }
}

void test_sort_uint32(void)
{
  long long maxalloc = 0;
  if (targeto && strcmp(targeto,"sort")) return;
  if (targetp && strcmp(targetp,"uint32")) return;
  storage_sort_uint32_x = callocplus(crypto_sort_uint32_BYTES*4096);
  test_sort_uint32_x = aligned(storage_sort_uint32_x,crypto_sort_uint32_BYTES*4096);
  if (crypto_sort_uint32_BYTES*4096 > maxalloc) maxalloc = crypto_sort_uint32_BYTES*4096;
  storage_sort_uint32_x2 = callocplus(maxalloc);
  test_sort_uint32_x2 = aligned(storage_sort_uint32_x2,crypto_sort_uint32_BYTES*4096);

  for (long long offset = 0;offset < 1;++offset) {
    if (targetoffset && atol(targetoffset) != offset) continue;
    if (offset && valgrind) break;
    printf("sort_uint32 offset %lld\n",offset);
    forked(test_sort_uint32_impl,-1);
    ++test_sort_uint32_x;
    ++test_sort_uint32_x2;
  }
  free(storage_sort_uint32_x2);
  free(storage_sort_uint32_x);
}
#undef crypto_sort_BYTES

