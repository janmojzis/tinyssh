#include "crypto_uint32.h"
#include "uint32_pack_big.h"
#include "uint32_unpack_big.h"
#include "uint64_pack_big.h"

#define FOR(i,n) for (i = 0;i < n;++i)

static crypto_uint32 _R(crypto_uint32 x,int c) { return (x >> c) | (x << (32 - c)); }
static crypto_uint32 _Ch(crypto_uint32 x,crypto_uint32 y,crypto_uint32 z) { return (x & y) ^ (~x & z); }
static crypto_uint32 _Maj(crypto_uint32 x,crypto_uint32 y,crypto_uint32 z) { return (x & y) ^ (x & z) ^ (y & z); }
static crypto_uint32 _Sigma0(crypto_uint32 x) { return _R(x, 2) ^ _R(x,13) ^ _R(x,22); }
static crypto_uint32 _Sigma1(crypto_uint32 x) { return _R(x, 6) ^ _R(x,11) ^ _R(x,25); }
static crypto_uint32 _sigma0(crypto_uint32 x) { return _R(x, 7) ^ _R(x,18) ^ (x >> 3); }
static crypto_uint32 _sigma1(crypto_uint32 x) { return _R(x,17) ^ _R(x,19) ^ (x >> 10); }

static const crypto_uint32 _K[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

static int hashblocks(unsigned char *x,const unsigned char *m,unsigned long long n)
{
  crypto_uint32 z[8],b[8],a[8],w[16],t;
  int i,j;

  FOR(i,8) z[i] = a[i] = uint32_unpack_big(x + 4 * i);

  while (n >= 64) {
    FOR(i,16) w[i] = uint32_unpack_big(m + 4 * i);

    FOR(i,64) {
      FOR(j,8) b[j] = a[j];
      t = a[7] + _Sigma1(a[4]) + _Ch(a[4],a[5],a[6]) + _K[i] + w[i%16];
      b[7] = t + _Sigma0(a[0]) + _Maj(a[0],a[1],a[2]);
      b[3] += t;
      FOR(j,8) a[(j+1)%8] = b[j];
      if (i%16 == 15)
        FOR(j,16)
          w[j] += w[(j+9)%16] + _sigma0(w[(j+1)%16]) + _sigma1(w[(j+14)%16]);
    }

    FOR(i,8) { a[i] += z[i]; z[i] = a[i]; }

    m += 64;
    n -= 64;
  }

  FOR(i,8) uint32_pack_big(x+4*i,z[i]);

  FOR(i,8) z[i] = b[i] = a[i] = 0;
  FOR(i,16) w[i] = 0;

  return n;
}

static const unsigned char iv[32] = {
    0x6a, 0x09, 0xe6, 0x67, 0xbb, 0x67, 0xae, 0x85,
    0x3c, 0x6e, 0xf3, 0x72, 0xa5, 0x4f, 0xf5, 0x3a,
    0x51, 0x0e, 0x52, 0x7f, 0x9b, 0x05, 0x68, 0x8c,
    0x1f, 0x83, 0xd9, 0xab, 0x5b, 0xe0, 0xcd, 0x19
};


int sha256_auth(unsigned char *out,const unsigned char *m,unsigned long long n,const unsigned char *k)
{
  crypto_uint64 i,b = n;
  unsigned char x[128],h[32];

  FOR(i,32) h[i] = iv[i];
  FOR(i,64) x[i] = 0x36;
  FOR(i,32) x[i] ^= k[i];

  hashblocks(h,x,64);
  hashblocks(h,m,n);
  m += n;
  n &= 63;
  m -= n;

  FOR(i,128) x[i] = 0;
  FOR(i,n) x[i] = m[i];
  x[n] = 128;

  n = 128-64*(n<56);
  uint64_pack_big(x+n-8,512 + (b<<3));
  hashblocks(h,x,n);

  FOR(i,64) x[i]       = 0x5c;
  FOR(i,32) x[i]      ^= k[i];
  FOR(i,32) x[i + 64] ^= h[i];

  FOR(i,32) out[i] = iv[i];

  for (i = 32;i < 64;++i) x[64 + i] = 0;
  x[64 + 32] = 0x80;
  x[64 + 62] = 3;

  hashblocks(out,x,128);

  FOR(i,128) x[i] = 0;
  FOR(i,32) h[i] = 0;

  return 0;
}

int sha256_hash(unsigned char *out,const unsigned char *m,unsigned long long n)
{
  crypto_uint64 i,b = n;
  unsigned char x[128],h[32];

  FOR(i,32) h[i] = iv[i];

  hashblocks(h,m,n);
  m += n;
  n &= 63;
  m -= n;

  FOR(i,128) x[i] = 0;
  FOR(i,n) x[i] = m[i];
  x[n] = 128;

  n = 128-64*(n<56);
  uint64_pack_big(x+n-8,b<<3);
  hashblocks(h,x,n);

  FOR(i,32) out[i] = h[i];

  FOR(i,128) x[i] = 0;
  FOR(i,32) h[i] = 0;

  return 0;
}
