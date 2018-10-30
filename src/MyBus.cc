#include <cstdlib>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h"

MyBus::MyBus() 
{

}

MyBus::~MyBus() 
{
    
}

char* MyBus::getPath(char* buffer, size_t size) //获取当前工作目录
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

int MyBus::createSharem(key_t key, int size, int flag) 
{
    flag = flag | 0666;
    int shm_id = shm_Manage.shmGet(key, size, flag);

}

int MyBus::getSharem(key_t key, int size, int flag) 
{
    int shm_id = shm_Manage.shmGet(key, size, flag);
}
