#ifndef _SOCKETTCP_H
#define _SOCKETTCP_H


#include <unistd.h>
#include <sys/socket.h>

#include <iostream>

const int MAXLEN = 10;

class socketTCP 
{
public:
    socketTCP() : fdTCP(-1), fdUDP(-1){}
    virtual ~socketTCP() 
    {
    }
    int getMysockTCP() 
    {
        return fdTCP;
    }
    int getMysockUDP() 
    {
        return fdUDP;
    }
    int setNonBlock(int fd);
    void initSocketTCP();
    void initSocketUDP();
    int Bind(const char* ip, int port);
    int Listen(int backlog = MAXLEN);
    int Connect(const char* ip, int port);
    int Accept();
    int Close(int fd);
    int Shutdown(int fd, int how);
    
    
    // virtual int sendTo() = 0;
    // virtual int recvFrom() = 0;

private:
    int fdTCP;
    int fdUDP;
};

#endif