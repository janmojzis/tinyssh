#ifndef crypto_hash_sha512_H
#define crypto_hash_sha512_H

#define crypto_hash_sha512_tinynacl_BYTES 64
extern int crypto_hash_sha512_tinynacl(unsigned char *, const unsigned char *, unsigned long long);

#define crypto_hash_sha512 crypto_hash_sha512_tinynacl
#define crypto_hash_sha512_BYTES crypto_hash_sha512_tinynacl_BYTES
#define crypto_hash_sha512_IMPLEMENTATION "tinynacl"
#define crypto_hash_sha512_VERSION "-"

#endif
