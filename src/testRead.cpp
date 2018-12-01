#include <assert.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>


#include <iostream>

#include "MyBus.h" 

using namespace std;

//本机IP 端口
const char* sourceIP = "127.0.0.1";
int sourcePort = 4099;

//本机中转进程IP 端口
const char* passIP = "127.0.0.1";
int passPort = 4097;

//对端中转进程ip 端口
const char* destPassIP = "127.0.0.1";
int destPassPort = 4096;

//目标进程ip 端口
const char* destIP = "127.0.0.1";
int destPort = 4098;
////dasdfasd

int shmid = 5275659;
int main() 
{
    MyBus station;
    BusCard* cardPtr = station.getChannelControl(shmid);
    station.prepareSocket(sourceIP, sourcePort);
    
    char p[256];
    
    //跨物理机发送
    while (1) 
    {
        station.recvFromNetwork(p);
        cout << p << endl;
        memset(p, 0, sizeof(p));
    }
    // station.recvFromNetwork(sourceIP, sourcePort, p);

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