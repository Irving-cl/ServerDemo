
#include "../include/def.h"

/* 
 * in_addr结构
 * typedef uint32_t in_addr_t;
 * struct in_addr {
 *     in_addr_t s_addr;
 * }
 */

/*
 * sockaddr_in结构
 * struct sockaddr_in {
 *     short sin_family;
 *     unsigned short sin_port;    // 端口号，必须采用网络数据格式，普通数字可以用htons()转换成网络数据格式的数字
 *     struct in_addr sin_addr;    // ip地址，必须按照网络字节顺序
 *     unsigned char sin_zero[8];  // 没有实际意义，为了和sockaddr结构大小相同
 * }
 */

void str_echo(int sockfd) {
    ssize_t    n;
    char       buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0) {
        write(sockfd, buf, n);
    }
    if (n < 0 && errno == EINTR) {    // EINTR：信号中断造成的错误返回值
        goto again;
    } else if (n < 0) {
        perror("str_echo: read error");
        exit(1);
    }
}

int main(int argc, char **argv) {
    int     i, maxi, maxfd, listenfd, connfd, sockfd;
    int     nready, client[FD_SETSIZE];
    ssize_t n;
    fd_set  rset, allset;
    char    buf[MAXLINE];
    socklen_t clilen;    // 即int，客户端地址长度
    struct sockaddr_in cliaddr, servaddr;

    /*
     * 创建TCP套接字，并且转换成一个监听套接字 
     */
    listenfd = socket(AF_INET, SOCK_STREAM, 0);            // 根据指定的地址族，数据类型和协议创建一个套接字
    bzero(&servaddr, sizeof(servaddr));                    // servaddr清零
    servaddr.sin_family = AF_INET;                         // 设置地址族
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);          // host to network long，将一个32位数从主机字节顺序转换成网络字节顺序
    servaddr.sin_port = htons(SERV_PORT);                  // host to network short，将一个16位数从主机字节顺序转换成网络字节顺序
    bind(listenfd, (SA *) &servaddr, sizeof(servaddr));    // 绑定socket和addr
    listen(listenfd, LISTENQ);                             // 使套接字开始监听，第二个参数backlog是等待连接队列的最大长度

    /*
     * 初始化
     */
    maxfd = listenfd;
    maxi = -1;                                             // client数组当前使用项的最大下标
    for (i = 0; i < FD_SETSIZE; i++) {                     // 将client数组中的每一项初始化为-1
        client[i] = -1;
    }
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);                             // 在描述符集中将监听描述符置为1

    for ( ; ; ) {
        rset = allset;
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    // 调用select

        if (FD_ISSET(listenfd, &rset)) {                   // listenfd可读，表示产生了新的客户连接
            clilen = sizeof(cliaddr);
            connfd = accept(listenfd, (SA *)&cliaddr, &clilen);
            for (i = 0; i < FD_SETSIZE; i++) {             // 在client数组中寻找可用的位置，保存connfd
                if (client[i] < 0) {
                    client[i] = connfd;
                    break;
                }
            }
            if (i == FD_SETSIZE) {                         // 客户连接数超过上限
                perror("too many clients");
                exit(-1);
            }
            FD_SET(connfd, &allset);                       // 在描述符集中添加新的描述符
            if (connfd > maxfd) {
                maxfd = connfd;                            // 更新maxfd
            }
            if (i > maxi) {
                maxi = i;                                  // 更新maxi
            }
            if (--nready <= 0) {                           // 已无其他可读描述符，进入下一个循环
                continue;
            }
        }

        for (i = 0; i <= maxi; i++) {                      // 检查所有已连接客户是否发来数据
            if ((sockfd = client[i]) < 0) {                // 这一格无客户连接
                continue;
            }
            if (FD_ISSET(sockfd, &rset)) {                 // 当前客户的描述符可读
                if ((n = read(sockfd, buf, MAXLINE)) == 0) {    // 当前客户关闭了连接
                    close(sockfd);                         // 释放已占用资源
                    FD_CLR(sockfd, &allset);               
                    client[i] = -1;
                } else {
                    write(sockfd, buf, n);                 // 进行回射
                }
                if (--nready <= 0) {
                    break;                                 // 已无其他可读描述符，进入下一个循环
                }
            }
        }
    }
}