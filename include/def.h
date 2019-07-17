
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

#define SA  struct sockaddr

#define LISTENQ        1024     /* 等待连接队列的最大长度 */
#define MAXLINE        4096     /* 一行的最大长度 */
#define SERV_PORT      9877

