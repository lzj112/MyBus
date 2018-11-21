#include "socketBus.h" 

void socketBus::startListening(const char* ip, int port) 
{
    initSocketfd();
    Bind(ip, port);
    Listen(10);
}

int socketBus::sendTo(const char* ip, int port, Notice buffer) 
{
    Connect(ip, port);
    int count = 0;
    int ret = 0;
    int length = sizeof(buffer);
    
    int sockfd = getMysockfd();
    while (count < length) 
    {
        ret = send(sockfd, (void *)&buffer, sizeof(buffer), 0);
        if (ret == -1 && errno != EAGAIN) 
        {
            break;
        }
        count += ret;
    }
    return count;
}