#ifndef _NETCOMM_H
#define _NETCOMM_H

/*
管理 :
跨主机的进程间通信
网络部分
*/

#include <mutex>
#include <vector>
#include <thread>

#include "Epoll.h"
#include "BusInfo.h"
#include "socketBus.h"

class NetComm 
{
public:
    
    NetComm() : isRun(true)
    {}

    void initList(int proj_id);
    int initShmList(int id);                              //初始化链表头结点
    int initShmList(int id, int);
    int updateList(int sockfd, const char* ip, int port);
    int updateList(const struct ProComm& str);
    int creShmQueue(int proj_id);
    // int delListNode(int fd, const RoutingTable& str);                            //哪个sockfd被关闭了就删除该节点
    // int delListNode(const char* ip, int port, const proToNetqueue& str);
    int isThereConn(const char* ip, int port);
    int getProShmQueue(const char* ip, int port, int flag);
    // int getListenFd();
    // int getListenFd();

    //在这里运行epoll run,读取数据拿出来让NetComm做
    void prepareSocket(const char* ip, int port);
    void runMyEpoll();
    void recvFromTCP(int confd);
    void recvFromUDP(int connfd);
    int getMessage(int connfd, PacketBody* buffer, int length);
    void saveMessage(int shmid, const PacketBody& str);
    void copy(PacketBody* ptr, const PacketBody& str);
    void copy(PacketBody& str, const Notice& tmp);
    void dealData(int connfd, const PacketBody& tmpbuffer);
    void forwarding(const Notice& str);
    void realseAll(NetComm* str);

    Epoll myEpoll;
    socketBus socketControl;    //socket TCP连接

private:
    
    int netList[3];                 //路由表头shmid
    // RoutingTable* netListHead_Addr;     //路由表头映射地址
    int proList[3];                 //进程对应shm队列表
    // proToNetqueue* proListHead_Addr;    //进程对应表映射地址

    bool isRun;
    std::mutex my_lock;
};

#endif