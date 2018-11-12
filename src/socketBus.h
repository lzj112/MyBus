#ifndef _SOCKETBUS_H
#define _SOCKETBUS_H

#include "socketTCP.h"

class socketBus : public socketTCP 
{
public:
    int sendTo();
    int recvFrom();
};

#endif