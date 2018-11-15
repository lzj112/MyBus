#include <assert.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#include "MyBus.h" 

using namespace std;

const char* sourceIP = "127.0.0.1";
int sourcePort = 4096;
const char* MyBusIP = "127.0.0.1";
int MyBusPort;

int main() 
{

    MyBus station;
    BusCard* cardPtr = station.initChannelControl(10); //创建共享内存控制块及通信队列


    //应该转换成package body类型把
    void* write = station.getLocalQueue(cardPtr, 1);  //获取写队列的映射地址

    // void* read = station.getMessageQueue(cardPtr, 0);   //获取读队列映射地址

    const char p[] = "hello,i`m labai,How are u ?";
    int length = strlen(p) + 1;
    
    //本机发送
    int ret = station.sendToLocal(cardPtr, write, p, length);


    //跨物理机发送
    ret = station.sendByNetwork(pid, shmid, ip, port);
}