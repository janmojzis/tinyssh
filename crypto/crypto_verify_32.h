#ifndef crypto_verify_32_H
#define crypto_verify_32_H

#define crypto_verify_32_tinynacl_BYTES 32
extern int crypto_verify_32_tinynacl(const unsigned char *, const unsigned char *);

#define crypto_verify_32 crypto_verify_32_tinynacl
#define crypto_verify_32_BYTES crypto_verify_32_tinynacl_BYTES
#define crypto_verify_32_IMPLEMENTATION "tinynacl"
#define crypto_verify_32_VERSION "-"

#endif
