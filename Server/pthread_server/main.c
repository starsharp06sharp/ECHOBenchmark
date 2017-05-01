#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
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
    pthread_t th;
    pthread_attr_t ta;

    pthread_attr_init(&ta);
    //不需要知道线程状态
    pthread_attr_setdetachstate(&ta, PTHREAD_CREATE_DETACHED);

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

    while(1) {
        ssock = accept(msock, (struct sockaddr *)&fsin, &alen);
        if (ssock < 0) {
            if (errno == EINTR) continue;
            errexit(-1, "accept: %s\n", strerror(errno));
        }
        
        if (pthread_create(&th, &ta, (void * (*)(void *))TCPechod, (void *)ssock) < 0) {
            errexit(-1, "pthread_create: %s\n", strerror(errno));
        }
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
    close(fd);

    return 0;
}
