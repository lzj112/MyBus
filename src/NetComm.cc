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

void NetComm::initList() 
{
    int res = initShmList();
    assert(res != -1);

    res = initShmList(1);
    assert(res != -1);
}

/*
同一个主机上的不同进程应该访问的都是
这一块共享内存
*/
int NetComm::initShmList()  //初始化路由表
{
    key_t key = ftok(PATH, 1234);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 1" << std::endl;
        return -1;
    }

    netListHead_ID = shmget(key, sizeof(RoutingTable), IPC_CREAT | 0666);
  
    if (netListHead_Addr == nullptr) 
    {
        netListHead_Addr = static_cast<RoutingTable *> (shmat(netListHead_ID, nullptr, 0));
    }

    return 0;
}

int NetComm::initShmList(int) //初始化中转通道
{
    key_t key = ftok(PATH, 3456);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 2" << std::endl;
        return -1;
    }

    proListHead_ID = shmget(key, sizeof(proToNetqueue), IPC_CREAT | 0666);
    if (proListHead_Addr == nullptr) 
    {
        proListHead_Addr = static_cast<proToNetqueue *> (shmat(proListHead_ID, nullptr, 0));
    }

    return 0;
}

int NetComm::creShmQueue(int proj_id) 
{
    key_t key = ftok(PATH, proj_id);
    assert(key != -1);

    int shmid = shmget(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
    assert(shmid != -1);

    return shmid;
}

int NetComm::updateList(int sockfd, const char* ip, int port) 
{
    if (ip == nullptr) 
    {
        std::cout << "infoTmp == nullptr in update 1" << std::endl;
    }
    /*
    如果端口一样ip就一样,就是同一个连接,就不会存储进来
    会直接用那个连接,所以port会是唯一的
    */
    //每个端口都唯一,这样保证了key唯一
    key_t key = static_cast<key_t> (sockfd);
    int shmid = shmget(key, sizeof(RoutingTable), IPC_CREAT | 0666);

    RoutingTable* shmTmpAddr = static_cast<RoutingTable *> (shmat(shmid, nullptr, 0));

    shmTmpAddr->sockfd = sockfd;
    shmTmpAddr->port = port;
    strcmp(shmTmpAddr->IP, ip);
    shmTmpAddr->shmidNext = netListHead_Addr->shmidNext;    //新节点指向头结点下一节点

    netListHead_Addr->shmidNext = shmid;    //头结点指向新节点

    shmdt(shmTmpAddr);
}

int NetComm::updateList(const struct ProComm& str) 
{
    if (proListHead_Addr == nullptr) 
    {
        std::cout << "prolisthead_addr == nullptr 2" << std::endl;
    }

    //进程pid也唯一,保证key唯一
    key_t key = static_cast<key_t> (str.destPort);
    if (key == -1) 
    {
        std::cout << "key == -1 in update 2" << std::endl;
        return -1;
    }

    int shmid = shmget(key, sizeof(proToNetqueue), IPC_CREAT | 0666);

    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(shmid, nullptr, 0));

    //赋值新节点
    tmpAddr->shmSelfId = shmid;
    tmpAddr->destPort = str.sourcePort;
    tmpAddr->sourcePort = str.destPort;
    strcmp(tmpAddr->destIP, str.sourceIP);
    strcmp(tmpAddr->sourceIP, str.destIP);
    tmpAddr->readQueue = creShmQueue(str.destPort | READ);
    tmpAddr->shmidNext = proListHead_Addr->shmidNext;   //新节点指向头结点下一节点
    proListHead_Addr->shmidNext = shmid;                //头结点指向新节点

    shmdt(tmpAddr);
}

int NetComm::delListNode(int fd, const RoutingTable& str) 
{
    int shmidTmp = netListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(shmidTmp, nullptr, 0));

    RoutingTable* prePtr = netListHead_Addr;    //指向前一个节点
    while (1)
    {
        //该节点中sockfd就是目标值
        if (nodePtr->sockfd == fd) 
        {
            //前一个节点指向该节点下一个节点
            int tmp = prePtr->shmidNext;
            prePtr->shmidNext = nodePtr->shmidNext;

            //删除该节点
            int res = shmdt(nodePtr);
            res = shmctl(tmp, IPC_RMID, 0);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        RoutingTable* tmpAddr = nodePtr;
        if (nextId == 0) 
        {
            break;
        }
        //该指针指向下一块共享内存
        nodePtr = static_cast<RoutingTable *> (shmat(nextId, nullptr, 0));
        //解除该节点的内存映射
        shmdt(tmpAddr);
    }
}

int NetComm::delListNode(const char* ip, int port, const proToNetqueue& str) 
{
    int shmidTmp = proListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(shmidTmp, nullptr, 0));

    proToNetqueue* prePtr = proListHead_Addr;    //指向前一个节点
    while (1)
    {
        //该节点中sockfd就是目标值
        if ((strcmp(ip, nodePtr->destIP) == 0) && port == nodePtr->destPort) 
        {
            //前一个节点指向该节点下一个节点
            int tmp = prePtr->shmidNext;
            prePtr->shmidNext = nodePtr->shmidNext;

            //删除该节点
            int res = shmdt(nodePtr);
            res = shmctl(tmp, IPC_RMID, 0);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        proToNetqueue* tmpAddr = nodePtr;
        //该指针指向下一块共享内存
        if (nextId == 0) 
        {
            break;
        }
        nodePtr = static_cast<proToNetqueue *> (shmat(nextId, nullptr, 0));
        //解除该节点的内存映射
        shmdt(tmpAddr);
    }
}

int NetComm::isThereConn(const char* ip, int port) 
{
    int nextNode = netListHead_Addr->shmidNext;
    if (nextNode == 0) 
    {
        return -1;
    }
    //指向头结点下一节点
    RoutingTable* nodePtr = static_cast<RoutingTable *> (shmat(nextNode, nullptr, 0));

    int connfd = -1;
    while (1)
    {
        if ((strcmp(ip, nodePtr->IP) == 0) && 
            port == nodePtr->port) 
        {
            connfd = nodePtr->sockfd;   //找到一条已有连接
            break;
        }

        int nextId = nodePtr->shmidNext;
        RoutingTable* tmpAddr = nodePtr;
        if (nextId == 0) 
        {
            break;
        }
        nodePtr = static_cast<RoutingTable *> (shmat(nextId, nullptr, 0));
        shmdt(tmpAddr);
    }

    return connfd;
}

int NetComm::getProShmQueue(const char* ip, int port, int flag) 
{
    if (flag > 1 || flag < 0) 
    {
        std::cout << "falg is wrong" << std::endl;
        return -1;
    }
    int nextNodeId = proListHead_Addr->shmidNext;
    if (nextNodeId == 0) 
    {
        return -1;
    }
    proToNetqueue* nodePtr = static_cast<proToNetqueue *> (shmat(nextNodeId, nullptr, 0));

    int queueID = -1;
    while (1)
    {
        if (nodePtr->sourcePort == port && (strcmp(nodePtr->sourceIP, ip) == 0)) 
        {
            // queueID = (flag == READ) ? nodePtr->readQueue : nodePtr->writeQueue;
            queueID = nodePtr->shmSelfId;
            break;
        }

        proToNetqueue* tmpAddr = nodePtr;
        nextNodeId = nodePtr->shmidNext;
        shmdt(tmpAddr);
        if (nextNodeId == 0) 
        {
            break;
        }
        nodePtr = static_cast<proToNetqueue *> (shmat(nextNodeId, nullptr, 0));
    }

    return queueID;
}


void NetComm::runMyEpoll() 
{
    int tcpfd = socketControl.getMysockfd();
    int udpfd = socketControl.getMysockfd(1);
    myEpoll.Create(tcpfd);
    myEpoll.Create(udpfd, 1);
    myEpoll.Add(tcpfd, EPOLLIN);
    myEpoll.Add(udpfd, EPOLLIN);


    signal(SIGPIPE, SIG_IGN);   //忽略sigpipe
    
    int ret;
    epoll_event events[FDNUMBER];
    while (isRun) 
    {
        ret = 0;
        myEpoll.Wait(ret, events);
        for (int i = 0; i < ret; i++) 
        {
            if (events[i].data.fd == udpfd) 
            {
                int connfd = events[i].data.fd;
                std::thread t(&NetComm::recvFromUDP, this, connfd);
                t.detach();
            }
            if (events[i].events & EPOLLIN) 
            {
                if (events[i].data.fd == tcpfd) 
                {
                    myEpoll.newConnect(tcpfd);
                }
                else 
                {
                    int connfd = events[i].data.fd;
                    std::thread t(&NetComm::recvFromTCP, this, connfd);
                    t.detach();
                }
            }
        }
        /*
        if (!readableSocket.empty()) 
        {
            for_each (readableSocket.begin(), readableSocket.end(), [=](int connfd){
                std::thread t(&NetComm::recvFrom, this, connfd);
                t.detach();
            });
        }*/
    }
}

//接收消息
int NetComm::getMessage(int connfd, void* buffer, int length) 
{
    int count = 0;
    int ret = 0;
    while (count < length) 
    {
        ret = recv(connfd, buffer, length, 0);
        count += ret;
        if (ret == -1) 
        {
            if (errno == EINTR || errno == EWOULDBLOCK) 
            {
                continue;
            }
            else 
            { 
                perror("recv is wrong");
                exit(1);
            }
        }
        if (ret == 0) 
        {
            // myEpoll.Ctl(connfd, EPOLL_CTL_DEL);
            myEpoll.Del(connfd);
            close(connfd);
            break;
        }
    }
    return count;
}

//将消息存入共享内存, shmid是进程通道表的对应节点共享内存id
void NetComm::saveMessage(int shmid, const PacketBody& str) 
{
    proToNetqueue* tmpAddr = static_cast<proToNetqueue *> (shmat(shmid, nullptr, 0));

    //读取队列是本机上进程通过此通道读取其他进程发来的信息
    //读取队列的头尾指针
    int front = tmpAddr->netQueue[0];
    int rear = tmpAddr->netQueue[1];

    //队列满 抛弃数据
    if ((rear + 1) % QUEUESIZE == front) 
    {
        return ;
    }
    
    //读队列是进程会用这个通道读取其他进程发来的数据
    PacketBody* queueAddr = (static_cast<PacketBody *> (shmat(tmpAddr->readQueue, nullptr, 0))) + rear;
    copy(queueAddr, str);
 
    tmpAddr->netQueue[1] = (tmpAddr->netQueue[1] + 1) % QUEUESIZE;
}

//将收到的数据写入共享内存缓冲区
void NetComm::copy(PacketBody* ptr, const PacketBody& str) 
{
    ptr->head.type = str.head.type;
    strcmp(ptr->buffer, str.buffer);
    ptr->head.bodySzie = str.head.bodySzie;
    ptr->netQuaad.destPort = str.netQuaad.destPort;
    ptr->netQuaad.sourcePort = str.netQuaad.sourcePort;
    strcmp(ptr->netQuaad.sourceIP, str.netQuaad.sourceIP);
    strcmp(ptr->netQuaad.destIP, str.netQuaad.destIP);
}

//将共享内存中的数据拿出来发给另一个物理机
void NetComm::copy(PacketBody& str, const Notice& tmp) 
{
    str.head.type = READY >> 1 + 12;
    strcmp(str.netQuaad.destIP, tmp.netQueaad.destIP);
    strcmp(str.netQuaad.sourceIP, str.netQuaad.sourceIP);
    str.netQuaad.destPort = tmp.netQueaad.destPort;
    str.netQuaad.sourcePort = tmp.netQueaad.sourcePort;

    BusCard* tmpAddr1 = static_cast<BusCard *> (shmat(tmp.shmid, nullptr, 0));
    if (tmpAddr1->netQueue[1] != tmpAddr1->netQueue[2])
    {
        PacketBody* tmpAddr2 = static_cast<PacketBody *> (shmat(tmpAddr1->netQueue[0], nullptr, 0));
        strcpy(str.buffer, tmpAddr2->buffer);
        tmpAddr1->netQueue[1]++;
        
        shmdt(tmpAddr1);
        shmdt(tmpAddr2);
    }
}

void NetComm::recvFromTCP(int connfd) 
{
    int offset = 0;
    PacketBody tmpBuffer;
    memset(&tmpBuffer, 0, sizeof(PacketBody));
    //先收包头
    offset = getMessage(connfd, (void *)(&tmpBuffer + offset), 12);
    //再收包体
    getMessage(connfd, (void *)(&tmpBuffer + offset), tmpBuffer.head.bodySzie);
    //处理数据
    dealData(connfd, tmpBuffer);
}

//收到跨机器发来的消息并存入共享内存后,通知本机进程共享内存id
void NetComm::inform(const char* ip, int port, int id) 
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int buffer = id;
    sendto(sockfd, (void *)&buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, sizeof(addr));
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
    int netShmid = isThereConn(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort);
    if (netShmid == -1) 
    {
        //路由表中没有这个链接,将发送端ip和端口保存
        updateList(connfd, tmpBuffer.netQuaad.sourceIP, tmpBuffer.netQuaad.sourcePort);    
    }

    //告知进程共享内存通道id
    inform(tmpBuffer.netQuaad.destIP, tmpBuffer.netQuaad.destPort, netShmid);
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
    int connfd = isThereConn(tmp.netQuaad.destIP, tmp.netQuaad.destPort);
    if (connfd == -1) 
    {
        connfd = socketControl.makeNewConn(tmp);
        updateList(connfd, tmp.netQuaad.destIP, tmp.netQuaad.destPort);
    }
    socketControl.sendTo(tmp, connfd);
}