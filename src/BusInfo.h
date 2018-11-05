#ifndef _BUSINFO_H_
#define _BUSINFO_H_

/*
创建的进程间共享内存的通道的控制信息
共享内存信息
*/

#include <sys/types.h>

struct BusCard
{
    int shmSelfId;    //本身这块共享内存的id
    int shmqueue_One; //两条收发队列的共享内存id
    int shmqueue_Two;
    key_t ftokKey;   //ftok的key
    /*
    proQueueNumber[0]存储读取队列id
    proQueueNumber[1]存储发送队列id
    proQueueNumber[2]存储当前进程id
    通信对端进程发现存储的进程id不是本进程id,那么对应的读写队列就是它的写读队列
    */
    int proQueuenNmber[3];
};

struct PacketHead
{
    size_t bodySzie;
};

struct PacketBody
{
    struct PacketHead head;
    char buffer[256];
};

struct ProComm //进程间通信的信息
{
    char* source_IP;
    int source_PORT;
    char* dest_IP;
    int dest_PORT;
};

#endif