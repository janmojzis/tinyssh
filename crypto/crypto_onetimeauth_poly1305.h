#ifndef crypto_onetimeauth_poly1305_H
#define crypto_onetimeauth_poly1305_H

#define crypto_onetimeauth_poly1305_tinynacl_BYTES 16
#define crypto_onetimeauth_poly1305_tinynacl_KEYBYTES 32
extern int crypto_onetimeauth_poly1305_tinynacl(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_tinynacl_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#define crypto_onetimeauth_poly1305 crypto_onetimeauth_poly1305_tinynacl
#define crypto_onetimeauth_poly1305_verify crypto_onetimeauth_poly1305_tinynacl_verify
#define crypto_onetimeauth_poly1305_BYTES crypto_onetimeauth_poly1305_tinynacl_BYTES
#define crypto_onetimeauth_poly1305_KEYBYTES crypto_onetimeauth_poly1305_tinynacl_KEYBYTES
#define crypto_onetimeauth_poly1305_IMPLEMENTATION "tinynacl"
#define crypto_onetimeauth_poly1305_VERSION "-"

#endif
