#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <iostream>

#include "ShmManage.h"

ShmManage::ShmManage() 
{

}

ShmManage::~ShmManage() 
{

}


int ShmManage::shmGet(key_t key, size_t size, int flag) 
{
    if (key < 0 || size < 0) 
    {
        perror("key < 0 or size < 0:");
        return -1;
    }
    int shm_id = shmget(key, size, flag);
    if (shm_id == -1) 
    {
        perror("shmget is failed : ");
        exit(1);
    }
    return shm_id;
}

void* ShmManage::shmAt(int shmid, const void *addr, int flag) 
{
    if (shmid <= 0) 
    {
        std::cout << "shmid at getShareAddr is <= 0" << std::endl;
        return nullptr;
    }
    void* tmp_Addr = shmat(shmid, addr, flag);
    if (tmp_Addr == (void *)-1) 
    {
        perror("ShareDt is failed : ");
        exit(1);
    }
    return tmp_Addr;
}

int ShmManage::shmDt(const void *addr) 
{
    if (addr == nullptr) 
    {
        std::cout << "addr at ShareDt is nullptr" << std::endl;
        return -1;
    }
    int ret;
    ret = shmdt(addr);
    if (ret == -1) 
    {
        perror("ShareDt is failed : ");
        exit(1);
    }
    return ret;
}

int ShmManage::shmCtl(int shmid, int cmd, struct shmid_ds *buf) 
{
    if (shmid <= 0) 
    {   
        return -1;
    }
    int ret = shmctl(shmid, cmd, buf);
    if (ret == -1) 
    {
        perror("ShareCtl is failed : ");
        exit(1);
    }
    return ret;
}