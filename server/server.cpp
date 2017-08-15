
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_FD_COUNT = 10000;

int nEpfd = -1;        // file descriptor for epoll
int nListenfd = -1;    // file descriptor for listening

bool set_sock_non_block(int nfd)
{
    int flags = fcntl(nfd, F_GETFL, 0);
    if (0 > flags)
    {
        printf("Error getting socket flags, errno<%d>\n", errno);
        return false;
    }
    if (fcntl(nfd, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        printf("Error setting socket O_NONBLOCK, errno<%d>\n", errno);
        return false;
    }
    return true;
}

bool start_listen()
{
    // Create a socket for listening
    nListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nListenfd)
    {
        printf("Error creating socket, errno<%d>\n", errno);
        return false;
    }
}

int main()
{
    // Create epoll object
    nEpfd = epoll_create(MAX_FD_COUNT);
    if (-1 == nEpfd)
    {
        printf("Error creating epoll, errno:<%d>\n", errno);
        exit(-1);
    }
}
