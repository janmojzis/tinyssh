#ifndef crypto_verify_16_H
#define crypto_verify_16_H

#define crypto_verify_16_tinyssh_BYTES 16
extern int crypto_verify_16_tinyssh(const unsigned char *, const unsigned char *);

#define crypto_verify_16 crypto_verify_16_tinyssh
#define crypto_verify_16_BYTES crypto_verify_16_tinyssh_BYTES
#define crypto_verify_16_IMPLEMENTATION "tinyssh"
#define crypto_verify_16_VERSION "-"

#endif
