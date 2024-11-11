/*
20130604
Jan Mojzis
Public domain.
*/

#include "byte.h"
#include "iptostr.h"

static char *iptostr4(char *, const unsigned char *);
static char *iptostr6(char *, const unsigned char *);

/*
The 'iptostr(strbuf,ip)' function converts IP address 'ip'
from network byte order into the 0-terminated string.
The 'ip' length is always 16 bytes. The caller must 
allocate at least IPTOSTR_LEN bytes for 'strbuf'.
*/
char *iptostr(char *strbuf, const unsigned char *ip) {

    static char staticbuf[IPTOSTR_LEN];

    if (!strbuf) strbuf = staticbuf; /* not thread-safe */

    if (byte_isequal("\0\0\0\0\0\0\0\0\0\0\377\377", 12, ip)) {
        return iptostr4(strbuf, ip + 12);
    }
    return iptostr6(strbuf, ip);
}


/* convert IPv4 address */
static char *iptostr4(char *strbuf, const unsigned char *ip) {

    long long i, len = 0;
    unsigned long long num;

    for (i = 3; i >= 0; --i) {
        num = ip[i];
        do {
            num /= 10; ++len;
        } while (num);
        if (i > 0) ++len;
    }
    strbuf += len;

    for (i = 3; i >= 0; --i) {
        num = ip[i];
        do {
            *--strbuf = '0' + (num % 10);
            num /= 10;
        } while (num);
        if (i > 0) *--strbuf = '.';
    }

    while (len < IPTOSTR_LEN) strbuf[len++] = 0;
    return strbuf;
}

/*
The 'ip' is splited into 8 16-bit blocks and
the 'countz' function counts successive zeros and
searches for largest zero-block.
'first' ... first position of the zero-block
'last'  ...  last position of the zero-block
*/
static void countz(long long *first, long long *last, const unsigned long long *ip) {

    long long i, j, e;
    long long count[8];


    for (i = 7; i >= 0; --i) count[i] = 0;

    e = 8;
    for (i = 7; i >= 0; --i) {
        if (!ip[i]) {
            for (j = i; j < e; ++j) ++count[j];
        }
        else {
            e = i;
        }
    }

    e = 0; j = 0;
    for (i = 7; i >= 0; --i) {
        if (count[i]) {
            if (count[i] >= e) { e = count[i]; j = i; }
        }
    }

    *first = j - count[j] + 1;
    *last = j;
}

/* convert IPv6 address */
static char *iptostr6(char *strbuf, const unsigned char *ip) {

    long long first, last, i;
    unsigned long long ip2[8];
    char *s = strbuf;

    for (i = 7; i >= 0; --i) {
        ip2[i] = ip[2 * i];
        ip2[i] <<= 8;
        ip2[i] += ip[2 * i + 1];
    }

    countz(&first, &last, ip2);

    strbuf += IPTOSTR_LEN - 1;
    *strbuf = 0;

    for (i = 7; i >= 0; --i) {
        if (i <= last && i >= first && first != last) {
            if (i == last) *--strbuf = ':';
            if (i == 7) *--strbuf = ':';
            continue;
        }
        do {
            *--strbuf = "0123456789abcdef"[ip2[i] & 15];
            ip2[i] >>= 4;
        } while (ip2[i]);
        if (i > 0) *--strbuf = ':';
    }
    i = strbuf - s;
    byte_copy(s,  IPTOSTR_LEN - i, strbuf);
    byte_zero(s + IPTOSTR_LEN - i, i);
    return s;
}
