#ifndef _SSHCRYPTO_H____
#define _SSHCRYPTO_H____

#include "buf.h"
#include "crypto.h"

/* crypto type */
#define sshcrypto_TYPEOLDCRYPTO 0x1 /* ecdsa-sha2-nistp256, ecdh-sha2-nistp256, aes128-ctr, aes256-ctr, hmac-sha2-256 */
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

/* key - sign */
#define sshcrypto_sign_PUBLICKEYMAX 64          /* space for nistp256ecdsa         pk  */
#define sshcrypto_sign_SECRETKEYMAX 64          /* space for nistp256ecdsa         sk  */
#define sshcrypto_sign_MAX          64          /* space for nistp256ecdsa/ed25519 sig */
#define sshcrypto_sign_BASE64PUBLICKEYMAX 141   /* space for nistp256ecdsa in base64 + 0-terminator */
#define sshcrypto_sign_BASE64PUBLICKEYMIN 69    /* space for ed25519 in base64 + 0-terminator */

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


/* cipher + mac */
#define sshcrypto_cipher_KEYMAX 128     /* space for 2 x sha512 */

#define sshcrypto_cipher_BEFORENMMAX sshcrypto_cipher_KEYMAX
#if defined(crypto_stream_aes128ctr_BEFORENMBYTES) && crypto_stream_aes128ctr_BEFORENMBYTES > sshcrypto_cipher_BEFORENMMAX
#undef sshcrypto_cipher_BEFORENMMAX
#define sshcrypto_cipher_BEFORENMMAX crypto_stream_aes128ctr_BEFORENMBYTES
#endif
#if defined(crypto_stream_aes256ctr_BEFORENMBYTES) && crypto_stream_aes256ctr_BEFORENMBYTES > sshcrypto_cipher_BEFORENMMAX
#undef sshcrypto_cipher_BEFORENMMAX
#define sshcrypto_cipher_BEFORENMMAX crypto_stream_aes256ctr_BEFORENMBYTES
#endif

struct sshcrypto_cipher {
    const char *name;
    const char *macname;
    int (*stream_xor)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
    int (*stream_beforenm)(unsigned char *,const unsigned char *);
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
extern const char *sshcrypto_cipher_macname;
extern int (*sshcrypto_stream_xor)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *,const unsigned char *);
extern int (*sshcrypto_stream_beforenm)(unsigned char *,const unsigned char *);
extern int (*sshcrypto_auth)(unsigned char *,const unsigned char *,unsigned long long,const unsigned char *);
extern long long sshcrypto_stream_keybytes;
extern long long sshcrypto_cipher_blockbytes;
extern long long sshcrypto_auth_bytes;
extern void (*sshcrypto_packet_put)(struct buf *);
extern int (*sshcrypto_packet_get)(struct buf *);

extern int sshcrypto_cipher_select(const unsigned char *, long long);
extern void sshcrypto_cipher_put(struct buf *);
extern void sshcrypto_cipher_macput(struct buf *b);

/* init/purge/random32 */
extern void sshcrypto_init(void);
extern void sshcrypto_purge(void);
extern void sshcrypto_random32(unsigned char *);

#endif
