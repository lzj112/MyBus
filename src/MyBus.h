#ifndef _MYBUS_H_
#define _MYBUS_H_

#include <map>
#include <vector>
#include <sys/sem.h>

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
    BusCard* getChannel(int shmid);                          //获得一个创建好的通道的信息
    int initShmQueue(BusCard* card);                         //初始化共享内存的队列
    void* getRecvQueue(BusCard* cardPtr);                    //获得读取队列的共享内存的映射地址
    void* getSendQueue(BusCard* cardPtr);                    //获得发送队列的共享内存的映射地址
    int sendTo();                                            //收发数据不管是跨机还是同主机都是这两个接口
    int recvFrom();

  private:
    ShmManage shm_Manage; //封装对共享内存的操作
    // std::vector<int, BusCard> table_Control;    //
    const int queueSize = 256;
};

#endif