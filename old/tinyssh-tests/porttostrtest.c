/*
20140201
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <stdio.h>
#include "fail.h"
#include "porttostr.h"
#include "crypto_uint16.h"

static void porttostr_bufferoverflow(void) {

    char buf[PORTTOSTR_LEN + 16];
    long long i, j;
    unsigned char port[2] = { 5, 5 };
    char ch[2] = {0x00, (char)0xff };

    if ((PORTTOSTR_LEN) < sizeof("65535")) fail("PORTTOSTR_LEN too small");

    for (j = 0; j < 2; ++j) {
        for (i = 0; i < sizeof buf; ++i) buf[i] = ch[j];
        porttostr(buf + 8, port);
        for (i = 0; i < 8; ++i) if (buf[i] != ch[j]) fail("porttostr writes before output");
        for (i = 0; i < 8; ++i) if (buf[i + PORTTOSTR_LEN + 8] != ch[j]) fail("porttostr writes after output");
    }
}

static struct vectors {
    const char *port;
    const char *portstr;
} testvectors[] = {
    { "\377\377", "65535" },
    { "\000\000", "0\0\0\0\0" },
    { 0, 0 }
};

static void porttostr_testvectors(void) {

    char *x, *y, *z;
    long long i, j;
    char buf[PORTTOSTR_LEN];

    for (i = 0; testvectors[i].port; ++i) {
        for (j = 0; j < sizeof buf; ++j) buf[j] = (char)0xff;
        x = porttostr(buf, (unsigned char *)testvectors[i].port);
        y = porttostr(0,   (unsigned char *)testvectors[i].port);
        z = (char *)testvectors[i].portstr;
        for (j = 0; j < PORTTOSTR_LEN; ++j) {
            if (buf[j] != z[j]) fail("bad output");
            if (x[j] != z[j]) fail("bad output");
            if (y[j] != z[j]) fail("bad output");
        }
    }
}

static void packport(unsigned char *y, crypto_uint16 x) {

    y[1] = x & 255; x >>= 8;
    y[0] = x & 255;
}

static void porttostr_full(void) {

    long long i, j;
    unsigned char port[2];
    char *x, *y;
    char buf[PORTTOSTR_LEN];
    char buf2[PORTTOSTR_LEN + 10];

    for (i = 0; i < 65536; ++i) {
        for (j = 0; j < sizeof buf; ++j) buf[j] = (char)0xff;
        for (j = 0; j < sizeof buf; ++j) buf2[j] = 0;

        packport(port, i);

        x = porttostr(buf, port);
        y = porttostr(0,   port);

        snprintf(buf2, sizeof buf2, "%lld", i);
        for (j = 0; j < PORTTOSTR_LEN; ++j) {
            if (buf[j] != buf2[j]) fail("bad output");
            if (x[j] != buf2[j]) fail("bad output");
            if (y[j] != buf2[j]) fail("bad output");
        }
    }
}

int main(void) {

    porttostr_bufferoverflow();
    porttostr_testvectors();
    porttostr_full();
    _exit(0);
}
