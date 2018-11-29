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
    socketControl.initSocketfd();
    socketControl.initSocketfd(1);
    socketControl.startListening(ip, port);

    int tcpfd = socketControl.getMysockfd();
    int udpfd = socketControl.getMysockfd(1);

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

    // if (netListHead_Addr == nullptr) 
    // {
    //     netListHead_Addr = static_cast<RoutingTable *> (shmat(netList, nullptr, 0));
    //     memset(netListHead_Addr, 0, sizeof(RoutingTable));
    // }

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
    // if (proListHead_Addr == nullptr) 
    // {
    //     proListHead_Addr = static_cast<proToNetqueue *> (shmat(proList, nullptr, 0));
    //     memset(proListHead_Addr, 0, sizeof(proToNetqueue));
    // }

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
    /*
    如果端口一样ip就一样,就是同一个连接,就不会存储进来
    会直接用那个连接,所以port会是唯一的
    */
    //每个端口都唯一,这样保证了key唯一
    int front = netList[1];
    int rear = netList[2];
    RoutingTable* tmpAddr = static_cast<RoutingTable *> (shmat(netList[0], nullptr, 0)) + rear;
    if ((rear + 1) % QUEUESIZE == front) 
    {
        return -1;
    }
    memset(tmpAddr, 0 ,sizeof(RoutingTable));
    
    tmpAddr->sockfd = sockfd;
    tmpAddr->port = port;
    strcpy(tmpAddr->IP, ip);
    netList[2]  = (netList[2] + 1) % QUEUESIZE;   //尾加一

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

// int NetComm::delListNode(int fd, const RoutingTable& str) 
// {
//     int shmidTmp = netListHead_Addr->shmidNext;
//     //指向头结点的下一个节点
//     RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(shmidTmp, nullptr, 0));

//     RoutingTable* prePtr = netListHead_Addr;    //指向前一个节点
//     while (1)
//     {
//         //该节点中sockfd就是目标值
//         if (nodePtr->sockfd == fd) 
//         {
//             //前一个节点指向该节点下一个节点
//             int tmp = prePtr->shmidNext;
//             prePtr->shmidNext = nodePtr->shmidNext;

//             //删除该节点
//             int res = shmdt(nodePtr);
//             res = shmctl(tmp, IPC_RMID, 0);
//         }
        
//         //前一节点指向该节点
//         prePtr = nodePtr;
//         int nextId = nodePtr->shmidNext;
//         RoutingTable* tmpAddr = nodePtr;
//         if (nextId == 0) 
//         {
//             break;
//         }
//         //该指针指向下一块共享内存
//         nodePtr = static_cast<RoutingTable *> (shmat(nextId, nullptr, 0));
//         //解除该节点的内存映射
//         shmdt(tmpAddr);
//     }
//     netListHead_Addr->listLength--;
// }

// int NetComm::delListNode(const char* ip, int port, const proToNetqueue& str) 
// {
//     int shmidTmp = proListHead_Addr->shmidNext;
//     //指向头结点的下一个节点
//     proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(shmidTmp, nullptr, 0));

//     proToNetqueue* prePtr = proListHead_Addr;    //指向前一个节点
//     while (1)
//     {
//         //该节点中sockfd就是目标值
//         if ((strcmp(ip, nodePtr->destIP) == 0) && port == nodePtr->destPort) 
//         {
//             //前一个节点指向该节点下一个节点
//             int tmp = prePtr->shmidNext;
//             prePtr->shmidNext = nodePtr->shmidNext;

//             //删除该节点
//             int res = shmdt(nodePtr);
//             res = shmctl(tmp, IPC_RMID, 0);
//         }
        
//         //前一节点指向该节点
//         prePtr = nodePtr;
//         int nextId = nodePtr->shmidNext;
//         proToNetqueue* tmpAddr = nodePtr;
//         //该指针指向下一块共享内存
//         if (nextId == 0) 
//         {
//             break;
//         }
//         nodePtr = static_cast<proToNetqueue *> (shmat(nextId, nullptr, 0));
//         //解除该节点的内存映射
//         shmdt(tmpAddr);
//     }
//     proListHead_Addr->listLength--;
// }

//从路由表获得连接sockfd
int NetComm::isThereConn(const char* ip, int port) 
{
    int front = netList[1];
    int rear = netList[2];
    //指向头结点下一节点
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(netList[0], nullptr, 0)) + front;

printf("get into 循环\n");
    int connfd = -1;
    for (int i = front;;) 
    {
        if (rear == i) 
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
printf("isthere conn is over and return %d\n", connfd);

    return connfd;
}

//从进程通道表获得对应表项
int NetComm::getProShmQueue(const char* ip, int port, int flag) 
{
    int front = proList[1];
    int rear = proList[2];
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(proList[0], nullptr, 0)) + front;
    if (nodePtr == (void *)-1) 
    {
        printf("prolist[0]===%d\n", proList[0]);
        perror("shmat ");
    }
    int queueID = -1;
    for (int i = front; ;) 
    {
    printf("循环了%d次\n", i);
        if (i == rear) 
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
printf("over!!!!!!!!!!!!!\n");
    return queueID;
}


void NetComm::runMyEpoll() 
{
    int tcpfd = socketControl.getMysockfd();
    int udpfd = socketControl.getMysockfd(1);
    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe
    
    int ret;
    epoll_event events[FDNUMBER];
    while (isRun) 
    {
        ret = 0;
        myEpoll.Wait(ret, events);
        for (int i = 0; i < ret; i++) 
        {
            // if (events[i].data.fd == udpfd) //本机发来的udp通知
            // {
            // std::cout << "收到本机udp通知\n" << std::endl;
            //     int connfd = events[i].data.fd;
            //     std::thread t(&NetComm::recvFromUDP, this, connfd);
            //     t.detach();
            // }
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
    while (count > 0) 
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
printf("1\n");
    //获取接收此消息的共享内存缓冲区(proToNetqueue)
    int proShmid = getProShmQueue(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, WRITE); 
    if (proShmid == -1) 
    {
printf("2\n");
        //如果没有该缓冲区,创建一个
        proShmid = updateList(tmpBuffer.netQuaad);
    }
printf("4\n");
    //shmid中获取写队列 写入数据 
    saveMessage(proShmid, tmpBuffer);
    //若是一个新的连接 更新路由表
printf("5\n");
    int netShmid = isThereConn(tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);
    if (netShmid == -1) 
    {
printf("6\n");
        //路由表中没有这个链接,将发送端ip和端口保存
        updateList(connfd, tmpBuffer.netQuaad.sourcePassIP, tmpBuffer.netQuaad.sourcePassPort);    
    }
printf("7\n");

    //告知进程共享内存通道偏移量
    //收到跨机器发来的消息并存入共享内存后,通知本机进程通道表共享内存id
    int tmp[2] = {proList[0], proShmid};
    socketControl.inform(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, tmp);

}


/*

测试至此 inform函数, proshmid是进程通道表的shmid, 

*/

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
    
// printf("1\n");
    int connfd = isThereConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
    if (connfd == -1) 
    {
    // printf("2\n");
        connfd = socketControl.makeNewConn(str.netQueaad.destPassIP, str.netQueaad.destPassPort);
    // printf("3\n");
        updateList(connfd, str.netQueaad.destPassIP, str.netQueaad.destPassPort);
    }
// printf("连接 = %d\n", connfd);
    socketControl.sendTo(tmp, connfd);
}