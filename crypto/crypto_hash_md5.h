#ifndef crypto_hash_md5_H
#define crypto_hash_md5_H

#define crypto_hash_md5_tinynacl_BYTES 16
extern int crypto_hash_md5_tinynacl(unsigned char *,const unsigned char *,unsigned long long);

#define crypto_hash_md5 crypto_hash_md5_tinynacl
#define crypto_hash_md5_BYTES crypto_hash_md5_tinynacl_BYTES
#define crypto_hash_md5_IMPLEMENTATION "tinynacl"
#define crypto_hash_md5_VERSION "-"

#endif
