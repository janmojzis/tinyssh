#ifndef crypto_hash_sha256_H
#define crypto_hash_sha256_H

#define crypto_hash_sha256_tinynacl_BYTES 32
extern int crypto_hash_sha256_tinynacl(unsigned char *,const unsigned char *,unsigned long long);

#define crypto_hash_sha256 crypto_hash_sha256_tinynacl
#define crypto_hash_sha256_BYTES crypto_hash_sha256_tinynacl_BYTES
#define crypto_hash_sha256_IMPLEMENTATION "tinynacl"
#define crypto_hash_sha256_VERSION "-"

#endif
