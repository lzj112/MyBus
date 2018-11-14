#ifndef _EPOLL_H
#define _EPOLL_H

#include <sys/epoll.h>

#include "socketBus.h"

#define FDNUMBER 1204
// const int FDNUMBER = 1024;

class Epoll 
{
public:
    Epoll() : epollFd(-1), listenFd(-1), runEpoll(true){}
    ~Epoll() 
    {
        close(listenFd);
    }
    void setNonblock(int fd);   
    void epoll_Create(int fd);
    void epoll_Ctl(int fd, int op);
    void epoll_Run();
    void Stop();
    void epollET(int epollFd, epoll_event* EVENTS, int ret);
    int newConnect(int listenfd);
    void shutDownFd(int fd);
    void addToTimeWheel(int fd);
    void setTimer();
    int recvFrom(int fd, void* ptr, int n);

private:
    int epollFd;            //epoll句柄
    epoll_event events[FDNUMBER];  //fd合集
    int listenFd;             //监听socketfd
    bool runEpoll;         //停止标志
    // TimeWheel timeWheel;   //时间轮
    int timerFd;           //定时器fd
}; 


#endif