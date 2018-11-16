#ifndef _BUSINFO_H_
#define _BUSINFO_H_

/*
创建的进程间共享内存的通道的控制信息
共享内存信息
*/

#include <sys/types.h>

/*
    [0][0]存储读取队列id,[0][1]存储发送队列id,[0][2]存储当前进程id
    通信对端进程发现存储的进程id不是本进程id,那么对应的读写队列就是它的写读队列
    [1][0],[1][1],[2][0],[2][1]存储两个队列的头尾指针
*/
struct BusCard
{
    key_t ftokKey;             //ftok的key
    int shmSelfId;             //本身这块共享内存的id
    int localQueue[3][3];      //本机通信队列
    int netQueue[3][2];        //跨机通信队列
};


//进程间通信的结构
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

//进程间通信需要的地址
struct ProComm 
{
    pid_t pid;  //使用线程id
    // char* source_IP;
    // int source_PORT;
    char* dest_IP;
    int dest_PORT;
};

//路由表
struct RoutingTable 
{
    // pid_t pid;
    char sourceIp[20];
    char destIp[20];
    int sourcePort;
    int destPort;
    int sockfd;
    int shmidNext;
};

//MyBus中转进程中存储的各进程与其共享内存通道的对应
struct proToNetqueue 
{
    pid_t pid;
    int readQueue;
    int writeQueue;
    int shmidNext;
};

#endif