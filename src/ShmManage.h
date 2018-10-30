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
    // key_t getKey(char* path, int proj_id); //获得shmget的key
    void shmGet(key_t key, size_t size, int flag);
    void shmAt(int shmid, const void* addr, int flag);
    void shmDt(void* addr);
    void shmCtl(int shmid, int cmd, struct shmid_ds* buf);
private:
    bool isclosed;  //程序结束时调用shmdt
    // ShareMry shm_Tmp;
    // int shm_Count; //引用计数,降至0时真正删除释放共享内存
};

#endif