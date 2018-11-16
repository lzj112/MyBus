#ifndef _NETCOMM_H
#define _NETCOMM_H

/*
管理 :
跨主机的进程间通信
网络部分
*/

// #include <typeinfo.h>

#include "BusInfo.h"
#include "socketBus.h"

class NetComm 
{
public:
    NetComm() : netListHead_ID(-1), netListHead_Addr(nullptr), 
                proListHead_ID(-2), proListHead_Addr(nullptr)
    {}
    int initShmList(const RoutingTable& str);                              //初始化链表头结点
    int initShmList(const proToNetqueue& str);
    int updateList(RoutingTable* str);
    int updateList(proToNetqueue* str);
    int delListNode(int fd, const RoutingTable& str);                            //哪个sockfd被关闭了就删除该节点
    int delListNode(int fd, const proToNetqueue& str);
    int isThereConn(const char* ip, int port, const RoutingTable& str);
    int isThereShm(int pid, const proToNetqueue& str);
    int initShmProList(); 
    
    socketBus socketControl;    //socket TCP连接

private:
    int netListHead_ID;                 //路由表头shmid
    RoutingTable* netListHead_Addr;             //路由表头映射地址
    int proListHead_ID;                 //进程对应shm队列表
    proToNetqueue* proListHead_Addr;            //进程对应表映射地址
};

#endif