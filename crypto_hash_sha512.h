#ifndef crypto_hash_sha512_H
#define crypto_hash_sha512_H

#include "haslib25519.h"
#ifndef HASLIB25519

#define crypto_hash_sha512_tinyssh_BYTES 64
extern int crypto_hash_sha512_tinyssh(unsigned char *, const unsigned char *, unsigned long long);

#define crypto_hash_sha512 crypto_hash_sha512_tinyssh
#define crypto_hash_sha512_BYTES crypto_hash_sha512_tinyssh_BYTES
#define crypto_hash_sha512_IMPLEMENTATION "tinyssh"
#define crypto_hash_sha512_VERSION "-"

#else

#include <lib25519.h>
#define crypto_hash_sha512_lib25519_BYTES lib25519_hash_sha512_BYTES
extern int crypto_hash_sha512_lib25519(unsigned char *, const unsigned char *, unsigned long long);

#define crypto_hash_sha512 crypto_hash_sha512_lib25519
#define crypto_hash_sha512_BYTES crypto_hash_sha512_lib25519_BYTES
#define crypto_hash_sha512_IMPLEMENTATION "lib25519"
#define crypto_hash_sha512_VERSION lib25519_version

#endif

#endif
