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
int NetComm::initShmRouteList() 
{
    key_t key = ftok("/home/lzj/MyBus", 1234);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed" << std::endl;
        return -1;
    }

    netListHead_ID = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);
    if (netListHead_ID == -1) 
    {
        std::cout << "netListHead_ID == -1 inside initShmListHead()" << std::endl;
        return -1;
    }
    if (netListHead_Addr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(netListHead_ID, nullptr, 0);
        if (*(int *)tmp == -1) 
        {
            std::cout << "shmat failed insied initShmListHead()" << std::endl;
            return -1;
        }
        netListHead_Addr = (RoutingTable *)tmp;
    }

    return 0;
}

int NetComm::updateList(RoutingTable* infoTmp) 
{
    if (infoTmp == nullptr) 
    {
        std::cout << "infoTmp == nullptr in update" << std::endl;
    }
    if (netListHead_Addr == nullptr) 
    {
        std::cout << "netListHead_Addr==nullptr in update" << std::endl;

    }
    /*
    如果端口一样ip就一样,就是同一个连接,就不会存储进来
    会直接用那个连接,所以port会是唯一的
    */
    key_t key = static_cast<key_t> (infoTmp->sockfd);
    int shmid = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);
    if (shmid == -1) 
    {
        std::cout << "shmget is failed in update" << std::endl;
        return -1;
    }

    RoutingTable* shmTmpAddr = (RoutingTable *)ShmManage::shmAt(shmid, nullptr, 0);
    if (*(int *)shmTmpAddr == -1) 
    {
        std::cout << "shmat is failed" << std::endl;
        return -1;
    } 

    shmTmpAddr->sockfd = infoTmp->sockfd;
    shmTmpAddr->destPort = infoTmp->destPort;
    shmTmpAddr->shmidNext = netListHead_Addr->shmidNext;
    shmTmpAddr->sourcePort = infoTmp->sourcePort;
    strcmp(shmTmpAddr->destIp, infoTmp->destIp);
    strcmp(shmTmpAddr->sourceIp, infoTmp->sourceIp);

    netListHead_Addr->shmidNext = shmid;

    int res = ShmManage::shmDt(shmTmpAddr);
    if (res == -1) 
    {
        std::cout << "shmdt is failed in update" << std::endl;
        return -1;
    }
}

int NetComm::delListNode(int fd) 
{
    int shmidTmp = netListHead_Addr->shmidNext;
    RoutingTable* nodePtr = (RoutingTable *)ShmManage::shmAt(shmidTmp, nullptr, 0);
    if (nodePtr == nullptr) 
    {
        std::cout << "shmat is failed in del" << std::endl;
        return 0;
    }

    int preShmId = netListHead_ID;
    RoutingTable* prePtr = headAddr;
    while (*(int *)nodePtr != -1) 
    {
        if (nodePtr->sockfd == fd) 
        {
            int tmp = prePtr->shmidNext;
            prePtr->shmidNext = nodePtr->shmidNext;

            int res = ShmManage::shmDt(nodePtr);
            assert(res != -1);
            res = ShmManage::shmCtl(tmp, IPC_RMID,0);
            assert(res != -1);
        }
        
        prePtr = nodePtr;
        int nextId = nodePtr->shmidNext;
        // int res = ShmManage::shmDt(nodePtr);
        nodePtr = (RoutingTable *)ShmManage::shmAt(nextId, nullptr, 0);
        // assert(*(int *)nodePtr != -1);
    }
}

int NetComm::isThereFd(RoutingTable* infoTmp, int fd) 
{
     
}

