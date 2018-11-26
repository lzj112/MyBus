#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "MyBus.h" 

using namespace std;

//本机IP 端口
const char* sourceIP = "127.0.0.1";
int sourcePort = 4098;

//本机中转进程IP 端口
const char* passIP = "127.0.0.1";
int passPort = 4096;

//对端中转进程ip 端口
const char* destPassIP = "127.0.0.1";
int destPassPort = 4097;

//目标进程ip 端口
const char* destIP = "127.0.0.1";
int destPort = 4099;

int main() 
{

    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列
    station.prepareSocket(sourceIP, sourcePort);

    const char p[] = "i`m fine fuck u!!!!!!!!!! ?";
    int length = strlen(p) + 1;

    //跨物理机发送
    ProComm tmp;
    strcmp(tmp.destIP, destIP);
    strcmp(tmp.sourceIP, sourceIP);
    tmp.destPort = destPort;
    tmp.sourcePort = sourcePort;
    int ret = station.sendByNetwork(cardPtr, passIP, passPort, destPassIP, destPassPort, 
                                        tmp, p);

   /* 
    本机测试
    while (1)
    {
        cout << "di " << i << " ci" << endl;
        //本机发送
        int ret = station.sendToLocal(cardPtr, p, length);
        sleep(1);
    }
    */
   
}