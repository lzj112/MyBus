#ifndef _ALLOCPORT_H_
#define _ALLOCPORT_H_

/*
为中转进程提供唯一端口号来bind
*/


class AllocPort 
{
public:
    AllocPort() 
    {
        this->port--;
    }
    ~AllocPort() 
    {
        if (port == 2048) 
        {
            port = 4096;
        }
    }
    int getPort() 
    {
        return port;
    }
private:
    static int port;
};

int AllocPort::port = 4096;

#endif