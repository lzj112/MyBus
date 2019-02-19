#include <assert.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>


#include <iostream>

#include "src/MyBus.h" 

using namespace std;

int shmid = 2981904;
int main() 
{
    MyBus station;
    BusCard* cardPtr = station.getChannelControl(shmid);
    station.prepareSocket("read1.json");
    
    char p[256];
    
    // while (1) 
    // {
    //     station.recvFromNetwork(p);
    //     cout << p << endl;
    //     memset(p, 0, sizeof(p));
    // }

   
    while (1)
    {
        station.recvFromLocal(cardPtr, p, 256);
        cout << "here is message from local:\n [" << p << "]" << endl;
        memset(p, '\0', sizeof(p));
        sleep(1);
    }
    
//    station.releaseAll(cardPtr);
}