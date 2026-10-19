/*
Make the test server change hash length between the initial KEX and rekey.

The production server enables the same KEX algorithms for the lifetime of a
connection, so an OpenSSH client normally selects the same algorithm again.
This test-only wrapper restricts the initial offer to a SHA-512 KEX and all
later offers to a SHA-256 KEX while retaining the production implementation.
*/

#include <stdlib.h>

#define sshcrypto_kex_put sshcrypto_kex_put_original
#define sshcrypto_kex_select sshcrypto_kex_select_original
#include "../sshcrypto_kex.c"
#undef sshcrypto_kex_put
#undef sshcrypto_kex_select

static void enable_hash_bytes(long long hash_bytes) {
    long long i;

    for (i = 0; sshcrypto_kexs[i].name; ++i)
        sshcrypto_kexs[i].flagenabled =
            sshcrypto_kexs[i].hash_bytes == hash_bytes;
}

static int initial = 1;

static long long selected_hash_bytes(void) {
    const char *initial_hash = getenv("TINYSSH_TEST_INITIAL_HASH");
    long long hash_bytes = crypto_hash_sha512_BYTES;

    if (initial_hash && initial_hash[0] == '2')
        hash_bytes = crypto_hash_sha256_BYTES;
    if (!initial)
        hash_bytes = hash_bytes == crypto_hash_sha512_BYTES
                         ? crypto_hash_sha256_BYTES
                         : crypto_hash_sha512_BYTES;
    return hash_bytes;
}

void sshcrypto_kex_put(struct buf *b) {
    enable_hash_bytes(selected_hash_bytes());
    sshcrypto_kex_put_original(b);
}

int sshcrypto_kex_select(const unsigned char *buf, long long len,
                         crypto_uint8 *kex_guess, int flaginitial) {
    int result;

    enable_hash_bytes(selected_hash_bytes());
    result = sshcrypto_kex_select_original(buf, len, kex_guess, flaginitial);
    if (result) initial = 0;
    return result;
}
