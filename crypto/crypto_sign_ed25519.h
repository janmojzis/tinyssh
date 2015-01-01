#ifndef crypto_sign_ed25519_H
#define crypto_sign_ed25519_H

#define crypto_sign_ed25519_tinynacl_SECRETKEYBYTES 64
#define crypto_sign_ed25519_tinynacl_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_tinynacl_BYTES 64
extern int crypto_sign_ed25519_tinynacl(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tinynacl_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tinynacl_keypair(unsigned char *,unsigned char *);

#define crypto_sign_ed25519 crypto_sign_ed25519_tinynacl
#define crypto_sign_ed25519_open crypto_sign_ed25519_tinynacl_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_tinynacl_keypair
#define crypto_sign_ed25519_BYTES crypto_sign_ed25519_tinynacl_BYTES
#define crypto_sign_ed25519_PUBLICKEYBYTES crypto_sign_ed25519_tinynacl_PUBLICKEYBYTES
#define crypto_sign_ed25519_SECRETKEYBYTES crypto_sign_ed25519_tinynacl_SECRETKEYBYTES
#define crypto_sign_ed25519_IMPLEMENTATION "tinynacl"
#define crypto_sign_ed25519_VERSION "-"

#endif
