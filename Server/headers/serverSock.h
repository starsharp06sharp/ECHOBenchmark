#ifndef SERVERSOCK_H_INCLUDED
#define SERVERSOCK_H_INCLUDED

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "utils.h"

unsigned short portbase = 0;

int passiveSock (const char *service, const char *transport, int qlen);

int passiveTCP (const char *service, int qlen)
{
    return passiveSock(service, "tcp", qlen);
}

int passiveUDP (const char *service)
{
    return passiveSock(service, "udp", 0);
}

int passiveSock (const char *service, const char *transport, int qlen)
{
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;

    int s, type;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if (pse = getservbyname(service, transport)) {
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port) + portbase);
    } else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0) {
        errexit(-1, "can't get \"%s\" service entry\n", service);
    }

    if ((ppe = getprotobyname(transport)) == 0) {
        errexit(-1, "can't get \"%s\" protocol entry\n", transport);
    }

    if (strcmp(transport, "udp") == 0) {
        type = SOCK_DGRAM;
    } else {
        type = SOCK_STREAM;
    }

    s = socket(PF_INET, type, ppe->p_proto);
    if (s < 0) {
        errexit(-1, "can't create socket: %s\n", strerror(errno));
    }

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        errexit(-1, "can't bind to %s port:%s\n", service, strerror(errno));
    }

    if (type == SOCK_STREAM && listen(s, qlen) < 0) {
        errexit(-1, "can't listen on %s port:%s\n", service, strerror(errno));
    }

    return s;
}

#endif // SERVERSOCK_H_INCLUDED
