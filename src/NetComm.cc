#include <cstring>
#include <assert.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <arpa/inet.h>
#include <sys/types.h>

#include <iostream>
#include <thread>
#include <algorithm>

#include "NetComm.h"

void NetComm::initList(int proj_id) 
{
    int res = netList.initNetShmList(proj_id);  //初始化路由表
    if (res == -1) 
    {
        printf("netlist init is wrong\n");
        return ;
    }
    res = netList.initTime(30, 60);   //初始化定时器 
    if (res == -1) 
    {
        printf("time init is wrong\n");
        return ;
    }
    else 
    {
        myEpoll.Add(res, EPOLL_CTL_ADD);
    }


    res = proList.initProShmList(proj_id);  //初始化进程通道表
    if (res == -1) 
    {
        printf("prolist init is wrong\n");
        return ;
    }
    res = proList.initTime(30, 180);      //初始化定时器
    if (res == -1) 
    {
        printf("time init is wrong\n");
        return ;
    }
    else 
    {
        myEpoll.Add(res, EPOLL_CTL_ADD);
    }
}

void NetComm::prepareSocket(const char* ip, int port) 
{
    socketControl.initSocketTCP();
    socketControl.initSocketUDP();
    socketControl.startListening(ip, port);

    int tcpfd = socketControl.getMysockTCP();
    int udpfd = socketControl.getMysockUDP();

    myEpoll.Create(tcpfd, udpfd);
    myEpoll.Add(tcpfd, EPOLLIN);
    myEpoll.Add(udpfd, EPOLLIN);
}

void NetComm::runMyEpoll() 
{
    int tcpfd = socketControl.getMysockTCP();
    int udpfd = socketControl.getMysockUDP();
    int netTimefd = netList.getTimerfd();
    int proTimefd = proList.getTimerfd();

    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe
    
    int ret;
    epoll_event events[FDNUMBER];
    bool isNetTimeOut = false;
    bool isProTimeOut = false;
    while (isRun) 
    {
        ret = 0;
        myEpoll.Wait(ret, events);
        for (int i = 0; i < ret; i++) 
        {
            if (events[i].events & EPOLLIN) 
            {
                if (events[i].data.fd == udpfd) //本机发来的udp通知
                {
                    std::cout << "收到本机udp通知\n" << std::endl;
                    int connfd = events[i].data.fd;
                    std::thread t(&NetComm::recvFromUDP, this, connfd);
                    t.detach();
                }
                else if (events[i].data.fd == tcpfd) 
                {
                    std::cout << "有别的物理机的连接的请求\n" << std::endl;
                    myEpoll.newConnect(tcpfd);
                }
                else if (events[i].data.fd == netTimefd) //定时事件先标记
                {
                    isNetTimeOut = true;
                }
                else if (events[i].data.fd == proTimefd) 
                {
                    isProTimeOut = true;
                }
                else 
                {
                    std::cout << "读取别的物理机发来的消息\n" << std::endl;
                    int connfd = events[i].data.fd;
                    std::thread t(&NetComm::recvFromTCP, this, connfd);
                    t.detach();
                }
            }
            if (isNetTimeOut) //最后处理定时任务,优先级不高
            {
                isNetTimeOut = false;
                netList.check();    //检查路由表满了么
            }
            if (isProTimeOut) 
            {
                isProTimeOut = false;
                proList.check();    //检查进程通道表满了么
            }
        }
    }
}

//从套接字接收消息
int NetComm::getMessage(int connfd, PacketBody* buffer, int length) 
{
    int count = length;
    int ret = 0;
    while (count > 0) //循环读取直到读完指定字节或出错
    {
        ret = recv(connfd, buffer, length, 0);
        if (ret == -1) 
        {
            if (errno == EINTR || errno == EWOULDBLOCK) 
            {
                ret = 0;
                continue;
            }
            else 
            { 
                perror("一个连接断开\n");
                return -1;
            }
        }
        if (ret == 0) 
        {
            myEpoll.Del(connfd);
            close(connfd);
            count = -1;
            break;
        }
        count -= ret;
        buffer += ret;
    }
    return count;
}


//将共享内存中的数据拿出来发给另一个物理机
void NetComm::copy(PacketBody& str, const Notice& tmp) 
{
    str.head.type = (READY >> 1) + 12;
    str.head.bodySzie = sizeof(PacketBody);
    strcpy(str.netQuaad.destIP, tmp.netQueaad.destIP);
    strcpy(str.netQuaad.sourceIP, str.netQuaad.sourceIP);
    strcpy(str.netQuaad.destPassIP, tmp.netQueaad.destPassIP);
    strcpy(str.netQuaad.sourcePassIP, tmp.netQueaad.sourcePassIP);
    str.netQuaad.destPort = tmp.netQueaad.destPort;
    str.netQuaad.sourcePort = tmp.netQueaad.sourcePort;
    str.netQuaad.destPassPort = tmp.netQueaad.destPassPort;
    str.netQuaad.sourcePassPort = tmp.netQueaad.sourcePassPort;
    str.head.bodySzie = tmp.head.bodySzie + sizeof(ProComm);

    BusCard* tmpAddr1 = static_cast<BusCard *> (shmat(tmp.shmid, nullptr, 0));
    if (tmpAddr1->netQueue[1] != tmpAddr1->netQueue[2])
    {
        PacketBody* tmpAddr2 = static_cast<PacketBody *> (shmat(tmpAddr1->netQueue[0], nullptr, 0)) + tmpAddr1->netQueue[1];
        strcpy(str.buffer, tmpAddr2->buffer);
        tmpAddr1->netQueue[1]++;
        
        shmdt(tmpAddr1);
        shmdt(tmpAddr2);
    }
}

void NetComm::recvFromTCP(int connfd) 
{
    PacketBody tmpBuffer;
    memset(&tmpBuffer, 0, sizeof(PacketBody));
    
    int ret = getMessage(connfd, &tmpBuffer, sizeof(PacketBody));
    if (ret == -1) 
    {
        netList.delNode(connfd);
        return ;
    }
    //处理数据
    dealData(connfd, tmpBuffer);
}

void NetComm::dealData(int connfd, const PacketBody& tmpBuffer) 
{
    //获取进程通道表对应表项的偏移量(proToNetqueue)
    int proShmid = proList.getProShmQueue(tmpBuffer.netQuaad); 
    if (proShmid == -1) 
    {
        //如果没有该缓冲区,创建一个
        proShmid = proList.updateList(tmpBuffer.netQuaad);
    }
    //shmid中获取写队列 写入数据 
    proList.saveMessage(proShmid, tmpBuffer);
    //若是一个新的连接 更新路由表
    int netShmid = netList.isThereConn(tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);
    if (netShmid == -1) 
    {
        //路由表中没有这个链接,将发送端ip和端口保存
        netList.updateList(connfd, tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);    
    }
    //告知进程共享内存通道偏移量
    //收到跨机器发来的消息并存入共享内存后,通知本机进程通道表共享内存id
    int tmp[2] = {proList.getShmID(), proShmid};
    socketControl.inform(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, tmp);

}

void NetComm::recvFromUDP(int connfd) 
{
    Notice tmpBuffer;
    socketControl.recvFrom(&tmpBuffer, sizeof(tmpBuffer));
    
    forwarding(tmpBuffer);
}

void NetComm::forwarding(const Notice& str)  
{
    //从共享内存中获取本机进程存入的buffer, 发往另一个物理机
    PacketBody tmp;
    copy(tmp, str);
  
    //防止连接失效
    int res = 0;
    int connfd = 0;
    do 
    {
        connfd = netList.isThereConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
        if (connfd == -1) 
        {
            connfd = socketControl.makeNewConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
            netList.updateList(connfd, str.netQueaad.destPassIP, str.netQueaad.destPassPort);
            printf("建立新连接%d\n", connfd);
        }
        else 
        {
            printf("使用已有连接%d\n", connfd);
        }
        res = socketControl.sendTo(tmp, connfd);   //失败时返回-1
    } while(res == -1);

}
