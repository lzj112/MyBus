#include <assert.h>
#include <unistd.h>
#include <sys/types.h>


#include <iostream>

#include "MyBus.h" 

using namespace std;

int main() 
{
    MyBus station;
    int shmid = 10125323;
    BusCard* cardPtr = station.getChannelControl(shmid);

    void* ptr = station.getMessageQueue(cardPtr, 0);

    char p[256];
    station.recvFromLocal(cardPtr, ptr, p, 256);

    cout << "here is message from local:\n [" << p << "]" << endl;
    
}