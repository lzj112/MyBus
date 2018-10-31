#include <assert.h>
#include <string.h>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h" 

using namespace std;

int main() 
{
    MyBus station;
    key_t key = station.getKey(1);

    int shmid = station.createShareM(key, 1024);
    assert(shmid != -1);

    char tmp[15] = "here is shm";
    char* t = (char *)station.getShareAddr(shmid);
    assert(t != nullptr);

    strcpy(t, tmp);

    int ret = station.ShareDt(t);
    assert(ret != -1);

    // ret = station.ShareCtl(shmid);  在读取之前删除读不到了
}