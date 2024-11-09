#ifndef crypto_hash_sha512_H
#define crypto_hash_sha512_H

#define crypto_hash_sha512_tinyssh_BYTES 64
extern int crypto_hash_sha512_tinyssh(unsigned char *, const unsigned char *, unsigned long long);

#define crypto_hash_sha512 crypto_hash_sha512_tinyssh
#define crypto_hash_sha512_BYTES crypto_hash_sha512_tinyssh_BYTES
#define crypto_hash_sha512_IMPLEMENTATION "tinyssh"
#define crypto_hash_sha512_VERSION "-"

#endif
