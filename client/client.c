
#include "../include/def.h"

int main(int argc, char **argv) {
    int    sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2) {
    	perror("usage: tcpcli <IP address>");
    	exit(-1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
}