#include "fe25519.h"
#include "cleanup.h"
#include "ge25519.h"

/* D = -121665/121666 */
static fe D = { 
    0x135978a3, 0x75eb4dca, 0x4141d8ab, 0x00700a4d,
    0x7779e898, 0x8cc74079, 0x2b6ffe73, 0x52036cee
};
/* D2 = 2 * -121665/121666 */
static fe D2 = { 
    0x26b2f159, 0xebd69b94, 0x8283b156, 0x00e0149a,
    0xeef3d130, 0x198e80f2, 0x56dffce7, 0x2406d9dc
};

static fe sqrtm1 = {
    0x4a0ea0b0, 0xc4ee1b27, 0xad2fe478, 0x2f431806,
    0x3dfbd7a7, 0x2b4d0099, 0x4fc1df0b, 0x2b832480,
};

static void neutral(ge25519 p) {
    fe_0(p[0]);
    fe_1(p[1]);
    fe_1(p[2]);
    fe_0(p[3]);
}


/*
p = q
*/
static void copy(ge25519 p, ge25519 q) {

    fe_copy(p[0], q[0]);
    fe_copy(p[1], q[1]);
    fe_copy(p[2], q[2]);
    fe_copy(p[3], q[3]);
}


/*
if (b) p = q;
*/
static void cmov(ge25519 p, ge25519 q, crypto_uint32 b) {

    fe_cmov(p[0], q[0], b);
    fe_cmov(p[1], q[1], b);
    fe_cmov(p[2], q[2], b);
    fe_cmov(p[3], q[3], b);
}

void ge25519_add(ge25519 o, ge25519 p, ge25519 q) {

    fe a, b, c, d, t, e, f, g, h;

    fe25519_sub(a, p[1], p[0]);
    fe25519_sub(t, q[1], q[0]);
    fe25519_mul(a, a, t);
    fe25519_add(b, p[0], p[1]);
    fe25519_add(t, q[0], q[1]);
    fe25519_mul(b, b, t);
    fe25519_mul(c, p[3], q[3]);
    fe25519_mul(c, c, D2);
    fe25519_mul(d, p[2], q[2]);
    fe25519_add(d, d, d);
    fe25519_sub(e, b, a);
    fe25519_sub(f, d, c);
    fe25519_add(g, d, c);
    fe25519_add(h, b, a);

    fe25519_mul(o[0], e, f);
    fe25519_mul(o[1], h, g);
    fe25519_mul(o[2], g, f);
    fe25519_mul(o[3], e, h);

    cleanup(a); cleanup(b); cleanup(c); cleanup(d); cleanup(t);
    cleanup(e); cleanup(f); cleanup(g); cleanup(h);
}


/* https://hyperelliptic.org/EFD/g1p/auto-code/twisted/extended/doubling/dbl-2008-hwcd.op3 */
static void dbl(ge25519 o, ge25519 p) {

    fe a, b, c, d, e, f, g, h;

    fe25519_sq(a, p[0]);        /* A  = X1^2  */
    fe25519_sq(b, p[1]);        /* B  = Y1^2  */
    fe25519_sq(c, p[2]);        /* t0 = Z1^2  */
    fe25519_add(c, c, c);       /* C  = 2*t0  */
    fe25519_neg(d, a);          /* D  = a*A   */
    fe25519_add(e, p[0], p[1]); /* t1 = X1+Y1 */
    fe25519_sq(e, e);           /* t2 = t1^2  */
    fe25519_sub(e, e, a);       /* t3 = t2-A  */
    fe25519_sub(e, e, b);       /* E  = t3-B  */
    fe25519_add(g, d, b);       /* G  = D+B   */
    fe25519_sub(f, g, c);       /* F  = G-C   */
    fe25519_sub(h, d, b);       /* H  = D-B   */

    fe25519_mul(o[0], e, f);    /* X3 = E*F   */
    fe25519_mul(o[1], g, h);    /* Y3 = G*H   */
    fe25519_mul(o[2], f, g);    /* Z3 = F*G   */
    fe25519_mul(o[3], e, h);    /* T3 = E*H   */

    cleanup(a); cleanup(b); cleanup(c); cleanup(d);
    cleanup(e); cleanup(f); cleanup(g); cleanup(h);
}


void ge25519_tobytes(unsigned char *s, ge25519 h) {

    fe x, y, z;

    fe25519_inv(z, h[2]);
    fe25519_mul(x, h[0], z);
    fe25519_mul(y, h[1], z);
    fe25519_tobytes(s, y);
    s[31] ^= fe25519_isnegative(x) << 7;

    cleanup(x); cleanup(y); cleanup(z);
}

int ge25519_frombytes_negate_vartime(ge25519 h, const unsigned char *s) {

    fe u, v, v3, vxx, check;
    int ret = -1;

    fe25519_frombytes(h[1], s);
    fe_1(h[2]);
    fe25519_sq(u,h[1]);
    fe25519_mul(v,u,D);
    fe25519_sub(u,u,h[2]);       /* u = y^2-1 */
    fe25519_add(v,v,h[2]);       /* v = dy^2+1 */

    fe25519_sq(v3,v);
    fe25519_mul(v3,v3,v);        /* v3 = v^3 */
    fe25519_sq(h[0],v3);
    fe25519_mul(h[0],h[0],v);
    fe25519_mul(h[0],h[0],u);    /* x = uv^7 */

    fe25519_pow22523(h[0],h[0]); /* x = (uv^7)^((q-5)/8) */
    fe25519_mul(h[0],h[0],v3);
    fe25519_mul(h[0],h[0],u);    /* x = uv^3(uv^7)^((q-5)/8) */

    fe25519_sq(vxx,h[0]);
    fe25519_mul(vxx,vxx,v);
    fe25519_sub(check,vxx,u);    /* vx^2-u */
    if (fe25519_isnonzero(check)) {
        fe25519_add(check,vxx,u);  /* vx^2+u */
        if (fe25519_isnonzero(check)) {
            goto cleanup;
        }
        fe25519_mul(h[0],h[0],sqrtm1);
    }

    if (fe25519_isnegative(h[0]) == (s[31] >> 7))
        fe25519_neg(h[0], h[0]);

    fe25519_mul(h[3],h[0],h[1]);
    ret = 0;

cleanup:
    cleanup(u); cleanup(v); cleanup(v3);
    cleanup(vxx); cleanup(check);
    return ret;
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
point multiplication using windowed method 
*/
void ge25519_scalarmult(ge25519 o, ge25519 q, const unsigned char *a) {

    long long i, j;
    ge25519 t[16], sp, p;
    unsigned char e[64];

    for (i = 0; i < 32; ++i) {
        e[2 * i + 0] = (a[i] >> 0) & 0x0f;
        e[2 * i + 1] = (a[i] >> 4) & 0x0f;
    }

    neutral(p);

    /* precompute points */
    copy(t[0], p);
    copy(t[1], q);
    for (i = 2; i < 16; ++i) {
        if ((i & 1) == 0) dbl(t[i], t[i / 2]);
        else ge25519_add(t[i], t[i - 1], q);
    }

    for (i = 63; i >= 0; --i) {
        for (j = 0; j < 4; ++j) dbl(p, p);
        for (j = 0; j < 16; ++j) cmov(sp, t[j], equal(e[i], j));
        ge25519_add(p, p, sp);
    }

    copy(o, p);

    cleanup(p); cleanup(t); cleanup(sp); cleanup(e);
}

static ge25519 baseq = {
  { 0x8f25d51a,0xc9562d60,0x9525a7b2,0x692cc760,0xfdd6dc5c,0xc0a4e231,0xcd6e53fe,0x216936d3 },
  { 0x66666658,0x66666666,0x66666666,0x66666666,0x66666666,0x66666666,0x66666666,0x66666666 },
  { 0x00000001,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000 },
  { 0xa5b7dda3,0x6dde8ab3,0x775152f5,0x20f09f80,0x64abe37d,0x66ea4e8e,0xd78b7665,0x67875f0f },
 };

void ge25519_scalarmult_base(ge25519 p, const unsigned char *a) {

    ge25519_scalarmult(p, baseq, a);
}
