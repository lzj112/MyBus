#ifndef _MYBUS_H_
#define _MYBUS_H_

#include <map>
#include <vector>
#include <sys/sem.h>

#include <mutex>
#include <thread>

#include "BusInfo.h"
#include "Parsing.h"
#include "socketBus.h"
#include "Parsing.h"


/* 
进程间通信组件
*/
class MyBus
{
public:
    MyBus();
    ~MyBus();
    
    key_t getKey(int proj_id, char* in_case_path = nullptr); //获得shmget的key
    BusCard* initChannelControl(int proj_id);                  //初始化控制块信息,以及两个队列
    BusCard* getChannelControl(int shmid);                   //获得一个创建好的控制块信息

    int initProChannelNode(const BusCard* card, const char* ip, int port);

    int initShmQueue(BusCard* card);                         //初始化共享内存的队列
    void* getLocalQueue(BusCard* cardPtr, int flag);       //获得创建的队列的地址,flag指定读写队列
    void prepareSocket(const char* fileName);

    int addQueueFront(BusCard* cardPtr, int flag);
    int addQueueRear(BusCard* CardPtr, int flag);

    int sendToLocal(BusCard* cardPtr, const char* buffer, int length);    //收发数据
    int recvFromLocal(BusCard* cardPtr, char* buffer, int length);
    void sendByNetwork(BusCard* card, const char* fileName, const char* buffer, int length);
    int recvFromNetwork(/*const char* ip, int port,*/char* buffer);  //////
    void saveLocalMessage(BusCard* card, const char* buffer);
    void release(int shmid);
    void releaseAll(BusCard* card);
private:
    void getPath(char *buffer, size_t size);                //使用ftok创建key
    int getQueueFront(BusCard* cardPtr, int flag);           //队列操作
    int getQueueRear(BusCard* cardPtr, int flag);

    std::mutex my_lock;
    
    // BusCard* card_;
    socketBus socketControl;
};

#endif