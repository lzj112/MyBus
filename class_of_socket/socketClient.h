#ifndef _SOCKETCLIENT_H
#define _SOCKETCLIENT_H

#include "socketTCP.h"

class socketClient : public socketTCP 
{
public:
    int sendTo();
    int recvFrom();
};

#endif