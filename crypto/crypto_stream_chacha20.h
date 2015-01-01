#ifndef crypto_stream_chacha20_H
#define crypto_stream_chacha20_H

#define crypto_stream_chacha20_tinynacl_KEYBYTES 32
#define crypto_stream_chacha20_tinynacl_NONCEBYTES 8
extern int crypto_stream_chacha20_tinynacl(unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);
extern int crypto_stream_chacha20_tinynacl_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);

#define crypto_stream_chacha20 crypto_stream_chacha20_tinynacl
#define crypto_stream_chacha20_xor crypto_stream_chacha20_tinynacl_xor
#define crypto_stream_chacha20_KEYBYTES crypto_stream_chacha20_tinynacl_KEYBYTES
#define crypto_stream_chacha20_NONCEBYTES crypto_stream_chacha20_tinynacl_NONCEBYTES
#define crypto_stream_chacha20_IMPLEMENTATION "tinynacl"
#define crypto_stream_chacha20_VERSION "-"

#endif
