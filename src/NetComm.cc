#include <sys/sem.h>

#include <iostream>

#include "NetComm.h"
#include "ShmManage.h"


/*
同一个主机上的不同进程应该访问的都是
这一块共享内存
*/
int NetComm::initShmListHead() 
{
    key_t key = ftok("/home/lzj/MyBus", 1234);
    if (key == -1) 
    {
        std::cout << "initShmListHead() is failed" << std::endl;
        return -1;
    }

    listHead_ID = ShmManage::shmGet(key, sizeof(RoutingTable), IPC_CREAT | 0666);
    if (listHead_ID == -1) 
    {
        std::cout << "listHead_ID == -1 inside initShmListHead()" << std::endl;
        return -1;
    }
    if (headAddr == nullptr) 
    {
        void* tmp = ShmManage::shmAt(listHead_ID, nullptr, 0);
        if (*(int *)tmp == -1) 
        {
            std::cout << "shmat failed insied initShmListHead()" << std::endl;
            return -1;
        }
        headAddr = tmp;
    }
}