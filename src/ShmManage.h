#ifndef _SHMMANAGE_H
#define _SHMMANAGE_H

/*
共享内存管理
*/
#include <sys/types.h>
#include "ShareMry.h"

class ShmManage
{
public:
    ShmManage();
    ~ShmManage();
    int shmGet(key_t key, size_t size, int flag);
    void* shmAt(int shmid, const void* addr, int flag);
    int shmDt(const void* addr);
    int shmCtl(int shmid, int cmd, struct shmid_ds* buf);
private:
    // bool isclosed;  //程序结束时调用shmdt
    // int shm_Count; //引用计数,降至0时真正删除释放共享内存
};

#endif