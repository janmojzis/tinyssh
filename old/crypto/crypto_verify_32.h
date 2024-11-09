#ifndef crypto_verify_32_H
#define crypto_verify_32_H

#define crypto_verify_32_tinyssh_BYTES 32
extern int crypto_verify_32_tinyssh(const unsigned char *, const unsigned char *);

#define crypto_verify_32 crypto_verify_32_tinyssh
#define crypto_verify_32_BYTES crypto_verify_32_tinyssh_BYTES
#define crypto_verify_32_IMPLEMENTATION "tinyssh"
#define crypto_verify_32_VERSION "-"

#endif
