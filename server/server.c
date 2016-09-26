
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>

#define	LISTENQ        1024
#define SERV_PORT      9877
#define INADDR_ANY     ((unsigned long int) 0x00000000)

/* 
 * in_addr结构
 * typedef uint32_t in_addr_t;
 * struct in_addr {
 * 	   in_addr_t s_addr;
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

int main(int argc, char **argv) {
	int    listenfd, connfd;
	pid_t  childpid;
	socklen_t chilen;
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
	Listen(listenfd, LISTENQ);                             // 使套接字开始监听，第二个参数backlog是等待连接队列的最大长度


	/*
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		if ( (childpid = Fork()) == 0) {
			Close(listenfd);
			str_echo(connfd);
			exit(0);
		}
		Close(connfd);
	}*/
}