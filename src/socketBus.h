#ifndef _SOCKETBUS_H
#define _SOCKETBUS_H

#include "socketTCP.h"
#include "BusInfo.h"

class socketBus : public socketTCP 
{
public:
    void startListening(const char* ip, int port);
    int sendTo(const char* ip, int port, Notice buffer);
    int sendTo(const PacketBody& str, int connfd = -1); 
    int makeNewConn(const char* ip, int port);
    void inform(const char* ip, int port, int* tmp); 
    void recvFrom(int* buf, int length);
    // void recvFrom(const char* ip, int port, int* buf, int length);
};  

#endif