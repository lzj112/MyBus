#ifndef _SOCKETTCP_H
#define _SOCKETTCP_H


#include <unistd.h>
#include <sys/socket.h>

#include <iostream>

const int MAXLEN = 10;

class socketTCP 
{
public:
    socketTCP() : my_sockfd(-1){}
    virtual ~socketTCP() 
    {
        int res = Close(my_sockfd);
        if (res == -1) 
        {
            std::cout << "~socketTCP is failed" << std::endl;
        }
    }
    int getMysockfd() 
    {
        return my_sockfd;
    }
    int setNonBlock(int fd);
    int initSocketfd(int domain = AF_INET, int type = SOCK_STREAM, int protocol = 0);
    int Bind(int port, const char* ip);
    int Listen(int backlog = MAXLEN);
    int Connect(int port, const char* ip);
    int Accept();
    int Close(int fd);
    int Shutdown(int fd, int how);
    
    virtual int sendTo() = 0;
    virtual int recvFrom() = 0;

private:
    int my_sockfd;
};

#endif