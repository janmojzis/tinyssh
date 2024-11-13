/* ----- verify, derived from supercop/crypto_verify/try.c */

/*
derived from djb work from lib25519/libntruprime
mj modifications:
- rename files to _crypto_-test.c and _crypto_<>.<>.inc
- fix compiler warnings
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "crypto_verify_32.h"

static unsigned char *test_verify_32_x;
static unsigned char *test_verify_32_y;

static void test_verify_32_check(void)
{
  unsigned char *x = test_verify_32_x;
  unsigned char *y = test_verify_32_y;
  int r;

  secret(x,32);
  secret(y,32);
  r = crypto_verify_32(x,y);
  public(x,32);
  public(y,32);
  public(&r,sizeof r);

  if (r == 0) {
    if (memcmp(x,y,32))
      fail("failure: different strings pass verify\n");
  } else if (r == -1) {
    if (!memcmp(x,y,32))
      fail("failure: equal strings fail verify\n");
  } else {
    fail("failure: weird return value\n");
  }
}

void test_verify_32_impl(long long impl)
{
  unsigned char *x = test_verify_32_x;
  unsigned char *y = test_verify_32_y;

  if (targetn && atol(targetn) != impl) return;

  randombytes(x,32);
  randombytes(y,32);
  test_verify_32_check();
  memcpy(y,x,32);
  test_verify_32_check();
  y[myrandom() % 32] = myrandom();
  test_verify_32_check();
  y[myrandom() % 32] = myrandom();
  test_verify_32_check();
  y[myrandom() % 32] = myrandom();
  test_verify_32_check();
}

static void test_verify_32(void)
{
  if (targeto && strcmp(targeto,"verify")) return;
  if (targetp && strcmp(targetp,"32")) return;

  test_verify_32_x = callocplus(32);
  test_verify_32_y = callocplus(32);

  for (long long offset = 0;offset < 2;++offset) {
    if (targetoffset && atol(targetoffset) != offset) continue;
    if (offset && valgrind) break;
    printf("verify_32 offset %lld\n",offset);
    forked(test_verify_32_impl,-1);
    ++test_verify_32_x;
    ++test_verify_32_y;
  }
}
