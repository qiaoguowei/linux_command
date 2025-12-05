#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>


int connect_tcpserver(const char *ip, int port) {
    int connfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in tcpserver_addr;
    memset(&tcpserver_addr, 0, sizeof(tcpserver_addr));

    tcpserver_addr.sin_family = AF_INET;
    tcpserver_addr.sin_addr = inet_addr(ip);
    tcpserver_addr.sin_port = htons(port);

    int ret = connect(connfd, (struct sockaddr*)&tcpserver_addr, sizeof(struct sockaddr_in));
    if (ret) {
        perror("connect()\n");
        return -1;
    }

    return connfd;
}

#define TEST_MESSAGE "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghigklmnopqrstuvwxyz\r\n"

int send_recv_tcppkt(int fd) {
    int res = send(fd, TEST_MESSAGE, strlen(TEST_MESSAGE), 0);
    if (res < 0) {
        exit(1);
    }

    char rbuffer[RBUFFER_LENGTH] = {0};
    res = recv(fd, rbuffer, RBUFFER_LENGTH, 0);
    if (res <= 0) {
        exit(1);
    }

    if (strcmp(rbuffer, TEST_MESSAGE) != 0) {
        printf("failed %s != '%s'\n", rbuffer, TEST_MESSAGE);
        return -1;
    }

    return 0
}

int main() {

}












