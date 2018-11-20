#include "socketBus.h" 

void socketBus::startListening(const char* ip, int port) 
{
    initSocketfd();
    Bind(ip, port);
    Listen(10);
}