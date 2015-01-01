/*
20140429
Jan Mojzis
Public domain.
*/

#include "e.h"
#include "str.h"
#include "byte.h"
#include "loginshell.h"

/*
The 'loginshell()' function converts shell path
into -shell name. For example:
'/bin/sh' -> '-sh'
'/usr/pkg/bin/bash' -> '-bash'
*/

int loginshell(char *out, long long outlen, const char *in) {

    long long i, len;

    if (!out || !in || outlen <= 0) { errno = EINVAL; return 0; }

    len = str_len(in);
    for (i = len; i > 0 && in[i] != '/'; --i);
    if (in[i] == '/') ++i;
    in += i;

    out[0] = '-';
    if (!str_copyn(out + 1, outlen - 1, in)) { errno = ENOMEM; return 0; }
    return 1;
}
