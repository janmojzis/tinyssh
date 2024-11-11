#include "haslibntruprime.h"
#ifdef HASLIBNTRUPRIME

#include <ntruprime.h>
#include "crypto_kem_sntrup761.h"

int crypto_kem_sntrup761_libntruprime_keypair(unsigned char *pk,unsigned char *sk)
{
  sntrup761_keypair(pk, sk);
  return 0;
}

int crypto_kem_sntrup761_libntruprime_enc(unsigned char *c,unsigned char *k,const unsigned char *pk)
{
  sntrup761_enc(c, k, pk);
  return 0;
}

int crypto_kem_sntrup761_libntruprime_dec(unsigned char *k,const unsigned char *c,const unsigned char *sk)
{
  sntrup761_dec(k, c, sk);
  return 0;
}

#endif
