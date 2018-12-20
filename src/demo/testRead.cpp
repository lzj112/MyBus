#include <assert.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>


#include <iostream>

#include "MyBus.h" 

using namespace std;

int shmid = 5275659;
int main() 
{
    MyBus station;
    BusCard* cardPtr = station.getChannelControl(shmid);
    station.prepareSocket("read1.json");
    
    char p[256];
    
    //跨物理机发送
    while (1) 
    {
        station.recvFromNetwork(p);
        cout << p << endl;
        memset(p, 0, sizeof(p));
    }

   /* 
    while (1)
    {
        station.recvFromLocal(cardPtr, p, 256);
        // if (strlen(p) == 0) 
        // {
        //     break;
        // }
        cout << "here is message from local:\n [" << p << "]" << endl;
        memset(p, '\0', sizeof(p));
        sleep(1);
    }
    */
//    station.releaseAll(cardPtr);
}