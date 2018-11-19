#ifndef _EPOLL_H
#define _EPOLL_H

#include <sys/epoll.h>

#include <vector>

#include "socketBus.h"

#define FDNUMBER 1204
// const int FDNUMBER = 1024;

class Epoll 
{
public:
    Epoll() : epollFd(-1), listenFd(-1){}
    ~Epoll() 
    {
        close(listenFd);
    }
    void setNonblock(int fd);   
    void Create(int fd);
    void Ctl(int fd, int op);
    
    int newConnect(int listenFd);
    
    std::vector<int> Wait();
    std::vector<int> epollET(int epollFd, epoll_event* EVENTS, int ret);
    
    // void addToTimeWheel(int fd);
    // void setTimer();
    // int recvFrom(int fd, void* ptr, int n);

private:
    int epollFd;            //epoll句柄
    epoll_event events[FDNUMBER];  //fd合集
    int listenFd;             //监听socketfd
    // TimeWheel timeWheel;   //时间轮
    // int timerFd;           //定时器fd
}; 


#endif