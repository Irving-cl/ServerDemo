
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define SA  struct sockaddr

#define LISTENQ        1024     /* 等待连接队列的最大长度 */
#define MAXLINE        4096     /* 一行的最大长度 */
#define SERV_PORT      9877

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
    int    listenfd, connfd;
    pid_t  childpid;
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


    for ( ; ; ) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (SA *) &cliaddr, &clilen);    // 阻塞于accept调用，等待客户连接完成
        if ((childpid = fork()) == 0) {                    // 为每一个客户派生一个处理它们的子进程
            close(listenfd);                               // 关闭监听描述符
            str_echo(connfd);                              // 回射
            exit(0);                                       // 子进程任务完成，终止
        }
        close(connfd);                                     // 关闭父进程的已连接描述符(引用计数减1)，防止这个套接字永远也关不掉
    }
}