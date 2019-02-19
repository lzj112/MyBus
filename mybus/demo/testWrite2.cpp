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
    station.prepareSocket("write2.json");
    
    // const char p[] = "i`m the another one !!!!!!!!!!!!!";
    // int length = sizeof(p);

    // //跨物理机发送
    // printf("发向另一个物理机送%s\n", p);

    // while (1)
    // {
        // station.sendByNetwork(cardPtr, "write2.json", p, length);
    //     sleep(1);
    // }

    const char mess[] = "from local!";
    int len = sizeof(mess);
    printf("向本机进程发送%s\n");
    station.sendToLocal(cardPtr, mess, len);
}