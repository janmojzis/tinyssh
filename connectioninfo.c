/*
20140131
Jan Mojzis
Public domain.
*/

#include <sys/types.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "e.h"
#include "byte.h"
#include "iptostr.h"
#include "porttostr.h"
#include "env.h"
#include "str.h"
#include "connectioninfo.h"

/*
The connectioninfo_fromfd function gets
informations about TCP connection from
getsockname(), getpeername() libc functions.
*/

static int connectioninfo_fromfd(char *localip, char *localport, char *remoteip, char *remoteport) {

    long long i;
    struct sockaddr_storage sa;
    socklen_t salen; 
    int fd = 0;
    unsigned char ip[16];
    unsigned char port[2];
    char *ipstr[2];
    char *portstr[2];
    int (*op[2])(int, struct sockaddr *, socklen_t *) = { getsockname, getpeername };
    
    ipstr[0] = localip;  portstr[0] = localport;
    ipstr[1] = remoteip; portstr[1] = remoteport;

    for (i = 0; i < 2; ++i) {
        
        salen = sizeof sa;
        if (op[i](fd, (struct sockaddr *)&sa, &salen) == -1) return 0;

        if (sa.ss_family == PF_INET) {
            struct sockaddr_in *sin = (struct sockaddr_in *)&sa;
            byte_copy(ip, 12, "\0\0\0\0\0\0\0\0\0\0\377\377");
            byte_copy(ip + 12, 4, &sin->sin_addr);
            byte_copy(port, 2, &sin->sin_port);
        }
#ifdef PF_INET6
        else if (sa.ss_family == PF_INET6) {
            struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)&sa;
            byte_copy(ip, 16, &sin6->sin6_addr);
            byte_copy(port, 2, &sin6->sin6_port);
        }
#endif
        else {
            errno = EPROTONOSUPPORT;
            return 0;
        }
        iptostr(ipstr[i], ip);
        porttostr(portstr[i], port);
    }
    return 1;

}

static int env(char *y, long long ylen, const char *e) {

    char *x;

    x = env_get(e);
    if (!x) return 0;
    if (!str_copyn(y, ylen, x)) return 0;
    return 1;
}

/*
The connectioninfo_fromtcpserverenv function gets
informations about TCP connection from environment.
*/
static int connectioninfo_fromtcpserverenv(char *localip, char *localport, char *remoteip, char *remoteport) {

    if (!env(localip, IPTOSTR_LEN, "TCPLOCALIP")) return 0;
    if (!env(localport, PORTTOSTR_LEN, "TCPLOCALPORT")) return 0;
    if (!env(remoteip, IPTOSTR_LEN, "TCPREMOTEIP")) return 0;
    if (!env(remoteport, PORTTOSTR_LEN, "TCPREMOTEPORT")) return 0;
    return 1;
}

/*
The connectioninfo_fromcurvecpserverenv function gets
informations about CurveCP connection from environment.
*/
static int connectioninfo_fromcurvecpserverenv(char *localip, char *localport, char *remoteip, char *remoteport) {

    if (!env(localip, IPTOSTR_LEN, "CURVECPLOCALIP")) return 0;
    if (!env(localport, PORTTOSTR_LEN, "CURVECPLOCALPORT")) return 0;
    if (!env(remoteip, IPTOSTR_LEN, "CURVECPREMOTEIP")) return 0;
    if (!env(remoteport, PORTTOSTR_LEN, "CURVECPREMOTEPORT")) return 0;
    return 1;
}

#define unknown "unknown"
#define port0 "0"

/*
The connectioninfo function gets informations about TCP/CurveCP connection.
*/
void connectioninfo(char *localip, char *localport, char *remoteip, char *remoteport) {
    if (connectioninfo_fromtcpserverenv(localip, localport, remoteip, remoteport)) return;
    if (connectioninfo_fromcurvecpserverenv(localip, localport, remoteip, remoteport)) return;
    if (connectioninfo_fromfd(localip, localport, remoteip, remoteport)) return;

    byte_copy(localip, sizeof unknown, unknown);
    byte_copy(remoteip, sizeof unknown, unknown);
    byte_copy(localport, sizeof port0, port0);
    byte_copy(remoteport, sizeof port0, port0);
    return;
}
