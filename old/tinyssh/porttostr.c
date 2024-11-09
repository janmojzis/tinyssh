/*
20130604
Jan Mojzis
Public domain.
*/

#include "crypto_uint16.h"
#include "porttostr.h"

/*
The 'porttostr(strbuf,port)' converts 'port' from network byte order
into the 0-terminated string. The 'port' length is always 2 bytes.
The caller must allocate at least PORTTOSTR_LEN bytes for 'strbuf'.
*/
char *porttostr(char *strbuf, const unsigned char *port) {

    long long len = 0;
    crypto_uint16 num;
    static char staticbuf[PORTTOSTR_LEN];

    if (!strbuf) strbuf = staticbuf; /* not thread-safe */

    num = port[0]; num <<= 8; num |= port[1];
    do {
        num /= 10; ++len;
    } while (num);
    strbuf += len;

    num = port[0]; num <<= 8; num |= port[1];
    do {
        *--strbuf = '0' + (num % 10);
        num /= 10;
    } while (num);

    while (len < PORTTOSTR_LEN) strbuf[len++] = 0;
    return strbuf;
}
