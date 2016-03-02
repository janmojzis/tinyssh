/*
20130604
Jan Mojzis
Public domain.
*/

#include "numtostr.h"

/*
The 'numtostr(strbuf,n)' converts number 'n' into the 0-terminated string.
The caller must allocate at least NUMTOSTR_LEN bytes for 'strbuf'.
The 'numtostr' function is ready for 128-bit integer.
*/
char *numtostr(char *strbuf, long long n) {

    long long len = 0;
    unsigned long long n1, n2;
    static char staticbuf[NUMTOSTR_LEN];
    int flagsign = 0;

    if (!strbuf) strbuf = staticbuf; /* not thread-safe */

    if (n < 0) {
        n1 = n2 = -(unsigned long long)n;
        flagsign = 1;
    }
    else {
        n1 = n2 = (unsigned long long)n;
    }

    do {
        n1 /= 10; ++len;
    } while (n1);
    if (flagsign) ++len;
    strbuf += len;

    do {
        *--strbuf = '0' + (n2 % 10);
        n2 /= 10;
    } while (n2);
    if (flagsign) *--strbuf = '-';

    while (len < NUMTOSTR_LEN) strbuf[len++] = 0;
    return strbuf;
}
