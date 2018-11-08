#ifndef _MYBUS_H_
#define _MYBUS_H_

#include <map>
#include <vector>
#include <sys/sem.h>

#include <mutex>

#include "BusInfo.h"
#include "ShmManage.h"

/*
进程间通信组件
*/
class MyBus
{
  public:
    MyBus();
    ~MyBus();
    char* getPath(char *buffer, size_t size);                //使用ftok创建key
    key_t getKey(int proj_id, char* in_case_path = nullptr); //获得shmget的key
    BusCard* initChannelControl(key_t key);                  //初始化控制块信息,以及两个队列
    BusCard* getChannelControl(int shmid);                          //获得一个创建好的控制块信息
    int initShmQueue(BusCard* card);                         //初始化共享内存的队列
    void* getMessageQueue(BusCard* cardPtr, int flag);       //获得创建的队列的地址,flag指定读写队列
    
    int addQueueFront(BusCard* cardPtr, int flag);
    int addQueueRear(BusCard* CardPtr, int flag);

    int sendToLocal(BusCard* cardPtr, void* shmMapAddr, const char* buffer, int length);    //收发数据
    int recvFromLocal(BusCard* cardPtr, void* shmMadAddr, char* buffer, int length);
    int sendByNetwork();
    int recvFromNetwork();

    int getQueueFront(BusCard* cardPtr, int flag);           //队列操作
    int getQueueRear(BusCard* cardPtr, int flag);

  private:
    ShmManage shm_Manage; //封装对共享内存的操作
    const int queueSize = 256;
    std::mutex my_lock;
};

#endif