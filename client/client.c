
#include "../include/def.h"

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
                return 0;   // EOF, 没有读到数据
            else
                break;      // EOF, 读到了一些数据
        } else
            return -1;      // error
    }
    *ptr = 0;
    return(n);
}

void str_cli(FILE *fp, int sockfd) {
    int    maxfdpl;                               // 待测试的描述符个数，值为待测试的最大描述符+1
    int    stdineof;                              // 表示标准输入的可读性
    fd_set rset;                                  // 读描述符集，这里有标准输入和socket两个
    char   buf[MAXLINE];
    int    n;

    stdineof = 0;
    FD_ZERO(&rset);                               // 描述符集置0
    for ( ; ; ) {
        if (stdineof == 0) {
            FD_SET(fileno(fp), &rset);            // 将标准输入置为感兴趣
        }
        FD_SET(sockfd, &rset);                    // 将socket置为感兴趣
        maxfdpl = max(fileno(fp), sockfd) + 1;    // 设置待测试的描述符个数
        select(maxfdpl, &rset, NULL, NULL, NULL); // 调用select，只有在两个描述符中任何一个可读时才返回

        if (FD_ISSET(sockfd, &rset)) {            // select返回后发现socket可读
            if ((n = read(sockfd, buf, MAXLINE)) == 0) {
                if (stdineof == 1) {              // 标准输入也已终止，正常退出
                    return;
                } else {                          // 服务器意外终止
                    perror("str_cli: server terminated prematurely");
                    exit(-1);
                }
            }
            write(fileno(stdout), buf, n);
        }

        if (FD_ISSET(fileno(fp), &rset)) {        // select返回后发现标准输入可读
            if ((n = read(fileno(fp), buf, MAXLINE)) == 0) {
                stdineof = 1;
                shutdown(sockfd, SHUT_WR);
                FD_CLR(fileno(fp), &rset);
                continue;                         // 此时还不能关闭客户端，因为可能有包在从服务端到客户端的路上
            }
            write(sockfd, buf, n);
        }
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