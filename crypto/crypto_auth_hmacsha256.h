#ifndef crypto_auth_hmacsha256_H
#define crypto_auth_hmacsha256_H

#define crypto_auth_hmacsha256_tinynacl_BYTES 32
#define crypto_auth_hmacsha256_tinynacl_KEYBYTES 32
extern int crypto_auth_hmacsha256_tinynacl(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *);
extern int crypto_auth_hmacsha256_tinynacl_verify(const unsigned char *, const unsigned char *, unsigned long long, const unsigned char *);

#define crypto_auth_hmacsha256 crypto_auth_hmacsha256_tinynacl
#define crypto_auth_hmacsha256_verify crypto_auth_hmacsha256_tinynacl_verify
#define crypto_auth_hmacsha256_BYTES crypto_auth_hmacsha256_tinynacl_BYTES
#define crypto_auth_hmacsha256_KEYBYTES crypto_auth_hmacsha256_tinynacl_KEYBYTES
#define crypto_auth_hmacsha256_IMPLEMENTATION "tinynacl"
#define crypto_auth_hmacsha256_VERSION "-"

#endif
