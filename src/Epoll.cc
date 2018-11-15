#include <fcntl.h>
#include <memory.h>
#include <signal.h>
#include <netinet/in.h>

#include "Epoll.h"
using namespace std;

void Epoll::setNonblock(int fd) 
{
    //设置文件描述符为非阻塞
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

void Epoll::Create(int fd) 
{
    epollFd = epoll_create(1); 
    listenFd = fd;
    setNonblock(listenFd);

    Ctl(listenFd, EPOLL_CTL_ADD);
}

void Epoll::Ctl(int fd, int op) 
{
    epoll_event ev;
    memset(&ev, 0, sizeof(ev));
    ev.data.fd = fd;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollFd, op, fd, &ev);
}

void Epoll::Stop() 
{
    runEpoll = false;
    close(epollFd);
}

int Epoll::newConnect(int fd) 
{
    /*
    新的连接套接字也每个都设置成非阻塞
    */
    struct sockaddr_in client;
    socklen_t cliLength = sizeof(client);
    int connfd = accept(fd, (sockaddr *)&client, &cliLength); //读取新连接
    setNonblock(connfd);  //设置为非阻塞
    return connfd;
}

void Epoll::epollET(int epollFd, epoll_event* events, int ret) 
{
    for (int i = 0; i < ret; i++)
    {
        if (events[i].events & EPOLLIN) //有EPOLLIN事件
        {
            if (events[i].data.fd == listenFd) //是新的连接请求
            {
                int connfd = newConnect(listenFd);
                cout << "有新连接" << connfd << endl;
                Ctl(connfd, EPOLL_CTL_ADD); //将新的连接socketfd添加到合集
                // addToTimeWheel(connfd);           //更新socket活跃度
            }
            else if (events[i].data.fd != timerFd) //请求发消息
            {
                // timeWheel.adjust(events[i].data.fd);
                // assignedTask(events[i].data.fd); //解析下载请求
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
        else if (events[i].events & EPOLLRDHUP) //是客户端断开连接
        {
            // disconnect(events[i].data.fd, errno);
        }
    }
}

void Epoll::Run() 
{
    signal(SIGINT, SIG_IGN);    //忽略软终端
    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe

    int ret;
    while (runEpoll) 
    {
        ret = epoll_wait(epollFd, events, FDNUMBER, 0); //执行一次非阻塞检测
        if (ret == -1) 
        {
            std::cout << "epoll_wait is wrong" << std::endl;
        }        

        if (ret == 0)   //无事件 
        {
            continue;
        }
        else 
        {
            epollET(epollFd, events, ret); 
        }
    }
}
