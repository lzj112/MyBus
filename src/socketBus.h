#ifndef _SOCKETBUS_H
#define _SOCKETBUS_H

#include "Epoll.h"
#include "socketTCP.h"

class socketBus : public socketTCP 
{
public:
    void startListening(const char* ip, int port);
    int sendTo();
    int recvFrom();
    Epoll myEpoll;
};

#endif