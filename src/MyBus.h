#ifndef _MYBUS_H_
#define _MYBUS_H_

#include <vector>
#include <map>

#include "ShmManage.h"

/*
进程间通信组件
*/
class MyBus 
{
public:
    MyBus();
    ~MyBus();
    char* getPath(char* buffer, size_t size);
    key_t getKey(int proj_id); //获得shmget的key
    int getShmid(int index);
private:
    ShmManage shm_Manage;   //封装对共享内存的操作
    // std::vector<ShareMry> shm_Tmp;  //多个共享内存对象
    std::map<int, ShareMry> shm_Tmp; //使用map是不是更好
};

#endif