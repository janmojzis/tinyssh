/*
- based on tweetnacl 20140427 (http://tweetnacl.cr.yp.to/software.html)
*/

#include "crypto_int64.h"
#include "crypto_uint32.h"
#include "crypto_uint64.h"
#include "cleanup.h"
#include "sc25519.h"

#define FOR(i,n) for (i = 0;i < n;++i)

static const crypto_uint64 L[32] = {0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7, 0xa2, 0xde, 0xf9, 0xde, 0x14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x10};

static void modL(unsigned char *r,crypto_int64 x[64])
{
  crypto_int64 carry,i,j;
  for (i = 63;i >= 32;--i) {
    carry = 0;
    for (j = i - 32;j < i - 12;++j) {
      x[j] += carry - 16 * x[i] * L[j - (i - 32)];
      carry = (x[j] + 128) >> 8;
      x[j] -= carry << 8;
    }
    x[j] += carry;
    x[i] = 0;
  }
  carry = 0;
  FOR(j,32) {
    x[j] += carry - (x[31] >> 4) * L[j];
    carry = x[j] >> 8;
    x[j] &= 255;
  }
  FOR(j,32) x[j] -= carry * L[j];
  FOR(i,32) {
    x[i+1] += x[i] >> 8;
    r[i] = x[i] & 255;
  }
}

void sc25519_reduce(unsigned char *s) {

    crypto_int64 t[64], i;

    for (i = 0; i < 64; ++i) t[i] = s[i];
    for (i = 0; i < 64; ++i) s[i] = 0;
    modL(s, t);

    cleanup(t);
}

void sc25519_muladd(unsigned char *s, const unsigned char *a, const unsigned char *b, const unsigned char *c) {

    crypto_int64 t[64], i, j;


    for (i = 0; i < 64; ++i) t[i] = 0;

    for (i = 0; i < 32; ++i) for (j = 0; j < 32; ++j) {
        t[i + j] += (crypto_int64)a[i] * (crypto_int64)b[j];
    }

    for (i = 0; i < 32; ++i) t[i] += c[i];
    modL(s, t);

    cleanup(t);
}
