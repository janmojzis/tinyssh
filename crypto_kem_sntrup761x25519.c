/*
20210314
20241211 - reformated using clang-format
20241215 - scalarmult_curve25519 -> dh_x25519
Jan Mojzis
Public domain.
*/

#include "cleanup.h"
#include "crypto_hash_sha512.h"
#include "crypto_dh_x25519.h"
#include "crypto_kem_sntrup761.h"
#include "crypto_kem_sntrup761x25519.h"

int crypto_kem_sntrup761x25519_tinyssh_enc(unsigned char *c, unsigned char *k,
                                           const unsigned char *pk) {

    unsigned char onetimesk[crypto_dh_x25519_SECRETKEYBYTES];
    unsigned char buf[crypto_kem_sntrup761_BYTES + crypto_dh_x25519_BYTES];

    /* sntrup761 */
    crypto_kem_sntrup761_enc(c, buf, pk);
    pk += crypto_kem_sntrup761_PUBLICKEYBYTES;
    c += crypto_kem_sntrup761_CIPHERTEXTBYTES;

    /* x25519 */
    crypto_dh_x25519_keypair(/*onetimepk*/ c, onetimesk);
    crypto_dh_x25519(buf + crypto_kem_sntrup761_BYTES, pk, onetimesk);

    /* hash together sntrup459176 KEM-key and x25519 shared secret */
    crypto_hash_sha512(k, buf, sizeof buf);

    /* cleanup */
    cleanup(buf);
    cleanup(onetimesk);

    return 0;
}

int crypto_kem_sntrup761x25519_tinyssh_dec(unsigned char *k,
                                           const unsigned char *c,
                                           const unsigned char *sk) {

    unsigned char buf[crypto_kem_sntrup761_BYTES + crypto_dh_x25519_BYTES];

    /* sntrup761 */
    crypto_kem_sntrup761_dec(buf, c, sk);
    sk += crypto_kem_sntrup761_SECRETKEYBYTES;
    c += crypto_kem_sntrup761_CIPHERTEXTBYTES;

    /* x25519 */
    crypto_dh_x25519(buf + crypto_kem_sntrup761_BYTES, c, sk);

    /* hash together sntrup459176 KEM-key and x25519 shared secret */
    crypto_hash_sha512(k, buf, sizeof buf);

    /* cleanup */
    cleanup(buf);

    return 0;
}

int crypto_kem_sntrup761x25519_tinyssh_keypair(unsigned char *pk,
                                               unsigned char *sk) {

    /* sntrup761 */
    crypto_kem_sntrup761_keypair(pk, sk);

    /* x25519 */
    crypto_dh_x25519_keypair(pk + crypto_kem_sntrup761_PUBLICKEYBYTES,
                             sk + crypto_kem_sntrup761_SECRETKEYBYTES);

    return 0;
}
