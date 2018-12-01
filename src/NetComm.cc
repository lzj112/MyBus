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
    int res = initShmList(proj_id);
    assert(res != -1);

    res = initShmList(proj_id, 1);
    assert(res != -1);
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

/*
同一个主机上的不同进程应该访问的都是
这一块共享内存
*/
int NetComm::initShmList(int id)  //初始化路由表
{
    key_t key = ftok(PATH, id + 1);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 1" << std::endl;
        return -1;
    }

    netList[0] = shmget(key, sizeof(RoutingTable) * QUEUESIZE, IPC_CREAT | 0666);
    if (netList[0] == -1) 
    {
        netList[0] = shmget(key, 0, 0);
        if (netList[0] == -1) 
        {
            perror("shmget in initshmlist");
        }
    }
    netList[1] = 0;
    netList[2] = 0;

    return 0;
}

int NetComm::initShmList(int id, int) //初始化中转通道
{
    key_t key = ftok(PATH, id + 2);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 2" << std::endl;
        return -1;
    }

    proList[0] = shmget(key, sizeof(proToNetqueue) * QUEUESIZE, IPC_CREAT | 0666);
    if (proList[0] == -1) 
    {
        proList[0] = shmget(key, 0, 0);
        if (proList[0] == -1)
        {
            perror("shmget in initshmlist");
        }
    }
    proList[1] = 0;
    proList[2] = 0;

    return 0;
}

int NetComm::creShmQueue(int proj_id) 
{
    key_t key = ftok(PATH, proj_id);
    assert(key != -1);

    int shmid = shmget(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        shmid = shmget(key, 0, 0);
        if (shmid == -1) 
        {
            perror("shmget in creshmqueue");
        }
    }

    return shmid;
}

//更新路由表
int NetComm::updateList(int sockfd, const char* ip, int port) 
{
// printf("参数 in updatelist ip = %s port = %d***\n", ip, port);
// printf("befor update RoutingTable front = %d rear = %d\n", netList[1], netList[2]);
    RoutingTable* tmpAddr = static_cast<RoutingTable *> (shmat(netList[0], nullptr, 0)) + netList[2];
    if ((netList[2] + 1) % QUEUESIZE == netList[1]) 
    {
        return -1;
    }
    memset(tmpAddr, 0 ,sizeof(RoutingTable));
    
    tmpAddr->sockfd = sockfd;
    tmpAddr->port = port;
    strcpy(tmpAddr->IP, ip);
// printf("RoutingTable更新后ip = %s port = %d\n", tmpAddr->IP, tmpAddr->port);
    netList[2]  = (netList[2] + 1) % QUEUESIZE;   //尾加一
// printf("after update RoutingTable front = %d rear = %d\n", netList[1], netList[2]);

    shmdt(tmpAddr);

    return 0;
}

//更新进程通道表
int NetComm::updateList(const struct ProComm& str) 
{
    int front = proList[1];
    int rear = proList[2];
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proList[0], nullptr, 0)) + rear;
    if ((rear + 1) % QUEUESIZE == front) 
    {
        return -1;
    }

    //赋值新节点
    tmpAddr->offset = rear;
    tmpAddr->destPort = str.sourcePassPort;
    tmpAddr->sourcePort = str.destPassPort;
    strcpy(tmpAddr->sourceIP, str.destPassIP);
    strcpy(tmpAddr->destIP, str.sourcePassIP);
    tmpAddr->readQueue[0] = creShmQueue(str.destPort | READ);
    tmpAddr->readQueue[1] = 0;
    tmpAddr->readQueue[2] = 0;

    proList[2] = (proList[2] + 1) % QUEUESIZE;   //尾加一
    
    shmdt(tmpAddr);
    
    return rear;
}

//从路由表获得连接sockfd
int NetComm::isThereConn(const char* ip, int port) 
{
// printf("参数 in isthereconn ip = %s port = %d---\n", ip, port);
// printf("RoutingTable : front + %d rear = %d\n", netList[1], netList[2]);
    //指向头结点下一节点
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(netList[0], nullptr, 0)) + netList[1];

    int connfd = -1;
    for (int i = netList[1];;) 
    {
    // printf("RoutingTable中的节点: ip = %s port = %d\n", (nodePtr+i)->IP, (nodePtr+i)->port);
        if (netList[2] == i) 
        {
            break;
        }   
        if ((strcmp((nodePtr + i)->IP, ip) == 0) && 
            (nodePtr + i)->port == port) 
        {
            connfd = (nodePtr + i)->sockfd;
            break;
        }         
        i = (i + 1) % QUEUESIZE;
    }

    return connfd;
}

//从进程通道表获得对应表项
int NetComm::getProShmQueue(const char* ip, int port, int flag) 
{
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(proList[0], nullptr, 0)) + proList[1];
    if (nodePtr == (void *)-1) 
    {
        perror("shmat ");
        return -1;
    }
    int queueID = -1;
    for (int i = proList[1]; ;) 
    {
        if (i == proList[2]) 
        {
            break ;
        }
        if ((strcmp((nodePtr + i)->sourceIP, ip) == 0) && 
            (nodePtr + i)->sourcePort == 0)
        {
            queueID = (nodePtr + i)->offset;
            break;
        }
        i = (i + 1) % QUEUESIZE;
    }
    return queueID;
}


void NetComm::runMyEpoll() 
{
    int tcpfd = socketControl.getMysockTCP();
    int udpfd = socketControl.getMysockUDP();
    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe
    
    int ret;
    epoll_event events[FDNUMBER];
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
                else 
                {
                    std::cout << "读取别的物理机发来的消息\n" << std::endl;
                    int connfd = events[i].data.fd;
                    std::thread t(&NetComm::recvFromTCP, this, connfd);
                    t.detach();
                }
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
                perror("recv is wrong");
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

//收到其他物理机发来的数据,将消息存入共享内存, shmid是进程通道表的对应节点的readqueueid
void NetComm::saveMessage(int offset, const PacketBody& str) 
{
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proList[0], nullptr, 0)) + offset;
    if (tmpAddr == (void *)-1) 
    {
        perror("shmat in savemessage in savemessage");
        return ;
    }
    //读取队列是本机上进程通过此通道读取其他进程发来的信息
    //读取队列的头尾指针
    int front = tmpAddr->readQueue[1];
    int rear = tmpAddr->readQueue[2];

    //队列满 抛弃数据
    if ((rear + 1) % QUEUESIZE == front) 
    {
        return ;
    }
    
    //读队列是进程会用这个通道读取其他进程发来的数据
    PacketBody* queueAddr = (static_cast<PacketBody *> (shmat(tmpAddr->readQueue[0], nullptr, 0))) + rear;
    copy(queueAddr, str);
    tmpAddr->readQueue[2] = (tmpAddr->readQueue[2] + 1) % QUEUESIZE;
}

//将收到的数据写入共享内存缓冲区
void NetComm::copy(PacketBody* ptr, const PacketBody& str) 
{
    ptr->head.type = str.head.type;
    strcpy(ptr->buffer, str.buffer);
    ptr->head.bodySzie = str.head.bodySzie;
    ptr->netQuaad.destPort = str.netQuaad.destPort;
    ptr->netQuaad.sourcePort = str.netQuaad.sourcePort;
    strcpy(ptr->netQuaad.sourceIP, str.netQuaad.sourceIP);
    strcpy(ptr->netQuaad.destIP, str.netQuaad.destIP);
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
    
    getMessage(connfd, &tmpBuffer, sizeof(PacketBody));
    
    //处理数据
    dealData(connfd, tmpBuffer);
}

void NetComm::dealData(int connfd, const PacketBody& tmpBuffer) 
{
    //获取接收此消息的共享内存缓冲区(proToNetqueue)
    int proShmid = getProShmQueue(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, WRITE); 
    if (proShmid == -1) 
    {
        //如果没有该缓冲区,创建一个
        proShmid = updateList(tmpBuffer.netQuaad);
    }
    //shmid中获取写队列 写入数据 
    saveMessage(proShmid, tmpBuffer);
    //若是一个新的连接 更新路由表
    int netShmid = isThereConn(tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);
    if (netShmid == -1) 
    {
        //路由表中没有这个链接,将发送端ip和端口保存
        updateList(connfd, tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);    
    }

    //告知进程共享内存通道偏移量
    //收到跨机器发来的消息并存入共享内存后,通知本机进程通道表共享内存id
    int tmp[2] = {proList[0], proShmid};
    socketControl.inform(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, tmp);

}

void NetComm::recvFromUDP(int connfd) 
{
    Notice tmpBuffer;
    int ret = recvfrom(connfd, (void *)&tmpBuffer, sizeof(tmpBuffer), 0, nullptr, nullptr);
    if (ret < 0) 
    {
        perror("recvFromUDP");
    }
    forwarding(tmpBuffer);
}

void NetComm::forwarding(const Notice& str)  
{
    //从共享内存中获取本机进程存入的buffer, 发往另一个物理机
    PacketBody tmp;
    copy(tmp, str);
    
    int connfd = isThereConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
    if (connfd == -1) 
    {
        connfd = socketControl.makeNewConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
        updateList(connfd, str.netQueaad.destPassIP, str.netQueaad.destPassPort);
        printf("建立新连接%d\n", connfd);
    }
    else 
    {
        printf("使用已有连接%d\n", connfd);
    }
    socketControl.sendTo(tmp, connfd);
}

void NetComm::realseAll() 
{
    shmctl(netList[0], IPC_RMID, nullptr);

    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(proList[0], nullptr, 0));
    shmctl(tmpAddr->readQueue[0], IPC_RMID, nullptr);
    shmdt(tmpAddr);
    shmctl(proList[0], IPC_RMID, nullptr);
}