#ifndef crypto_onetimeauth_poly1305_H
#define crypto_onetimeauth_poly1305_H

#define crypto_onetimeauth_poly1305_tinyssh_BYTES 16
#define crypto_onetimeauth_poly1305_tinyssh_KEYBYTES 32
extern int crypto_onetimeauth_poly1305_tinyssh(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_onetimeauth_poly1305_tinyssh_verify(const unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);

#define crypto_onetimeauth_poly1305 crypto_onetimeauth_poly1305_tinyssh
#define crypto_onetimeauth_poly1305_verify crypto_onetimeauth_poly1305_tinyssh_verify
#define crypto_onetimeauth_poly1305_BYTES crypto_onetimeauth_poly1305_tinyssh_BYTES
#define crypto_onetimeauth_poly1305_KEYBYTES crypto_onetimeauth_poly1305_tinyssh_KEYBYTES
#define crypto_onetimeauth_poly1305_IMPLEMENTATION "tinyssh"
#define crypto_onetimeauth_poly1305_VERSION "-"

#endif
