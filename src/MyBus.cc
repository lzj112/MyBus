#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h"

MyBus::MyBus()
{
}

MyBus::~MyBus()
{
}

//获取当前工作目录
char *MyBus::getPath(char *buffer, size_t size)
{
    if (buffer == nullptr)
    {
        std::cout << "buffer is nullptr" << std::endl;
        return nullptr;
    }
    buffer = getcwd(buffer, size);
    if (buffer == nullptr)
    {
        perror("getcwd is failed : ");
    }
    return buffer; //getcwd失败返回的也是nullptr
}

//ftok获得key
key_t MyBus::getKey(int proj_id, char *in_case_path)
{
    char buffer[50];
    if (in_case_path != nullptr) //如果用户提供了文件目录
    {
        strcpy(buffer, in_case_path);
    }

    char *path = getPath(buffer, (size_t)50);
    if (path == nullptr)
    {
        perror("path is nullptr : ");
        exit(1);
    }
    key_t keyTmp = ftok(buffer, proj_id); //获得key
    if (keyTmp == -1)
    {
        perror("ftok failed : ");
        exit(1);
    }
    return keyTmp;
}

BusCard* MyBus::initChannelControl(key_t key) 
{
    //开辟存储控制块的共享内存
    int shmid = shm_Manage.shmGet(key, sizeof(BusCard), IPC_CREAT | 0666);
    if (shmid = -1) 
    {
        return nullptr;
    }

    //挂载到当前进程
    void* ptrTmp = shm_Manage.shmAt(shmid, nullptr, 0);
    if (*(int *)ptrTmp == -1) 
    {
        return nullptr;
    }

    BusCard* cardPtr = (BusCard *)ptrTmp;
    
    cardPtr->shmSelfId = shmid;
    cardPtr->ftokKey = key;
    cardPtr->proQueuenNmber[2] = (int)getpid();
    
    //初始化两个读写队列
    initShmQueue(cardPtr);

    return cardPtr;
}

int MyBus::initShmQueue(BusCard* card) 
{
    for (int i = 0; i < 2; i++) 
    {
        int keyTmp = (card->ftokKey >> 16) + i;  
        int key_1 = getKey(keyTmp);
    
        int shmid = shm_Manage.shmGet(keyTmp, sizeof(PacketBody) * queueSize, IPC_CREAT | 0666);
        if (shmid == -1) 
        {
           return -1;
        }
        card->shmqueue_One = shmid;
        card->proQueuenNmber[i] = shmid;
        
        shmid = 0;
        keyTmp = 0;
    }
    return 0;
}

