#ifndef crypto_sign_ed25519_H
#define crypto_sign_ed25519_H

#define crypto_sign_ed25519_tinyssh_SECRETKEYBYTES 64
#define crypto_sign_ed25519_tinyssh_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_tinyssh_BYTES 64
extern int crypto_sign_ed25519_tinyssh(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tinyssh_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tinyssh_keypair(unsigned char *,unsigned char *);

#define crypto_sign_ed25519 crypto_sign_ed25519_tinyssh
#define crypto_sign_ed25519_open crypto_sign_ed25519_tinyssh_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_tinyssh_keypair
#define crypto_sign_ed25519_BYTES crypto_sign_ed25519_tinyssh_BYTES
#define crypto_sign_ed25519_PUBLICKEYBYTES crypto_sign_ed25519_tinyssh_PUBLICKEYBYTES
#define crypto_sign_ed25519_SECRETKEYBYTES crypto_sign_ed25519_tinyssh_SECRETKEYBYTES
#define crypto_sign_ed25519_IMPLEMENTATION "tinyssh"
#define crypto_sign_ed25519_VERSION "-"

#endif
