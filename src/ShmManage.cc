#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "ShmManage.h"

ShmManage::ShmManage() 
{

}

ShmManage::~ShmManage() 
{

}

int ShmManage::myErrno(int err) 
{
    
}

int ShmManage::shmGet(key_t key, size_t size, int flag) 
{
    int shm_id = shmget(key, size, flag);
    if (shm_id == -1) 
    {
        perror("shmget is failed : ");
        // exit(1);
        myErrno(errno);
    }
    
}

void* ShmManage::shmAt(int shmid, const void *addr, int flag) 
{

}

int ShmManage::shmDt(void *addr) 
{

}

int ShmManage::shmCtl(int shmid, int cmd, struct shmid_ds *buf) 
{

}