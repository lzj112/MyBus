#include <assert.h>
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

    char* tmp = (char *)station.getShareAddr(shmid);
    assert(tmp != nullptr);

    cout << "here is from shm`s message : " << tmp << endl;
    
    int ret = station.ShareDt(tmp);
    assert(ret != -1);

    ret = station.ShareCtl(shmid);
}