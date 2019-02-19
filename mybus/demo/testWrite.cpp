#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "src/MyBus.h" 

using namespace std;

int main() 
{

    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列
    station.prepareSocket("write1.json");
    
    const char p[] = "hello world ?";
    int length = sizeof(p);

    //跨物理机发送
    printf("发向本机送%s\n", p);

    // while (1) 
    // {
        // station.sendByNetwork(cardPtr, "write1.json", p, length);
    //     sleep(1);
    // }
    station.sendToLocal(cardPtr, p, length);
    //    station.releaseAll(cardPtr);
}