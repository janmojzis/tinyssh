#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "crypto.h"
#include "randombytes.h"
#include "sshcrypto.h"

static unsigned char a[32];
static unsigned char sk[256];
static unsigned char pk[256];
static unsigned char k[64];
static unsigned char m[512];
static unsigned char sm[512];
static unsigned long long smlen;
static unsigned long long mlen;
static double speed;

static unsigned char n[8];
static unsigned char space[10240];

static int flagtimeout;
static void timeout(int x) {
    flagtimeout = x = 1;
    signal(SIGALRM, timeout);
}

int main(void) {

    long long count;

    signal(SIGALRM, timeout);

    /* scalarmult test */
    randombytes(sk, crypto_scalarmult_curve25519_SCALARBYTES);
    crypto_scalarmult_curve25519_base(pk, sk);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_scalarmult_curve25519(k, sk, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_scalarmult_curve25519: %lld dh/s (%s)\n", count, crypto_scalarmult_curve25519_IMPLEMENTATION);

    randombytes(sk, crypto_scalarmult_nistp256_SCALARBYTES);
    crypto_scalarmult_nistp256_base(pk, sk);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_scalarmult_nistp256(k, sk, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_scalarmult_nistp256: %lld dh/s (%s)\n", count, crypto_scalarmult_nistp256_IMPLEMENTATION);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_scalarmult_nistp256_base(pk, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_scalarmult_nistp256_base: %lld dh/s (%s)\n", count, crypto_scalarmult_nistp256_IMPLEMENTATION);



    /* sign test */
    crypto_sign_ed25519_keypair(pk, sk);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_sign_ed25519(sm, &smlen, m + crypto_sign_ed25519_BYTES, sizeof(m) - crypto_sign_ed25519_BYTES, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_sign_ed25519: %lld sigs/s (%s)\n", count, crypto_sign_ed25519_IMPLEMENTATION);
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_sign_ed25519_open(m, &mlen, sm, sizeof sm, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_sign_ed25519_open: %lld sigs/s (%s)\n", count, crypto_sign_ed25519_IMPLEMENTATION);


    /* sign test */
    crypto_sign_nistp256ecdsa_keypair(pk, sk);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_sign_nistp256ecdsa(sm, &smlen, m + crypto_sign_nistp256ecdsa_BYTES, sizeof(m) - crypto_sign_nistp256ecdsa_BYTES, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_sign_nistp256ecdsa: %lld sigs/s (%s)\n", count, crypto_sign_nistp256ecdsa_IMPLEMENTATION);
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_sign_nistp256ecdsa_open(m, &mlen, sm, sizeof sm, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_sign_nistp256ecdsa_open: %lld sigs/s (%s)\n", count, crypto_sign_nistp256ecdsa_IMPLEMENTATION);


    /* stream test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_stream_chacha20_xor(space, space, sizeof space, n, k) != 0) return 111;
        ++count;
    }
    printf("crypto_stream_chacha20_xor: %lld MB/s (%s)\n", count/100, crypto_stream_chacha20_IMPLEMENTATION);

    /* aes128 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_core_aes128encrypt(space, k, k, 0) != 0) return 111;
        ++count;
    }
    speed = (double)count * 16 / 1000000;
    printf("crypto_core_aes128encrypt: %0.2f MB/s (%s)\n", speed, crypto_core_aes128encrypt_IMPLEMENTATION);

    /* aes256 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_core_aes256encrypt(space, k, k, 0) != 0) return 111;
        ++count;
    }
    speed = (double)count * 16 / 1000000;
    printf("crypto_core_aes256encrypt: %0.2f MB/s (%s)\n", speed, crypto_core_aes256encrypt_IMPLEMENTATION);

   /* stream test */
#ifdef crypto_stream_aes128ctr_IMPLEMENTATION
    alarm(1); flagtimeout = 0; count = 0;
    crypto_stream_aes128ctr_beforenm(kk, k);
    while (!flagtimeout) {
        if (crypto_stream_aes128ctr_xor_afternm(space, space, sizeof space, n, kk) != 0) return 111;
        ++count;
    }
    printf("crypto_stream_aes128ctr_xor: %lld MB/s (%s)\n", count/100, crypto_stream_aes128ctr_IMPLEMENTATION);
#endif

   /* stream test */
#ifdef crypto_stream_aes256ctr_IMPLEMENTATION
    alarm(1); flagtimeout = 0; count = 0;
    crypto_stream_aes256ctr_beforenm(kk, k);
    while (!flagtimeout) {
        if (crypto_stream_aes256ctr_xor_afternm(space, space, sizeof space, n, kk) != 0) return 111;
        ++count;
    }
    printf("crypto_stream_aes256ctr_xor: %lld MB/s (%s)\n", count/100, crypto_stream_aes256ctr_IMPLEMENTATION);
#endif

    /* onetimeauth test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_onetimeauth_poly1305(a, space, sizeof space, k) != 0) return 111;
        ++count;
    }
    printf("crypto_onetimeauth_poly1305: %lld MB/s (%s)\n", count/100, crypto_onetimeauth_poly1305_IMPLEMENTATION);


    /* onetimeauth test */
#ifdef crypto_onetimeauth_ghash_IMPLEMENTATION
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_onetimeauth_ghash(a, space, sizeof space, k) != 0) return 111;
        ++count;
    }
    printf("crypto_onetimeauth_ghash: %lld MB/s (%s)\n", count/100, crypto_onetimeauth_ghash_IMPLEMENTATION);
#endif

    /* auth test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_auth_hmacsha256(a, space, sizeof space, k) != 0) return 111;
        ++count;
    }
    printf("crypto_auth_hmacsha256: %lld MB/s (%s)\n", count/100, crypto_auth_hmacsha256_IMPLEMENTATION);

    /* sha test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_hash_sha512(a, space, sizeof space) != 0) return 111;
        ++count;
    }
    printf("crypto_hash_sha512: %lld MB/s (%s)\n", count/100, crypto_hash_sha512_IMPLEMENTATION);

    /* sha test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_hash_sha256(a, space, sizeof space) != 0) return 111;
        ++count;
    }
    printf("crypto_hash_sha256: %lld MB/s (%s)\n", count/100, crypto_hash_sha256_IMPLEMENTATION);

#ifdef crypto_hash_sha3512_IMPLEMENTATION
    /* sha3 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_hash_sha3512(a, space, sizeof space) != 0) return 111;
        ++count;
    }
    printf("crypto_hash_sha3512: %lld MB/s (%s)\n", count/100, crypto_hash_sha3512_IMPLEMENTATION);
#endif

    /* randombytes test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        randombytes(space, sizeof space);
        ++count;
    }
    printf("randombytes: %lld MB/s (%s)\n", count/100, randombytes_implementation);

    return 0;
}
