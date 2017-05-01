#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <errno.h>
#include "serverSock.h"

#define QLEN 32
#define BUFSIZE 4096
#define MAX_SOCKET (1024 + 1)

int TCPechod(int fd);

int main(int argc, char **argv)
{
    char *service = "echo";
    struct sockaddr_in fsin;
    int msock, ssock;
    unsigned int alen = sizeof(fsin);

    int epfd;
    struct epoll_event event;
    struct epoll_event events[MAX_SOCKET];
    int event_num;
    int event_sock;

    switch (argc) {
    case 1:
        break;
    case 2:
        service = argv[1];
        break;
    default:
        errexit(-1, "usage: server_epoll [port]\n");
    }

    msock = passiveTCP(service, QLEN);

    epfd = epoll_create(MAX_SOCKET);
    event.events = EPOLLIN;
    event.data.fd = msock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, msock, &event);

    while(1) {
        event_num = epoll_wait(epfd, events, MAX_SOCKET, -1);
        for (int i = 0; i < event_num; ++i) {
            event_sock = events[i].data.fd;
            if (event_sock == msock) {
                ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
                if (ssock < 0) {
                    errexit(-1, "accept: %s\n", strerror(errno));
                }
                event.data.fd = ssock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, ssock, &event);
            } else {
                if (TCPechod(event_sock) == 0) {
                    close(event_sock);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, event_sock, NULL);
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
