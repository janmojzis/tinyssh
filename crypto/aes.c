/*
- based on crypto_core/aes256encrypt/ref from supercop-20140622
- added bitsliced 'bytesub' 
*/

#include "cleanup.h"
#include "aessbox.h"
#include "crypto_uint32.h"
#include "aes.h"

static unsigned char gmul2(unsigned char a) {

    unsigned char x = a >> 7;   /* 0x00 or 0x01 */
    x = -x;                     /* 0x00 or 0xff */
    x &= 0x1b;                  /* 0x00 or 0x1b */
    a <<= 1;
    return x ^ a;
}

/* functions for aessbox macro */
static void xor2(crypto_uint32 *r,  const crypto_uint32 *x) { *r ^= *x; }
static void and2(crypto_uint32 *r,  const crypto_uint32 *x) { *r &= *x; }
static void or2(crypto_uint32 *r,   const crypto_uint32 *x) { *r |= *x; }
static void copy2(crypto_uint32 *r, const crypto_uint32 *x) { *r  = *x; }


/* 16x SubBytes parallel using bitslicing */
static void bytesub(unsigned char *r, unsigned char *c) {

    crypto_uint32 t0, t1, t2, t3, t4, t5, t6, t7;
    crypto_uint32 b0 = 0, b1 = 0, b2 = 0, b3 = 0, b4 = 0, b5 = 0, b6 = 0, b7 = 0;
    long long j;

    for (j = 0; j < 16; ++j) {
        b0 <<= 1; b0 |= 1 & (c[j]     );
        b1 <<= 1; b1 |= 1 & (c[j] >> 1);
        b2 <<= 1; b2 |= 1 & (c[j] >> 2);
        b3 <<= 1; b3 |= 1 & (c[j] >> 3);
        b4 <<= 1; b4 |= 1 & (c[j] >> 4);
        b5 <<= 1; b5 |= 1 & (c[j] >> 5);
        b6 <<= 1; b6 |= 1 & (c[j] >> 6);
        b7 <<= 1; b7 |= 1 & (c[j] >> 7);
    }

    aessbox(b0, b1, b2, b3, b4, b5, b6, b7, t0, t1, t2, t3, t4, t5, t6, t7);

    for (j = 0; j < 16; ++j) {
        r[15 - j]   = 0; r[15 - j] |= 1 & b5; b5 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b2; b2 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b7; b7 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b3; b3 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b6; b6 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b4; b4 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b1; b1 >>= 1;
        r[15 - j] <<= 1; r[15 - j] |= 1 & b0; b0 >>= 1;
        r[15 - j] ^= 0x63;
    }
}

void aes_core(unsigned char *out, const unsigned char *in, const unsigned char *k, long long rounds) {

    unsigned char expanded[4][60];
    unsigned char state[4][4];
    unsigned char newstate[4][4];
    unsigned char a0, a1, a2, a3;
    long long i, j, r;

    /* load expanded key */
    for (j = 0; j < 4 * (rounds + 1); ++j) 
        for (i = 0; i < 4; ++i)
            expanded[i][j] = k[j * 4 + i];

    /* AddRoundKey */
    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
            state[i][j] = in[j * 4 + i] ^ expanded[i][j];

    for (r = 0; r < rounds; ++r) {

        /* SubBytes */
        bytesub((unsigned char *)newstate, (unsigned char *)state);

        /* ShiftRows */
        for (i = 0; i < 4; ++i)
            for (j = 0; j < 4; ++j)
                state[i][j] = newstate[i][(j + i) % 4];

        /* MixColumns */
        if (r < rounds - 1)
            for (j = 0; j < 4; ++j) {
                a0 = state[0][j];
                a1 = state[1][j];
                a2 = state[2][j];
                a3 = state[3][j];
	        state[0][j] = gmul2(a0 ^ a1) ^ a1 ^ a2 ^ a3;
	        state[1][j] = gmul2(a1 ^ a2) ^ a2 ^ a3 ^ a0;
	        state[2][j] = gmul2(a2 ^ a3) ^ a3 ^ a0 ^ a1;
	        state[3][j] = gmul2(a3 ^ a0) ^ a0 ^ a1 ^ a2;
            }

        /* AddRoundKey */
        for (i = 0; i < 4; ++i)
            for (j = 0; j < 4; ++j)
                state[i][j] ^= expanded[i][r * 4 + 4 + j];
    }

    /* store output  */
    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
            out[j * 4 + i] = state[i][j];

    /* cleanup */
    cleanup(expanded); cleanup(state); cleanup(newstate);
}

int aes_expandkey256(unsigned char *o, const unsigned char *k) {

    unsigned char expanded[4][60], temp[16], roundconstant = 1;
    long long i, j;

    for (j = 0; j < 8; ++j)
        for (i = 0; i < 4; ++i)
            expanded[i][j] = k[j * 4 + i];

    for (j = 8; j < 60; ++j) {
        if (j % 4)
            for (i = 0; i < 4; ++i) temp[i] = expanded[i][j - 1];
        else if (j % 8) {
            for (i = 0; i < 4; ++i) temp[i] = expanded[i][j - 1];
            bytesub(temp, temp);
        }
        else {
            for (i = 0; i < 4; ++i) temp[i] = expanded[(i + 1) % 4][j - 1];
            bytesub(temp, temp);
            temp[0] ^= roundconstant;
            roundconstant = gmul2(roundconstant);
        }
        for (i = 0; i < 4; ++i) expanded[i][j] = temp[i] ^ expanded[i][j - 8];
    }

    for (j = 0; j < 60; ++j)
        for (i = 0; i < 4; ++i)
            o[j * 4 + i] = expanded[i][j];

    cleanup(temp); cleanup(expanded);
    return 0;
}

int aes_expandkey128(unsigned char *o, const unsigned char *k) {

    unsigned char expanded[4][44], temp[16], roundconstant = 1;
    long long i, j;

    for (j = 0; j < 4; ++j)
        for (i = 0; i < 4; ++i)
            expanded[i][j] = k[j * 4 + i];

    for (j = 4; j < 44; ++j) {
        if (j % 4)
            for (i = 0; i < 4; ++i) temp[i] = expanded[i][j - 1];
        else {
            for (i = 0; i < 4; ++i) temp[i] = expanded[(i + 1) % 4][j - 1];
            bytesub(temp, temp);
            temp[0] ^= roundconstant;
            roundconstant = gmul2(roundconstant);
        }
        for (i = 0; i < 4; ++i) expanded[i][j] = temp[i] ^ expanded[i][j - 4];
    }
    for (j = 0; j < 44; ++j)
        for (i = 0; i < 4; ++i)
            o[j * 4 + i] = expanded[i][j];

    cleanup(temp); cleanup(expanded);
    return 0;
}
