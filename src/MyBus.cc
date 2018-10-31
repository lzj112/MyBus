#include <cstdlib>
#include <unistd.h>
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
char* MyBus::getPath(char* buffer, size_t size) 
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
    return buffer;  //getcwd失败返回的也是nullptr
}

//ftok获得key
key_t MyBus::getKey(int proj_id)
{
    char buffer[50];
    char* path = getPath(buffer, (size_t)50);
    if (path == nullptr) 
    {
        perror("path is nullptr : ");
        exit(1);
    }
    key_t keyTmp = ftok(buffer, proj_id);
    if (keyTmp == -1) 
    {
        perror("ftok failed : ");
        exit(1);
    }
    return keyTmp;
}

//得到一个共享内存的标识符
int MyBus::createShareM(key_t key, int size, int flag) 
{
    flag = flag | 0666;
    int shm_Id = shm_Manage.shmGet(key, size, flag);
    return shm_Id;
}

//得到一个共享内存的标识符
int MyBus::getShareM(key_t key, int size, int flag) 
{
    int shm_Id = shm_Manage.shmGet(key, size, flag);
    return shm_Id;
}

//将共享内存挂载到当前进程
void* MyBus::getShareAddr(int shmid, const void* addr, int flag) 
{
    void* addr_Ptr = nullptr;
    addr_Ptr = shm_Manage.shmAt(shmid, addr, flag);
    if (*(int *)addr_Ptr == -1) //shmat出错返回-1
    {
        addr_Ptr = nullptr;
    }
    return addr_Ptr;
}

//从进程分离共享内存
int MyBus::ShareDt(const void* addr) 
{
    int ret;
    ret = shm_Manage.shmDt(addr);
    return ret;
}

int MyBus::ShareCtl(int shmid, int cmd, struct shmid_ds* buf)  
{
    int ret = shm_Manage.shmCtl(shmid, cmd, buf);
    return ret;
}