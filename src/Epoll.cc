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

// void Epoll::Create(int fd) 
// {
//     epollFd = epoll_create(1); 
//     listenFd = fd;
//     setNonblock(listenFd);

//     Add(listenFd, EPOLLIN);
// }

void Epoll::Create(int fd, int flag) 
{
    if (flag == 0) 
    {
        listenFd = fd;
    }
    else 
    {
        udpfd = fd;
    }
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

int Epoll::newConnect(int listenFd) 
{
    int connfd;
    /*
    新的连接套接字也每个都设置成非阻塞
    */
    struct sockaddr_in client;
    socklen_t cliLength = sizeof(client);
    while (1)
   { 
        memset(&client, 0, cliLength);   
        connfd = accept(listenFd, (sockaddr *)&client, &cliLength); //读取新连接
        if (connfd <= 0) 
        {
            break;
        }
        setNonblock(connfd);  //设置为非阻塞
        Add(connfd, EPOLLIN);       //将新的连接socketfd添加到合集
    }
    if (connfd == -1) 
    {
        if (errno != EAGAIN && errno != ECONNABORTED && errno != EINTR) 
        {
            perror("accept is wrong : ");
        }
    }
    return connfd;
}

/*
std::vector<int> Epoll::epollET(int epollFd, epoll_event* events, int ret) 
{
    epoll_event events[FDNUMBER];
    std::vector<int> tmp;
    for (int i = 0; i < ret; i++)
    {
        if (events[i].events & EPOLLIN) //有EPOLLIN事件
        {
            if (events[i].data.fd == listenFd) //是新的连接请求
            {
                //循环读取防止多个连接到来
                newConnect(listenFd);
            }
            else if (events[i].data.fd & EPOLLIN) //有数据可读
            {
                tmp.push_back(events[i].data.fd);
                // timeWheel.adjust(events[i].data.fd);
                // assignedTask(events[i].data.fd); //解析下载请求
                
            }
            else if (events[i].data.fd == udpfd) 
            {
                
            }
        //     else    //定时器到期
        //     {
        //         uint64_t numExp;
        //         ssize_t s = read(events[i].data.fd, &numExp, sizeof(uint64_t));
        //         if (s == sizeof(uint64_t))
        //         {
        //             timeWheel.tick(); //指定定时任务
        //         }
        //     }
        }
    }
    return tmp;
}
*/

// epoll_event* Epoll::Wait(int& ret, epoll_event* events)
void Epoll::Wait(int& ret, epoll_event* events)
{
    // epoll_event events[FDNUMBER];  //fd合集
    ret = epoll_wait(epollFd, events, FDNUMBER, 0); //执行一次非阻塞检测
    if (ret == -1) 
    {
        perror("epoll_wait has err ");
        exit(1);
    }    
    
    // return events;
}





