/*
20140222
Jan Mojzis
Public domain.
*/

#include "purge.h"
#include "trymlock.h"
#include "packet.h"

struct packet packet = {0};

void packet_purge(void) {
    purge(&packet, sizeof packet);
    trymunlock(&packet, sizeof packet);
}

void packet_init(void) {

    trymlock(&packet, sizeof packet);
    purge(&packet, sizeof packet);

    packet.flagkeys = 0;
    packet.flagauthorized = 0;
    packet.flagrekeying = 0;
    packet.flagclosesent = 0;
    packet.flageofsent = 0;
    packet.flagchanneleofreceived = 0;
    packet.sendpacketid = 0;
    packet.receivepacketid = 0;
    packet.packet_length = 0;

    buf_init(&packet.hellosend, packet.hellosendspace, sizeof packet.hellosendspace);
    buf_init(&packet.helloreceive, packet.helloreceivespace, sizeof packet.helloreceivespace);
    buf_init(&packet.kexsend, packet.kexsendspace, sizeof packet.kexsendspace);
    buf_init(&packet.kexrecv, packet.kexrecvspace, sizeof packet.kexrecvspace);
    buf_init(&packet.hashbuf, packet.hashbufspace, sizeof packet.hashbufspace);
    buf_init(&packet.sendbuf, packet.sendbufspace, sizeof packet.sendbufspace);
    buf_init(&packet.recvbuf, packet.recvbufspace, sizeof packet.recvbufspace);
}

