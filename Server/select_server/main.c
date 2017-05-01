#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "serverSock.h"

#define QLEN 32
#define BUFSIZE 4096

int TCPechod(int fd);

int main(int argc, char **argv)
{
    char *service = "echo";
    struct sockaddr_in fsin;
    unsigned int alen = sizeof(fsin);
    int msock, ssock;
    fd_set rfds;
    fd_set afds;
    int nfds;

    switch (argc) {
    case 1:
        break;
    case 2:
        service = argv[1];
        break;
    default:
        errexit(-1, "usage: server_select [port]\n");
    }

    msock = passiveTCP(service, QLEN);

    nfds = getdtablesize();
    // printf("dtable size: %d\n", nfds);
    FD_ZERO(&afds);
    FD_SET(msock, &afds);

    while(1) {
        memcpy(&rfds, &afds, sizeof(rfds));

        if (select(nfds, &rfds, NULL, NULL, NULL) < 0) {
            errexit(-1, "select: %s\n", strerror(errno));
        }
        if (FD_ISSET(msock, &rfds)) {
            ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
            if (ssock < 0) {
                errexit(-1, "accept: %s\n", strerror(errno));
            }
            FD_SET(ssock, &afds);
        }
        for (int fd = 0; fd < nfds; ++fd) {
            if (fd != msock && FD_ISSET(fd, &rfds)) {
                if (TCPechod(fd) == 0) {
                    close(fd);
                    FD_CLR(fd, &afds);
                }
            }
        }
    }
}

int TCPechod(int fd)
{
    char buf[BUFSIZE];
    int cc;

    cc = read(fd, buf, sizeof(buf));
    if (cc < 0) {
        errexit(-1, "echo read: %s\n", strerror(errno));
    }
    if (cc > 0 && write(fd, buf, cc) < 0) {
        errexit(-1, "echo write:%s\n", strerror(errno));
    }

    return cc;
}