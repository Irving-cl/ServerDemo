
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

const int MAX_FD_COUNT = 10000;
const int MAX_CONN = 1000;

int nEpfd = -1;                                 // file descriptor for epoll
int nListenfd = -1;                             // file descriptor for listening
const int EPOLL_WAIT_TIME = -1;                 // Epoll wait time
const char *IP = "127.0.0.1";                   // Listen IP
const unsigned short PORT = 9027;               // Listen port
struct sockaddr_in sAddr;                       // socket address
struct epoll_event szEpEvents[MAX_FD_COUNT];    // array to store epoll_event
struct epoll_event ev;

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

void fill_sock_addr(struct sockaddr_in* ps_addr, const char* pszIP, unsigned short wPort, short int sin_family)
{
    memset(ps_addr, 0, sizeof(struct sockaddr_in));
    ps_addr->sin_family = sin_family;
    ps_addr->sin_port = htons(wPort);
    ps_addr->sin_addr.s_addr = inet_addr(pszIP);
}

bool start_listen(const char* pszIP, unsigned short wPort)
{
    // Create a socket for listening
    nListenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == nListenfd)
    {
        printf("Error creating socket, errno<%d>\n", errno);
        return false;
    }

    // fill socket address
    fill_sock_addr(&sAddr, pszIP, wPort, AF_INET);

    // Bind socket and address
    if (-1 == bind(nListenfd, (struct sockaddr*)&sAddr, sizeof(struct sockaddr_in)))
    {
        printf("Error binding socket to the address, errno<%d>\n", errno);
        return false;
    }

    // Start listening
    if (-1 == listen(nListenfd, MAX_CONN))
    {
        printf("Error start listening, errno<%d>\n", errno);
        return false;
    }

    // Post listen socket on epoll
    ev.data.fd = nListenfd;
    ev.events = EPOLLIN;    // when there is a connection, EPOLLIN will occur
    int nRet = epoll_ctl(nEpfd, EPOLL_CTL_ADD, nListenfd, &ev);
    if (-1 == nRet)
    {
        printf("Error calling epoll_ctl, errno<%d>\n", errno);
        return false;
    }

    return true;
}

void do_accept()
{
    struct sockaddr_in saddrRemote;
    int nAddrLen = sizeof(saddrRemote);
    int nfd = accept(nListenfd, (struct sockaddr*)&saddrRemote, (size_t*)&nAddrLen);
    if (-1 == nfd)
    {
        if ((errno != EAGAIN) && (errno != EWOULDBLOCK))
        {
            printf("Error accepting, errno:%d\n", errno);
            return;
        }
    }
    
    if (false == set_sock_non_block(nfd))
    {
        return;
    }

    // Report
    printf("Accept a connection, socket:%d\n", nfd);

    // Since a connection is established, recv msg from the client
    ev.data.fd = nfd;
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLRDHUP | EPOLLET;    // when there is a connection, EPOLLIN will occur
    int nRet = epoll_ctl(nEpfd, EPOLL_CTL_ADD, nfd, &ev);
    if (-1 == nRet)
    {
        printf("Error calling epoll_ctl, errno<%d>\n", errno);
        return;
    }
}

void do_recv(int nfd, int nEvents)
{
    if ((nEvents & EPOLLERR) || (nEvents & EPOLLHUP) || (nEvents & EPOLLRDHUP))
    {   // Connection close
        printf("Socket<%d> disconnect.\n", nfd);
        close(nfd);    // epoll will clear the fd automatically if we close it
        return;
    }

    // Receive data
    char szBuf[1024] = {0};
    ssize_t ulbytes = read(nfd, szBuf, 1024);
    printf("Socket<%d> receive:%s\n", nfd, szBuf);
    int nLen = strlen(szBuf);
    send(nfd, szBuf, nLen, 0);    // Send it back
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

    // Start listening
    if (false == start_listen(IP, PORT))
    {
        exit(-1);
    }

    // Main loop
    for ( ; ; )
    {
        int nfds = epoll_wait(nEpfd, szEpEvents, MAX_FD_COUNT - 3, EPOLL_WAIT_TIME);
        if (0 > nfds)
        {
            printf("epoll_wait error, errno<%d>\n", errno);
            break;
        }
        for (int i = 0; i < nfds; i++)
        {
            int nfd = szEpEvents[i].data.fd;
            if (nfd == nListenfd)
            {
                do_accept();
            }
            else
            {
                do_recv(nfd, szEpEvents[i].events);
            }
        }
    }
}
