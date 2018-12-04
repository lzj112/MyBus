#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <netinet/in.h>

#include <iostream>

#include "Epoll.h"
#include "BusInfo.h"

void Epoll::setNonblock(int fd) 
{
    //设置文件描述符为非阻塞
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void Epoll::Create(int fdtcp, int fdudp) 
{
    listenFd = fdtcp;
    udpfd = fdudp;
}

void Epoll::Add(int fd, int op) 
{
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op | EPOLLET;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &ev);
}

void Epoll::Del(int fd) 
{
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, &ev);
}

int Epoll::newConnect(int listenFd) //好像在accept阻塞了 不是用的非阻塞么?明天查一下
{
    int connfd = -1;
    /*
    新的连接套接字也每个都设置成非阻塞
    */
    struct sockaddr_in client;
    socklen_t cliLength = sizeof(client);
    //防止连接淤积
    while (1)
    { 
        memset(&client, 0, cliLength);   
        connfd = accept(listenFd, (sockaddr *)&client, &cliLength); //读取新连接
        if (connfd <= 0) 
        {
            break;
        }
        else 
        {
            setNonblock(connfd);  //设置为非阻塞
            Add(connfd, EPOLLIN);       //将新的连接socketfd添加到合集
        }
    }
    if (connfd == -1) 
    {
        if (errno != EAGAIN && errno != ECONNABORTED && errno != EINTR) 
        {
            perror("accept is wrong : ");
        }
    }
    return 0;
}

void Epoll::Wait(int& ret, epoll_event* events)
{
    ret = epoll_wait(epollFd, events, FDNUMBER, 0); //执行一次非阻塞检测
    if (ret == -1) 
    {
        perror("epoll_wait has err ");
        exit(1);
    }    
    if (ret > 0) 
    {
        std::cout << "epoll got somethings\n";
    }
}





