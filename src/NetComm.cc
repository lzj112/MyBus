#include <cstring>
#include <assert.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <iostream>
#include <thread>

#include "NetComm.h"
#include "ShmManage.h"

void NetComm::initList() 
{
    RoutingTable str1;
    int res = initShmList(str1);
    assert(res != -1);

    proToNetqueue str2;
    res = initShmList(str2);
    assert(res != -1);
}

/*
同一个主机上的不同进程应该访问的都是
这一块共享内存
*/
int NetComm::initShmList(const RoutingTable& str) 
{
    key_t key = ftok(PATH, 1234);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 1" << std::endl;
        return -1;
    }

    netListHead_ID = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);
  
    if (netListHead_Addr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(netListHead_ID, nullptr, 0);
        netListHead_Addr = (RoutingTable *)tmp;
    }

    return 0;
}

int NetComm::initShmList(const proToNetqueue& str) 
{
    key_t key = ftok(PATH, 3456);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 2" << std::endl;
        return -1;
    }

    proListHead_ID = ShmManage::shmGet(key, sizeof(proToNetqueue), IPC_CREAT | 0666);
    if (proListHead_Addr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(proListHead_ID, nullptr, 0);
        proListHead_Addr = (proToNetqueue *)tmp;
    }

    return 0;
}

int NetComm::creShmQueue(int proj_id) 
{
    key_t key = ftok(PATH, proj_id);
    assert(key != -1);

    int shmid = ShmManage::shmGet(key, sizeof(PacketBody) * QUEUESIZE, IPC_CREAT | 0666);
    assert(shmid != -1);

    return shmid;
}

int NetComm::updateList(RoutingTable* str) 
{
    if (str == nullptr) 
    {
        std::cout << "infoTmp == nullptr in update 1" << std::endl;
    }
    if (netListHead_Addr == nullptr) 
    {
        std::cout << "netListHead_Addr==nullptr in update 1" << std::endl;

    }
    /*
    如果端口一样ip就一样,就是同一个连接,就不会存储进来
    会直接用那个连接,所以port会是唯一的
    */
    //每个端口都唯一,这样保证了key唯一
    key_t key = static_cast<key_t> (str->sockfd);
    int shmid = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);

    RoutingTable* shmTmpAddr = (RoutingTable *)ShmManage::shmAt(shmid, nullptr, 0);

    shmTmpAddr->sockfd = str->sockfd;
    shmTmpAddr->destPort = str->destPort;
    shmTmpAddr->shmidNext = netListHead_Addr->shmidNext;    //新节点指向头结点下一节点
    shmTmpAddr->sourcePort = str->sourcePort;
    strcmp(shmTmpAddr->destIp, str->destIp);
    strcmp(shmTmpAddr->sourceIp, str->sourceIp);

    netListHead_Addr->shmidNext = shmid;    //头结点指向新节点

    ShmManage::shmDt(shmTmpAddr);
}

int NetComm::updateList(proToNetqueue* str) 
{
    if (str == nullptr) 
    {
        std::cout << "pointer is nullptr 2" << std::endl;
    }
    if (proListHead_Addr == nullptr) 
    {
        std::cout << "prolisthead_addr == nullptr 2" << std::endl;
    }

    //进程pid也唯一,保证key唯一
    key_t key = static_cast<key_t> (str->pid);
    if (key == -1) 
    {
        std::cout << "key == -1 in update 2" << std::endl;
        return -1;
    }

    int shmid = ShmManage::shmGet(key, sizeof(proToNetqueue), IPC_CREAT | 0666);

    proToNetqueue* tmpAddr = (proToNetqueue *)ShmManage::shmAt(shmid, nullptr, 0);

    //赋值新节点
    tmpAddr->pid = str->pid;
    tmpAddr->readQueue = creShmQueue(str->pid | READ);
    tmpAddr->writeQueue = creShmQueue(str->pid | WRITE);
    tmpAddr->shmidNext = proListHead_Addr->shmidNext;   //新节点指向头结点下一节点
    
    proListHead_Addr->shmidNext = shmid;                //头结点指向新节点

    ShmManage::shmDt(tmpAddr);
}

int NetComm::delListNode(int fd, const RoutingTable& str) 
{
    int shmidTmp = netListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    RoutingTable* nodePtr = (RoutingTable *)ShmManage::shmAt(shmidTmp, nullptr, 0);

    RoutingTable* prePtr = netListHead_Addr;    //指向前一个节点
    while (nodePtr != (void *)-1) 
    {
        //该节点中sockfd就是目标值
        if (nodePtr->sockfd == fd) 
        {
            //前一个节点指向该节点下一个节点
            int tmp = prePtr->shmidNext;
            prePtr->shmidNext = nodePtr->shmidNext;

            //删除该节点
            int res = ShmManage::shmDt(nodePtr);
            res = ShmManage::shmCtl(tmp, IPC_RMID, 0);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        RoutingTable* tmpAddr = nodePtr;
        //该指针指向下一块共享内存
        nodePtr = (RoutingTable *)ShmManage::shmAt(nextId, nullptr, 0);
        //解除该节点的内存映射
        ShmManage::shmDt(tmpAddr);
    }
}

int NetComm::delListNode(int pid, const proToNetqueue& str) 
{
    int shmidTmp = proListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    proToNetqueue* nodePtr = (proToNetqueue *)ShmManage::shmAt(shmidTmp, nullptr, 0);

    proToNetqueue* prePtr = proListHead_Addr;    //指向前一个节点
    while (nodePtr != (void *)-1) 
    {
        //该节点中sockfd就是目标值
        if (nodePtr->pid == pid) 
        {
            //前一个节点指向该节点下一个节点
            int tmp = prePtr->shmidNext;
            prePtr->shmidNext = nodePtr->shmidNext;

            //删除该节点
            int res = ShmManage::shmDt(nodePtr);
            res = ShmManage::shmCtl(tmp, IPC_RMID, 0);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        proToNetqueue* tmpAddr = nodePtr;
        //该指针指向下一块共享内存
        nodePtr = (proToNetqueue *)ShmManage::shmAt(nextId, nullptr, 0);
        //解除该节点的内存映射
        ShmManage::shmDt(tmpAddr);
    }
}

int NetComm::isThereConn(const char* ip, int port, const RoutingTable& str) 
{
    int nextNode = netListHead_Addr->shmidNext;
    if (nextNode == 0) 
    {
        return -1;
    }
    //指向头结点下一节点
    RoutingTable* nodePtr = (RoutingTable *)ShmManage::shmAt(nextNode, nullptr, 0);

    int connfd = -1;
    while (nodePtr == (void *)-1) 
    {
        if ((strcmp(ip, nodePtr->destIp) == 0) && 
            port == nodePtr->destPort) 
        {
            connfd = nodePtr->sockfd;   //找到一条已有连接
            break;
        }

        int nextId = nodePtr->shmidNext;
        RoutingTable* tmpAddr = nodePtr;
        nodePtr = (RoutingTable *)ShmManage::shmAt(nextId, nullptr, 0);
        ShmManage::shmDt(tmpAddr);
    }

    return connfd;
}

int NetComm::getProShmQueue(int fd, int flag) 
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

    proToNetqueue* nodePtr = (proToNetqueue *)ShmManage::shmAt(nextNodeId, nullptr, 0);

    int queueID = -1;
    while (nodePtr != (void *)-1) 
    {
        if (nodePtr->pid == fd) 
        {
            queueID = (flag == 0) ? nodePtr->readQueue : nodePtr->writeQueue;
            break;
        }

        proToNetqueue* tmpAddr = nodePtr;
        nextNodeId = nodePtr->shmidNext;
        ShmManage::shmDt(tmpAddr);
        nodePtr = (proToNetqueue *)ShmManage::shmAt(nextNodeId, nullptr, 0);
    }

    return queueID;
}

int NetComm::getListenFd() 
{
    int listeningFd = socketControl.getMysockfd();
    return listeningFd;
}