#include "socketBus.h" 

void socketBus::startListening(const char* ip, int port) 
{
    initSocketfd();
    Bind(port, ip);
    Listen(10);
}