#include <assert.h>
#include <unistd.h>
#include <sys/types.h>


#include <iostream>

#include "MyBus.h" 

using namespace std;

int shmid = 10125323;
int main() 
{
    MyBus station;
    BusCard* cardPtr = station.getChannelControl(shmid);

    void* ptr = station.getLocalQueue(cardPtr, 0);

    char p[256];
    
    while (1)
    {
        station.recvFromLocal(cardPtr, ptr, p, 256);

        cout << "here is message from local:\n [" << p << "]" << endl;
    }
}