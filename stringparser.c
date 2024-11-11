/*
20140203
Jan Mojzis
Public domain.
*/

#include "byte.h"
#include "e.h"
#include "bug.h"
#include "stringparser.h"

/*
The 'stringparser' function parses items from
comma-separated list. Empty strings are ignored.
*/
long long stringparser(const unsigned char *buf, long long len, long long pos, unsigned char **x, long long *xlen) {

    long long i;

    if (!buf || len < 0 || len > 1073741824 || pos < 0 || pos > 1073741824 || !x || !xlen) bug_inval();

    for (;;) {
        if (pos >= len) return 0;

        for (i = pos; i < len; ++i) {
            if (buf[i] == ',') break;
        }
        *x = (unsigned char *)buf + pos;
        *xlen = i - pos;
        pos = i + 1;
        if (*xlen != 0) return pos;
    }
}
