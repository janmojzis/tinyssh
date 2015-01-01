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
char *numtostr(char *strbuf, num n) {

    long long len = 0;
    num_u nn;
    static char staticbuf[NUMTOSTR_LEN];
    int flagsign = 0;

    if (!strbuf) strbuf = staticbuf; /* not thread-safe */

    if (n < 0) {
        n = -n;
        flagsign = 1;
    }

    nn = n;
    do {
        nn /= 10; ++len;
    } while (nn);
    if (flagsign) ++len;
    strbuf += len;

    nn = n;
    do {
        *--strbuf = '0' + (nn % 10);
        nn /= 10;
    } while (nn);
    if (flagsign) *--strbuf = '-';

    while (len < NUMTOSTR_LEN) strbuf[len++] = 0;
    return strbuf;
}
