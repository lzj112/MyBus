#ifndef _BUSINFO_H_
#define _BUSINFO_H_

/*
创建的进程间共享内存的通道的控制信息
共享内存信息
*/

#include <cstring>
#include <sys/types.h>

#define READ 0
#define WRITE 1
#define READY 3
#define PATH "/home/lzj/MyBus"
#define QUEUESIZE 256
#define PacketBodyBufferSize 256

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
    int netQueue[3];           //跨机队列,发送缓冲区   
};


//进程间通信需要的地址
struct ProComm 
{
public:
    char sourceIP[10];
    int sourcePort;
    char destIP[10];
    int destPort;
    char sourcePassIP[10];
    int sourcePassPort;
    char destPassIP[10];
    int destPassPort;
    ProComm(){
    }
    ProComm(const char* sourceIP, int sourcePort, const char* passIP, int passPort,
            const char* destIP, int destPort, const char* destPassIP, int destPassPort)
    {
        strcpy(this->destIP, destIP);
        strcpy(this->sourceIP, sourceIP);
        strcpy(this->sourcePassIP, passIP);
        strcpy(this->destPassIP, destPassIP);
        this->destPort = destPort;
        this->sourcePort = sourcePort;
        this->sourcePassPort = passPort;
        this->destPassPort = destPassPort;
    }
    ProComm& operator= (const ProComm& str) 
    {
        if (&str == this) 
        {
            return *this;
        }
        destPort = str.destPort;
        sourcePort = str.sourcePort;
        destPassPort = str.destPassPort;
        sourcePassPort = str.sourcePassPort;
        strcpy(destIP, str.destIP);
        strcpy(sourceIP, str.sourceIP);
        strcpy(destPassIP, str.destPassIP);
        strcpy(sourcePassIP, str.sourcePassIP);

        return *this;
    }
};

//进程间通信的结构
struct PacketHead  
{
    int type;
    int bodySzie;
    char unused[4]; //用于对齐 无意义
};

struct PacketBody
{
    struct PacketHead head;
    struct ProComm netQuaad;    //四元组 确定对端IP port
    char buffer[256];
};

//通知中转进程
struct Notice 
{
    struct PacketHead head;
    struct ProComm netQueaad;
    int shmid;      //buscard的shmid
};

//路由表
struct RoutingTable 
{
    char IP[8];
    int port;
    int sockfd;
    int listLength;
    int shmidNext;
};

//MyBus中转进程中存储的各进程与其共享内存通道的对应
struct proToNetqueue 
{   
    // int shmSelfId;
    int offset;
    char sourceIP[10];  //本机中转进程ip por
    int sourcePort;         
    char destIP[10];    //对端中转进程ip port
    int destPort;
    int readQueue[3];
    int listLength;
    int shmidNext;
};

#endif