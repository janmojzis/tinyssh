#ifndef crypto_hash_sha256_H
#define crypto_hash_sha256_H

#define crypto_hash_sha256_tinyssh_BYTES 32
extern int crypto_hash_sha256_tinyssh(unsigned char *,const unsigned char *,unsigned long long);

#define crypto_hash_sha256 crypto_hash_sha256_tinyssh
#define crypto_hash_sha256_BYTES crypto_hash_sha256_tinyssh_BYTES
#define crypto_hash_sha256_IMPLEMENTATION "tinyssh"
#define crypto_hash_sha256_VERSION "-"

#endif
