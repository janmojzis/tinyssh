#include "str.h"

/*
The 'str_len(s)' function calculates the length of the string 's'.
*/
long long str_len(const char *s) {

    long long i;

    for (i = 0; s[i]; ++i);
    return i;
}

/*
The 'str_start(s,t)' function returns 1 if t is a prefix of s, 0 otherwise.
*/
int str_start(const char *s, const char *t) {

    char x;

    for (;;) {
        x = *t++;
        if (!x) return 1;
        if (x != *s++) return 0;
    }
}

/*
The 'str_equaln(y,ylen,x)' function returns 1 if the length of string 'x' is ylen and 
if y and x match up to, 0 otherwise.
*/
int str_equaln(const char *y, long long ylen, const char *x) {

    long long i;

    if (str_len(x) != ylen) return 0;
    for (i = 0; i < ylen; ++i) if (y[i] != x[i]) return 0;
    return 1;
}

/*
The 'str_copyn(y,ylen,x)' function copies the string pointed to by 'x' (including \0) to the buffer pointed to by 'y'.
If the length of string x is smaller than ylen or equal, then function returns 1.
If the length of string x is biger than ylen, then only ylen - 1 bytes of 'x' are copied and function returns 0.
The 'y' string is always \0 terminated.
*/
int str_copyn(char *y, long long ylen, const char *x) {

    long long len = str_len(x);
    int ret = 1;

    if (ylen <= 0) return 0;
    if (ylen - 1 < len) { 
        len = ylen - 1;
        ret = 0;
    }
    y[len] = 0;
    while (len > 0) { *y++ = *x++; --len; }
    return ret;
}
