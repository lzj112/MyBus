#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "MyBus.h" 

using namespace std;

// //本机IP 端口
// const char* sourceIP = "127.0.0.1";
// int sourcePort = 4098;

// //本机中转进程IP 端口
// const char* passIP = "127.0.0.1";
// int passPort = 4096;

// //对端中转进程ip 端口
// const char* destPassIP = "127.0.0.1";
// int destPassPort = 4097;

// //目标进程ip 端口
// const char* destIP = "127.0.0.1";
// int destPort = 4099;

int main() 
{

    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列
    station.prepareSocket("write1.json");
    
    const char p[] = "hello world ?";
    int length = strlen(p) + 1;

    //跨物理机发送
    printf("发向另一个物理机送%s\n", p);

    // while (1)
    // {
        station.sendByNetwork(cardPtr, "write1.json", p, length);
    //     sleep(1);
    // }

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
//    station.releaseAll(cardPtr);
}