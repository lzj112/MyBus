#ifndef _SOCKETSERVER_H
#define _SOCKETSERVER_H

#include "socketTCP.h"

class socketServer : public socketTCP 
{
public:
    int sendTo();
    int recvFrom();
};

#endif