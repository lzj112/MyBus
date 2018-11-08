#ifndef _BUSINFO_H_
#define _BUSINFO_H_

/*
创建的进程间共享内存的通道的控制信息
共享内存信息
*/

#include <sys/types.h>

/*
    proQueueNumber[0][0]存储读取队列id,[0][1]存储发送队列id,[0][2]存储当前进程id
    通信对端进程发现存储的进程id不是本进程id,那么对应的读写队列就是它的写读队列
    proQueueNumber[1][0],[1][1],[2][0],[2][1]存储两个队列的头尾指针
    */
struct BusCard
{
    key_t ftokKey;   //ftok的key
    int shmSelfId;    //本身这块共享内存的id
    int proQueuenNmber[3][3];

};

/*
进程间通信的结构
*/
const int PacketBodyBufferSize = 256;
struct PacketHead  
{
    size_t bodySzie;
};

struct PacketBody
{
    struct PacketHead head;
    char buffer[256];
};

struct ProComm //进程间通信需要的地址
{
    char* source_IP;
    int source_PORT;
    char* dest_IP;
    int dest_PORT;
};

#endif