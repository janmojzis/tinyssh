/*
20140429
Jan Mojzis
Public domain.
*/

#include "e.h"
#include "loginshell.h"

/*
The 'loginshell()' function converts shell path
into -shell name. For example:
'/bin/sh' -> '-sh'
'/usr/pkg/bin/bash' -> '-bash'
*/

int loginshell(char *out, long long outlen, const char *in) {

    long long len, pos = -1;

    if (!out || !in || outlen < 2) { errno = EINVAL; return 0; }

    for (len = 0; in[len]; ++len) if (in[len] == '/') pos = len;
    in += pos + 1;
    len -= pos + 1;
    if (len > outlen - 2) len = outlen - 2; /* truncate name */

    *out++ = '-';
    while (len > 0) { *out++ = *in++; --len; }
    *out = 0;

    return 1;
}
