/*
20140918
Jan Mojzis
Public domain.
*/

#include "cleanup.h"
#include "fe.h"
#include "fep256.h"
#include "gep256.h"

/* b = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b  */
static const fe paramb = {
     0x27d2604b, 0x3bce3c3e, 0xcc53b0f6, 0x651d06b0,
     0x769886bc, 0xb3ebbd55, 0xaa3a93e7, 0x5ac635d8
};

/* XXX - fake neutral point at infinity */
static void neutral(gep256 p) {
    fe_0(p[0]);
    fe_0(p[1]);
    fe_0(p[2]);
}
static void neutral_precomp(gep256_precomp p) {
    fe_0(p[0]);
    fe_0(p[1]);
}

/*
p = q
*/
static void copy(gep256 p, gep256 q) {

    fe_copy(p[0], q[0]);
    fe_copy(p[1], q[1]);
    fe_copy(p[2], q[2]);
}

/*
if (b) p = q;
*/
static void cmov(gep256 p, gep256 q, crypto_uint32 b) {

    fe_cmov(p[0], q[0], b);
    fe_cmov(p[1], q[1], b);
    fe_cmov(p[2], q[2], b);
}
static void cmov_precomp(gep256_precomp p, gep256_precomp q, crypto_uint32 b) {

    fe_cmov(p[0], q[0], b);
    fe_cmov(p[1], q[1], b);
}


/*
if (a == b) return 1;
else return 0;
*/
static unsigned char equal(unsigned char a, unsigned char b) {

    unsigned char x = a ^ b;
    crypto_uint32 y = x;
    y -= 1;
    y >>= 31;
    return y;
}

/*
if (p == oo) return 1;
else return 0
*/
static int isneutral(gep256 p) {
    return fe_iszero3(p[0], p[1], p[2]);
}
static int isneutral_precomp(gep256_precomp p) {
    return fe_iszero2(p[0], p[1]);
}

static void madd(gep256 out, gep256 p, gep256_precomp qp) {

    gep256 q, o;
    fe ZZ1, S2, R, RR, U2, H, HH, HHH, t0, t1;

    fe_copy(q[0], qp[0]);
    fe_copy(q[1], qp[1]);
    fe_1(q[2]);

    fep256_sq(ZZ1, p[2]);
    fep256_mul(S2, ZZ1, p[2]);
    fep256_mul(S2, S2, q[1]);     /* S2 = Y2 * Z1^3   */
    fep256_sub(R, p[1], S2);      /* R  = X1 - S2     */
    fep256_mul(U2, q[0], ZZ1);    /* U2 = X2 * Z1^2   */
    fep256_sub(H, p[0], U2);      /* H  = X1 - U2     */

    fep256_mul(o[2], H, p[2]);    /* Z3 = H * Z1 */

    fep256_sq(RR, R);             /* R^2 */
    fep256_sq(HH, H);             /* H^2 */
    fep256_mul(HHH, HH, H);       /* H^3 */
    fep256_mul(t0, p[0], HH);     /*     X1 * H^2 */
    fep256_mul2(t1, t0);          /* 2 * X1 * H^2 */
    fep256_sub(o[0], RR, t1);
    fep256_add(o[0], o[0], HHH);  /* X3 = R^2 + H^3 - 2 * X1 * H^2 */

    fep256_sub(o[1], t0, o[0]);
    fep256_mul(o[1], R, o[1]);
    fep256_mul(t1, p[1], HHH);
    fep256_sub(o[1], o[1], t1);  /* Y3 = R * (X1 * H^2 - X3) - X1 * H^3 */

    /* if (p = neutral) o = q */
    /* if (q = neutral) o = p */
    cmov(o, q, isneutral(p));
    cmov(o, p, isneutral_precomp(qp));
    copy(out, o);

    cleanup(ZZ1); cleanup(S2); cleanup(t0); cleanup(o);
    cleanup(R); cleanup(RR);  cleanup(U2); cleanup(t1);
    cleanup(H); cleanup(HH); cleanup(HHH); cleanup(q);
}

void gep256_add(gep256 out, gep256 p, gep256 q) {

    fe ZZ1, ZZ2, S1, S2, R, RR, U1, U2, H, HH, HHH, t0, t1;
    gep256 o;

    fep256_sq(ZZ1, p[2]);
    fep256_sq(ZZ2, q[2]);
    fep256_mul(S1, ZZ2, q[2]);
    fep256_mul(S2, ZZ1, p[2]);
    fep256_mul(S1, S1, p[1]);     /* S1 = Y1 * Z2^3   */
    fep256_mul(S2, S2, q[1]);     /* S2 = Y2 * Z1^3   */
    fep256_sub(R, S1, S2);        /* R  = S1 - S2     */
    fep256_mul(U1, p[0], ZZ2);    /* U1 = X1 * Z2^2   */
    fep256_mul(U2, q[0], ZZ1);    /* U2 = X2 * Z1^2   */
    fep256_sub(H, U1, U2);        /* H  = U1 - U2     */

    fep256_mul(o[2], H, p[2]);
    fep256_mul(o[2], o[2], q[2]); /* Z3 = H * Z1 * Z2 */

    fep256_sq(RR, R);             /* R^2 */
    fep256_sq(HH, H);             /* H^2 */
    fep256_mul(HHH, HH, H);       /* H^3 */
    fep256_mul(t0, U1, HH);       /*     U1 * H^2 */
    fep256_mul2(t1, t0);          /* 2 * U1 * H^2 */
    fep256_sub(o[0], RR, t1);
    fep256_add(o[0], o[0], HHH);  /* X3 = R^2 + H^3 - 2 * U1 * H^2 */

    fep256_sub(o[1], t0, o[0]);
    fep256_mul(o[1], R, o[1]);
    fep256_mul(t1, S1, HHH);
    fep256_sub(o[1], o[1], t1);  /* Y3 = R * (U1 * H^2 - X3) - S1 * H^3 */

    /* if (p = neutral) o = q */
    /* if (q = neutral) o = p */
    cmov(o, q, isneutral(p));
    cmov(o, p, isneutral(q));
    copy(out, o);

    cleanup(ZZ1); cleanup(ZZ2); cleanup(S1); cleanup(S2);
    cleanup(R); cleanup(RR); cleanup(U1); cleanup(U2);
    cleanup(H); cleanup(HH); cleanup(HHH);
    cleanup(t0); cleanup(t1); cleanup(o);
}

static void dbl(gep256 p, gep256 q) {

    gep256 o;
    fe t0, t1, t2, S, S2, M, ZZ, U;

    fep256_mul2(t0, q[1]);      /* t0 = 2 * Y1                  */
    fep256_mul2(t1, t0);        /* t1 = 4 * Y1                  */
    fep256_mul(t2, t0, t1);     /* t2 = 8 * Y1^2                */
    fep256_sq(U, t2);           /* U = 64 * Y1^4                */

    fep256_mul(S, t2, q[0]);
    fep256_mul2(S, S);          /* S  = 16 * X * Y^2            */
    fep256_mul2(S2, S);         /* S2 = 32 * X * Y^2            */

    fep256_mul(o[2], t1, q[2]); /* Z3 = 4 * Y1 * Z1             */

    fep256_sq(ZZ, q[2]);
    fep256_add(t1, q[0], ZZ);
    fep256_sub(t2, q[0], ZZ);
    fep256_mul(M, t1, t2);
    fep256_mul6(M, M);          /* M  = 6 * (X1^2 - Z1^4)       */
    fep256_sq(o[0], M);
    fep256_sub(o[0], o[0], S2); /* X3 = M^2 - 2S                */
    fep256_sub(o[1], S, o[0]);
    fep256_mul(o[1], o[1], M);
    fep256_sub(o[1], o[1], U);  /* Y3 = M * (S - X3) - 64 * Y1^4 */

    copy(p, o);

    cleanup(t0); cleanup(t1); cleanup(t2); 
    cleanup(o); cleanup(S); cleanup(S2);
    cleanup(M); cleanup(ZZ); cleanup(U);
}

/*
checks if point is on curve x^3 - 3x + b = y^2
*/
static int checkaffine(gep256 p) {

    fe tmpx, tmpy;
    int ret = -1;

    fep256_sq(tmpx, p[0]);
    fep256_mul(tmpx, tmpx, p[0]);   /*           x ^ 3          */
    fep256_mul3(tmpy,  p[0]);       /*                   3x     */
    fep256_sub(tmpy, paramb, tmpy); /*                  -3x + b */
    fep256_add(tmpx, tmpx, tmpy);   /*           x ^ 3 - 3x + b */

    fep256_sq(tmpy, p[1]);          /*  y ^ 2                   */

    fep256_sub(tmpx, tmpx, tmpy);   /*  y ^ 2  = x ^ 3 - 3x + b */
    if (!fep256_isnonzero(tmpx)) ret = 0;

    cleanup(tmpx); cleanup(tmpy);
    return ret;
}


/*
conversion from Jacobian coordinates into affine form
*/
static int toaffine(gep256 p) {

    fe invz, tmp;

    if (!fep256_isnonzero(p[2])) return -1;

    fep256_inv(invz, p[2]);
    fe_copy(tmp, invz);

    fep256_sq(invz, invz);
    fep256_mul(p[0], p[0], invz);

    fep256_mul(invz, invz, tmp);
    fep256_mul(p[1], p[1], invz);

    fe_1(p[2]);

    cleanup(invz); cleanup(tmp);
    return checkaffine(p);
}


int gep256_tobytes(unsigned char *out, gep256 in) {

    if (toaffine(in) != 0) return -1;

    fep256_tobytes(out     , in[0]);
    fep256_tobytes(out + 32, in[1]);
    return 0;
}

int gep256_frombytes(gep256 out, const unsigned char *in) {

    fep256_frombytes(out[0], in     );
    fep256_frombytes(out[1], in + 32);
    fe_1(out[2]);

    return checkaffine(out);
}


/*
point multiplication using windowed method 
*/
void gep256_scalarmult(gep256 o, gep256 q, const unsigned char *a) {

    long long i, j;
    gep256 t[16], sp, p;
    unsigned char e[64];

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[31 - i] >> 0) & 0x0f;
        e[2 * i + 1] = (a[31 - i] >> 4) & 0x0f;
    }

    neutral(p);

    /* precompute points */
    copy(t[0], p);
    copy(t[1], q);
    for (i = 2; i < 16; ++i) {
        if ((i & 1) == 0) dbl(t[i], t[i / 2]);
        else gep256_add(t[i], t[i - 1], q);
    }

    for (i = 63; i >= 0; --i) {
        for (j = 0; j < 4; ++j) dbl(p, p);
        for (j = 0; j < 16; ++j) cmov(sp, t[j], equal(e[i], j));
        gep256_add(p, p, sp);
    }

    copy(o, p);

    cleanup(p); cleanup(t); cleanup(sp); cleanup(e);
}

static gep256_precomp base[33][8] = {
#include "gep256.base"
};

static unsigned char isnegative(signed char b) {

    crypto_uint32 x = b;
    x >>= 31;
    return x;
}

static void select(gep256_precomp t, long long pos, signed char b) {

    gep256_precomp minust;
    unsigned char bnegative = isnegative(b);
    unsigned char babs = b - (((-bnegative) & b) << 1);
    long long i;

    neutral_precomp(t);
    for (i = 0; i < 8; ++i) cmov_precomp(t, base[pos][i], equal(babs, i + 1));
    fe_copy(minust[0], t[0]);
    fep256_neg(minust[1], t[1]);
    cmov_precomp(t, minust, bnegative);
}

/*
base-point multiplication using precomputed tables
*/
void gep256_scalarmult_base(gep256 p, const unsigned char *a) {

    long long i;
    gep256_precomp sp;
    signed char e[65], carry;

    neutral(p);

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[31 - i] >> 0) & 0x0f;
        e[2 * i + 1] = (a[31 - i] >> 4) & 0x0f;
    }
    e[64] = 0;

    carry = 0;
    for (i = 0; i < 65; ++i) {
        e[i] += carry;
        carry = e[i] + 8;
        carry >>= 4;
        e[i] -= carry << 4;
    }

    for (i = 1; i < 65; i += 2) {
        select(sp, i / 2, e[i]);
        madd(p, p, sp);
    }

    for (i = 0; i < 4; ++i) dbl(p, p);

    for (i = 0; i < 65; i += 2) {
        select(sp, i / 2, e[i]);
        madd(p, p, sp);
    }

    cleanup(sp); cleanup(e);
}
