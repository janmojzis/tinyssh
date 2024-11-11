/*
20140302
Jan Mojzis
Public domain.
*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "fail.h"
#include "porttostr.h"
#include "iptostr.h"
#include "connectioninfo.h"
#include "byte.h"

static void connectioninfotest_empty(void) {

    char localip[IPTOSTR_LEN];
    char localport[PORTTOSTR_LEN];
    char remoteip[IPTOSTR_LEN];
    char remoteport[PORTTOSTR_LEN];

    connectioninfo(localip, localport, remoteip, remoteport);

    if (strcmp(localip,    "unknown")) fail("connectioninfo failed");
    if (strcmp(localport,  "0")) fail("connectioninfo failed");
    if (strcmp(remoteip,   "unknown")) fail("connectioninfo failed");
    if (strcmp(remoteport, "0")) fail("connectioninfo failed");
}

static void connectioninfotest_tcpserverenv(void) {

    char localip[IPTOSTR_LEN];
    char localport[PORTTOSTR_LEN];
    char remoteip[IPTOSTR_LEN];
    char remoteport[PORTTOSTR_LEN];

    if (setenv("TCPLOCALIP",    "aaax", 1) == -1) fail("setenv failed")
    if (setenv("TCPLOCALPORT",  "bbbx", 1) == -1) fail("setenv failed")
    if (setenv("TCPREMOTEIP",   "cccx", 1) == -1) fail("setenv failed")
    if (setenv("TCPREMOTEPORT", "dddx", 1) == -1) fail("setenv failed")
    connectioninfo(localip, localport, remoteip, remoteport);

    if (strcmp(localip,    "aaax")) fail("connectioninfo failed");
    if (strcmp(localport,  "bbbx")) fail("connectioninfo failed");
    if (strcmp(remoteip,   "cccx")) fail("connectioninfo failed");
    if (strcmp(remoteport, "dddx")) fail("connectioninfo failed");

    if (unsetenv("TCPLOCALIP") == -1) fail("unsetenv failed")
    if (unsetenv("TCPLOCALPORT") == -1) fail("unsetenv failed")
    if (unsetenv("TCPREMOTEIP") == -1) fail("unsetenv failed")
    if (unsetenv("TCPREMOTEPORT") == -1) fail("unsetenv failed")
}

static void connectioninfotest_curvecpserverenv(void) {
#ifdef TODO

    char localip[IPTOSTR_LEN];
    char localport[PORTTOSTR_LEN];
    char remoteip[IPTOSTR_LEN];
    char remoteport[PORTTOSTR_LEN];

    if (setenv("CURVECPLOCALIP",    "aaay", 1) == -1) fail("setenv failed")
    if (setenv("CURVECPLOCALPORT",  "bbby", 1) == -1) fail("setenv failed")
    if (setenv("CURVECPREMOTEIP",   "cccy", 1) == -1) fail("setenv failed")
    if (setenv("CURVECPREMOTEPORT", "dddy", 1) == -1) fail("setenv failed")
    connectioninfo(localip, localport, remoteip, remoteport);

    if (strcmp(localip,    "aaay")) fail("connectioninfo failed");
    if (strcmp(localport,  "bbby")) fail("connectioninfo failed");
    if (strcmp(remoteip,   "cccy")) fail("connectioninfo failed");
    if (strcmp(remoteport, "dddy")) fail("connectioninfo failed");

    if (unsetenv("CURVECPLOCALIP") == -1) fail("unsetenv failed")
    if (unsetenv("CURVECPLOCALPORT") == -1) fail("unsetenv failed")
    if (unsetenv("CURVECPREMOTEIP") == -1) fail("unsetenv failed")
    if (unsetenv("CURVECPREMOTEPORT") == -1) fail("unsetenv failed")
#endif
}


static struct vectors {
    const char *port;
    const char *portstr;
} testvectors[] = {
    { "\377\000", "65280" },
    { "\377\001", "65281" },
    { "\377\002", "65282" },
    { "\377\003", "65283" },
    { "\377\004", "65284" },
    { "\377\005", "65285" },
    { "\377\006", "65286" },
    { "\377\007", "65287" },
    { "\377\010", "65288" },
    { "\377\011", "65289" },
    { "\377\012", "65290" },
    { "\377\013", "65291" },
    { "\377\014", "65292" },
    { "\377\015", "65293" },
    { "\377\016", "65294" },
    { "\377\017", "65295" },
    { "\377\020", "65296" },
    { "\377\021", "65297" },
    { "\377\022", "65298" },
    { "\377\023", "65299" },
    { 0, 0 }
};

static void connectioninfotest_fromfd(void) {

    int fd;
    struct sockaddr_in sa;
    long long i, j;
    char localip[IPTOSTR_LEN];
    char localport[PORTTOSTR_LEN];
    char remoteip[IPTOSTR_LEN];
    char remoteport[PORTTOSTR_LEN];

    /* create socket */
    fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (fd == -1) fail("socket failed");

    /* bind */
    for (i = 0; testvectors[i].port; ++i) {
        byte_zero(&sa, sizeof sa);
        sa.sin_family = PF_INET;
        byte_copy(&sa.sin_addr, 4, "\177\0\0\1");
        byte_copy(&sa.sin_port, 2, testvectors[i].port);
        if (bind(fd, (struct sockaddr *)&sa, sizeof sa) == 0) goto bind;
    }
    fail("bind failed");

bind:
    /* connect */
    for (j = i; testvectors[j].port; ++j) {
        byte_zero(&sa, sizeof sa);
        sa.sin_family = PF_INET;
        byte_copy(&sa.sin_addr, 4, "\177\0\0\1");
        byte_copy(&sa.sin_port, 2, testvectors[i].port);
        if (connect(fd, (struct sockaddr *)&sa, sizeof sa) == 0) goto connect;
    }
    fail("connect failed");

connect:

    close(0);
    if (dup(fd) != 0) fail("dup failed");
    close(fd);

    connectioninfo(localip, localport, remoteip, remoteport);
    if (strcmp(localip,    "127.0.0.1")) fail("connectioninfo failed");
    if (strcmp(localport,  testvectors[i].portstr)) fail("connectioninfo failed");
    if (strcmp(remoteip,   "127.0.0.1")) fail("connectioninfo failed");
    if (strcmp(remoteport,  testvectors[j].portstr)) fail("connectioninfo failed");
    close(0);
}

int main(void) {

    connectioninfotest_empty();
    connectioninfotest_tcpserverenv();
    connectioninfotest_empty();
    connectioninfotest_curvecpserverenv();
    connectioninfotest_empty();
    connectioninfotest_fromfd();
    connectioninfotest_empty();
    _exit(0);
}
