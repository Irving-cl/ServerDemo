
#include "def.h"

ssize_t readline(int fd, void *vptr, size_t maxlen) {
    ssize_t n, rc;
    char    c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ((rc = read(fd, &c, 1)) == 1) {
            *ptr++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1)
                return 0;   /* EOF, 没有读到数据 */
            else
                break;      /* EOF, 读到了一些数据 */
        } else
            return -1;      /* error */
    }
    *ptr = 0;
    return(n);
}

void str_cli(FILE *fp, int sockfd) {
    char    sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL) {
        write(sockfd, sendline, strlen(sendline));
        if (readline(sockfd, recvline, MAXLINE) == 0) {
            perror("str_cli: server terminated prematurely");
        }
        fputs(recvline, stdout);
    }
}

int main(int argc, char **argv) {
    int    sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
        perror("usage: tcpcli <IP address>");
        exit(-1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* 创建套接字，用服务器的IP地址和端口号装填一个套接字地址结构 */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    /* 连接到服务器 */
    connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    str_cli(stdin, sockfd);
    exit(0);
}
