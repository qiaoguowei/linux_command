#include <stdio.h>
#include <liburing.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <string.h>
// io_uring, tcp server
// multhread, select/poll, epoll, coroutine, io_uring, reactor

// io
/* 3个系统调用 io_uring_setup(); io_uring_register(); io_uring_enter();  ---> liburing*/

#define ENTERIES_LENGTH 4096

enum {
    READ,
    WRITE,
    ACCEPT 
};

struct  conninfo {
    int connfd;
    int type;
};

void set_recv_event(struct io_uring* ring, int fd, void* buf, size_t len, int flag) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    io_uring_prep_recv(sqe, fd, buf, len, flag);

    struct conninfo ci = {
        .connfd = fd,
        .type = READ
    };

    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));

}
void set_send_event(struct io_uring* ring, int fd, const void* buf, size_t len, int flag) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    io_uring_prep_send(sqe, fd, buf, len, flag);

    struct conninfo ci = {
        .connfd = fd,
        .type = WRITE
    };

    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));

}

void set_accept_event(struct io_uring* ring, int fd, struct sockaddr* clientaddr, 
    socklen_t* clientlen, int flag) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    io_uring_prep_accept(sqe, fd, (struct sockaddr*)&clientaddr, clientlen, flag);

    struct conninfo ci = {
        .connfd = fd,
        .type = ACCEPT
    };

    memcpy(&sqe->user_data, &ci, sizeof(struct conninfo));

}

int init_server(unsigned short port) {

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	if (-1 == bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(struct sockaddr))) {
		perror("bind");
		return -1;
	}

	listen(sockfd, 10);

	return sockfd;
}

int main() {
    int listenfd = init_server(9001);

    struct io_uring_params params;
    memset(&params, 0, sizeof(struct io_uring_params));

    struct io_uring ring;
    io_uring_queue_init_params(ENTERIES_LENGTH, &ring, &params) ;

    struct sockaddr_in clientaddr;
    socklen_t clientlen = sizeof(clientaddr);
    set_accept_event(&ring, listenfd, (struct sockaddr*)&clientaddr, &clientlen, 0);

    char buffer[1024] = {0};

    while (1) {
        struct io_uring_cqe *cqe;
        io_uring_submit(&ring);

        io_uring_wait_cqe(&ring, &cqe);

        struct io_uring_cqe* cqes[10];
        int cqecount = io_uring_peek_batch_cqe(&ring, cqes, 10);

        int i = 0;
        int count = 0;
        for (i = 0; i < cqecount; i++) {
            cqe = cqes[i];
            count++;
            struct conninfo ci;
            memcpy(&ci, &cqe->user_data, sizeof(ci));

            if (ci.type == ACCEPT) {
                set_accept_event(&ring, listenfd, (struct sockaddr*)&clientaddr, &clientlen, 0);
                int connfd = cqe->res;
                set_recv_event(&ring, connfd, buffer, 1024, 0);
            } else if (ci.type == READ) {
                int bytes_read = cqe->res;
                if (bytes_read == 0) {
                    close(ci.connfd);
                } else if (bytes_read < 0) {

                }
                printf("buffer : %s\n", buffer);

                set_send_event(&ring, ci.connfd, buffer, bytes_read, 0);
            } else if (ci.type == WRITE) {

                set_recv_event(&ring, ci.connfd, buffer, 1024, 0);
            }
        }
        io_uring_cq_advance(&ring, count);
    }
}