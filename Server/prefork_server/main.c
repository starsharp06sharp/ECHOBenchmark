#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "serverSock.h"

#define QLEN 32
#define BUFSIZE 4096

void normalExit(int sig);
void reaper(int sig);
void prefork(int nproc);
int TCPechod(int fd);

int main(int argc, char **argv)
{
    char *service = "echo";
    struct sockaddr_in fsin;
    unsigned int alen = sizeof(fsin);
    int msock, ssock;
    int nproc;

    switch (argc) {
    case 1:
        break;
    case 2:
        service = argv[1];
        break;
    default:
        errexit(-1, "usage: server_fork [port]\n");
    }

    msock = passiveTCP(service, QLEN);

    signal(SIGINT, normalExit);
    signal(SIGCHLD, reaper);

    nproc = sysconf(_SC_NPROCESSORS_CONF) * 2;
    printf("%d process\n", nproc);

    prefork(nproc);

    while(1) {
        ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
        if (ssock < 0) {
            if (errno == EINTR) continue;
            errexit(-1, "accept: %s\n", strerror(errno));
        }
        TCPechod(ssock);
        close(ssock);
    }
}

int TCPechod(int fd)
{
    char buf[BUFSIZE];
    int cc;

    while (cc = read(fd, buf, sizeof(buf))) {
        if (cc < 0) {
            errexit(-1, "echo read: %s\n", strerror(errno));
        }
        if (write(fd, buf, cc) < 0) {
            errexit(-1, "echo write:%s\n", strerror(errno));
        }
    }

    return 0;
}

void normalExit(int sig)
{
    errexit(0, "\nExiting...\n");
}

void reaper(int sig)
{
    int status;

    while (wait3(&status, WNOHANG, (struct rusage *) NULL) >= 0) {
        ;
    }
}

void prefork(int nproc)
{
    nproc--;//主进程不算
    while(nproc--) {
        switch (fork()) {
            case 0:
                return;
            default:
                break;
            case -1:
                errexit(-1, "fork: %s\n", strerror(errno));
        }
    }
}
