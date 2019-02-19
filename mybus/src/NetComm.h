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
#include "RouTabMgt.h"
#include "ProTabMgt.h"


class NetComm 
{
public:
    
    NetComm() : isRun(true){}
    void initList(int proj_id);
    void prepareSocket(const char* ip, int port);
    void runMyEpoll();
    void recvFromTCP(int confd);
    void recvFromUDP(int connfd);
    int getMessage(int connfd, PacketBody* buffer, int length);
    void saveMessage(int shmid, const PacketBody& str);
    void copy(PacketBody& str, const Notice& tmp);
    void dealData(int connfd, const PacketBody& tmpbuffer);
    void forwarding(const Notice& str);

private:
    Epoll myEpoll;
    socketBus socketControl;    //socket TCP连接
    
    RouTabMgt netList;  //路由表
    ProTabMgt proList;  //进程通道表

    bool isRun;
    std::mutex my_lock;
};

#endif