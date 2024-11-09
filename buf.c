/*
20140108
Jan Mojzis
Public domain.

The 'buf' library holds a string in allocated space.
The string length is limited by allocated memory.
All 'buf_put*' functions checks the limit and if
the string exceeds the limit, 'buf_put*' function
immediately exits with 111 status code.
*/

#include "byte.h"
#include "str.h"
#include "purge.h"
#include "randombytes.h"
#include "uint32_pack_big.h"
#include "bug.h"
#include "buf.h"

/*
Initialize 'b' structure.
*/
void buf_init_(const char *fn, unsigned long long line,
                struct buf *b, unsigned char *buf, long long alloc) {

    if (!b || !buf || alloc <= 0 || alloc > 1073741824) bug_inval_(fn, line);

    b->buf = buf;
    b->len = 0;
    b->alloc = alloc;
    purge(b->buf, b->alloc);
}

/*
Remove content of 'b'.
*/
void buf_purge_(const char *fn, unsigned long long line,
                struct buf *b) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf) bug_inval_(fn, line);

    purge(b->buf, b->len);
    b->len = 0;
}

/*
Retun if 'b' has available space for string of length 'len'.
*/
int buf_ready_(const char *fn, unsigned long long line,
               struct buf *b, long long len) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf || len < 0 || len > 1073741824) bug_inval_(fn, line);
    return (b->len + len < b->alloc);
}


/*
Put string 'x' of length 'len'.
*/
int buf_put_(const char *fn, unsigned long long line,
             struct buf *b, const unsigned char *x, long long len) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf || !x || len < 0 || len > 1073741824) bug_inval_(fn, line);
    if (b->len + len >= b->alloc) bug_nomem_(fn, line);

    byte_copy(b->buf + b->len, len, x);
    b->len += len;
    return 1;
}

/*
Put 0-terminated string 'x'.
*/
int buf_puts_(const char *fn, unsigned long long line,
             struct buf *b, const char *x) {
    return buf_put_(fn, line, b, (const unsigned char *)x, str_len(x));
}

/*
Put zero bytes of length 'len'.
*/
int buf_putzerobytes_(const char *fn, unsigned long long line,
                      struct buf *b, long long len) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf || len < 0 || len > 1073741824) bug_inval_(fn, line);
    if (b->len + len >= b->alloc) bug_nomem_(fn, line);

    byte_zero(b->buf + b->len, len);
    b->len += len;
    return 1;
}

/*
Put random bytes of length 'len'.
*/
int buf_putrandombytes_(const char *fn, unsigned long long line,
                        struct buf *b, long long len) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf || len < 0 || len > 1073741824) bug_inval_(fn, line);
    if (b->len + len >= b->alloc) bug_nomem_(fn, line);

    randombytes(b->buf + b->len, len);
    b->len += len;
    return 1;
}

/*
Put padding of length 'len'.
*/
int buf_putpadding_(const char *fn, unsigned long long line,
                        struct buf *b, long long len) {

    if (!b || b->len < 0 || b->len > 1073741824 || b->alloc <= 0 || b->alloc > 1073741824 || !b->buf || len < 0 || len > 1073741824) bug_inval_(fn, line);
    if (b->len + len >= b->alloc) bug_nomem_(fn, line);

    purge(b->buf + b->len, len);
    b->len += len;
    return 1;
}

/*
Put 32-bit unsigned integer in big-endian format.
*/
int buf_putnum32_(const char *fn, unsigned long long line,
                  struct buf *b, crypto_uint32 x) {

    unsigned char s[4];

    uint32_pack_big(s, x);
    return buf_put_(fn, line, b, s, sizeof s);
}

/*
Put 8-bit unsigned integer.
*/
int buf_putnum8_(const char *fn, unsigned long long line,
                 struct buf *b, crypto_uint8 u) {

    return buf_put_(fn, line, b, &u, 1);
}

/*
Put string 'x' of length 'len' and format it as a SSH-string.
*/
int buf_putstringlen_(const char *fn, unsigned long long line,
                      struct buf *b, const unsigned char *x, long long len) {

    if (!buf_putnum32_(fn, line, b, len)) return 0;
    return buf_put_(fn, line, b, x, len);
}

/*
Put 0-terminated string 'x' and format it as a SSH-string.
*/
int buf_putstring_(const char *fn, unsigned long long line,
                   struct buf *b, const char *x) {

    return buf_putstringlen_(fn, line, b, (unsigned char *)x, str_len(x));
}

/*
Put SSH shared secret (bignum formated into wire format)
*/
int buf_putsharedsecret_(const char *fn, unsigned long long line,
                   struct buf *b, const unsigned char *x, long long len) {

    long long pos;

    if (len < 0 || len > 1073741824 || !b || !x) bug_inval_(fn, line);

    for (pos = 0; pos < len; ++pos) if (x[pos]) break;

    if (x[pos] & 0x80) {
        buf_putnum32_(fn, line, b, len - pos + 1);
        buf_putnum8_(fn, line, b, 0);
    }
    else {
        buf_putnum32_(fn, line, b, len - pos + 0);
    }
    return buf_put_(fn, line, b, x + pos, len - pos);
}


/*
Put string 'x' of length 'len' and encode it into base64.
*/
static const unsigned char b64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
int buf_putbase64_(const char *fn, unsigned long long line,
                   struct buf *b, const unsigned char *x, long long len) { 

    long long i;
    unsigned long long bits = 0, v = 0;
    unsigned char ch;

    if (len < 0 || len > 1073741824 || !b || !x) bug_inval_(fn, line);

    for (i = 0; i < len; ++i) {
        v <<= 8; v += x[i]; bits += 8;
        while (bits > 6) {
            ch = b64chars[((v >> (bits - 6)) & 63)];
            buf_putnum8_(fn, line, b, ch);
            bits -= 6;
        }
    }

    if (bits) {
        v <<= (6 - bits);
        ch = b64chars[v & 63];
        buf_putnum8_(fn, line, b, ch);
    }
    while (b->len & 3) { 
        buf_putnum8_(fn, line, b, '=');
    }
    return 1;
}
