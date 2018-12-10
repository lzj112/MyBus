#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "MyBus.h" 

using namespace std;


int main() 
{
    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列
    station.prepareSocket("write2.json");
    
    const char p[] = "i`m the another one !!!!!!!!!!!!!";
    int length = strlen(p) + 1;

    //跨物理机发送
    printf("发向另一个物理机送%s\n", p);

    // while (1)
    // {
        station.sendByNetwork(cardPtr, "write2.json", p, length);
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