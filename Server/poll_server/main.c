#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>

#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "serverSock.h"

#define QLEN 32
#define BUFSIZE 4096
#define MAX_SOCKET (1024 + 1)

void rmfd(struct pollfd fdarr[], int fd_num, int i);
int TCPechod(int fd);

int main(int argc, char **argv)
{
    char *service = "echo";
    struct sockaddr_in fsin;
    unsigned int alen = sizeof(fsin);
    int msock, ssock;

    struct pollfd fdarr[MAX_SOCKET];
    int fd_num = 1;
    int event_num;

    switch (argc) {
    case 1:
        break;
    case 2:
        service = argv[1];
        break;
    default:
        errexit(-1, "usage: server_poll [port]\n");
    }

    msock = passiveTCP(service, QLEN);

    for (int i = 0; i < MAX_SOCKET; ++i) {
        fdarr[i].fd = -1;
    }
    fdarr[0].fd = msock;
    fdarr[0].events = POLLIN;

    while(1) {
        event_num = poll(fdarr, fd_num, -1);
        if (event_num < 0) {
            errexit(-1, "poll: %s\n", strerror(errno));
        }

        for (int i = 0; i < fd_num; ++i) {
            if (fdarr[i].revents & POLLIN) {
                if (fdarr[i].fd == msock) {
                    ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
                    if (ssock < 0) {
                        errexit(-1, "accept: %s\n", strerror(errno));
                    }
                    fdarr[fd_num].fd = ssock;
                    fdarr[fd_num].events = POLLIN;
                    fd_num++;
                } else {
                    if (TCPechod(fdarr[i].fd) == 0) {
                        close(fdarr[i].fd);
                        rmfd(fdarr, fd_num, i);
                        fd_num--;
                    }
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

void rmfd(struct pollfd fdarr[], int fd_num, int i)
{
    for (; i < fd_num - 1; i++) {
        fdarr[i] = fdarr[i + 1];
    }
}