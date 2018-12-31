#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include "crypto.h"
#include "randombytes.h"

static unsigned char sk[2560];
static unsigned char pk[2560];
static unsigned char c[2560];
static unsigned char a[16];
static unsigned char n[8];
static unsigned char k[32];
static unsigned char h[64];
static unsigned char space[10240];
static unsigned char m[51200];
static unsigned char sm[51200];
static unsigned long long smlen;
static unsigned long long mlen;

static int flagtimeout;
static void timeout(int x) {
    flagtimeout = x = 1;
    signal(SIGALRM, timeout);
}

int main(void) {

    long long count;

    signal(SIGALRM, timeout);


#ifdef crypto_hash_sha512_IMPLEMENTATION
    /* sha512 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_hash_sha512(h, space, sizeof space) != 0) return 111;
        ++count;
    }
    printf("crypto_hash_sha512: %lld MB/s (%s)\n", count/100, crypto_hash_sha512_IMPLEMENTATION);
#endif

#ifdef crypto_hash_sha256_IMPLEMENTATION
    /* sha256 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_hash_sha256(h, space, sizeof space) != 0) return 111;
        ++count;
    }
    printf("crypto_hash_sha256: %lld MB/s (%s)\n", count/100, crypto_hash_sha256_IMPLEMENTATION);
#endif

#ifdef crypto_stream_chacha20_IMPLEMENTATION
	/* chacha20 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_stream_chacha20_xor(space, space, sizeof space, n, k) != 0) return 111;
        ++count;
    }
    printf("crypto_stream_chacha20_xor: %lld MB/s (%s)\n", count/100, crypto_stream_chacha20_IMPLEMENTATION);
#endif

#ifdef crypto_onetimeauth_poly1305_IMPLEMENTATION
    /* onetimeauth test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_onetimeauth_poly1305(a, space, sizeof space, k) != 0) return 111;
        ++count;
    }
    printf("crypto_onetimeauth_poly1305: %lld MB/s (%s)\n", count/100, crypto_onetimeauth_poly1305_IMPLEMENTATION);
#endif

#ifdef crypto_sign_ed25519_IMPLEMENTATION
    /* ed25519 test */
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
    	if (crypto_sign_ed25519_keypair(pk, sk) != 0) return 111;
		++count;
	}
    printf("crypto_sign_ed25519_keyipair: %lld keypairs/s (%s)\n", count, crypto_sign_ed25519_IMPLEMENTATION);

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
#endif

#ifdef crypto_kem_sntrup4591761_IMPLEMENTATION
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761_keypair(pk, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761_keypair: %lld keypairs/s (%s)\n", count, crypto_kem_sntrup4591761_IMPLEMENTATION);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761_enc(c, k, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761_enc: %lld encryptions/s (%s)\n", count, crypto_kem_sntrup4591761_IMPLEMENTATION);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761_dec(k, c, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761_dec: %lld decryptions/s (%s)\n", count, crypto_kem_sntrup4591761_IMPLEMENTATION);
#endif

#ifdef crypto_kem_sntrup4591761x25519_IMPLEMENTATION
    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761x25519_keypair(pk, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761x25519_keypair: %lld keypairs/s (%s)\n", count, crypto_kem_sntrup4591761x25519_IMPLEMENTATION);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761x25519_enc(c, k, pk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761x25519_enc: %lld encryptions/s (%s)\n", count, crypto_kem_sntrup4591761x25519_IMPLEMENTATION);

    alarm(1); flagtimeout = 0; count = 0;
    while (!flagtimeout) {
        if (crypto_kem_sntrup4591761x25519_dec(k, c, sk) != 0) return 111;
        ++count;
    }
    printf("crypto_kem_sntrup4591761x25519_dec: %lld decryptions/s (%s)\n", count, crypto_kem_sntrup4591761x25519_IMPLEMENTATION);
#endif

	return 0;
}
