#ifndef crypto_sign_nistp256ecdsa_H
#define crypto_sign_nistp256ecdsa_H

#define crypto_sign_nistp256ecdsa_tinynacl_SECRETKEYBYTES 64
#define crypto_sign_nistp256ecdsa_tinynacl_PUBLICKEYBYTES 64
#define crypto_sign_nistp256ecdsa_tinynacl_BYTES 64
extern int crypto_sign_nistp256ecdsa_tinynacl(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_nistp256ecdsa_tinynacl_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_nistp256ecdsa_tinynacl_keypair(unsigned char *,unsigned char *);

#define crypto_sign_nistp256ecdsa crypto_sign_nistp256ecdsa_tinynacl
#define crypto_sign_nistp256ecdsa_open crypto_sign_nistp256ecdsa_tinynacl_open
#define crypto_sign_nistp256ecdsa_keypair crypto_sign_nistp256ecdsa_tinynacl_keypair
#define crypto_sign_nistp256ecdsa_BYTES crypto_sign_nistp256ecdsa_tinynacl_BYTES
#define crypto_sign_nistp256ecdsa_PUBLICKEYBYTES crypto_sign_nistp256ecdsa_tinynacl_PUBLICKEYBYTES
#define crypto_sign_nistp256ecdsa_SECRETKEYBYTES crypto_sign_nistp256ecdsa_tinynacl_SECRETKEYBYTES
#define crypto_sign_nistp256ecdsa_IMPLEMENTATION "tinynacl"
#define crypto_sign_nistp256ecdsa_VERSION "-"

#endif
