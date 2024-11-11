/*
20140108
Jan Mojzis
Public domain.
*/

#ifndef BUF_H____
#define BUF_H____

#include "crypto_uint8.h"
#include "crypto_uint32.h"

struct buf {
    unsigned char *buf;
    long long len;
    long long alloc;
};

extern void buf_init_(const char *, unsigned long long, struct buf *, unsigned char *, long long);
extern void buf_purge_(const char *, unsigned long long, struct buf *);
extern int buf_ready_(const char *, unsigned long long, struct buf *, long long);

extern int buf_put_(const char *, unsigned long long, struct buf *, const unsigned char *, long long);
extern int buf_puts_(const char *, unsigned long long, struct buf *, const char *);
extern int buf_putzerobytes_(const char *, unsigned long long, struct buf *, long long);
extern int buf_putrandombytes_(const char *, unsigned long long, struct buf *, long long);
extern int buf_putpadding_(const char *, unsigned long long, struct buf *, long long);
extern int buf_putnum32_(const char *, unsigned long long, struct buf *, crypto_uint32);
extern int buf_putnum8_(const char *, unsigned long long, struct buf *, crypto_uint8);
extern int buf_putstringlen_(const char *, unsigned long long, struct buf *, const unsigned char *, long long);
extern int buf_putstring_(const char *, unsigned long long, struct buf *, const char *);
extern int buf_putsharedsecret_(const char *, unsigned long long, struct buf *, const unsigned char *, long long);
extern int buf_putbase64_(const char *, unsigned long long, struct buf *, const unsigned char *, long long);

#define buf_init(a, b, cc) buf_init_(__FILE__, __LINE__, (a), (b), (cc))
#define buf_purge(a) buf_purge_(__FILE__, __LINE__, (a))
#define buf_ready(a, b) buf_ready_(__FILE__, __LINE__, (a), (b))

#define buf_put(a, b, cc) buf_put_(__FILE__, __LINE__, (a), (b), (cc))
#define buf_puts(a, b) buf_puts_(__FILE__, __LINE__, (a), (b))
#define buf_putzerobytes(a, b)  buf_putzerobytes_(__FILE__, __LINE__, (a), (b))
#define buf_putrandombytes(a, b)  buf_putrandombytes_(__FILE__, __LINE__, (a), (b))
#define buf_putpadding(a, b)  buf_putpadding_(__FILE__, __LINE__, (a), (b))
#define buf_putnum32(a, b) buf_putnum32_(__FILE__, __LINE__, (a), (b))
#define buf_putnum8(a, b) buf_putnum8_(__FILE__, __LINE__, (a), (b))
#define buf_putstringlen(a, b, cc) buf_putstringlen_(__FILE__, __LINE__, (a), (b), (cc))
#define buf_putstring(a, b) buf_putstring_(__FILE__, __LINE__, (a), (b))
#define buf_putsharedsecret(a, b, cc) buf_putsharedsecret_(__FILE__, __LINE__, (a), (b), (cc))
#define buf_putbase64(a, b, cc) buf_putbase64_(__FILE__, __LINE__, (a), (b), (cc))

#endif

