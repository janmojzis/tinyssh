#ifndef crypto_kem_sntrup761_H
#define crypto_kem_sntrup761_H

#define crypto_kem_sntrup761_tinynacl_SECRETKEYBYTES 1763
#define crypto_kem_sntrup761_tinynacl_PUBLICKEYBYTES 1158
#define crypto_kem_sntrup761_tinynacl_CIPHERTEXTBYTES 1039
#define crypto_kem_sntrup761_tinynacl_BYTES 32
extern int crypto_kem_sntrup761_tinynacl_keypair(unsigned char *, unsigned char *);
extern int crypto_kem_sntrup761_tinynacl_enc(unsigned char *, unsigned char *, const unsigned char *);
extern int crypto_kem_sntrup761_tinynacl_dec(unsigned char *, const unsigned char *, const unsigned char *);

#define crypto_kem_sntrup761_keypair crypto_kem_sntrup761_tinynacl_keypair
#define crypto_kem_sntrup761_enc crypto_kem_sntrup761_tinynacl_enc
#define crypto_kem_sntrup761_dec crypto_kem_sntrup761_tinynacl_dec
#define crypto_kem_sntrup761_SECRETKEYBYTES crypto_kem_sntrup761_tinynacl_SECRETKEYBYTES
#define crypto_kem_sntrup761_PUBLICKEYBYTES crypto_kem_sntrup761_tinynacl_PUBLICKEYBYTES
#define crypto_kem_sntrup761_CIPHERTEXTBYTES crypto_kem_sntrup761_tinynacl_CIPHERTEXTBYTES
#define crypto_kem_sntrup761_BYTES crypto_kem_sntrup761_tinynacl_BYTES
#define crypto_kem_sntrup761_IMPLEMENTATION "tinynacl"
#define crypto_kem_sntrup761_VERSION "-"

#endif