#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "MyBus.h" 

using namespace std;

// const char* sourceIP = "127.0.0.1";
// int sourcePort = 4096;
// const char* passIP = "127.0.0.1";
// int MyBusPort;
int shmBufferID;    //本机中转进程共享内存id

int main() 
{

    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列


    //应该转换成package body类型把
    void* write = station.getLocalQueue(cardPtr, WRITE);  //获取写队列的映射地址

    // void* read = station.getMessageQueue(cardPtr, 0);   //获取读队列映射地址

    const char p[] = "hello,i`m labai,How are u ?";
    int length = strlen(p) + 1;
    
    while (1)
    {
        //本机发送
        int ret = station.sendToLocal(cardPtr, write, p, length);
    }
    /*
    //对端中转进程ip 端口
    const char* passIP = "127.0.0.1";
    int passport = 4096;
    //目标进程ip 端口
    const char* destIP = "127.0.0.1";
    int destPort;
    //跨物理机发送
    ret = station.sendByNetwork(shmBufferID, passIP, destPort,
                                destIP, destPort, p);
    */
}