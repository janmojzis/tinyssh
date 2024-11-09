#include "randombytes.h"
#include "cleanup.h"
#include "crypto_hash_sha512.h"
#include "crypto_verify_32.h"
#include "ge25519.h"
#include "sc25519.h"
#include "crypto_sign_ed25519.h"

int crypto_sign_ed25519_tinyssh(unsigned char *sm, unsigned long long *smlen, const unsigned char *m, unsigned long long n, const unsigned char *skorig) {

    long long i;
    unsigned char nonce[64], hram[64], sk[64], pk[32];
    ge25519 R;

    /* compute secret key from seed sk = H(skorig), H = sha512 */
    crypto_hash_sha512(sk, skorig, 32);
    sk[0] &= 248;
    sk[31] &= 63;
    sk[31] |= 64;

    /* copy m to sm, copy secret key and public key */
    *smlen = n + 64;
    for (i = 31; i >= 0; --i) pk[i     ] = skorig[i + 32];
    for (i = n - 1; i >= 0; --i) sm[i + 64] = m[i];
    for (i = 31; i >= 0; --i) sm[i + 32] = sk[i + 32];

    /* get pseudorandom nonce = H(sk2, m) */
    crypto_hash_sha512(nonce, sm + 32, n + 32);
    sc25519_reduce(nonce);

    /* copy pk to sm */
    for (i = 31; i >= 0; --i) sm[i + 32] = pk[i];

    /* compute R */
    ge25519_scalarmult_base(R, nonce);
    ge25519_tobytes(sm, R);

    /* calculate hram = H(r, a, m) */
    crypto_hash_sha512(hram, sm, n + 64);
    sc25519_reduce(hram);

    /* compute S */
    sc25519_muladd(sm + 32, hram, sk, nonce);

    /* cleanup */
    cleanup(nonce); cleanup(hram); cleanup(sk); cleanup(pk); cleanup(R);
    return 0;
}

int crypto_sign_ed25519_tinyssh_open(unsigned char *m, unsigned long long *mlen, const unsigned char *sm, unsigned long long n, const unsigned char *pk) {

    long long i;
    unsigned char pkcopy[32], rcopy[32], scopy[32], hram[64], rcheck[32];
    ge25519 R, S, A;
    int ret = -1;

    /* check input */
    if (n < 64) goto fail;
    if (sm[63] & 224) goto fail;

    /* unpack pk */
    if (ge25519_frombytes_negate_vartime(A, pk) != 0) goto fail;

    /* copy pk, r, s */
    for (i = 0; i < 32; ++i) pkcopy[i] = pk[i];
    for (i = 0; i < 32; ++i) rcopy[i]  = sm[i];
    for (i = 0; i < 32; ++i) scopy[i]  = sm[i + 32];

    /* copy sm to m and copy pk to m */
    for (i = n - 1; i >= 0; --i) m[i] = sm[i];
    for (i = 0; i < 32; ++i) m[i + 32] = pkcopy[i];

    /* calculate hram = H(r, a, m) */
    crypto_hash_sha512(hram, m, n);
    sc25519_reduce(hram);

    /* compute R */
    ge25519_scalarmult(A, A, hram);
    ge25519_scalarmult_base(S, scopy);
    ge25519_add(R, S, A);

    /* check R */
    ge25519_tobytes(rcheck, R);
    if (crypto_verify_32(rcheck, rcopy) != 0) goto fail;

    /* copy message */
    n -= 64; *mlen = n;
    for (i = 0; i <  n; ++i) m[i] = m[i + 64];
    for (i = 0; i < 64; ++i) m[i + n] = 0;
    ret = 0;
    goto cleanup;

fail:
    for (i = 0; i < n; ++i) m[i] = 0;

cleanup:
    cleanup(pkcopy); cleanup(rcopy); cleanup(scopy);
    cleanup(hram); cleanup(rcheck);
    cleanup(R); cleanup(S); cleanup(A);
    return ret;
}

int crypto_sign_ed25519_tinyssh_keypair(unsigned char *pk, unsigned char *sk) {

    unsigned char h[64];
    ge25519 A;
    long long i;

    randombytes(sk, 32);
    crypto_hash_sha512(h, sk, 32);
    h[0] &= 248;
    h[31] &= 63;
    h[31] |= 64;

    ge25519_scalarmult_base(A, h);
    ge25519_tobytes(pk, A);

    for (i = 31; i >= 0; --i) sk[i + 32] = pk[i];
    cleanup(h); cleanup(A);
    return 0;
}
