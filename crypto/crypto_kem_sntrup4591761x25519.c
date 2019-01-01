/*
20181225
Jan Mojzis
Public domain.
*/

#include "randombytes.h"
#include "cleanup.h"
#include "crypto_hash_sha512.h"
#include "crypto_scalarmult_curve25519.h"
#include "crypto_kem_sntrup4591761.h"
#include "crypto_kem_sntrup4591761x25519.h"

#define sntrup4591761_BYTES crypto_kem_sntrup4591761_BYTES
#define x25519_BYTES crypto_scalarmult_curve25519_BYTES
#define x25519_SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES
#define sx_BYTES sntrup4591761_BYTES + x25519_BYTES
#define sntrup4591761_PUBLICKEYBYTES crypto_kem_sntrup4591761_PUBLICKEYBYTES
#define sntrup4591761_SECRETKEYBYTES crypto_kem_sntrup4591761_SECRETKEYBYTES
#define sx_PUBLICKEYBYTES sntrup4591761_PUBLICKEYBYTES + x25519_BYTES
#define sx_SECRETKEYBYTES sntrup4591761_SECRETKEYBYTES + x25519_SCALARBYTES

static unsigned char returnmask(int x) {

    unsigned long long ret = 1ULL;
    ret <<= 8 * sizeof(int);
    ret -= (unsigned long long)(unsigned int)x;
    ret >>= 8 * sizeof(int);
    return ret - 1;
}

int crypto_kem_sntrup4591761x25519_tinynacl_enc(unsigned char *c,
                                                unsigned char *k,
                                                const unsigned char *pk) {

    int r = 0;
    long long i;
    unsigned char onetimesk[x25519_SCALARBYTES];
    unsigned char buf[sx_BYTES];
    unsigned char tmp[sx_BYTES];
    unsigned char b;

    /* sntrup4591761 */
    r |= crypto_kem_sntrup4591761_enc(c, buf, pk);
    pk += crypto_kem_sntrup4591761_PUBLICKEYBYTES;
    c += crypto_kem_sntrup4591761_CIPHERTEXTBYTES;

    /* x25519 */
    randombytes(onetimesk, sizeof onetimesk);
    r |= crypto_scalarmult_curve25519_base(/*onetimepk*/ c, onetimesk);
    r |= crypto_scalarmult_curve25519(buf + sntrup4591761_BYTES, onetimesk, pk);

    /* if something fails, fill the buffer with random data */
    randombytes(tmp, sizeof tmp);
    b = returnmask(r);
    for (i = 0; i < sx_BYTES; ++i) tmp[i] = b & (tmp[i] ^ buf[i]);
    for (i = 0; i < sx_BYTES; ++i) buf[i] ^= tmp[i];

    /* hash together sntrup459176 KEM-key and x25519 shared secret */
    crypto_hash_sha512(k, buf, sizeof buf);

    /* cleanup */
    cleanup(buf);
    cleanup(onetimesk);
    return r;
}

int crypto_kem_sntrup4591761x25519_tinynacl_dec(unsigned char *k,
                                                const unsigned char *c,
                                                const unsigned char *sk) {

    int r = 0;
    long long i;
    unsigned char buf[sx_BYTES];
    unsigned char tmp[sx_BYTES];
    unsigned char b;

    /* sntrup4591761 */
    r |= crypto_kem_sntrup4591761_dec(buf, c, sk);
    sk += crypto_kem_sntrup4591761_SECRETKEYBYTES;
    c += crypto_kem_sntrup4591761_CIPHERTEXTBYTES;

    /* x25519 */
    r |= crypto_scalarmult_curve25519(buf + sntrup4591761_BYTES, sk, c);

    /* if something fails, fill the buffer with random data */
    randombytes(tmp, sizeof tmp);
    b = returnmask(r);
    for (i = 0; i < sx_BYTES; ++i) tmp[i] = b & (tmp[i] ^ buf[i]);
    for (i = 0; i < sx_BYTES; ++i) buf[i] ^= tmp[i];

    /* hash together sntrup459176 KEM-key and x25519 shared secret */
    crypto_hash_sha512(k, buf, sizeof buf);

    /* cleanup */
    cleanup(buf);
    return r;
}

int crypto_kem_sntrup4591761x25519_tinynacl_keypair(unsigned char *pk,
                                                    unsigned char *sk) {

    int r = 0;
    long long i;
    unsigned char b;

    /* sntrup4591761 */
    r |= crypto_kem_sntrup4591761_keypair(pk, sk);

    /* x25519 */
    randombytes(sk + sntrup4591761_SECRETKEYBYTES, x25519_SCALARBYTES);
    r |= crypto_scalarmult_curve25519_base(pk + sntrup4591761_PUBLICKEYBYTES,
                                           sk + sntrup4591761_SECRETKEYBYTES);

    b = ~returnmask(r);
    for (i = 0; i < sx_PUBLICKEYBYTES; ++i) pk[i] &= b;
    for (i = 0; i < sx_SECRETKEYBYTES; ++i) sk[i] &= b;

    return r;
}
