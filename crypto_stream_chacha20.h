#ifndef crypto_stream_chacha20_H
#define crypto_stream_chacha20_H

#define crypto_stream_chacha20_tinyssh_KEYBYTES 32
#define crypto_stream_chacha20_tinyssh_NONCEBYTES 8
extern int crypto_stream_chacha20_tinyssh(unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);
extern int crypto_stream_chacha20_tinyssh_xor(unsigned char *, const unsigned char *, unsigned long long, const unsigned char *, const unsigned char *);

#define crypto_stream_chacha20 crypto_stream_chacha20_tinyssh
#define crypto_stream_chacha20_xor crypto_stream_chacha20_tinyssh_xor
#define crypto_stream_chacha20_KEYBYTES crypto_stream_chacha20_tinyssh_KEYBYTES
#define crypto_stream_chacha20_NONCEBYTES crypto_stream_chacha20_tinyssh_NONCEBYTES
#define crypto_stream_chacha20_IMPLEMENTATION "tinyssh"
#define crypto_stream_chacha20_VERSION "-"

#endif
