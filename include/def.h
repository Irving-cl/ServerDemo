
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define SA  struct sockaddr

#define LISTENQ        1024     /* 等待连接队列的最大长度 */
#define MAXLINE        4096     /* 一行的最大长度 */
#define SERV_PORT      9877

#define	min(a,b)	((a) < (b) ? (a) : (b))
#define	max(a,b)	((a) > (b) ? (a) : (b))
