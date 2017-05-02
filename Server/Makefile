CFLAGS := -I headers -Wall -O2 -std=c99

all: server_block server_fork server_pthread server_prefork server_select server_poll server_epoll server_prefork_epoll

server_block: block_server/main.c
	$(CC) block_server/main.c -o server_block $(CFLAGS)

server_fork: fork_server/main.c
	$(CC) fork_server/main.c -o server_fork $(CFLAGS)

server_pthread: pthread_server/main.c
	$(CC) pthread_server/main.c -o server_pthread $(CFLAGS) -lpthread

server_prefork: prefork_server/main.c
	$(CC) prefork_server/main.c -o server_prefork $(CFLAGS)

server_select: select_server/main.c
	$(CC) select_server/main.c -o server_select $(CFLAGS)

server_poll: poll_server/main.c
	$(CC) poll_server/main.c -o server_poll $(CFLAGS)

server_epoll: epoll_server/main.c
	$(CC) epoll_server/main.c -o server_epoll $(CFLAGS)

server_prefork_epoll: prefork_epoll_server/main.c
	$(CC) prefork_epoll_server/main.c -o server_prefork_epoll $(CFLAGS)

.PHONY: clean
clean:
	rm server_*