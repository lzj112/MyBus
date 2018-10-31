#ifndef _MYBUS_H_
#define _MYBUS_H_

#include <map>
#include <vector>
#include <sys/sem.h>

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
    key_t getKey(int proj_id, char* in_case_path = nullptr); //获得shmget的key
    int createShareM(key_t key, int size, int flag = IPC_CREAT);
    int getShareM(key_t key, int size, int flag = IPC_CREAT);
    // int getShmid(int index);
    void* getShareAddr(int shmid, const void* addr = nullptr, int flag = 0);
    int ShareDt(const void* addr);
    int ShareCtl(int shmid, int cmd = IPC_RMID, struct shmid_ds* buf = nullptr);
    int as();
private:
    ShmManage shm_Manage;   //封装对共享内存的操作

};

#endif