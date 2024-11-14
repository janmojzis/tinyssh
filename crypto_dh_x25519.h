#ifndef crypto_dh_x25519_H
#define crypto_dh_x25519_H

#define crypto_dh_x25519_tinyssh_BYTES 32
#define crypto_dh_x25519_tinyssh_PUBLICKEYBYTES 32
#define crypto_dh_x25519_tinyssh_SECRETKEYBYTES 32
extern int crypto_dh_x25519_tinyssh(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_dh_x25519_tinyssh_keypair(unsigned char *,unsigned char *);

#define crypto_dh_x25519 crypto_dh_x25519_tinyssh
#define crypto_dh_x25519_keypair crypto_dh_x25519_tinyssh_keypair
#define crypto_dh_x25519_BYTES crypto_dh_x25519_tinyssh_BYTES
#define crypto_dh_x25519_PUBLICKEYBYTES crypto_dh_x25519_tinyssh_PUBLICKEYBYTES
#define crypto_dh_x25519_SECRETKEYBYTES crypto_dh_x25519_tinyssh_SECRETKEYBYTES
#define crypto_dh_x25519_IMPLEMENTATION "tinyssh"
#define crypto_dh_x25519_VERSION "-"

#endif
