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
    Epoll() : listenFd(-1), udpfd(-1)
    {
        epollFd = epoll_create(1);
    }
    ~Epoll() 
    {
        close(listenFd);
    }
    void setNonblock(int fd);   
    void Create(int fd, int flag = 0);
    // void Ctl(int fd, int op);
    void Add(int fd, int op);    
    void Del(int fd);
    void Mod(int fd);
    
    int newConnect(int listenFd);
    
    // std::vector<int> Wait();
    epoll_event* Wait(int& ret);
    std::vector<int> epollET(int epollFd, epoll_event* EVENTS, int ret);
    
    // void addToTimeWheel(int fd);
    // void setTimer();
    // int recvFrom(int fd, void* ptr, int n);

private:
    int epollFd;            //epoll句柄
    epoll_event events[FDNUMBER];  //fd合集
    int listenFd;             //监听socketfd
    int udpfd;
    // TimeWheel timeWheel;   //时间轮
    // int timerFd;           //定时器fd
}; 


#endif