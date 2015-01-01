#include "crypto_uint32.h"
#include "crypto_stream.h"

typedef struct {
    crypto_uint32 input[16];
} ECRYPT_chacha20_amd64_xmm6_ctx;

extern void ECRYPT_chacha20_amd64_xmm6_encrypt_bytes(ECRYPT_chacha20_amd64_xmm6_ctx *, const unsigned char *, unsigned char *, crypto_uint32);

static crypto_uint32 load_littleendian(const unsigned char *x) {

    crypto_uint32 y = 0;
    long long i;

    for (i = 3; i >= 0; --i) y = (y << 8) | x[i];
    return y;
}

/* expand 32-byte k */
static const crypto_uint32 sigma[4] = {0x61707865, 0x3320646E, 0x79622D32, 0x6B206574};

int crypto_stream_xor(
          unsigned char *c,
    const unsigned char *m, unsigned long long l,
    const unsigned char *n,
    const unsigned char *k
) {

    long long i;
    unsigned char xspace[256 + 16];
    unsigned char *x = xspace;
    unsigned char cspace[sizeof(ECRYPT_chacha20_amd64_xmm6_ctx) + 16];
    ECRYPT_chacha20_amd64_xmm6_ctx *ctx = (ECRYPT_chacha20_amd64_xmm6_ctx *)cspace;

    x   += 15 & (-(long)x);
    ctx += 15 & (-(long)ctx);

    for (i = 0; i < 4; ++i) {
        ctx->input[ 0 + i] = sigma[i];
        ctx->input[ 4 + i] = load_littleendian(k + 4 * i);
        ctx->input[ 8 + i] = load_littleendian(k + 4 * i + 16);
    }
    for (i = 0; i < 2; ++i) {
        ctx->input[12 + i] = 0;
        ctx->input[14 + i] = load_littleendian(n + 4 * i);
    }

    while (l >= 256) {
        for (i = 0; i < 256; ++i) x[i] = m[i];
        ECRYPT_chacha20_amd64_xmm6_encrypt_bytes(ctx, x, x, 256);
        for (i = 0; i < 256; ++i) c[i] = x[i];

        l -= 256;
        c += 256;
        m += 256;
    }
    if (l > 0) {
        for (i = 0; i < l; ++i) x[i] = m[i];
        ECRYPT_chacha20_amd64_xmm6_encrypt_bytes(ctx, x, x, l);
        for (i = 0; i < l; ++i) c[i] = x[i];
    }
    for (i = 0; i < sizeof xspace; ++i) xspace[i] = 0;
    for (i = 0; i < sizeof cspace; ++i) cspace[i] = 0;
    return 0;
}

int crypto_stream(
          unsigned char *c, unsigned long long l,
    const unsigned char *n,
    const unsigned char *k
) {

    long long i;

    for (i = 0; i < l; ++i) c[i] = 0;
    return crypto_stream_xor(c, c, l, n, k);
}
