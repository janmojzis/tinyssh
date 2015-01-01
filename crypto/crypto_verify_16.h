#ifndef crypto_verify_16_H
#define crypto_verify_16_H

#define crypto_verify_16_tinynacl_BYTES 16
extern int crypto_verify_16_tinynacl(const unsigned char *, const unsigned char *);

#define crypto_verify_16 crypto_verify_16_tinynacl
#define crypto_verify_16_BYTES crypto_verify_16_tinynacl_BYTES
#define crypto_verify_16_IMPLEMENTATION "tinynacl"
#define crypto_verify_16_VERSION "-"

#endif
