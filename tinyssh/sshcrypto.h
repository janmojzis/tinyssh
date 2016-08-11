#ifndef _SSHCRYPTO_H____
#define _SSHCRYPTO_H____

#include "buf.h"
#include "crypto.h"

/* crypto type */
#define sshcrypto_TYPEOLDCRYPTO 0x1 /* ecdsa-sha2-nistp256, ecdh-sha2-nistp256, aes256-ctr, hmac-sha2-256 */
#define sshcrypto_TYPENEWCRYPTO 0x2 /* ssh-ed25519, curve25519-sha256@libssh.org, chacha20-poly1305@openssh.com */
#define sshcrypto_TYPEPQCRYPTO  0x4 /* TODO, TODO, chacha20-poly1305@openssh.com */

/* kex - dh + hash */
#define sshcrypto_dh_PUBLICKEYMAX 64 + 1 /* space for nistp256            pk */
#define sshcrypto_dh_SECRETKEYMAX 32     /* space for nistp256/curve25519 sk */
#define sshcrypto_dh_MAX          32     /* space for nistp256/curve25519 k  */
#define sshcrypto_hash_MAX        64     /* space for sha512                 */

struct sshcrypto_kex {
    const char *name;
    int (*dh)(unsigned char *, unsigned char *, unsigned char *);
    int (*dh_keypair)(unsigned char *, unsigned char *);
    long long dh_publickeybytes;
    long long dh_secretkeybytes;
    long long dh_bytes;
    int (*hash)(unsigned char *, const unsigned char *, unsigned long long);
    long long hash_bytes;
    void (*buf_putsharedsecret)(struct buf *, const unsigned char *);
    void (*buf_putdhpk)(struct buf *, const unsigned char *);
    unsigned int cryptotype;
    int flagenabled;
};
extern struct sshcrypto_kex sshcrypto_kexs[];

extern const char *sshcrypto_kex_name;
extern int (*sshcrypto_dh)(unsigned char *, unsigned char *, unsigned char *);
extern int (*sshcrypto_dh_keypair)(unsigned char *, unsigned char *);
extern long long sshcrypto_dh_publickeybytes;
extern long long sshcrypto_dh_secretkeybytes;
extern long long sshcrypto_dh_bytes;
extern int (*sshcrypto_hash)(unsigned char *, const unsigned char *, unsigned long long);
extern long long sshcrypto_hash_bytes;
extern void (*sshcrypto_buf_putsharedsecret)(struct buf *, const unsigned char *);
extern void (*sshcrypto_buf_putdhpk)(struct buf *, const unsigned char *);

extern int sshcrypto_kex_select(const unsigned char *, long long, crypto_uint8 *);
extern void sshcrypto_kex_put(struct buf *);

#if defined(crypto_scalarmult_curve25519_BYTES) && defined(crypto_hash_sha256_BYTES)
/* sshcrypto_kex_curve25519.c */
extern int curve25519_dh(unsigned char *, unsigned char *, unsigned char *);
extern int curve25519_keypair(unsigned char *, unsigned char *);
extern void curve25519_putdhpk(struct buf *, const unsigned char *);
extern void curve25519_putsharedsecret(struct buf *, const unsigned char *);
#endif

#if defined(crypto_scalarmult_nistp256_BYTES) && defined(crypto_hash_sha256_BYTES)
/* sshcrypto_kex_nistp256.c */
extern int nistp256_dh(unsigned char *, unsigned char *, unsigned char *);
extern int nistp256_keypair(unsigned char *, unsigned char *);
extern void nistp256_putdhpk(struct buf *, const unsigned char *);
extern void nistp256_putsharedsecret(struct buf *, const unsigned char *);
#endif



/* key - sign */
#define sshcrypto_sign_PUBLICKEYMAX 64          /* space for nistp256ecdsa         pk  */
#define sshcrypto_sign_SECRETKEYMAX 64          /* space for nistp256ecdsa         sk  */
#define sshcrypto_sign_MAX          64          /* space for nistp256ecdsa/ed25519 sig */
#define sshcrypto_sign_BASE64PUBLICKEYMAX 141   /* space for nistp256ecdsa in base64 + 0-terminator */
#define sshcrypto_sign_BASE64PUBLICKEYMIN 69    /* space for ed25519 in base64 + 0-terminator */
#define sshcrypto_sign_NAMEMAX 20               /* space for string ecdsa-sha2-nistp256 + 0-terminator */

struct sshcrypto_key {
    const char *name;
    int (*sign)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
    int (*sign_open)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
    int (*sign_keypair)(unsigned char *, unsigned char *);
    unsigned char sign_publickey[sshcrypto_sign_PUBLICKEYMAX];
    long long sign_publickeybytes;
    long long sign_secretkeybytes;
    long long sign_bytes;
    const char *sign_publickeyfilename;
    const char *sign_secretkeyfilename;
    unsigned int cryptotype;
    int sign_flagserver;
    int sign_flagclient;
    void (*buf_putsignature)(struct buf *, const unsigned char *);
    void (*buf_putsignpk)(struct buf *, const unsigned char *);
    void (*buf_putsignpkbase64)(struct buf *, const unsigned char *);
    int (*parsesignature)(unsigned char *, const unsigned char *, long long);
    int (*parsesignpk)(unsigned char *, const unsigned char *, long long);
};
extern struct sshcrypto_key sshcrypto_keys[];


extern const char *sshcrypto_key_name;
extern int (*sshcrypto_sign)(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern unsigned char *sshcrypto_sign_publickey;
extern long long sshcrypto_sign_publickeybytes;
extern long long sshcrypto_sign_secretkeybytes;
extern long long sshcrypto_sign_bytes;
extern const char *sshcrypto_sign_secretkeyfilename;
extern void (*sshcrypto_buf_putsignature)(struct buf *, const unsigned char *);
extern void (*sshcrypto_buf_putsignpk)(struct buf *, const unsigned char *);

extern int sshcrypto_key_select(const unsigned char *, long long);
extern void sshcrypto_key_put(struct buf *);

#ifdef crypto_sign_ed25519_BYTES
/* sshcrypto_key_ed25519.c */
extern void ed25519_putsignature(struct buf *, const unsigned char *);
extern void ed25519_putsignpk(struct buf *, const unsigned char *);
extern void ed25519_putsignpkbase64(struct buf *, const unsigned char *);
extern int ed25519_parsesignpk(unsigned char *, const unsigned char *, long long);
extern int ed25519_parsesignature(unsigned char *, const unsigned char *, long long);
#endif

#ifdef crypto_sign_nistp256ecdsa_BYTES
/* sshcrypto_key_nistp256ecdsa.c */
extern void nistp256ecdsa_putsignature(struct buf *, const unsigned char *);
extern void nistp256ecdsa_putsignpk(struct buf *, const unsigned char *);
extern void nistp256ecdsa_putsignpkbase64(struct buf *, const unsigned char *);
extern int nistp256ecdsa_parsesignpk(unsigned char *, const unsigned char *, long long);
extern int nistp256ecdsa_parsesignature(unsigned char *, const unsigned char *, long long);
#endif


/* cipher + mac */
#define sshcrypto_cipher_KEYMAX 128     /* space for 2 x sha512 */

struct sshcrypto_cipher {
    const char *name;
    int (*stream_xor)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
    int (*auth)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
    long long stream_keybytes;
    long long cipher_blockbytes;
    long long auth_bytes;
    void (*packet_put)(struct buf *);
    int (*packet_get)(struct buf *);
    unsigned int cryptotype;
    int flagenabled;
};
extern struct sshcrypto_cipher sshcrypto_ciphers[];

extern const char *sshcrypto_cipher_name;
extern int (*sshcrypto_stream_xor)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int (*sshcrypto_auth)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern long long sshcrypto_stream_keybytes;
extern long long sshcrypto_cipher_blockbytes;
extern long long sshcrypto_auth_bytes;
extern void (*sshcrypto_packet_put)(struct buf *);
extern int (*sshcrypto_packet_get)(struct buf *);

extern int sshcrypto_cipher_select(const unsigned char *, long long);
extern int sshcrypto_cipher_macselect(const unsigned char *, long long);
extern void sshcrypto_cipher_put(struct buf *);
extern void sshcrypto_cipher_macput(struct buf *b);


/* from sshcrypto_cipher_chachapoly.c */
extern void chachapoly_packet_put(struct buf *);
extern int chachapoly_packet_get(struct buf *);

/* from sshcrypto_cipher_aesctr.c */
extern void aesctr_packet_put(struct buf *);
extern int aesctr_packet_get(struct buf *);
/* from sshcrypto_cipher_aesctr256.c */
extern int aesctr256_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);

/* init/purge */
extern void sshcrypto_init(void);
extern void sshcrypto_purge(void);

#endif
