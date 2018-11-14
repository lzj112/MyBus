#ifndef _NETCOMM_H
#define _NETCOMM_H

/*
管理 :
跨主机的进程间通信
网络部分
*/

#include "BusInfo.h"
#include "socketBus.h"

class NetComm 
{
public:
    NetComm() : listHead_ID(-1), headAddr(nullptr){}
    int initShmListHead();                              //初始化链表头结点
    int updateList(RoutingTable* infoTmp);              //有新的连接,更新链表节点
    int delListNode(int fd);                            //哪个sockfd被关闭了就删除该节点
    int isThereFd(RoutingTable* infoTmp);               //查看是否有连接存在
   
    socketBus socketControl;    //路由表 链表

private:
    int listHead_ID;            //路由表头shmid
    RoutingTable* headAddr;             //路由表头映射地址
};

#endif