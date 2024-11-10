#ifndef crypto_scalarmult_curve25519_H
#define crypto_scalarmult_curve25519_H

#include "haslib25519.h"
#ifndef HASLIB25519

#define crypto_scalarmult_curve25519_tinynacl_BYTES 32
#define crypto_scalarmult_curve25519_tinynacl_SCALARBYTES 32
extern int crypto_scalarmult_curve25519_tinynacl(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_tinynacl_base(unsigned char *,const unsigned char *);

#define crypto_scalarmult_curve25519 crypto_scalarmult_curve25519_tinynacl
#define crypto_scalarmult_curve25519_base crypto_scalarmult_curve25519_tinynacl_base
#define crypto_scalarmult_curve25519_BYTES crypto_scalarmult_curve25519_tinynacl_BYTES
#define crypto_scalarmult_curve25519_SCALARBYTES crypto_scalarmult_curve25519_tinynacl_SCALARBYTES
#define crypto_scalarmult_curve25519_IMPLEMENTATION "tinynacl"
#define crypto_scalarmult_curve25519_VERSION "-"

#else

#include <lib25519.h>
extern int crypto_scalarmult_curve25519_lib25519(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_lib25519_base(unsigned char *,const unsigned char *);

#define crypto_scalarmult_curve25519 crypto_scalarmult_curve25519_lib25519
#define crypto_scalarmult_curve25519_base crypto_scalarmult_curve25519_lib25519_base
#define crypto_scalarmult_curve25519_BYTES lib25519_nG_montgomery25519_POINTBYTES
#define crypto_scalarmult_curve25519_SCALARBYTES lib25519_nG_montgomery25519_SCALARBYTES
#define crypto_scalarmult_curve25519_IMPLEMENTATION "lib25519"
#define crypto_scalarmult_curve25519_VERSION lib25519_version
#endif

#endif
