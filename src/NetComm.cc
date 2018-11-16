#include <cstring>
#include <assert.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <iostream>
#include <thread>

#include "NetComm.h"
#include "ShmManage.h"

/*
同一个主机上的不同进程应该访问的都是
这一块共享内存
*/
int NetComm::initShmList(const RoutingTable& str) 
{
    key_t key = ftok("/home/lzj/MyBus", 1234);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 1" << std::endl;
        return -1;
    }

    netListHead_ID = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);
    if (netListHead_ID == -1) 
    {
        std::cout << "netListHead_ID == -1 inside initShmListHead() 1" << std::endl;
        return -1;
    }
    if (netListHead_Addr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(netListHead_ID, nullptr, 0);
        if (tmp == (void *)-1) 
        {
            std::cout << "shmat failed insied initShmListHead() 1" << std::endl;
            return -1;
        }
        netListHead_Addr = (RoutingTable *)tmp;
    }

    return 0;
}

int NetComm::initShmList(const proToNetqueue& str) 
{
    key_t key = ftok("/home/lzj/MyBus", 3456);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed 2" << std::endl;
        return -1;
    }

    proListHead_ID = ShmManage::shmGet(key, sizeof(proToNetqueue), IPC_CREAT | 0666);
    if (proListHead_ID == -1) 
    {
        std::cout << "netListHead_ID == -1 inside initShmListHead() 2" << std::endl;
        return -1;
    }
    if (proListHead_Addr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(proListHead_ID, nullptr, 0);
        if (tmp == (void *)-1) 
        {
            std::cout << "shmat failed insied initShmListHead() 2" << std::endl;
            return -1;
        }
        proListHead_Addr = (proToNetqueue *)tmp;
    }

    return 0;
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
    if (shmid == -1) 
    {
        std::cout << "shmget is failed in update 1" << std::endl;
        return -1;
    }

    RoutingTable* shmTmpAddr = (RoutingTable *)ShmManage::shmAt(shmid, nullptr, 0);
    if (shmTmpAddr == (void *)-1) 
    {
        std::cout << "shmat is failed " << std::endl;
        return -1;
    } 

    shmTmpAddr->sockfd = str->sockfd;
    shmTmpAddr->destPort = str->destPort;
    shmTmpAddr->shmidNext = netListHead_Addr->shmidNext;    //新节点指向头结点下一节点
    shmTmpAddr->sourcePort = str->sourcePort;
    strcmp(shmTmpAddr->destIp, str->destIp);
    strcmp(shmTmpAddr->sourceIp, str->sourceIp);

    netListHead_Addr->shmidNext = shmid;    //头结点指向新节点

    int res = ShmManage::shmDt(shmTmpAddr);
    if (res == -1) 
    {
        std::cout << "shmdt is failed in update 1" << std::endl;
        return -1;
    }
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
    if (shmid == -1) 
    {
        std::cout << "shmget is failed in update 2" << std::endl;
        return -1;
    }

    proToNetqueue* tmpAddr = (proToNetqueue *)ShmManage::shmAt(shmid, nullptr, 0);
    if (tmpAddr == (void *)-1) 
    {
        std::cout << "shmat is failed in update 2" << std::endl;
        return -1;
    }

    //赋值新节点
    // tmpAddr->pid = str->pid;
    tmpAddr->readQueue = str->readQueue;
    tmpAddr->writeQueue = str->writeQueue;
    tmpAddr->shmidNext = proListHead_Addr->shmidNext;   //新节点指向头结点下一节点
    proListHead_Addr->shmidNext = shmid;                //头结点指向新节点

    int res = ShmManage::shmDt(tmpAddr);
    if (res == -1) 
    {
        std::cout << "shmdt is failed in update 2" << std::endl;
        return -1;
    }
}

int NetComm::delListNode(int fd, const RoutingTable& str) 
{
    int shmidTmp = netListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    RoutingTable* nodePtr = (RoutingTable *)ShmManage::shmAt(shmidTmp, nullptr, 0);
    if (nodePtr == (void *)-1) 
    {
        std::cout << "shmat is failed in del 1" << std::endl;
        return 0;
    }

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
            assert(res != -1);
            res = ShmManage::shmCtl(tmp, IPC_RMID, 0);
            assert(res != -1);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        RoutingTable* tmpAddr = nodePtr;
        //该指针指向下一块共享内存
        nodePtr = (RoutingTable *)ShmManage::shmAt(nextId, nullptr, 0);
        //解除该节点的内存映射
        int res = ShmManage::shmDt(tmpAddr);
        assert(res != -1);
    }
}

int NetComm::delListNode(int pid, const proToNetqueue& str) 
{
    int shmidTmp = proListHead_Addr->shmidNext;
    //指向头结点的下一个节点
    proToNetqueue* nodePtr = (proToNetqueue *)ShmManage::shmAt(shmidTmp, nullptr, 0);
    if (nodePtr == (void *)-1) 
    {
        std::cout << "shmat is failed in del 2" << std::endl;
        return 0;
    }

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
            assert(res != -1);
            res = ShmManage::shmCtl(tmp, IPC_RMID, 0);
            assert(res != -1);
        }
        
        //前一节点指向该节点
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        proToNetqueue* tmpAddr = nodePtr;
        //该指针指向下一块共享内存
        nodePtr = (proToNetqueue *)ShmManage::shmAt(nextId, nullptr, 0);
        //解除该节点的内存映射
        int res = ShmManage::shmDt(tmpAddr);
        assert(res != -1);
    }
}

int NetComm::isThereConn(const char* ip, int port, const RoutingTable& str) 
{
    if (netListHead_Addr == nullptr) 
    {
        std::cout << "netheadaddr == nullptr" << std::endl;
        return -1;
    }

    int nextNode = netListHead_Addr->shmidNext;
    if (nextNode == 0) 
    {
        return -1;
    }
    //指向头结点下一节点
    RoutingTable* nodePtr = (RoutingTable *)ShmManage::shmAt(nextNode, nullptr, 0);
    if (nodePtr == (void *)-1) 
    {
        return -1;
    }

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
        int res = ShmManage::shmDt(tmpAddr);
        assert(res != -1);
    }

    return connfd;
}

int NetComm::isThereShm(int fd, const proToNetqueue& str) 
{
    
}

