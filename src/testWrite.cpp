#include <assert.h>
#include <cstring>
#include <sys/types.h>

#include <iostream>

#include "MyBus.h" 

using namespace std;

int main() 
{

    MyBus station;
    key_t key = station.getKey(10);

    BusCard* cardPtr = station.initChannelControl(key); //创建共享内存控制块及通信队列


    //应该转换成package body类型把
    void* write = station.getMessageQueue(cardPtr, 1);  //获取写队列的映射地址

    // void* read = station.getMessageQueue(cardPtr, 0);   //获取读队列映射地址

    const char p[] = "hello,i`m labai,How are u ?";
    int length = strlen(p) + 1;
    int ret = station.sendToLocal(cardPtr, write, p, length);
}