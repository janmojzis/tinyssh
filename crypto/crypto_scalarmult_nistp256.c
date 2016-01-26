/*
20140917
Jan Mojzis
Public domain.
*/

#include "cleanup.h"
#include "gep256.h"
#include "crypto_scalarmult_nistp256.h"

int crypto_scalarmult_nistp256_tinynacl(unsigned char *q, const unsigned char *n, const unsigned char *p) {

    gep256 P, Q;
    long long i;
    int ret = -1;

    if (gep256_frombytes(P, p) != 0) goto fail;
    gep256_scalarmult(Q, P, n);
    if (gep256_tobytes(q, Q) != 0) goto fail;
    ret = 0;
    goto cleanup;

fail:
    for (i = 0; i < 64; ++i) q[i] = 0;

cleanup:
    cleanup(P); cleanup(Q);
    return ret;
}

int crypto_scalarmult_nistp256_tinynacl_base(unsigned char *q, const unsigned char *n) {

    gep256 Q;
    int ret = -1;

    gep256_scalarmult_base(Q, n);
    if (gep256_tobytes(q, Q) == 0) ret = 0;

    cleanup(Q);
    return ret;
}
